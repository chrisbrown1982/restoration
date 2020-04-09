#include <png.h>

int main() {
  char header[8];
  int header_size=8, is_png, i;
  FILE *fp = fopen("foobar.png","rb");
  png_structp png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info = png_create_info_struct(png_ptr);
  png_bytep raw_data; // = png_malloc(png_ptr, 4096*4096*pixel_size);
  png_bytepp row_pointers;
  png_uint_32 row_bytes;
  png_uint_32 dim=2048;

  printf ("%s\n", PNG_LIBPNG_VER_STRING);
  if (!fp) return(-1);
  fread (header, 1, header_size, fp);
  is_png = !png_sig_cmp(header,0,header_size);
  if (!is_png) {printf("not a png\n"); return(-2);}
  else printf("it is a png\n");
  png_init_io(png_ptr,fp);
  png_set_sig_bytes(png_ptr,header_size);
  png_read_info(png_ptr, info_ptr);
  //png_set_rgb_to_gray(png_ptr, 1, -1, -1);
  //png_read_update_info(png_ptr, info_ptr);
  row_bytes = png_get_rowbytes(png_ptr,info_ptr);
  raw_data = png_malloc(png_ptr, dim*row_bytes);
  row_pointers = png_malloc(png_ptr, dim*sizeof(png_bytep));
  for (i=0; i<dim; i++)
    row_pointers[i] = raw_data+i*row_bytes;
  png_set_rows(png_ptr, info_ptr, row_pointers);
  printf ("bytes per pixel is %d\n", row_bytes/dim);
  png_read_rows(png_ptr,row_pointers,NULL,dim);

  //png_read_png(png_ptr,info_ptr,PNG_TRANSFORM_IDENTITY,NULL);
  //printf ("picvajz\n");
  //row_pointers = png_get_rows(png_ptr, info_ptr);
  //printf ("salaukovina\n");
}

