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

// Function to compute the DCT of an 8x8 block
void dct(double input[BLOCK_SIZE][BLOCK_SIZE], double output[BLOCK_SIZE][BLOCK_SIZE]) {
    for (int u = 0; u < BLOCK_SIZE; u++) {
        for (int v = 0; v < BLOCK_SIZE; v++) {
            double sum = 0.0;
            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    sum += input[x][y] * cos(((2 * x + 1) * u * M_PI) / (2.0 * BLOCK_SIZE)) *
                                               cos(((2 * y + 1) * v * M_PI) / (2.0 * BLOCK_SIZE));
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
                    sum += cu * cv * input[u][v] * cos(((2 * x + 1) * u * M_PI) / (2.0 * BLOCK_SIZE)) *
                                                         cos(((2 * y + 1) * v * M_PI) / (2.0 * BLOCK_SIZE));
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

// Verify the compression and decompression process
void verify(double **original_matrix, double **dequantized_matrix) {
    int differences = 0;
    double error_margin = 1.0;  // Set an acceptable error margin due to rounding
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
    double error_margin = 1.0;  // Allowable error margin

    // Compare all elements in the block
    for (int x = 0; x < BLOCK_SIZE; x++) {
        for (int y = 0; y < BLOCK_SIZE; y++) {
            if (fabs(original[x][y] - reconstructed[x][y]) > error_margin) {
                differences++;
            }
        }
    }

    // If no differences found, verification is successful
    if (differences == 0) {
        return 1;  // Successful verification
    } else {
        printf("Verification failed: %d differences found.\n", differences);
        return 0;  // Verification failed
    }
}


// Main function
int main() {
    // Step 1: Dynamically allocate memory for the matrices
    int **quantization_matrix = (int **)malloc(ROWS * sizeof(int *));
    double **dct_matrix = (double **)malloc(ROWS * sizeof(double *));
    double **quantized_matrix = (double **)malloc(ROWS * sizeof(double *));
    double **dequantized_matrix = (double **)malloc(ROWS * sizeof(double *));
    double checker = 0;
    
    for (int i = 0; i < ROWS; i++) {
        quantization_matrix[i] = (int *)malloc(COLS * sizeof(int));
        dct_matrix[i] = (double *)malloc(COLS * sizeof(double));
        quantized_matrix[i] = (double *)malloc(COLS * sizeof(double));
        dequantized_matrix[i] = (double *)malloc(COLS * sizeof(double));
    }

    // Step 2: Generate the 1920x1080 quantization matrix
    generate_quantization_matrix(quantization_matrix);

    // Step 3: Generate a random 1920x1080 matrix (simulating DCT coefficients)
    generate_random_matrix(dct_matrix);

    // Step 4: Quantize the random matrix
    quantize(dct_matrix, quantization_matrix, quantized_matrix);

    // Step 5: Dequantize the quantized matrix
    dequantize(quantized_matrix, quantization_matrix, dequantized_matrix);

    // Step 6: Verify if the decompressed matrix matches the original matrix
    verify(dct_matrix, dequantized_matrix);

    // Step 7: Perform DCT and IDCT verification on each 8x8 block
    double dct_block[BLOCK_SIZE][BLOCK_SIZE], idct_block[BLOCK_SIZE][BLOCK_SIZE];

    for (int i = 0; i < ROWS; i += BLOCK_SIZE) {
        for (int j = 0; j < COLS; j += BLOCK_SIZE) {
            // Extract the 8x8 block
            for (int x = 0; x < BLOCK_SIZE; x++) {
                for (int y = 0; y < BLOCK_SIZE; y++) {
                    dct_block[x][y] = dct_matrix[i + x][j + y];
                }
            }

            // Perform DCT on the block
            dct(dct_block, idct_block);

            // Perform IDCT on the transformed block
            double reconstructed_block[BLOCK_SIZE][BLOCK_SIZE];
            idct(idct_block, reconstructed_block);

            // Verify DCT and IDCT
            checker += verify_dct_idct(dct_block, reconstructed_block);
        }

    }
    if (checker == (ROWS * COLS) / (BLOCK_SIZE * BLOCK_SIZE)) {
        printf("Compression and decompression were successful with no significant differences.\n");
    } else {
        double t = (ROWS * COLS) / (BLOCK_SIZE * BLOCK_SIZE) - checker;
        printf("There was a difference of %d pixels\n", (int)t);
    }
    

    // Free allocated memory
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
