#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>
#include <jerror.h>

void
read_JPEG_file (char * filename)
{
  struct jpeg_decompress_struct cinfo;
  FILE * infile;/* source file */
  JSAMPARRAY buffer;/* Output row buffer */
  int row_stride, i, jasar;/* physical row width in output buffer */
  struct jpeg_error_mgr jerr;

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    exit(1);
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  printf ("serdar %d %d %d\n", cinfo.output_width, cinfo.output_components, cinfo.output_height);
  row_stride = cinfo.output_width * cinfo.output_components;
  buffer = (JSAMPARRAY) malloc(sizeof(JSAMPROW)*cinfo.output_height);
  for (i=0; i<cinfo.output_height; i++) 
    buffer[i] = (JSAMPROW) malloc (sizeof(JSAMPLE)*row_stride);
  i = 0;
  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines (&cinfo, &(buffer[i]), 1);
    i++;
  }
  jasar = jpeg_read_scanlines(&cinfo, buffer, 8192);
  printf ("nemanja %d %d\n", cinfo.output_scanline, jasar);

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  fclose(infile);

  printf ("nemanja\n");
}

int main()
{
  read_JPEG_file("foobar.jpg");
}

