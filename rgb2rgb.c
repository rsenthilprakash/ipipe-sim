#include "rgb2rgb.h"

void color_convert_rgb2rgb(unsigned char *out_img, const unsigned char *in_img, size_t width, size_t height, const double *rgb2rgb_matrix)
{
    const double RR = rgb2rgb_matrix[0];
    const double RG = rgb2rgb_matrix[1];
    const double RB = rgb2rgb_matrix[2];
    const double GR = rgb2rgb_matrix[3];
    const double GG = rgb2rgb_matrix[4];
    const double GB = rgb2rgb_matrix[5];
    const double BR = rgb2rgb_matrix[6];
    const double BG = rgb2rgb_matrix[7];
    const double BB = rgb2rgb_matrix[8];

    size_t i;
    size_t j;

    const unsigned char *in_ptr = in_img;
    unsigned char *out_ptr = out_img;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            unsigned char r_in = *in_ptr++;
            unsigned char g_in = *in_ptr++;
            unsigned char b_in = *in_ptr++;

            unsigned char r_out = (unsigned char)((r_in * RR) + (g_in * RG) + (b_in * RB));
            unsigned char g_out = (unsigned char)((r_in * GR) + (g_in * GG) + (b_in * GB));
            unsigned char b_out = (unsigned char)((r_in * BR) + (g_in * BG) + (b_in * BB));

            *out_ptr++ = r_out;
            *out_ptr++ = g_out;
            *out_ptr++ = b_out;
        }
    }
}
