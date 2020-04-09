/*
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

void abort_(const char * s, ...)
{
        va_list args;
        va_start(args, s);
        vfprintf(stderr, s, args);
        fprintf(stderr, "\n");
        va_end(args);
        abort();
}




struct image {
        int x, y;
        int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
};	

void read_png_file(struct image *t, char* file_name)
{
	// struct image *retVal = malloc(sizeof(struct image));
        char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(file_name, "rb");
        if (!fp)
                abort_("[read_png_file] File %s could not be opened for reading", file_name);
        fread(header, 1, 8, fp);
        if (png_sig_cmp(header, 0, 8))
                abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


        /* initialize stuff */
        t->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!t->png_ptr)
                abort_("[read_png_file] png_create_read_struct failed");

        t->info_ptr = png_create_info_struct(t->png_ptr);
        if (!t->info_ptr)
                abort_("[read_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(t->png_ptr)))
                abort_("[read_png_file] Error during init_io");

        png_init_io(t->png_ptr, fp);
        png_set_sig_bytes(t->png_ptr, 8);

        png_read_info(t->png_ptr, t->info_ptr);

        t->width = png_get_image_width(t->png_ptr, t->info_ptr);
        t->height = png_get_image_height(t->png_ptr, t->info_ptr);
        t->color_type = png_get_color_type(t->png_ptr, t->info_ptr);
        t->bit_depth = png_get_bit_depth(t->png_ptr, t->info_ptr);

        t->number_of_passes = png_set_interlace_handling(t->png_ptr);
        png_read_update_info(t->png_ptr, t->info_ptr);


        /* read file */
        if (setjmp(png_jmpbuf(t->png_ptr)))
                abort_("[read_png_file] Error during read_image");

        t->row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * t->height);
        for (t->y=0; t->y<t->height; t->y++)
                t->row_pointers[t->y] = (png_byte*) malloc(png_get_rowbytes(t->png_ptr,t->info_ptr));

        png_read_image(t->png_ptr, t->row_pointers);

        fclose(fp);

}


void write_png_file(struct image *t, char* file_name)
{
        /* create file */
        FILE *fp = fopen(file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);


        /* initialize stuff */
        t->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!t->png_ptr)
                abort_("[write_png_file] png_create_write_struct failed");

        t->info_ptr = png_create_info_struct(t->png_ptr);
        if (!t->info_ptr)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(t->png_ptr)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(t->png_ptr, fp);


        /* write header */
        if (setjmp(png_jmpbuf(t->png_ptr)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(t->png_ptr, t->info_ptr, t->width, t->height,
                     t->bit_depth, t->color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(t->png_ptr, t->info_ptr);


        /* write bytes */
        if (setjmp(png_jmpbuf(t->png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(t->png_ptr, t->row_pointers);


        /* end write */
        if (setjmp(png_jmpbuf(t->png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(t->png_ptr, NULL);

        /* cleanup heap allocation */
        for (t->y=0; t->y<t->height; t->y++)
                free(t->row_pointers[t->y]);
        free(t->row_pointers);

        fclose(fp);
}


void process_file(struct image *t, struct image *t2)
{
        if (png_get_color_type(t->png_ptr, t->info_ptr) == PNG_COLOR_TYPE_RGB)
                abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
                       "(lacks the alpha channel)");

        if (png_get_color_type(t->png_ptr, t->info_ptr) != PNG_COLOR_TYPE_RGBA)
                abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
                       PNG_COLOR_TYPE_RGBA, png_get_color_type(t->png_ptr, t->info_ptr));

        for (t->y=0; t->y<t->height; t->y++) {
                png_byte* row = t->row_pointers[t->y];
                for (t->x=0; t->x<t->width; t->x++) {
                        png_byte* ptr = &(row[t->x*4]);
                        // printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
                        //       x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

                        /* set red value to 0 and green value to the blue one */

			/* set all black pixels to white */
			if (ptr[0] <= 20 && ptr[1] <= 20 && ptr[2] <= 20) {
				ptr[0] = 255;
				ptr[1] = 255;
				ptr[2] = 255;
			}

                        // ptr[0] = 0;
                        // ptr[1] = ptr[2];
                }
        }

	// merge images
        // replace white pixels in image 1 with pixels from image 2
	 for (t->y=0; t->y<t->height; t->y++) {
                png_byte* row1 = t->row_pointers[t->y];
    		png_byte* row2 = t2->row_pointers[t->y];
                for (t->x=0; t->x<t->width; t->x++) {
                        png_byte* ptr1 = &(row1[t->x*4]);
			png_byte* ptr2 = &(row2[t->x*4]);

                        

                        /* set all black pixels to white */
                        if (ptr1[0] == 255 && ptr1[1] == 255 && ptr1[2] == 255) {
                                ptr1[0] = ptr2[0]; 
                                ptr1[1] = ptr2[1];
                                ptr1[2] = ptr2[2];
                        }

                        // ptr[0] = 0;
                        // ptr[1] = ptr[2];
                }
        }




}


int main(int argc, char **argv)
{
        if (argc != 4)
                abort_("Usage: program_name <file1_in> <file2_in> <file_out>");

	struct image *image1 = malloc(sizeof(struct image));

        struct image *image2 = malloc(sizeof(struct image));


        read_png_file(image1, argv[1]);
        read_png_file(image2, argv[2]);

        process_file(image1, image2);

        write_png_file(image1, argv[3]);

        return 0;
}
