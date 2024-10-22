#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROWS 1920
#define COLS 1080
#define BLOCK_SIZE 8
#define m_pi   3.14159265358979323846264338327950288

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

// Function to compute the DCT of an 8x8 block
void dct(double input[BLOCK_SIZE][BLOCK_SIZE], double output[BLOCK_SIZE][BLOCK_SIZE]) {
    for (int u = 0; u < BLOCK_SIZE; u++) {
        for (int v = 0; v < BLOCK_SIZE; v++) {
            double sum = 0.0;
            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    sum += input[x][y] * cos(((2 * x + 1) * u * m_pi) / (2.0 * BLOCK_SIZE)) *
                                               cos(((2 * y + 1) * v * m_pi) / (2.0 * BLOCK_SIZE));
                }
            }
            double cu = (u == 0) ? 1.0 / sqrt(2.0) : 1.0;
            double cv = (v == 0) ? 1.0 / sqrt(2.0) : 1.0;
            output[u][v] = 0.25 * cu * cv * sum;
        }
    }
}

// Function to compute the IDCT of an 8x8 block
void idct(double input[BLOCK_SIZE][BLOCK_SIZE], double output[BLOCK_SIZE][BLOCK_SIZE]) {
    for (int x = 0; x < BLOCK_SIZE; x++) {
        for (int y = 0; y < BLOCK_SIZE; y++) {
            double sum = 0.0;
            for (int u = 0; u < BLOCK_SIZE; u++) {
                for (int v = 0; v < BLOCK_SIZE; v++) {
                    double cu = (u == 0) ? 1.0 / sqrt(2.0) : 1.0;
                    double cv = (v == 0) ? 1.0 / sqrt(2.0) : 1.0;
                    sum += cu * cv * input[u][v] * cos(((2 * x + 1) * u * m_pi) / (2.0 * BLOCK_SIZE)) *
                                                         cos(((2 * y + 1) * v * m_pi) / (2.0 * BLOCK_SIZE));
                }
            }
            output[x][y] = 0.25 * sum;
        }
    }
}

// Dynamically generate the 1920x1080 quantization matrix
void generate_quantization_matrix(int **quantization_matrix) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            // Tile the 8x8 matrix across 1920x1080
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

// Quantize using the generated 1920x1080 quantization matrix
void quantize(double **dct_matrix, int **quantization_matrix, double **result) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            result[i][j] = round(dct_matrix[i][j] / quantization_matrix[i][j]);
        }
    }
}

// Dequantize the matrix
void dequantize(double **quantized_matrix, int **quantization_matrix, double **result) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            result[i][j] = quantized_matrix[i][j] * quantization_matrix[i][j];
        }
    }
}

void verify(double **original_matrix, double **dequantized_matrix) {
    int differences = 0;
    double error_margin = 10.0;  // Set an acceptable error margin due to rounding
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

int verify_dct_idct(double original[BLOCK_SIZE][BLOCK_SIZE], double reconstructed[BLOCK_SIZE][BLOCK_SIZE]) {
    int differences = 0;
    double error_margin = 10.0;  // Allowable error margin

    for (int x = 0; x < BLOCK_SIZE; x++) {
        for (int y = 0; y < BLOCK_SIZE; y++) {
            if (fabs(original[x][y] - reconstructed[x][y]) > error_margin) {
                differences++;
            }
        }
    }

    // If no differences found, verification is successful
    if (differences == 0) {
        return 1;  
    } else {
        printf("Verification failed: %d differences found.\n", differences);
        return 0; 
    }
}
void print_matrix(FILE *file,double **matrix, int rows, int cols) {
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file,"%5.1f \t", matrix[i][j]);
        }
        fprintf(file,"\n");
    }
}

int main() {
    int **quantization_matrix = (int **)malloc(ROWS * sizeof(int *));
    double **dct_matrix = (double **)malloc(ROWS * sizeof(double *));
    double **quantized_matrix = (double **)malloc(ROWS * sizeof(double *));
    double **dequantized_matrix = (double **)malloc(ROWS * sizeof(double *));
    double checker = 0;
    FILE *file1,*file2,*file3;
    file1 = fopen("quantized.txt", "w");
    file2 = fopen("raw.txt","w");
    
    if (file1 == NULL) {
        printf("Error opening file!\n");
        return 1; 
    }
    if (file2 == NULL) {
        printf("Error opening file!\n");
        return 1; 
    }
    file3 = fopen("generated.txt","w");
    
    if (file1 == NULL) {
        printf("Error opening file!\n");
        return 1; 
    }


    for (int i = 0; i < ROWS; i++) {
        quantization_matrix[i] = (int *)malloc(COLS * sizeof(int));
        dct_matrix[i] = (double *)malloc(COLS * sizeof(double));
        quantized_matrix[i] = (double *)malloc(COLS * sizeof(double));
        dequantized_matrix[i] = (double *)malloc(COLS * sizeof(double));
    }

    generate_quantization_matrix(quantization_matrix);

    generate_random_matrix(dct_matrix);
    print_matrix(file2,dct_matrix,ROWS,COLS);

    double dct_block[BLOCK_SIZE][BLOCK_SIZE];
    for (int i = 0; i < ROWS; i += BLOCK_SIZE) {
        for (int j = 0; j < COLS; j += BLOCK_SIZE) {
            // Extract the 8x8 block
            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    dct_block[x][y] = dct_matrix[i + x][j + y];
                }
            }

            double dct_output[BLOCK_SIZE][BLOCK_SIZE];
            dct(dct_block, dct_output);

            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    dct_matrix[i + x][j + y] = dct_output[x][y];
                }
            }
        }
    }
    

    quantize(dct_matrix, quantization_matrix, quantized_matrix);
    print_matrix(file1,quantized_matrix,ROWS,COLS);

    dequantize(quantized_matrix, quantization_matrix, dequantized_matrix);

    double idct_block[BLOCK_SIZE][BLOCK_SIZE];
    for (int i = 0; i < ROWS; i += BLOCK_SIZE) {
        for (int j = 0; j < COLS; j += BLOCK_SIZE) {
            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    idct_block[x][y] = dequantized_matrix[i + x][j + y];
                }
            }
            double idct_output[BLOCK_SIZE][BLOCK_SIZE];
            idct(idct_block, idct_output);

            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    dct_matrix[i + x][j + y] = idct_output[x][y];
                }
            }
        }
    }
    print_matrix(file3,dct_matrix,ROWS,COLS);

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
