#ifndef PNG_UTILS_H_INCLUDED
#define PNG_UTILS_H_INCLUDED

#include <stddef.h>

unsigned char * alloc_and_read_png_color(size_t *out_width, size_t *out_height, const char *file_name);
void write_png_color(unsigned char *img_data, size_t width, size_t height, const char *file_name);

#endif
