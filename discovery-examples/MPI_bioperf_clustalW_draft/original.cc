/***
 * mpi_njtree_master implemented a task-farm with MPI
 *  
 *  It implements 	
  
       for (i = 1; i <= last_seq - first_seq; i++) {
	    for (j = i+1; j <= (last_seq-first_seq+1) ; j++) {
		sumd += tmat[i][j];
	    }
	}

	for (i = 1; i <= last_seq - first_seq+1; i++) 
	    for (j = 1; j <= (last_seq-first_seq+1) ; j++)
		rdiq[i]+= tmat[i][j];
 * 
 ***/


#ifdef SERIAL_NJTREE

	for (i = 1; i <= last_seq - first_seq; i++) {
	    for (j = i+1; j <= (last_seq-first_seq+1) ; j++) {
		tmat[j][i] = tmat[i][j];
		sumd += tmat[i][j];
	    }
	}

	for (i = 1; i <= last_seq - first_seq+1; i++) 
	    for (j = 1; j <= (last_seq-first_seq+1) ; j++)
		rdiq[i]+= tmat[i][j];

#else

  /* we'll use MPI. */

	for (i = 1; i <= last_seq - first_seq; i++) {
	    for (j = i+1; j <= (last_seq-first_seq+1) ; j++) {
		tmat[j][i] = tmat[i][j];
	    }
	}

   mpi_njtree_master(&sumd, rdiq, tmat, (last_seq-first_seq+1));

#endif


static void mpi_njtree_master(double *sumd, double *rdiq,
			      double **tmat, int tmat_len)
{
    MPI_Request *mpirequest;
    MPI_Status *mpistatus;
    MPI_Status status;
    int i, j;
    int *mpirow;
    int myidx;
    int *buffer_len;
    char **sbuffer;
    int *position;
    double *rdiq_tmp;
    int work, dest;
    int np;

    MPI_Comm_size(MPI_COMM_WORLD, &np);

	/* 
	 * Prepare to send data to slaves
	 */
	*sumd = 0.0;

	/*
	 * First, we need to determine which row is sending to which
	 * slave. We have a total of (tmat_len) rows. The length of each
	 * row is (tmat_len).
	 */


	/*
	 * mpirow[1..tmat_len] = the rank of the MPI process
	 * who is responsible for this row.
	 */
	mpirow = (int *) malloc((tmat_len+1) * sizeof(int));
	assert(mpirow);

	myidx = 1;
	while (myidx <= tmat_len) {
	    for (i = 1; i <= (np - 1) && myidx <= tmat_len; i++) {
		mpirow[myidx] = i;
		myidx++;
	    }
	}


	/*
	 * Now it's time to pack and send rows.
	 *
	 * We need (np-1) buffers.
	 */
	sbuffer = (char **) malloc(np * sizeof(char *));
	assert(sbuffer);
	buffer_len = (int *) calloc(np, sizeof(int));
	assert(buffer_len);


	position = (int *) malloc(np * sizeof(int));

	/* determine the length of each of the (np-1) buffer */
	for (i = 1; i <= tmat_len; i++)
	    buffer_len[mpirow[i]] += (tmat_len);

	for (i = 1; i < np; i++)
	    buffer_len[i] =
		sizeof(int) + buffer_len[i] * sizeof(double) +
		(tmat_len+1) * sizeof(int);

	/*
	 * Allocate space for sbuffer[1..(np-1)]. We'll also
	 * send the mpirow[1..(tmat_len-1)] array to slaves.
	 */
	for (i = 1; i < np; i++) {
	    sbuffer[i] = (char *) malloc(buffer_len[i]);
	    assert(sbuffer[i]);
	}


	/*
	 * Now we pack mpirow[] then tmat[].
	 */
	for (i = 1; i < np; i++) {
	    position[i] = 0;
	    MPI_Pack(&tmat_len, 1, MPI_INT, sbuffer[i], buffer_len[i],
		     &position[i], MPI_COMM_WORLD);
	    MPI_Pack(mpirow, (tmat_len+1), MPI_INT, sbuffer[i], buffer_len[i],
		     &position[i], MPI_COMM_WORLD);
	}


	for (i = 1; i <= tmat_len; i++) {
	    MPI_Pack(&(tmat[i][1]), tmat_len, MPI_DOUBLE,
		     sbuffer[mpirow[i]], buffer_len[mpirow[i]],
		     &position[mpirow[i]], MPI_COMM_WORLD);
	}


	/*
	 * Send length then the actual data to slaves.
	 */

	mpirequest = (MPI_Request *) malloc(np * sizeof(MPI_Request));
	assert(mpirequest);
	mpistatus = (MPI_Status *) malloc(np * sizeof(MPI_Status));
	assert(mpistatus);

	for (i = 1; i < np; i++) {
	    MPI_Send(&buffer_len[i], 1, MPI_INT, i, DOING_NJTREE,
		     MPI_COMM_WORLD);
	}


	for (i = 1; i < np; i++) {
	    MPI_Isend(sbuffer[i], buffer_len[i], MPI_PACKED, i,
		      NJTREE_DATA, MPI_COMM_WORLD, &mpirequest[i - 1]);
	}
	MPI_Waitall(np - 1, mpirequest, mpistatus);


	/* Wait for result to come back */

	/*
	 * Allocate space for rdiq[1..(tmat_len)]
	 */
	rdiq_tmp = (double *) calloc((tmat_len+1), sizeof(double));
	assert(rdiq);

	work = (np - 1);
	*sumd = 0.0;
	while (work > 0) {

	    MPI_Recv(rdiq_tmp, (tmat_len+1), MPI_DOUBLE, MPI_ANY_SOURCE,
		     TMAT_ROW_SUM, MPI_COMM_WORLD, &status);

	    dest = status.MPI_SOURCE;

	    *sumd += rdiq_tmp[0];

	    for (i = 1; i <= (tmat_len); i++)
		rdiq[i] += rdiq_tmp[i];

	    work--;
	}

	/* house cleaning */
	for (i = 1; i < np; i++) {
	    free(sbuffer[i]);
	}
	free(sbuffer);
	free(buffer_len);
	free(position);


	free(rdiq_tmp);
	free(mpirow);
	free(mpirequest);
	free(mpistatus);

	/*
    for (i = 1; i < np; i++) {
	MPI_Send(0, 0, MPI_INT, i, MY_ENDING_TAG, MPI_COMM_WORLD);
    }
    */

    return;
}
