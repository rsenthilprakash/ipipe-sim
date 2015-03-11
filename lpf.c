#include "lpf.h"

#include <string.h>
#include <stdio.h>

static inline unsigned char get_pixel_at_pos_and_color(const unsigned char *ptr,
						       int width, int x, int y,
						       int color)
{
    return *(ptr + (3 * width * x) + (3 * y) + color);
}

void lpf_rectangular(unsigned char *out_img, const unsigned char *in_img,
		     size_t width, size_t height, unsigned int kernel_size)
{
    if (!(kernel_size & 1))
	return;
    else {
	size_t skip = ((size_t)kernel_size - 1) / 2;
	size_t i;
	unsigned char *out_ptr = out_img;
	const unsigned char *in_ptr = in_img;

	for (i = 0; i < skip; i++) {
	    memcpy(out_ptr, in_ptr, 3 * width);
	    out_ptr += 3 * width;
	    in_ptr += 3 * width;
	}
	for (i = skip; i < height - skip; i++) {
	    size_t j;

	    for (j = 0; j < skip; j++) {
		*out_ptr++ = *in_ptr++;
		*out_ptr++ = *in_ptr++;
		*out_ptr++ = *in_ptr++;
	    }
	    for (j = skip; j < width - skip; j++) {
		double div = (double)kernel_size * kernel_size;
		double accum_r = 0.0;
		double accum_g = 0.0;
		double accum_b = 0.0;
		int k;
		int l;
		int skip_int = (int)skip;

                for (k = -skip_int; k <= skip_int; k++) {
                    for (l = -skip_int; l <= skip_int; l++) {
                        unsigned char pix_r = get_pixel_at_pos_and_color(in_ptr, width, k, l, 0);
                        unsigned char pix_g = get_pixel_at_pos_and_color(in_ptr, width, k, l, 1);
                        unsigned char pix_b = get_pixel_at_pos_and_color(in_ptr, width, k, l, 2);

                        accum_r += (double)pix_r;
                        accum_g += (double)pix_g;
                        accum_b += (double)pix_b;
		    }
		}

		*out_ptr++ = (unsigned char)(accum_r / div);
		*out_ptr++ = (unsigned char)(accum_g / div);
		*out_ptr++ = (unsigned char)(accum_b / div);

		in_ptr += 3;
	    }
            for (j = width - skip; j < width; j++) {
                *out_ptr++ = *in_ptr++;
                *out_ptr++ = *in_ptr++;
                *out_ptr++ = *in_ptr++;
            }
        }
        for (i = height - skip; i < height; i++) {
            memcpy(out_ptr, in_ptr, 3 * width);
            out_ptr += 3 * width;
            in_ptr += 3 * width;
        }
    }
}
