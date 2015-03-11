#ifndef LPF_H_INCLUDED
#define LPF_H_INCLUDED

#include <stddef.h>

void lpf_rectangular(unsigned char *out_img, const unsigned char *in_img,
                     size_t width, size_t height, unsigned int kernel_size);

#endif
