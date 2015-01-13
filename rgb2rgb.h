#ifndef RGB2RGB_H_INCLUDED
#define RGB2RGB_H_INCLUDED

#include <stddef.h>

void color_convert_rgb2rgb(unsigned char *out_img, const unsigned char *in_img, size_t width, size_t height, const double *rgb2rgb_matrix);

#endif
