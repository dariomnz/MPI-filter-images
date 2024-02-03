#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "PPMtypes.h"

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
    if (strcmp(filter_name, "blur_3") == 0)
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

void root_read_header(char *file_name, struct PPMHeader *header)
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

void root_write_header(char *file_name, struct PPMHeader *header)
{
    FILE *file = fopen(file_name, "w");
    fprintf(file, "%c%c\n", header->signature[0], header->signature[1]);
    fprintf(file, "%u %u\n", header->width, header->height);
    fprintf(file, "%u\n", header->max_color);
    fclose(file);
}

void get_slice(struct PPMHeader *header, size_t *offset, size_t *size, size_t *rows_part, size_t *rows_offset)
{
    int rank, size_world;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size_world);
    (*rows_part) = header->height / size_world;
    if (rank == size_world - 1)
    {
        (*rows_part) += header->height % size_world;
    }
    size_t row_size = header->width * sizeof(struct PPMData);
    (*rows_offset) = ((header->height / size_world) * rank);
    (*offset) = (*rows_offset) * row_size + header->data_offset;
    (*size) = row_size * (*rows_part);
}

struct PPMData *PPMread(char *file_name, struct PPMHeader *header)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        root_read_header(file_name, header);
        print_header(header);
    }

    MPI_Bcast(header, sizeof(struct PPMHeader), MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);

    size_t offset, part_size, rows_part, rows_offset;
    get_slice(header, &offset, &part_size, &rows_part, &rows_offset);

    struct PPMData *data = (struct PPMData *)malloc(part_size);
    MPI_File_read_at_all(file, offset, data, part_size, MPI_BYTE, MPI_STATUS_IGNORE);

    MPI_File_close(&file);
    return data;
}

void PPMwrite(char *file_name, struct PPMHeader *header, struct PPMData *data)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0)
    {
        root_write_header(file_name, header);
    }
    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, file_name, MPI_MODE_WRONLY, MPI_INFO_NULL, &file);

    size_t offset, part_size, rows_part, rows_offset;
    get_slice(header, &offset, &part_size, &rows_part, &rows_offset);

    MPI_File_write_at(file, offset, data, part_size, MPI_BYTE, MPI_STATUS_IGNORE);
    MPI_File_close(&file);
}

void PPMfilter(struct filter *filter, struct PPMHeader *header, struct PPMData **data)
{

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0)
        printf("Filter name: %s\n", filter->name);

    size_t offset, part_size, rows_part, rows_offset;
    get_slice(header, &offset, &part_size, &rows_part, &rows_offset);

    int size_extra_rows = filter->filterHeight / 2;
    size_t extra_row_size = size_extra_rows * header->width * sizeof(struct PPMData);

    struct PPMData *past_rows = (struct PPMData *)malloc(extra_row_size);
    struct PPMData *next_rows = (struct PPMData *)malloc(extra_row_size);

    int past_rank = (rank - 1 + size) % size;
    int next_rank = (rank + 1) % size;

    MPI_Sendrecv(&(*data)[0], extra_row_size, MPI_BYTE, past_rank, 0, next_rows, extra_row_size, MPI_BYTE, next_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Sendrecv(&(*data)[(rows_part - size_extra_rows) * header->width], extra_row_size, MPI_BYTE, next_rank, 0, past_rows, extra_row_size, MPI_BYTE, past_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    struct PPMData *result = (struct PPMData *)malloc(part_size);

    for (int x = 0; x < header->width; x++)
        for (int y = 0; y < rows_part; y++)
        {
            double red = 0.0, green = 0.0, blue = 0.0;

            for (int filterY = 0; filterY < filter->filterHeight; filterY++)
                for (int filterX = 0; filterX < filter->filterWidth; filterX++)
                {
                    int imageX = (x - (filterX - filter->filterWidth / 2)) % header->width;
                    int imageY = (y - (filterY - filter->filterHeight / 2));

                    if (imageY < 0)
                    {
                        imageY += size_extra_rows;
                        red += past_rows[imageY * header->width + imageX].r * filter->matrix[filterY][filterX];
                        green += past_rows[imageY * header->width + imageX].g * filter->matrix[filterY][filterX];
                        blue += past_rows[imageY * header->width + imageX].b * filter->matrix[filterY][filterX];
                    }
                    else if (imageY >= rows_part)
                    {
                        imageY = imageY - rows_part;
                        red += next_rows[imageY * header->width + imageX].r * filter->matrix[filterY][filterX];
                        green += next_rows[imageY * header->width + imageX].g * filter->matrix[filterY][filterX];
                        blue += next_rows[imageY * header->width + imageX].b * filter->matrix[filterY][filterX];
                    }
                    else
                    {
                        red += (*data)[imageY * header->width + imageX].r * filter->matrix[filterY][filterX];
                        green += (*data)[imageY * header->width + imageX].g * filter->matrix[filterY][filterX];
                        blue += (*data)[imageY * header->width + imageX].b * filter->matrix[filterY][filterX];
                    }
                }

            result[y * header->width + x].r = min(max((filter->factor * red + filter->bias), 0), 255);
            result[y * header->width + x].g = min(max((filter->factor * green + filter->bias), 0), 255);
            result[y * header->width + x].b = min(max((filter->factor * blue + filter->bias), 0), 255);
        }

    struct PPMData *temp = *data;
    *data = result;
    free(temp);
    free(past_rows);
    free(next_rows);
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start, end;
    start = MPI_Wtime();
    if (argc < 3)
    {
        if (rank == 0)
            usage(argv[0]);
        MPI_Finalize();
        exit(0);
    }

    struct filter *filter = select_filter(argv);

    char *file_name = argv[2];
    char *file_name_out = argv[3];
    if (rank == 0)
    {
        printf(" Input file: %s output file: %s\n", file_name, file_name_out);
    }
    if (file_exists(file_name) == 0)
    {
        if (rank == 0)
            fprintf(stderr, "Error: File %s not exists", file_name);
        MPI_Finalize();
        exit(0);
    }

    struct PPMHeader header;
    memset(&header, 0, sizeof(struct PPMHeader));

    struct PPMData *data = PPMread(file_name, &header);
    PPMfilter(filter, &header, &data);

    PPMwrite(file_name_out, &header, data);

    free(data);
    end = MPI_Wtime();
    if (rank == 0)
        printf("Processing time %d proc: %f (ms)\n", size, (end - start) * 1000);

    MPI_Finalize();
    return 0;
}
