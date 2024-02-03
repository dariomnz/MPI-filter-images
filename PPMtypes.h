#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

struct __attribute__((packed)) PPMHeader
{
    char signature[2];
    uint32_t width;
    uint32_t height;
    uint16_t max_color;
    uint64_t data_offset;
};

struct __attribute__((packed)) PPMData
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct filter
{
    char *name;
    uint8_t filterWidth;
    uint8_t filterHeight;
    double matrix[10][10];
    double factor;
    double bias;
};

extern struct filter filter_blur_3;
extern struct filter filter_blur_5;
extern struct filter filter_gaussian_3;
extern struct filter filter_gaussian_5;
extern struct filter filter_motion_blur;
extern struct filter filter_find_edges_horizontal;
extern struct filter filter_find_edges_vertical;
extern struct filter filter_find_edges_45_degrees;
extern struct filter filter_find_edges_all_directions;
extern struct filter filter_sharpen;
extern struct filter filter_sharpen_subtle;
extern struct filter filter_edge_detection_exaggerated;
extern struct filter filter_emboss_45_degrees;
extern struct filter filter_emboss_exaggerated;

void print_header(struct PPMHeader *header);
int file_exists(char *filename);

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))