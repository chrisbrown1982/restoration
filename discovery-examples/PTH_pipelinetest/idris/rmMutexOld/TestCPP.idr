module TestCpp

import CppDSL

%default total
%access public export

varIncrement : (v : String) -> CExp
varIncrement v =
  (BinOp Plus (Var v Stop) (Num 1 Stop) Stop)

varDecrement : (v : String) -> CExp
varDecrement v =
  (BinOp Minus (Var v Stop) (Num 1 Stop) Stop)

encodeInt : Int -> Nat
encodeInt 0 = 0
encodeInt n =
  if n < 0
    then toNat (n * -2)
    else toNat (n * 2 - 1)

||| 'decodeInt is possibly not total due to: Prelude.Nat.Nat implementation of Prelude.Interfaces.Integral'
partial
decodeInt : Nat -> Int
decodeInt Z = 0
decodeInt n@(S k) =
  if n `mod` 2 == 0
    then fromNat (n `div` 2) * -1
    else fromNat $ (n + 1) `div` 2

-- ----------------------------------------------------------------------------
-- Original test.cpp functions.
-- ----------------------------------------------------------------------------

||| pthread_mutex_lock(&monk);
||| bool state = (inBuffer < 0);
||| pthread_mutex_unlock(&monk);
||| return state;
fnBufferIsEmptyOrig : CExp
fnBufferIsEmptyOrig =
    PThOp (MutexLock "monk") $
    Decl CBool "state" $
    Assign "state" inBufferLTZero $
    PThOp (MutexUnlock "monk") $
    Return (Var "state" Stop) Stop
  where
    inBufferLTZero : CExp
    inBufferLTZero =
      BinOp LT (Var "inBuffer" Stop) (Num 0 Stop) Stop

||| pthread_mutex_lock(&monk);
||| bool state = (inBuffer >= BUFFER_SIZE);
||| pthread_mutex_unlock(&monk);
||| return state;
fnBufferIsFullOrig : CExp
fnBufferIsFullOrig =
    PThOp (MutexLock "monk") $
    Decl CBool "state" $
    Assign "state" inBufferGTEZero $
    PThOp (MutexUnlock "monk") $
    Return (Var "state" Stop) Stop
  where
    inBufferGTEZero : CExp
    inBufferGTEZero =
      BinOp GTE (Var "inBuffer" Stop) (Num 0 Stop) Stop


||| int id = *((int*)threadarg);
||| cout << "StageOne, " << id << endl;
||| int i = 0;
||| while (i < 101) {
|||   if (!BufferIsFull()) {
|||     pthread_mutex_lock(&monk);
|||     if (inBuffer < BUFFER_SIZE) {
|||       inBuffer = inBuffer + 1;  
|||     }
|||     cout << id << "; Put in buffer, " << inBuffer << endl;
|||     pthread_mutex_unlock(&monk);
|||     sleep(i*2);
|||     i = i + 1;
|||   }
||| }
||| pthread_exit(NULL);
fnStageOneOrig : CExp
fnStageOneOrig =
    Decl CInt "id" $
    Assign "id" derefCastThreadarg $
    Decl CInt "i" $
    Assign "i" (Num 0 Stop) $ 
    While iLT101 whileBody $
    PThOp ExitNull $
    Stop
  where
    derefCastThreadarg : CExp
    derefCastThreadarg =
      UniOp Deref (UniOp (Cast (CPtr CInt)) (Var "threadarg" Stop) Stop) Stop
    
    iLT101 : CExp
    iLT101 = BinOp LT (Var "i" Stop) (Num 101 Stop) Stop

    notBufferIsFull : CExp
    notBufferIsFull =
      UniOp CNot (App "BufferIsFull" Stop Stop) Stop
    
    inBufferLTBufferSize : CExp
    inBufferLTBufferSize =
      BinOp LT (Var "inBuffer" Stop) (Var "BUFFER_SIZE" Stop) Stop

    ifInIfTrueBranch : CExp
    ifInIfTrueBranch =
      Assign "inBuffer" (varIncrement "inBuffer") Stop
    
    ifTrueBranch : CExp
    ifTrueBranch =
      PThOp (MutexLock "monk") $
      If inBufferLTBufferSize ifInIfTrueBranch Stop $
      CIO COut (Var "id" Stop) $
      CIO COut (Str "; Put in buffer, " Stop) $
      CIO COut (Var "inBuffer" Stop) $
      CIO COut Stop $ -- endl; don't like this.
      PThOp (MutexUnlock "monk") $
      App "sleep" (BinOp Multiply (Var "i" Stop) (Num 2 Stop) Stop) $
      Assign "i" (varIncrement "i") $
      Stop

    whileBody : CExp
    whileBody =
      If notBufferIsFull ifTrueBranch Stop Stop

||| int id = *((int*)threadarg);
||| cout << "StageTwo, " << id << endl;
||| int i = 0;
||| while (true) {
|||   if (!BufferIsEmpty()) {
|||     pthread_mutex_lock(&monk);
|||     cout << id << "; inBuffer : " << inBuffer << endl;
|||     inBuffer--;
|||     cout << id << "; inBuffer now: " << inBuffer << endl;
|||     pthread_mutex_unlock(&monk);
|||     i = i + 1;
|||   }
|||   sleep(i*4);
||| }
||| pthread_exit(NULL);
fnStageTwoOrig : CExp
fnStageTwoOrig =
    Decl CInt "id" $
    Assign "id" derefCastThreadarg $
    Decl CInt "i" $
    Assign "i" (Num 0 Stop) $ 
    While (Bool True Stop) whileBody $
    PThOp ExitNull $
    Stop
  where
    derefCastThreadarg : CExp
    derefCastThreadarg =
      UniOp Deref (UniOp (Cast (CPtr CInt)) (Var "threadarg" Stop) Stop) Stop

    notBufferIsEmpty : CExp
    notBufferIsEmpty =
      UniOp CNot (App "BufferIsEmpty" Stop Stop) Stop
    
    ifTrueBranch : CExp
    ifTrueBranch =
      PThOp (MutexLock "monk") $
      CIO COut (Var "id" Stop) $
      CIO COut (Str "; inBuffer : " Stop) $
      CIO COut (Var "inBuffer" Stop) $
      CIO COut Stop $
      Assign "inBuffer" (varDecrement "inBuffer") $
      CIO COut (Var "id" Stop) $
      CIO COut (Str "; inBuffer now: " Stop) $
      CIO COut (Var "inBuffer" Stop) $
      CIO COut Stop $
      PThOp (MutexUnlock "monk") $
      Assign "i" (varIncrement "i") $
      Stop

    whileBody : CExp
    whileBody =
      If notBufferIsEmpty ifTrueBranch Stop $
      App "sleep" (BinOp Multiply (Var "i" Stop) (Num 3 Stop) Stop) $
      Stop

||| pthread_t threads[NUM_THREADS*2];
||| int rc;
||| int i;
||| 
||| for(i = 0; i < NUM_THREADS; i++) {
|||   int *arg = (int*) malloc(sizeof(*arg));
|||   cout << "main() : creating first batch thread, " << i << endl;
|||   // StageOne(&i);
|||   *arg = i;
|||   rc = pthread_create(&threads[i], NULL, StageOne, arg);
||| 
|||   if (rc) {
|||     cout << "Error: unable to create thread, " << rc << endl;
|||     exit(-1);
|||   }
||| }
||| cout << endl;
||| cout << endl;
||| for(i = 0; i < NUM_THREADS; i++) {
|||   int *arg = (int*) malloc(sizeof(*arg));
|||   *arg = i+NUM_THREADS;
|||   cout << "main(): creating second batch thread, " << i+NUM_THREADS << endl;
|||   rc = pthread_create(&threads[i+NUM_THREADS], NULL, StageTwo, arg);
||| 
|||   if (rc) {
|||     cout << "Error: unable to create thread, " << rc << endl;
|||     exit(-1);
|||   }
||| }
||| 
||| pthread_join(threads[1], NULL);
fnMainOrig : CExp
fnMainOrig =
    Decl (CPThTy ThreadT) "t1" $
    Decl (CPThTy ThreadT) "t2" $
    Decl CInt "rc" $
    Decl CInt "i" $
    For "i" iLTNumThreads forStageOneBody $
    For "i" iLTNumThreads forStageTwoBody $
    PThOp (JoinNull "t1") $
    PThOp (JoinNull "t2") $
    Stop
  where
    iLTNumThreads : CExp
    iLTNumThreads =
      BinOp LT (Var "i" Stop) (Var "NUM_THREADS" Stop) Stop
    
    mallocArg : CExp
    mallocArg =
      App "malloc" (App "sizeof" (UniOp Deref (Var "arg" Stop) Stop) Stop) Stop
    
    ||| Note that we do not deal with the return value of pthread_create.
    forStageOneBody : CExp
    forStageOneBody =
      Decl (CPtr CInt) "arg" $
      Assign "arg" (UniOp (Cast (CPtr CInt)) (mallocArg) Stop) $
      CIO COut (Str "main() : creating first batch thread, " Stop) $
      CIO COut (Var "i" Stop) $
      CIO COut Stop $
      AssPtr "arg" (Var "i" Stop) $
      PThOp (CreateNull "t1" "StageOne" "arg") $
      Stop
    
    ||| Note that we do not deal with the return value of pthread_create.
    forStageTwoBody : CExp
    forStageTwoBody =
      Decl (CPtr CInt) "arg" $
      Assign "arg" (UniOp (Cast (CPtr CInt)) (mallocArg) Stop) $
      CIO COut (Str "main() : creating second batch thread, " Stop) $
      CIO COut (BinOp Plus (Var "i" Stop) (Var "NUM_THREADS" Stop) Stop) $
      CIO COut Stop $
      AssPtr "arg" (BinOp Plus (Var "i" Stop) (Var "NUM_THREADS" Stop) Stop) $
      PThOp (CreateNull "t2" "StageTwo" "arg") $
      Stop

||| test-orig.cpp (253ef4f)
prOrig : CDecl
prOrig =
  GVar CInt "NUM_THREADS" (Num (encodeInt 1) Stop) $
  GVar CInt "BUFFER_SIZE" (Num (encodeInt 10) Stop) $
  GVar CInt "inBuffer" (Num (encodeInt 2) Stop) $
  GVar (CPThTy MutexT) "monk" Stop $
  Fun CBool "BufferIsEmpty" Arrete fnBufferIsEmptyOrig $
  Fun CBool "BufferIsFull" Arrete fnBufferIsFullOrig $
  Fun (CPtr CVoid) "StageOne" (Arg (CPtr CVoid) "threadarg" Arrete) fnStageOneOrig $
  Fun (CPtr CVoid) "StageTwo" (Arg (CPtr CVoid) "threadarg" Arrete) fnStageTwoOrig $
  Fun CInt "main" Arrete fnMainOrig $
  Quit
