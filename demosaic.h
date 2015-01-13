#ifndef DEMOSAIC_H_INCLUDED
#define DEMOSAIC_H_INCLUDED

#include <stddef.h>

void demosaic_simple_interpolate(unsigned char *out_img, size_t width, size_t height, const unsigned char *in_raw);

#endif
