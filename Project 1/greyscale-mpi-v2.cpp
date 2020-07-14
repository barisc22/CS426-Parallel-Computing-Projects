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

	if (rank == 0) {
		char line[13];
		int flag = 0;
		int sum = 0;
		double start = MPI_Wtime();
		sum = calculateLines(sum);
		cout << "Number of Lines: " << sum << endl;
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
		cout << "Number of Rows: " << noOfRows << endl;
		const int linesToGo = noOfRows / sizeS;
		int arrSize1 = linesToGo * noOfRows;
		int* sizeHop = new int[1];
		sizeHop[0] = arrSize1;
		int square = sqrt(sizeS) * noOfRows;
		int sqrtN = sqrt(sizeS);
		for (int i = 1; i < sizeS; i++) {
			MPI_Send(sizeHop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			for (int p = 0; p < (noOfRows / sqrt(sizeS)); p++) {
				for (int j = (p * noOfRows) + (floor(i / sqrt(sizeS)) * (noOfRows / sqrt(sizeS)) * noOfRows) + ((noOfRows / sqrt(sizeS)) * (i % sqrtN)); j < (p * noOfRows) + (floor(i / sqrt(sizeS)) * (noOfRows / sqrt(sizeS)) * noOfRows) + (noOfRows / sqrt(sizeS) * (i % sqrtN)) + (noOfRows / sqrt(sizeS)); j++) {
					char* arrForGoGeneral = (char*)malloc(13);
					arrForGoGeneral = image[j];
					MPI_Send(arrForGoGeneral, 13, MPI_CHAR, i, 0, MPI_COMM_WORLD);
					delete(arrForGoGeneral);
				}
			}
		}
		int colorArrSize;
		float* colorArr = new float[sum * 3];
		int k = 0;
		for (int i = 0; i < sizeS; i++) {
			if (i == 0) {
				for (int p = 0; p < (noOfRows / sqrt(sizeS)); p++) {
					for (int j = (p * noOfRows) + (floor(i / sqrt(sizeS)) * (noOfRows / sqrt(sizeS)) * noOfRows) + ((noOfRows / sqrt(sizeS)) * (i % sqrtN)); j < (p * noOfRows) + (floor(i / sqrt(sizeS)) * (noOfRows / sqrt(sizeS)) * noOfRows) + (noOfRows / sqrt(sizeS) * (i % sqrtN)) + (noOfRows / sqrt(sizeS)); j++) {
						int rowFlag = 0;
						char* pt;
						pt = strtok(image[j], ",");
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
					colorArr[3*j] = red;
					colorArr[3*j + 1] = green;
					colorArr[3*j + 2] = blue;
					}
				}
			}
			else if (i < sizeS) {
				int t = 0;
				colorArrSize = arrSize1 * 3;
				float* tempColorArr = new float[colorArrSize];
				MPI_Recv(tempColorArr, colorArrSize, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				for (int p = 0; p < (noOfRows / sqrt(sizeS)); p++) {
					for (int j = (p * noOfRows) + (floor(i / sqrt(sizeS)) * (noOfRows / sqrt(sizeS)) * noOfRows) + ((noOfRows / sqrt(sizeS)) * (i % sqrtN)); j < (p * noOfRows) + (floor(i / sqrt(sizeS)) * (noOfRows / sqrt(sizeS)) * noOfRows) + (noOfRows / sqrt(sizeS) * (i % sqrtN)) + (noOfRows / sqrt(sizeS)); j++) {
						colorArr[3 * j] = tempColorArr[t];
						colorArr[3 * j + 1] = tempColorArr[t + 1];
						colorArr[3 * j + 2] = tempColorArr[t + 2];
						t += 3;
					}
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
		printf("Parallel Image ConversionV2 took %.5f seconds\n", end - start);
	}
	else if (rank  > 0) {
		int arrNo[1];
		MPI_Recv(arrNo, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		const int sizeArr = arrNo[0];
		
		char** papo = (char**)malloc(sizeArr * sizeof(char*));
		char* pap;
		for (int i = 0; i < sizeArr; i++) {
			papo[i] = (char*)malloc(13);
			char* pap = (char*)malloc(13);
			MPI_Recv(pap, 13, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int k = 0; k < 13; k++) {
				papo[i][k] = pap[k];
			}
			delete(pap);
		}
		
		int sizeForColors = 3 * sizeArr;
		float* arrForColors = new float[sizeForColors];
		for (int i = 0; i < sizeArr; i++) {
			int rowFlag = 0;
			char* pt;
			pt = strtok(papo[i], ",");
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
			arrForColors[3*i] = red;
			arrForColors[3*i + 1] = green;
			arrForColors[3*i + 2] = blue;
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