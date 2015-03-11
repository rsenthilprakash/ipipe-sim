#include "lpf.h"
#include "png_utils.h"
#include <stdio.h>
#include <stdlib.h>

const unsigned int RECT_KERNEL_SIZE = 5;

int main(int argc, char *argv[])
{
    size_t width;
    size_t height;
    unsigned char *in_img;
    unsigned char *out_img;
    const char *in_file_name;
    const char *out_file_name;

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

    lpf_rectangular(out_img, in_img, width, height, RECT_KERNEL_SIZE);

    write_png_color(out_img, width, height, out_file_name);

    printf("File %s wrote\n", out_file_name);

    free(in_img);
    free(out_img);

    return 0;
}
