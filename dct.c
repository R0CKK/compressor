#include <stdio.h>
#include <math.h>

#define N 8
#define m_PI   3.14159265358979323846264338327950288

// Quantization matrix
int quantization_matrix[N][N] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
};

// Input 8x8 image matrix (grayscale values)
double image[N][N] = {
    {52, 55, 61, 66, 70, 61, 64, 73},
    {63, 59, 55, 90, 109, 85, 69, 72},
    {62, 59, 68, 113, 144, 104, 66, 73},
    {63, 58, 71, 122, 154, 106, 70, 69},
    {67, 61, 68, 104, 126, 88, 68, 70},
    {79, 65, 60, 70, 77, 68, 58, 75},
    {85, 71, 64, 59, 55, 61, 65, 83},
    {87, 79, 69, 68, 65, 76, 78, 94}
};

// Apply 2D DCT
void dct_2d(double matrix[N][N], double result[N][N]) {
    int u, v, x, y;
    double cu, cv, sum;
    
    for (u = 0; u < N; u++) {
        for (v = 0; v < N; v++) {
            if (u == 0) cu = 1.0 / sqrt(N);
            else cu = sqrt(2.0) / sqrt(N);
            
            if (v == 0) cv = 1.0 / sqrt(N);
            else cv = sqrt(2.0) / sqrt(N);
            
            sum = 0.0;
            for (x = 0; x < N; x++) {
                for (y = 0; y < N; y++) {
                    sum += matrix[x][y] * cos((2 * x + 1) * u * m_PI / (2.0 * N)) * 
                                          cos((2 * y + 1) * v * m_PI / (2.0 * N));
                }
            }
            result[u][v] = cu * cv * sum;
        }
    }
}

// Quantize DCT coefficients
void quantize(double dct_matrix[N][N], int quant_matrix[N][N], double result[N][N]) {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            result[i][j] = round(dct_matrix[i][j] / quant_matrix[i][j]);
        }
    }
}

// Dequantize DCT coefficients
void dequantize(double quantized_matrix[N][N], int quant_matrix[N][N], double result[N][N]) {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            result[i][j] = quantized_matrix[i][j] * quant_matrix[i][j];
        }
    }
}

// Apply inverse 2D DCT
void inverse_dct_2d(double matrix[N][N], double result[N][N]) {
    int x, y, u, v;
    double cu, cv, sum;
    
    for (x = 0; x < N; x++) {
        for (y = 0; y < N; y++) {
            sum = 0.0;
            for (u = 0; u < N; u++) {
                for (v = 0; v < N; v++) {
                    if (u == 0) cu = 1.0 / sqrt(N);
                    else cu = sqrt(2.0) / sqrt(N);
                    
                    if (v == 0) cv = 1.0 / sqrt(N);
                    else cv = sqrt(2.0) / sqrt(N);
                    
                    sum += cu * cv * matrix[u][v] * cos((2 * x + 1) * u * m_PI / (2.0 * N)) * 
                                                  cos((2 * y + 1) * v * m_PI / (2.0 * N));
                }
            }
            result[x][y] = round(sum);
        }
    }
}

int main() {
    double dct_matrix[N][N], quantized_matrix[N][N], dequantized_matrix[N][N], reconstructed_image[N][N];
    
    // Step 1: Apply 2D DCT
    dct_2d(image, dct_matrix);
    
    // Step 2: Quantize the DCT coefficients
    quantize(dct_matrix, quantization_matrix, quantized_matrix);
    
    // Step 3: Dequantize the DCT coefficients
    dequantize(quantized_matrix, quantization_matrix, dequantized_matrix);
    
    // Step 4: Apply inverse DCT
    inverse_dct_2d(dequantized_matrix, reconstructed_image);
    
    // Print the reconstructed image
    printf("Reconstructed Image:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%6.2f ", reconstructed_image[i][j]);
        }
        printf("\n");
    }

    return 0;
}
