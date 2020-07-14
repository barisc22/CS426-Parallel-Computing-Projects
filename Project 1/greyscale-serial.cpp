#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <mpi.h>
using namespace std;
int calculateLines(int& sum);
int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	char line[20];
	int flag = 0;
	int noOfRows;

	int sum = 0;
	double start = MPI_Wtime();

	sum = calculateLines(sum);
	FILE* file = fopen("image.txt", "r");
	FILE* fileToWrite = fopen("output.txt", "w");

	while (fgets(line, sizeof(line), file) != NULL) {
		if (flag == 0) {
			noOfRows = atoi(line);
			flag = true;
			flag++;
		}
		else {
			int rowFlag = 0;
			char* pt;
			pt = strtok(line, ",");
			float red = 0;
			float green = 0;
			float blue = 0;
			while (pt != NULL) {
				if (rowFlag == 0) {
					red = atoi(pt);
					red = red * 0.21;
				}
				else if (rowFlag == 1) {
					green = atoi(pt);
					green = green * 0.71;
				}
				else if (rowFlag == 2) {
					blue = atoi(pt);
					blue = blue * 0.07;
				}
				pt = strtok(NULL, ",");
				rowFlag++;
			}
			fprintf(fileToWrite, "%g,%g,%g\n", red, green, blue);
			flag++;
			delete(pt);
		}
	}

	fclose(file);
	double end = MPI_Wtime();
	printf("Our timers precision is %.20f seconds\n", MPI_Wtick());
	printf("Serial Image Conversion took %.5f seconds\n", end - start);
	MPI_Finalize();
}

int calculateLines(int& sum) {
	FILE* file = fopen("image.txt", "r");
	char line[20];

	while (fgets(line, sizeof(line), file) != NULL) {
		sum++;
	}
	fclose(file);

	return sum;
}