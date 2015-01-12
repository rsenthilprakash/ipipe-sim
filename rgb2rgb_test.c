#include "png_utils.h"
#include <stdio.h>
#include <stdlib.h>

static void color_convert_rgb2rgb(unsigned char *out_img, const unsigned char *in_img, size_t width, size_t height)
{
    const double RR = 0.0;
    const double RG = 0.0;
    const double RB = 1.0;
    const double GR = 0.0;
    const double GG = 1.0;
    const double GB = 0.0;
    const double BR = 1.0;
    const double BG = 0.0;
    const double BB = 0.0;

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

int main(int argc, char *argv[])
{
    size_t width;
    size_t height;
    const char *in_file_name;
    const char *out_file_name;
    unsigned char *in_img;
    unsigned char *out_img;

    if (argc != 3) {
        printf("Usage:\n");
        printf("%s <input png (rgb) file> <outout png file name>\n", argv[0]);
        return -1;
    }

    in_file_name = argv[1];
    out_file_name = argv[2];

    in_img = alloc_and_read_png_color(&width, &height, in_file_name);

    if (in_img == NULL)
        return -1;

    printf("Input size: %u x %u\n", width, height);

    out_img = malloc(width * height * 3);

    color_convert_rgb2rgb(out_img, in_img, width, height);

    write_png_color(out_img, width, height, out_file_name);

    printf("File %s wrote\n", out_file_name);

    free(in_img);
    free(out_img);

    return 0;
}
