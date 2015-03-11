#include "demosaic.h"
#include "png_utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    size_t width;
    size_t height;
    unsigned char *in_raw;
    unsigned char *out_img;
    const char *in_file_name;
    const char *out_file_name;
    FILE *fp;
    size_t num_pixels_read;

    if (argc != 5) {
        printf("Usage:\n");
        printf("%s <input raw file> <width> <hieght> <output png file name>\n", argv[0]);
        return -1;
    }

    in_file_name = argv[1];
    width = atoi(argv[2]);
    height = atoi(argv[3]);
    out_file_name = argv[4];

    in_raw = malloc(width * height * sizeof (*in_raw));

    fp = fopen(in_file_name, "rb");
    if (!fp) {
        printf("Error opening %s\n", in_file_name);
        free(in_raw);
        return -2;
    }

    num_pixels_read = fread(in_raw, sizeof(*in_raw), width * height, fp);
    if (num_pixels_read != width * height) {
        printf("Insufficient data in %s. Expected: %u, read: %u\n", in_file_name, width * height, num_pixels_read);
        free(in_raw);
        return -2;
    }

    out_img = malloc(3 * width * height * sizeof (*out_img));

    demosaic_simple_interpolate(out_img, width, height, in_raw);

    write_png_color(out_img, width, height, out_file_name);

    free(in_raw);
    free (out_img);

    return 0;
}
