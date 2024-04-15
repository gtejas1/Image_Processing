#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define min(x, y) ((x) < (y) ? (x) : (y))

int xdim;
int ydim;
unsigned char* image;

void ReadPGM(FILE*);
void WritePGM(FILE*);
void resampleImage(char* inputFilename, char* outputFilename, float scalingFactor);

int main(int argc, char** argv)
{
	if (argc != 4)
	{
		printf("Usage: MyProgram <input_ppm> <output_ppm> <scaling_factor>\n");
		printf("       <input_ppm>: PGM file \n");
		printf("       <output_ppm>: PGM file \n");
		printf("       <scaling_factor>: Positive float scaling factor\n");
		exit(0);
	}

	float scalingFactor = atof(argv[3]);

	// Perform image resampling with bilinear interpolation
	resampleImage(argv[1], argv[2], scalingFactor);

	return 0;
}

void resampleImage(char* inputFilename, char* outputFilename, float scalingFactor) {
	FILE* fp;
	FILE* fp1;
	int i, j;

	/* begin reading PGM.... */
	printf("Begin reading PGM.... \n");
	if ((fp = fopen(inputFilename, "rb")) == NULL)
	{
		printf("Read error...\n");
		exit(EXIT_FAILURE);
	}
	ReadPGM(fp);

	// Perform image resampling with bilinear interpolation
	int new_xdim = (int)(xdim * scalingFactor);
	int new_ydim = (int)(ydim * scalingFactor);
	unsigned char* resampled_image = (unsigned char*)malloc(sizeof(unsigned char) * new_xdim * new_ydim);

	for (j = 0; j < new_ydim; j++) {
		for (i = 0; i < new_xdim; i++) {
			float original_x = i / scalingFactor;
			float original_y = j / scalingFactor;

			int x0 = (int)floor(original_x);
			int y0 = (int)floor(original_y);
			int x1 = min(xdim - 1, x0 + 1);
			int y1 = min(ydim - 1, y0 + 1);

			float alpha = original_x - x0;
			float beta = original_y - y0;

			// Bilinear interpolation
			unsigned char value = (unsigned char)(
				(1 - alpha) * (1 - beta) * image[y0 * xdim + x0] +
				alpha * (1 - beta) * image[y0 * xdim + x1] +
				(1 - alpha) * beta * image[y1 * xdim + x0] +
				alpha * beta * image[y1 * xdim + x1]
				);

			resampled_image[j * new_xdim + i] = value;
		}
	}


	/* Begin writing PGM.... */
	printf("Begin writing PGM.... \n");
	if ((fp1 = fopen(outputFilename, "wb")) == NULL)
	{
		perror("Write error");
		exit(EXIT_FAILURE);
	}
	WritePGM(fp1, new_xdim,new_ydim,resampled_image);

	free(image);
	free(resampled_image);
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
