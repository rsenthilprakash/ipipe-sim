#include "rgb2rgb.h"
#include "png_utils.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    size_t width;
    size_t height;
    const char *in_file_name;
    const char *out_file_name;
    unsigned char *in_img;
    unsigned char *out_img;
    const double rgb2rgb_matrix[9] = {0.0, 0.0, 1.0,
                                      0.0, 1.0, 0.0,
                                      1.0, 0.0, 0.0};

    if (argc != 3) {
        printf("Usage:\n");
        printf("%s <input png (rgb) file> <output png file name>\n", argv[0]);
        return -1;
    }

    in_file_name = argv[1];
    out_file_name = argv[2];

    if (!validate_and_query_png_size(&width, &height, in_file_name)) {
        printf("Unsupported png file %s\n", in_file_name);
        return -2;
    }

    printf("Input size: %u x %u\n", width, height);

    in_img = malloc(width * height * 3);

    read_png_color(in_img, width, height, in_file_name);

    out_img = malloc(width * height * 3);

    color_convert_rgb2rgb(out_img, in_img, width, height, rgb2rgb_matrix);

    write_png_color(out_img, width, height, out_file_name);

    printf("File %s wrote\n", out_file_name);

    free(in_img);
    free(out_img);

    return 0;
}
