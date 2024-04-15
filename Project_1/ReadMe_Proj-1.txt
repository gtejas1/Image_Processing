# Project 1

This program performs image resampling on a Portable Graymap (PGM) image using bilinear interpolation. The code is written in C and reads a PGM image file, resamples it based on a specified scaling factor, and writes the resampled image to a new PGM file.

## Usage
    Project_1.exe <input_pgm> <output_pgm> <scaling_factor>

    - `<input_pgm>`: Input PGM file path.
    - `<output_pgm>`: Output PGM file path for the resampled image.
    - `<scaling_factor>`: Positive float scaling factor for resampling.

## For this project the scaling_factor (t) values used are:
Upsampling    : 2.5
Downsampling  : 0.25