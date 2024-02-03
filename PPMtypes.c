#include "PPMtypes.h"

struct filter filter_blur_3 = {
    .name = "blur_3",
    .filterWidth = 3,
    .filterHeight = 3,
    .matrix = {{0.2, 0.2, 0.2},
               {0.2, 0.2, 0.2},
               {0.2, 0.2, 0.2}},
    .factor = 1.0,
    .bias = 0.0};

struct filter filter_blur_5 = {
    .name = "blur_5",
    .filterWidth = 5,
    .filterHeight = 5,
    .matrix = {
        {0, 0, 1, 0, 0},
        {0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0},
        {0, 0, 1, 0, 0}},
    .factor = 1.0 / 13.0,
    .bias = 0.0};

struct filter filter_gaussian_3 = {
    .name = "gaussian_3",
    .filterWidth = 3,
    .filterHeight = 3,
    .matrix = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}},
    .factor = 1.0 / 16.0,
    .bias = 0.0};

struct filter filter_gaussian_5 = {
    .name = "gaussian_5",
    .filterWidth = 5,
    .filterHeight = 5,
    .matrix = {
        {1, 4, 6, 4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1, 4, 6, 4, 1}},
    .factor = 1.0 / 256.0,
    .bias = 0.0};

struct filter filter_motion_blur = {
    .name = "motion_blur",
    .filterWidth = 9,
    .filterHeight = 9,
    .matrix = {
        {1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1}},
    .factor = 1.0 / 9.0,
    .bias = 0.0};

struct filter filter_find_edges_horizontal = {
    .name = "find_edges_horizontal",
    .filterWidth = 5,
    .filterHeight = 5,
    .matrix = {
        {0, 0, -1, 0, 0},
        {0, 0, -1, 0, 0},
        {0, 0, 2, 0, 0},
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0}},
    .factor = 1.0,
    .bias = 0.0};

struct filter filter_find_edges_vertical = {
    .name = "find_edges_vertical",
    .filterWidth = 5,
    .filterHeight = 5,
    .matrix = {
        {0, 0, -1, 0, 0},
        {0, 0, -1, 0, 0},
        {0, 0, 4, 0, 0},
        {0, 0, -1, 0, 0},
        {0, 0, -1, 0, 0}},
    .factor = 1.0,
    .bias = 0.0};

struct filter filter_find_edges_45_degrees = {
    .name = "find_edges_45_degrees",
    .filterWidth = 5,
    .filterHeight = 5,
    .matrix = {
        {-1, 0, 0, 0, 0},
        {0, -2, 0, 0, 0},
        {0, 0, 6, 0, 0},
        {0, 0, 0, -2, 0},
        {0, 0, 0, 0, -1}},
    .factor = 1.0,
    .bias = 0.0};

struct filter filter_find_edges_all_directions = {
    .name = "find_edges_all_directions",
    .filterWidth = 3,
    .filterHeight = 3,
    .matrix = {
        {-1, -1, -1},
        {-1, 8, -1},
        {-1, -1, -1}},
    .factor = 1.0,
    .bias = 0.0};

struct filter filter_sharpen = {
    .name = "sharpen",
    .filterWidth = 3,
    .filterHeight = 3,
    .matrix = {
        {-1, -1, -1},
        {-1, 9, -1},
        {-1, -1, -1}},
    .factor = 1.0,
    .bias = 0.0};

struct filter filter_sharpen_subtle = {
    .name = "sharpen_subtle",
    .filterWidth = 5,
    .filterHeight = 5,
    .matrix = {
        {-1, -1, -1, -1, -1},
        {-1, 2, 2, 2, -1},
        {-1, 2, 8, 2, -1},
        {-1, 2, 2, 2, -1},
        {-1, -1, -1, -1, -1}},
    .factor = 1.0 / 8.0,
    .bias = 0.0};

struct filter filter_edge_detection_exaggerated = {
    .name = "edge_detection_exaggerated",
    .filterWidth = 3,
    .filterHeight = 3,
    .matrix = {
        {1, 1, 1},
        {1, -7, 1},
        {1, 1, 1}},
    .factor = 1.0,
    .bias = 0.0};

struct filter filter_emboss_45_degrees = {
    .name = "emboss_45_degrees",
    .filterWidth = 3,
    .filterHeight = 3,
    .matrix = {
        {-1, -1, 0},
        {-1, 0, 1},
        {0, 1, 1}},
    .factor = 1.0,
    .bias = 128.0};

struct filter filter_emboss_exaggerated = {
    .name = "emboss_exaggerated",
    .filterWidth = 5,
    .filterHeight = 5,
    .matrix = {
        {-1, -1, -1, -1, 0},
        {-1, -1, -1, 0, 1},
        {-1, -1, 0, 1, 1},
        {-1, 0, 1, 1, 1},
        {0, 1, 1, 1, 1}},
    .factor = 1.0,
    .bias = 128.0};

void print_header(struct PPMHeader *header)
{
    printf("Signature: %c%c\n", header->signature[0], header->signature[1]);
    printf("Width: %u\n", header->width);
    printf("Height: %u\n", header->height);
    printf("Max_color: %u\n", header->max_color);
    printf("Data offset: %ld\n", header->data_offset);
}

int file_exists(char *filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}