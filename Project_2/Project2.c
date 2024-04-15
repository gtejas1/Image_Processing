#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

int xdim, ydim, new_xdim, new_ydim;
unsigned char* image;

// Function prototypes for image processing, filtering, and saving
void ReadPGM(FILE*);
void WritePGM(FILE*, int, int, unsigned char*);

void gaussian_filter(unsigned char* image, unsigned char* filtered_image, int width, int height, int neighborhood_size, float sigma);
void bilateral_filter(unsigned char* image, unsigned char* filtered_image, int width, int height, int neighborhood_size, float sigma_space, float sigma_color);

int main() {

    // Read the input image
    FILE* fp = fopen("D:\\ImageProcessing_Spring2024\\proj_2\\test-img.pgm", "rb");
    if (!fp) {
        printf("Error opening input file\n");
        exit(EXIT_FAILURE);
    }
    ReadPGM(fp);
    fclose(fp);

    // Apply Gaussian filtering
    unsigned char* gaussian_filtered_image = (unsigned char*)malloc(sizeof(unsigned char) * xdim * ydim);
    gaussian_filter(image, gaussian_filtered_image, xdim, ydim, 5, 2);

    // Apply bilateral filtering
    unsigned char* bilateral_filtered_image = (unsigned char*)malloc(sizeof(unsigned char) * xdim * ydim);
    bilateral_filter(image, bilateral_filtered_image, xdim, ydim, 5, 2, 5);

    // Write the filtered images to separate files
    FILE* gaussian_output_fp = fopen("D:\\ImageProcessing_Spring2024\\proj_2\\gaussian_filtered.pgm", "wb");
    FILE* bilateral_output_fp = fopen("D:\\ImageProcessing_Spring2024\\proj_2\\bilateral_filtered.pgm", "wb");

    WritePGM(gaussian_output_fp, xdim, ydim, gaussian_filtered_image);
    WritePGM(bilateral_output_fp, xdim, ydim, bilateral_filtered_image);

    // Close the output files
    fclose(gaussian_output_fp);
    fclose(bilateral_output_fp);

    // Free memory
    free(image);
    free(gaussian_filtered_image);
    free(bilateral_filtered_image);

    return 0;
}


// Implementation of Gaussian filtering
void gaussian_filter(unsigned char* image, unsigned char* filtered_image, int width, int height, int neighborhood_size, float sigma) {
    int radius = (neighborhood_size - 1) / 2;
    float sum, weight;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            sum = 0;
            float totalWeight = 0;

            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    int nx = min(max(x + kx, 0), width - 1);
                    int ny = min(max(y + ky, 0), height - 1);

                    float distance = sqrt((kx * kx + ky * ky) / (2.0 * sigma * sigma));
                    weight = exp(-distance);

                    sum += image[ny * width + nx] * weight;
                    totalWeight += weight;
                }
            }
            filtered_image[y * width + x] = sum / totalWeight; 
        }
    }
}

// Implementation of Bilateral filtering
void bilateral_filter(unsigned char* image, unsigned char* filtered_image, int width, int height, int neighborhood_size, float sigma_space, float sigma_color) {
    int radius = (neighborhood_size - 1) / 2;
    float sum, weight;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            sum = 0;
            float totalWeight = 0;

            for (int ky = -radius; ky <= radius; ky++) {
                for (int kx = -radius; kx <= radius; kx++) {
                    int nx = min(max(x + kx, 0), width - 1);
                    int ny = min(max(y + ky, 0), height - 1);

                    float color_difference = fabs(image[y * width + x] - image[ny * width + nx]);
                    float spatial_distance = sqrt((kx * kx + ky * ky) / (2.0 * sigma_space * sigma_space));
                    float intensity_weight = exp(-color_difference / (2.0 * sigma_color * sigma_color));
                    float spatial_weight = exp(-spatial_distance);

                    weight = intensity_weight * spatial_weight;

                    sum += image[ny * width + nx] * weight;
                    totalWeight += weight;
                }
            }
            filtered_image[y * width + x] = sum / totalWeight; 
        }
    }
}
void ReadPGM(FILE* fp)
{
    int c;
    int i, j;
    int val;
    unsigned char* line;
    char buf[1024];

    while ((c = fgetc(fp)) == '#')
        fgets(buf, 1024, fp);
    ungetc(c, fp);
    if (fscanf(fp, "P%d\n", &c) != 1)
    {
        printf("Read error ....");
        exit(EXIT_FAILURE);
    }
    if (c != 5 && c != 2)
    {
        printf("Read error ....");
        exit(EXIT_FAILURE);
    }

    if (c == 5)
    {
        while ((c = fgetc(fp)) == '#')
            fgets(buf, 1024, fp);
        ungetc(c, fp);
        if (fscanf(fp, "%d%d", &xdim, &ydim) != 2)
        {
            printf("Failed to read width/height\n");
            exit(EXIT_FAILURE);
        }
        printf("Width=%d, Height=%d\n", xdim, ydim);

        image = (unsigned char*)malloc(sizeof(unsigned char) * xdim * ydim);
        getc(fp);

        line = (unsigned char*)malloc(sizeof(unsigned char) * xdim);
        for (j = 0; j < ydim; j++)
        {
            fread(line, 1, xdim, fp);
            for (i = 0; i < xdim; i++)
            {
                image[j * xdim + i] = line[i];
            }
        }
        free(line);
    }
    else if (c == 2)
    {
        while ((c = fgetc(fp)) == '#')
            fgets(buf, 1024, fp);
        ungetc(c, fp);
        if (fscanf(fp, "%d%d", &xdim, &ydim) != 2)
        {
            printf("Failed to read width/height\n");
            exit(EXIT_FAILURE);
        }
        printf("Width=%d, Height=%d\n", xdim, ydim);

        image = (unsigned char*)malloc(sizeof(unsigned char) * xdim * ydim);
        getc(fp);

        for (j = 0; j < ydim; j++)
            for (i = 0; i < xdim; i++)
            {
                fscanf(fp, "%d", &val);
                image[j * xdim + i] = val;
            }
    }

    fclose(fp);
}

void WritePGM(FILE* fp, int new_xdim, int new_ydim, unsigned char* resampled_image)
{
    int i, j;

    fprintf(fp, "P5\n%d %d\n%d\n", new_xdim, new_ydim, 255);
    for (j = 0; j < new_ydim; j++)
        for (i = 0; i < new_xdim; i++)
        {
            fputc(resampled_image[j * new_xdim + i], fp);
        }

    fclose(fp);
}