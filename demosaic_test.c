#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <png.h>

#define PNG_DEBUG 3

enum ColorType {
    RED,
    GREEN_RED,
    GREEN_BLUE,
    BLUE,
};

static void abort_(const char * s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

static void write_png_color(unsigned char *img_data, size_t width, size_t height, const char *file_name)
{
    png_byte bit_depth = 8;
    png_byte color_type = PNG_COLOR_TYPE_RGB;

    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep * row_pointers;
    int i;

    FILE *fp = fopen(file_name, "wb");

    if (!fp)
        abort_("[write_png_file] File could not be opened for writing");

    /* initialize stuff */
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   if (!png_ptr)
        abort_("[write_png_file] png_create_write_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("[write_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during init_io");

    png_init_io(png_ptr, fp);

    /* write header */
    if (setjmp(png_jmpbuf(png_ptr)))
       abort_("[write_png_file] Error during writing header");

    png_set_IHDR(png_ptr, info_ptr, width, height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
    for (i = 0; i < height; i++)
        row_pointers[i] = img_data + (i * width * 3);

    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing bytes");

    png_write_image(png_ptr, row_pointers);

    /* end write */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during end of write");

    png_write_end(png_ptr, NULL);

    free(row_pointers);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
}

static enum ColorType get_color_at_pixel(size_t i, size_t j)
{
    enum ColorType color;

    if (i % 2) {
        /* odd line GBGBGB... */
        if (j % 2) /* odd pixel */
            color = BLUE;
        else
            color = GREEN_BLUE;
    }
    else {
        /* even line RGRGRG... */
        if (j % 2) /* odd pixel */
            color = GREEN_RED;
        else
            color = RED;
    }

    return color;
}

static void demosaic_simple_interpolate(unsigned char *out_img, size_t width, size_t height, const unsigned char *in_raw)
{
    unsigned char *out_ptr = out_img;
    const size_t num_work_lines = 3;
    unsigned char *work_mem = malloc(num_work_lines * 3 * (width + 2));
    unsigned char *line_0 = work_mem;
    unsigned char *line_1 = line_0 + width + 2;
    unsigned char *line_2 = line_1 + width + 2;
    size_t i;

    /* e.g 6 x 4 image
     * Before padding
     * R00 G01 R02 G03 R04 G05
     * G10 B11 G12 B13 G14 B15
     * R20 G21 R22 G23 R24 G25
     * G30 B31 G32 B33 G34 B35
     *
     * After padding, image becomes 8 x 6
     * row[-1] = row[1], row[h - 1] = row[h - 1],  col[-1] = col[1] and  col[w + 1] = col[w - 1]
     * (B11) G10 B11 G12 B13 G14 B15 (G14) - row[1] repeated above row[0] i.e line[-1]
     * (G01) R00 G01 R02 G03 R04 G05 (R04) - () entries represent padding in a row
     * (B11) G10 B11 G12 B13 G14 B15 (G14)
     * (G21) R20 G21 R22 G23 R24 G25 (R24)
     * (B31) G30 B31 G32 B33 G34 B35 (G34)
     * (G21) R20 G21 R22 G23 R24 G25 (R24) - row[5] = row[3]
     */

    for (i = 0; i < height; i++) {
        size_t j;
        size_t k;

        if (i == 0) {
            memcpy(line_1 + 1, in_raw, width);
            line_1[0] = line_1[2];
            line_1[width + 1] = line_1[width - 1];

            memcpy(line_2 + 1, in_raw + (1 * width), width);
            line_2[0] = line_2[2];
            line_2[width + 1] = line_2[width - 1];

            memcpy(line_0, line_1, width + 2);
        }
        else if (i == (height - 1)) {
            memcpy(line_0 + 1, in_raw + (height - 2) * width, width);
            line_0[0] = line_0[2];
            line_0[width + 1] = line_0[width - 1];

            memcpy(line_1 + 1, in_raw + (height - 1) * width, width);
            line_1[0] = line_1[2];
            line_1[width + 1] = line_1[width - 1];

            memcpy(line_2, line_0, width + 2);
        }
        else {
            memcpy(line_0 + 1, in_raw + (i - 1) * width, width);
            line_0[0] = line_0[2];
            line_0[width + 1] = line_0[width - 1];

            memcpy(line_1 + 1, in_raw + i * width, width);
            line_1[0] = line_1[2];
            line_1[width + 1] = line_1[width - 1];

            memcpy(line_2 + 1, in_raw + (i + 1) * width, width);
            line_2[0] = line_2[2];
            line_2[width + 1] = line_2[width - 1];
        }

        for (j = 0, k = 1; j < width; j++, k++) {
            unsigned char r;
            unsigned char g;
            unsigned char b;

            enum ColorType color = get_color_at_pixel(i, j);

            switch (color) {
            case RED:
                r = line_1[k];
                g = (line_0[k] + line_2[k] + line_1[k - 1] + line_1[k + 1]) / 4;
                b = (line_0[k - 1] + line_0[k + 1] + line_2[k - 1] + line_2[k + 2]) / 4;
                break;
            case GREEN_RED:
                g = line_1[k];
                r = (line_1[k - 1] + line_1[k + 1]) / 2;
                b = (line_0[k] + line_2[k]) / 2;
                break;
            case GREEN_BLUE:
                g = line_1[k];
                r = (line_0[k] + line_2[k]) / 2;
                b = (line_1[k - 1] + line_1[k + 1]) / 2;
                break;
            case BLUE:
                b = line_1[k];
                g = (line_0[k] + line_2[k] + line_1[k - 1] + line_1[k + 1]) / 4;
                r = (line_0[k - 1] + line_0[k + 1] + line_2[k - 1] + line_2[k + 2]) / 4;
                break;
            }

            *out_ptr++ = r;
            *out_ptr++ = g;
            *out_ptr++ = b;
        }
    }

    free(work_mem);
}

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
        printf("%s <input raw file> <width> <hieght> <outout png file name>\n", argv[0]);
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
