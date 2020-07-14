#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream> 
#include <stddef.h>
#include <math.h>
using namespace std;
int calculateLines(int& sum);

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int rank;
	int sizeS;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &sizeS);
	char line[13];
	int flag = 0;
	double start = MPI_Wtime();
	int sum = 0;
	sum = calculateLines(sum);
	int noOfRows = sqrt((sum - 1));
	char** image = (char**)malloc(sum * sizeof(char*));

	FILE* file = fopen("image.txt", "r");
	FILE* fileToWrite = fopen("output.txt", "w");
	while (fgets(line, sizeof(line), file) != NULL) {
		if (flag > 0) {
			image[flag - 1] = (char*)malloc(13);
			for (int i = 0; i < sizeof(line); i++) {
				image[flag - 1][i] = line[i];
			}
		}
		flag++;
	}
	fclose(file);

	const int linesToGoGeneral = noOfRows / sizeS;
	int arrSize1 = linesToGoGeneral * noOfRows;
	char** arrForGoGeneral = (char**)malloc(arrSize1 * sizeof(char*));
	int* sizeHop = new int[1];
	int* sizeHop1 = new int[1];
	sizeHop[0] = arrSize1;
	int remainder = noOfRows - (linesToGoGeneral * (sizeS - 1));
	if (noOfRows > (linesToGoGeneral * (sizeS - 1))) {
		remainder = remainder * noOfRows;
	}
	else {
		remainder = linesToGoGeneral * noOfRows;
	}
	sizeHop1[0] = remainder;
	char** arrForGoRemainder = (char**)malloc(remainder * sizeof(char*));
	int dummyVar = 0;
	int colorArrSize;
	float* colorArr = new float[sum * 3];
	if (rank == 0) {
		int k = 0;
		for (int i = 0; i < sizeS; i++) {
			if (i == 0) {
				for (k = 0; k < arrSize1; k++) {
					int rowFlag = 0;
					char* pt;
					pt = strtok(image[k], ",");
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
					colorArr[3 * k] = red;
					colorArr[3 * k + 1] = green;
					colorArr[3 * k + 2] = blue;
				}
			}
			else if (i < sizeS - 1) {
				colorArrSize = arrSize1 * 3;
				float* tempColorArr = new float[colorArrSize];
				MPI_Recv(tempColorArr, colorArrSize, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				for (int i = 0; i < 3 * arrSize1; i += 3) {
					colorArr[3 * k] = tempColorArr[i];
					colorArr[3 * k + 1] = tempColorArr[i + 1];
					colorArr[3 * k + 2] = tempColorArr[i + 2];
					k++;
				}
				delete(tempColorArr);
			}

			else if (i == sizeS - 1) {
				colorArrSize = remainder * 3;
				float* tempColorArr = new float[colorArrSize];
				MPI_Recv(tempColorArr, colorArrSize, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				for (int i = 0; i < 3 * remainder; i += 3) {
					colorArr[3 * k] = tempColorArr[i];
					colorArr[3 * k + 1] = tempColorArr[i + 1];
					colorArr[3 * k + 2] = tempColorArr[i + 2];
					k++;
				}
				delete(tempColorArr);
			}
		}
			for (int i = 0; i < sum - 1; i++) {
				float red = 0;
				float green = 0;
				float blue = 0;
				red = colorArr[3 * i];
				green = colorArr[3 * i + 1];
				blue = colorArr[3 * i + 2];
				fprintf(fileToWrite, "%g,%g,%g\n", red, green, blue);
			}
			double end = MPI_Wtime();
			printf("Our timers precision is %.20f seconds\n", MPI_Wtick());
			printf("Parallel Image ConversionV3 took %.5f seconds\n", end - start);
	}

	else if (rank > 0) 
	{
		int arrNo[1];
		int sizeArr;
		int imageSizeArr;
		if (rank < (sizeS - 1)) {
			sizeArr = arrSize1;
			imageSizeArr = arrSize1;
		}
		else if(rank == (sizeS - 1)){
			sizeArr = remainder;
			imageSizeArr = arrSize1;
		}

		int sizeForColors = 3 * sizeArr;
		float* arrForColors = new float[sizeForColors];
		for (int i = 0; i < sizeArr; i++) 
		{
			int rowFlag = 0;
			char* pt;
			pt = strtok(image[(rank* imageSizeArr)+i], ",");
			float red = 0;
			float green = 0;
			float blue = 0; 
			while (pt != NULL) 
			{
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
			arrForColors[3 * i] = red;
			arrForColors[3 * i + 1] = green;
			arrForColors[3 * i + 2] = blue;
		}
		MPI_Send(arrForColors, sizeForColors, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}

	MPI_Finalize();
}
int calculateLines(int& sum) {
	FILE* file = fopen("image.txt", "r");
	char line[13];

	while (fgets(line, sizeof(line), file) != NULL) {	
		sum++;
	}
	fclose(file);

	return sum;
}