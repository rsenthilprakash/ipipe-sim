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
