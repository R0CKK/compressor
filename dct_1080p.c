#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROWS 1920
#define COLS 1080
#define BLOCK_SIZE 8

// 8x8 Quantization Matrix (standard JPEG-like)
int base_quantization_matrix[BLOCK_SIZE][BLOCK_SIZE] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
};

// Dynamically generate the 1920x1080 quantization matrix
void generate_quantization_matrix(int **quantization_matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            quantization_matrix[i][j] = base_quantization_matrix[i % BLOCK_SIZE][j % BLOCK_SIZE];
        }
    }
}

// Generate a random matrix of size 1920x1080
void generate_random_matrix(double **matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Random integers between 0 and 255 (for image pixel-like values)
            matrix[i][j] = rand() % 256;
        }
    }
}

void quantize(double **dct_matrix, int **quantization_matrix, double **result) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            result[i][j] = round(dct_matrix[i][j] / quantization_matrix[i][j]);
        }
    }
}

void dequantize(double **quantized_matrix, int **quantization_matrix, double **result) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            result[i][j] = quantized_matrix[i][j] * quantization_matrix[i][j];
        }
    }
}

// Verify the compression and decompression process
void verify(double **original_matrix, double **dequantized_matrix) {
    int differences = 0;
    double error_margin = 59.0; 
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (fabs(original_matrix[i][j] - dequantized_matrix[i][j]) > error_margin) {
                differences++;
            }
        }
    }
    if (differences == 0) {
        printf("Compression and decompression were successful with no significant differences.\n");
    } else {
        printf("There were %d differences found in the decompressed matrix.\n", differences);
    }
}

int main() {
    int **quantization_matrix = (int **)malloc(ROWS * sizeof(int *));
    double **dct_matrix = (double **)malloc(ROWS * sizeof(double *));
    double **quantized_matrix = (double **)malloc(ROWS * sizeof(double *));
    double **dequantized_matrix = (double **)malloc(ROWS * sizeof(double *));
    
    for (int i = 0; i < ROWS; i++) {
        quantization_matrix[i] = (int *)malloc(COLS * sizeof(int));
        dct_matrix[i] = (double *)malloc(COLS * sizeof(double));
        quantized_matrix[i] = (double *)malloc(COLS * sizeof(double));
        dequantized_matrix[i] = (double *)malloc(COLS * sizeof(double));
    }

    generate_quantization_matrix(quantization_matrix);

    generate_random_matrix(dct_matrix);

    quantize(dct_matrix, quantization_matrix, quantized_matrix);

    dequantize(quantized_matrix, quantization_matrix, dequantized_matrix);

    verify(dct_matrix, dequantized_matrix);

    for (int i = 0; i < ROWS; i++) {
        free(quantization_matrix[i]);
        free(dct_matrix[i]);
        free(quantized_matrix[i]);
        free(dequantized_matrix[i]);
    }
    
    free(quantization_matrix);
    free(dct_matrix);
    free(quantized_matrix);
    free(dequantized_matrix);

    return 0;
}
