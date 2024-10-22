#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file1, *file2;
    file1 = fopen("raw.txt", "r");
    file2 = fopen("generated.txt", "r");

    // Check if the files opened successfully
    if (file1 == NULL || file2 == NULL) {
        printf("Error opening one of the files!\n");
        return 1; // Exit with an error code
    }

    double buffer1[256];
    double buffer2[256];
    double diff;
    double sum = 0;
    int count = 0;

    // Read integers from both files until the end of one of them
    while (fscanf(file1, "%d", &buffer1[count]) == 1 && 
           fscanf(file2, "%d", &buffer2[count]) == 1 && 
           count < 256) {
        // Calculate the difference for the current pair of integers
        diff = buffer1[count] - buffer2[count];
        sum += diff; // Accumulate the difference
        count++; // Increment the count of valid integers read
    }

    // Close the files
    fclose(file1);
    fclose(file2);

    // Calculate the average difference if any integers were read
    if (count > 0) {
        double avg_diff = (double)sum / count; // Use double for precision
        printf("Average difference is %.2f\n", avg_diff);
    } else {
        printf("No integers were read from the files.\n");
    }

    return 0; // Exit successfully
}
