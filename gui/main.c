/*******************************************************************************************
 *
 *   LayoutName v1.0.0 - Tool Description
 *
 *   LICENSE: Propietary License
 *
 *   Copyright (c) 2022 raylib technologies. All Rights Reserved.
 *
 *   Unauthorized copying of this file, via any medium is strictly prohibited
 *   This project is proprietary and confidential unless the owner allows
 *   usage in any other form by expresely written permission.
 *
 **********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <unistd.h>

static void ButtonActionFilter();

#define BLUR_3 0
#define BLUR_5 1
#define GAUSSIAN_3 2
#define GAUSSIAN_5 3
#define MOTION_BLUR 4
#define FIND_EDGES_HORIZONTAL 5
#define FIND_EDGES_VERTICAL 6
#define FIND_EDGES_45_DEGREES 7
#define FIND_EDGES_ALL_DIRECTIONS 8
#define SHARPEN 9
#define SHARPEN_SUBTLE 10
#define EDGE_DETECTION_EXAGGERATED 11
#define EMBOSS_45_DEGREES 12
#define EMBOSS_EXAGGERATED 13
#define MAX_FILTER 14

char *GetFilterName(int filterID)
{
    switch (filterID)
    {
    case BLUR_3:
        return "blur_3";
    case BLUR_5:
        return "blur_5";
    case GAUSSIAN_3:
        return "gaussian_3";
    case GAUSSIAN_5:
        return "gaussian_5";
    case MOTION_BLUR:
        return "motion_blur";
    case FIND_EDGES_HORIZONTAL:
        return "find_edges_horizontal";
    case FIND_EDGES_VERTICAL:
        return "find_edges_vertical";
    case FIND_EDGES_45_DEGREES:
        return "find_edges_45_degrees";
    case FIND_EDGES_ALL_DIRECTIONS:
        return "find_edges_all_directions";
    case SHARPEN:
        return "sharpen";
    case SHARPEN_SUBTLE:
        return "sharpen_subtle";
    case EDGE_DETECTION_EXAGGERATED:
        return "edge_detection_exaggerated";
    case EMBOSS_45_DEGREES:
        return "emboss_45_degrees";
    case EMBOSS_EXAGGERATED:
        return "emboss_exaggerated";
    default:
        return "unknown_filter";
    }
}

Texture2D ppm_input_texture, ppm_output_texture;

struct __attribute__((packed)) PPMData
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

Texture2D LoadPPM(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to open file: %s", filename);
        // exit(EXIT_FAILURE);
        return LoadTexture("");
    }

    // Read PPM header
    char format[3];
    int width, height, maxColor;
    fscanf(file, "%2s %d %d %d", format, &width, &height, &maxColor);

    if (strcmp(format, "P6") != 0 || maxColor != 255)
    {
        TraceLog(LOG_ERROR, "Invalid PPM file format: %s", filename);
        fclose(file);
        // exit(EXIT_FAILURE);
        return LoadTexture("");
    }

    // Read binary data
    size_t data_size = width * height * sizeof(struct PPMData);
    struct PPMData *data = (struct PPMData *)malloc(data_size);
    fread(data, data_size, 1, file);

    fclose(file);

    for (int i = 0; i < width * height; i++)
    {
        unsigned char aux = data[i].r;
        data[i].r = data[i].g;
        data[i].g = aux;
        unsigned char aux2 = data[i].g;
        data[i].g = data[i].b;
        data[i].b = aux2;
    }

    // Convert PPM data to Raylib Image format
    Image image = {
        .data = data,
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8};

    return LoadTextureFromImage(image);
}

int main()
{
    int windowWidth = 640;
    int windowHeight = 350;

    int new_windowWidth = 640;
    int new_windowHeight = 350;

    double zoom = new_windowWidth / windowWidth;

    InitWindow(windowWidth, windowHeight, "layout_name");

    int text_size = GuiGetStyle(DEFAULT, TEXT_SIZE);

    int ListViewFilterEditMode = false;
    int ListViewFilterActive = BLUR_3;
    bool TextBoxInputFileEditMode = false;
    char TextBoxInputFileText[128] = "default.ppm";
    bool TextBoxOutputFileEditMode = false;
    char TextBoxOutputFileText[128] = "default_out.ppm";

    char ListViewFilterOptions[1024] = "";

    for (int i = 0; i < MAX_FILTER; i++)
    {
        const char *filterName = GetFilterName(i);
        strcat(ListViewFilterOptions, filterName);
        if (i < MAX_FILTER - 1)
            strcat(ListViewFilterOptions, ";");
    }

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        // BeginMode2D(camera);
        // if (ListViewFilterEditMode)
        //     GuiLock();
        int size_Width = zoom * 120;
        int size_Height = zoom * 120;

        GuiLabel((Rectangle){48 * zoom, 48 * zoom, 120 * zoom, 24 * zoom}, "Input file:");
        if (GuiTextBox((Rectangle){48 * zoom, 96 * zoom, 120 * zoom, 24 * zoom}, TextBoxInputFileText, 128, TextBoxInputFileEditMode))
            TextBoxInputFileEditMode = !TextBoxInputFileEditMode;
        GuiLabel((Rectangle){192 * zoom, 48 * zoom, 120 * zoom, 24 * zoom}, "Output file:");
        if (GuiTextBox((Rectangle){192 * zoom, 96 * zoom, 120 * zoom, 24 * zoom}, TextBoxOutputFileText, 128, TextBoxOutputFileEditMode))
            TextBoxOutputFileEditMode = !TextBoxOutputFileEditMode;
        if (GuiButton((Rectangle){480 * zoom, 72 * zoom, 120 * zoom, 24 * zoom}, "Apply filter"))
            ButtonActionFilter(ListViewFilterActive, TextBoxInputFileText, TextBoxOutputFileText);

        GuiListView((Rectangle){336 * zoom, 48 * zoom, 120 * zoom, 72 * zoom}, ListViewFilterOptions, &ListViewFilterEditMode, &ListViewFilterActive);

        if (IsTextureReady(ppm_input_texture))
        {
            double factor = 264.0 / ppm_input_texture.width;
            DrawTexturePro(ppm_input_texture, (Rectangle){0, 0, ppm_input_texture.width, ppm_input_texture.height}, (Rectangle){48 * zoom, 136 * zoom, ppm_input_texture.width * factor * zoom, ppm_input_texture.height * factor * zoom}, (Vector2){0, 0}, 0.0f, WHITE);
        }
        if (IsTextureReady(ppm_output_texture))
        {
            double factor = 264.0 / ppm_output_texture.width;
            DrawTexturePro(ppm_output_texture, (Rectangle){0, 0, ppm_output_texture.width, ppm_output_texture.height}, (Rectangle){336 * zoom, 136 * zoom, ppm_output_texture.width * factor * zoom, ppm_output_texture.height * factor * zoom}, (Vector2){0, 0}, 0.0f, WHITE);
        }

        if (IsWindowResized())
        {
            new_windowWidth = GetScreenWidth();
            new_windowHeight = GetScreenHeight();
            zoom = new_windowWidth / (double)windowWidth;

            GuiSetStyle(DEFAULT, TEXT_SIZE, text_size * zoom);
            printf("Resize %d %d zoom %f\n", GetScreenWidth(), GetScreenHeight(), zoom);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

static void ButtonActionFilter(int filterID, char *inputFile, char *outputfile)
{
    char *filter_name = GetFilterName(filterID);
    // char aux_input_file[128];
    // char aux_output_file[128];
    // sprintf(aux_input_file, "../%s", inputFile);
    // sprintf(aux_output_file, "../%s", outputfile);
    printf("Filter: %s InputFile: %s OutputFile: %s\n", filter_name, inputFile, outputfile);

    ppm_input_texture = LoadPPM(inputFile);

    char command[1024];
    sprintf(command, "./filter.sh %s %s %s", filter_name, inputFile, outputfile);
    printf("Command: %s\n", command);
    int result = system(command);
    if (result != 0)
    {
        printf("Error: appling filter %d\n", result);
    }

    ppm_output_texture = LoadPPM(outputfile);
}
