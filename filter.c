#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
// #include <mpi.h>
#include <time.h>
#include <pthread.h>
#include "PPMtypes.h"

#define NUM_PTHREADS 6

struct FilterParams
{
    struct filter *filter;
    struct PPMHeader *header;
    size_t rows_part_offset;
    size_t rows_part;
    struct PPMData *data_in;
    struct PPMData *data_out;
};

void usage(char *programName)
{
    printf("Usage: %s <filter_type> <input_file> <output_file>\n", programName);
    printf("Available Filter Types:\n");
    printf("  - blur_3\n");
    printf("  - blur_5\n");
    printf("  - gaussian_3\n");
    printf("  - gaussian_5\n");
    printf("  - motion_blur\n");
    printf("  - find_edges_horizontal\n");
    printf("  - find_edges_vertical\n");
    printf("  - find_edges_45_degrees\n");
    printf("  - find_edges_all_directions\n");
    printf("  - sharpen\n");
    printf("  - sharpen_subtle\n");
    printf("  - edge_detection_exaggerated\n");
    printf("  - emboss_45_degrees\n");
    printf("  - emboss_exaggerated\n");
}

struct filter *select_filter(char **argv)
{
    struct filter *filter;
    char *filter_name = argv[1];
    if (strcmp(filter_name, "no_filter") == 0)
        filter = &filter_no_filter;
    else if (strcmp(filter_name, "blur_3") == 0)
        filter = &filter_blur_3;
    else if (strcmp(filter_name, "blur_5") == 0)
        filter = &filter_blur_5;
    else if (strcmp(filter_name, "gaussian_3") == 0)
        filter = &filter_gaussian_3;
    else if (strcmp(filter_name, "gaussian_5") == 0)
        filter = &filter_gaussian_5;
    else if (strcmp(filter_name, "motion_blur") == 0)
        filter = &filter_motion_blur;
    else if (strcmp(filter_name, "find_edges_horizontal") == 0)
        filter = &filter_find_edges_horizontal;
    else if (strcmp(filter_name, "find_edges_vertical") == 0)
        filter = &filter_find_edges_vertical;
    else if (strcmp(filter_name, "find_edges_45_degrees") == 0)
        filter = &filter_find_edges_45_degrees;
    else if (strcmp(filter_name, "find_edges_all_directions") == 0)
        filter = &filter_find_edges_all_directions;
    else if (strcmp(filter_name, "sharpen") == 0)
        filter = &filter_sharpen;
    else if (strcmp(filter_name, "sharpen_subtle") == 0)
        filter = &filter_sharpen_subtle;
    else if (strcmp(filter_name, "edge_detection_exaggerated") == 0)
        filter = &filter_edge_detection_exaggerated;
    else if (strcmp(filter_name, "emboss_45_degrees") == 0)
        filter = &filter_emboss_45_degrees;
    else if (strcmp(filter_name, "emboss_exaggerated") == 0)
        filter = &filter_emboss_exaggerated;
    else
    {
        printf("Invalid filter type: %s\n", filter_name);
        usage(argv[0]);
        exit(1);
    }
    return filter;
}

void read_header(char *file_name, struct PPMHeader *header)
{
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
        exit(1);
    char buffer[1024];
    fgets(buffer, 1024, file);
    header->signature[0] = buffer[0];
    header->signature[1] = buffer[1];
    int lines = 0;
    do
    {
        fgets(buffer, 1024, file);
        if (buffer[0] == '#')
            continue;
        if (lines == 0)
        {
            sscanf(buffer, "%u %u", &header->width, &header->height);
        }
        else
        {
            sscanf(buffer, "%hu", &header->max_color);
        }
        lines++;
    } while (lines < 2);

    header->data_offset = ftell(file);
    fclose(file);
}

void write_header(char *file_name, struct PPMHeader *header)
{
    FILE *file = fopen(file_name, "w");
    fprintf(file, "%c%c\n", header->signature[0], header->signature[1]);
    fprintf(file, "%u %u\n", header->width, header->height);
    fprintf(file, "%u\n", header->max_color);
    fclose(file);
}

struct PPMData *PPMread(char *file_name, struct PPMHeader *header)
{
    read_header(file_name, header);
    print_header(header);

    int file = open(file_name, O_RDONLY, NULL);
    if (file < 0)
        exit(1);

    size_t data_size = header->height * header->width * sizeof(struct PPMData);
    struct PPMData *data = (struct PPMData *)malloc(data_size);
    pread(file, data, data_size, header->data_offset);
    close(file);
    return data;
}

void PPMwrite(char *file_name, struct PPMHeader *header, struct PPMData *data)
{
    print_header(header);
    write_header(file_name, header);

    int file = open(file_name, O_WRONLY | O_CREAT | O_APPEND, NULL);
    if (file < 0)
        exit(1);

    size_t data_size = header->height * header->width * sizeof(struct PPMData);
    pwrite(file, data, data_size, header->data_offset);
    close(file);
}

void PPMfilter(void *params_ptr)
{
    struct FilterParams *params = (struct FilterParams *)params_ptr;
    struct filter *filter = params->filter;
    struct PPMHeader *header = params->header;
    size_t rows_part_offset = params->rows_part_offset;
    size_t rows_part = params->rows_part;
    struct PPMData *data_in = params->data_in;
    struct PPMData *data_out = params->data_out;

    printf("Filter name: %s, from %ld, to %ld\n", filter->name, rows_part_offset, rows_part_offset + rows_part);

    for (int x = 0; x < header->width; x++)
        for (int y = rows_part_offset; y < rows_part_offset + rows_part; y++)
        {
            double red = 0.0, green = 0.0, blue = 0.0;

            for (int filterY = 0; filterY < filter->filterHeight; filterY++)
                for (int filterX = 0; filterX < filter->filterWidth; filterX++)
                {
                    int imageX = (x - (filterX - filter->filterWidth / 2)) % header->width;
                    int imageY = (y - (filterY - filter->filterHeight / 2)) % header->height;

                    red += data_in[imageY * header->width + imageX].r * filter->matrix[filterY][filterX];
                    green += data_in[imageY * header->width + imageX].g * filter->matrix[filterY][filterX];
                    blue += data_in[imageY * header->width + imageX].b * filter->matrix[filterY][filterX];
                }

            data_out[y * header->width + x].r = min(max((filter->factor * red + filter->bias), 0), 255);
            data_out[y * header->width + x].g = min(max((filter->factor * green + filter->bias), 0), 255);
            data_out[y * header->width + x].b = min(max((filter->factor * blue + filter->bias), 0), 255);
        }
}

int main(int argc, char **argv)
{

    struct filter *filter = select_filter(argv);

    char *file_name = argv[2];
    char *file_name_out = argv[3];

    printf(" Input file: %s output file: %s\n", file_name, file_name_out);

    if (file_exists(file_name) == 0)
    {
        fprintf(stderr, "Error: File %s not exists", file_name);
    }

    struct PPMHeader header;
    memset(&header, 0, sizeof(struct PPMHeader));

    struct PPMData *data_in;
    struct PPMData *data_out;

    data_in = PPMread(file_name, &header);
    size_t data_size = header.height * header.width * sizeof(struct PPMData);
    data_out = malloc(data_size);
    size_t rows_part = header.height / NUM_PTHREADS;

    struct FilterParams filterParams[NUM_PTHREADS];

    size_t rows_part_pthread = header.height / NUM_PTHREADS;
    size_t rows_part_pthread_res = header.height % NUM_PTHREADS;

    struct timespec start, end;
    double elapsed_time;
    clock_gettime(CLOCK_MONOTONIC, &start);

    pthread_t thread_id[NUM_PTHREADS];
    int result;
    for (size_t i = 0; i < NUM_PTHREADS; i++)
    {
        filterParams[i].filter = filter;
        filterParams[i].header = &header;
        filterParams[i].rows_part_offset = rows_part_pthread * i;
        filterParams[i].rows_part = rows_part;
        if (i == NUM_PTHREADS - 1)
            filterParams[i].rows_part += rows_part_pthread_res;
        filterParams[i].data_in = data_in;
        filterParams[i].data_out = data_out;

        result = pthread_create(&thread_id[i], NULL, (void *)PPMfilter, &filterParams[i]);
        if (result != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (size_t i = 0; i < NUM_PTHREADS; i++)
    {
        result = pthread_join(thread_id[i], NULL);
        if (result != 0)
        {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Processing time: %f seg\n", elapsed_time);

    PPMwrite(file_name_out, &header, data_out);

    free(data_in);
    free(data_out);
    return 0;
}
