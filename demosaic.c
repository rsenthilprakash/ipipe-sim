#include "demosaic.h"
#include <string.h>
#include <stdlib.h>

enum ColorType {
    RED,
    GREEN_RED,
    GREEN_BLUE,
    BLUE,
};

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

void demosaic_simple_interpolate(unsigned char *out_img, size_t width, size_t height, const unsigned char *in_raw)
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
