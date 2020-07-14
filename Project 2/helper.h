#define _CRT_SECURE_NO_DEPRECATE
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
typedef int bool;
enum { false, true };
float *MPI_Map_Func(float *arr, int size, float (*func)(float)){

	int rank;
	int procSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &procSize);

	if(rank == 0){  
        int line = size / procSize;
        int remainder;
		int *sizeGeneral = (int *)malloc(1*sizeof(int));
		int *sizeRemainder = (int *)malloc(1*sizeof(int));
		sizeGeneral[0] = line;
		if (size > (line * procSize)) {
			remainder = size - (line * (procSize-1));
		}
		else {
			remainder = line;
		}
		sizeRemainder[0] = remainder;
		//To send the arrays to first processes
		float* arrForGeneral = arrForGeneral = (float*)malloc(line*sizeof(float));
		//To send the array to the last process
		float* arrForRemainder = arrForGeneral = (float*)malloc(remainder*sizeof(float));
		//Divide the array, send them to first processes
		for (int i = 1; i < procSize - 1; i++) {
			MPI_Send(sizeGeneral, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			for (int j = 0; j < line; j++) {
				arrForGeneral[j] = arr[(i * line) + j];
			}
			MPI_Send(arrForGeneral, line, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		}	
		//Send the last array
		MPI_Send(sizeRemainder, 1, MPI_INT, procSize - 1, 0, MPI_COMM_WORLD);
		int dummyVar = 0;
		for (int i = (line * (procSize - 1)); i < size; i++) {
			arrForRemainder[dummyVar] = arr[i];
			dummyVar++;
		}
		MPI_Send(arrForRemainder, remainder, MPI_FLOAT, procSize - 1, 0, MPI_COMM_WORLD);

		//Master also does some work
		float* newArray = (float* )malloc(size*sizeof(float));
		for(int i = 0; i < line; i++){
				newArray[i] = (*func)(newArray[i]);
		}
		//Combine the arrays that comes from processes
		float* cameArray1;
		float* cameArray2 = (float* )malloc(remainder*sizeof(float));
		int counter = line;
		for (int i = 1; i < procSize-1; i++) {
			cameArray1 = (float* )malloc(line*sizeof(float));
			MPI_Recv(cameArray1, line, MPI_FLOAT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(int j = 0; j < line; j++){
				newArray[counter] = cameArray1[j];
				counter++;
			}
			free(cameArray1);
		}
		//Add the last array
		int lastProc = procSize - 1;
		MPI_Recv(cameArray2, remainder, MPI_FLOAT, lastProc, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(int j = 0; j < remainder; j++){
			newArray[counter] = cameArray2[j];
			counter++;
		}
		free(cameArray2);
		return (newArray);
    }
    else if(rank < procSize){
    	//Size of the array
    	int* arrNo = (int* )malloc(1*sizeof(int));
		MPI_Recv(arrNo, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int sizeArr = arrNo[0];
		//Array
		float* procArray = (float* )malloc(sizeArr*sizeof(float));
		MPI_Recv(procArray, sizeArr, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(int i = 0; i < sizeArr; i++){
    		//Take square
			procArray[i] = (*func)(procArray[i]);
		}
		//Send the array to Master
		MPI_Send(procArray, sizeArr, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
		free(procArray);
		free(arrNo);
    }
}

float MPI_Fold_Func(float *arr, int size, float initial_value, float (*func)(float, float)){
	int rank;
	int procSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &procSize);
	if(rank == 0){  
        int line = size / procSize;
        int remainder;
        
		int *sizeGeneral = malloc(1*sizeof(int));
		int *sizeRemainder = malloc(1*sizeof(int));
		sizeGeneral[0] = line;
		if (size > (line * procSize)) {
			remainder = size - (line * (procSize-1));
		}
		else {
			remainder = line;
		}
		sizeRemainder[0] = remainder;

		float* arrForGeneral = arrForGeneral = malloc(line*sizeof(float));
		float* arrForRemainder = arrForGeneral = malloc(remainder*sizeof(float));
		for (int i = 1; i < procSize - 1; i++) {
			MPI_Send(sizeGeneral, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			for (int j = 0; j < line; j++) {
				arrForGeneral[j] = arr[(i * line) + j];
			}
			MPI_Send(arrForGeneral, line, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		}	

		MPI_Send(sizeRemainder, 1, MPI_INT, procSize - 1, 0, MPI_COMM_WORLD);
		int dummyVar = 0;
		for (int i = (line * (procSize - 1)); i < size; i++) {
			arrForRemainder[dummyVar] = arr[i];
			dummyVar++;
		}
		MPI_Send(arrForRemainder, remainder, MPI_FLOAT, procSize - 1, 0, MPI_COMM_WORLD);

		//This time there is one number comes from all the processes, so create an array with the number of processes
		float* newArray = malloc(size*sizeof(float));
		newArray[0] = arr[0];
		for(int i = 1; i < line; i++){
			//Calculate the first number from array
			newArray[0] = (*func)(newArray[0], arr[i]);
		}
		//Calculate the other numbers. Since all processes return one value, do not calculate the last process separately
		float cameArray1;
		for (int i = 1; i < procSize; i++) {
			MPI_Recv(&cameArray1, line, MPI_FLOAT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			newArray[i] = cameArray1;
		}
		//Calculate the hit count from the numbers that came from all processes
		float hit;
		for(int i = 0; i < procSize; i++){
			hit = (*func)(hit, newArray[i]);
		}
		return hit;
    }
    else if(rank < procSize){
    	int* arrNo = (int* )malloc(1*sizeof(int));
		MPI_Recv(arrNo, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int sizeArr = arrNo[0];

		float* procArray = malloc(sizeArr*sizeof(float));
		MPI_Recv(procArray, sizeArr, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		float initial = procArray[0];
		for(int i = 1; i < sizeArr; i++){
			//Calculate the total values as Master
			initial = (*func)(initial, procArray[i]);
		}
		//Send them to Master
		MPI_Send(&initial, 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
		free(procArray);
		free(arrNo);
    }
}
//This is the same as the Map function except return values. There could be minor changes. 
float *MPI_Filter_Func(float *arr, int size, bool (*func)(float)){
	int rank;
	int procSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &procSize);
	if(rank == 0){  
        int line = size / procSize;
        int remainder;
        
		int *sizeGeneral = malloc(1*sizeof(int));
		int *sizeRemainder = malloc(1*sizeof(int));
		sizeGeneral[0] = line;
		if (size > (line * procSize)) {
			remainder = size - (line * (procSize-1));
		}
		else {
			remainder = line;
		}
		sizeRemainder[0] = remainder;
		float* arrForGeneral = arrForGeneral = malloc(line*sizeof(float));
		float* arrForRemainder = arrForGeneral = malloc(remainder*sizeof(float));
		for (int i = 1; i < procSize - 1; i++) {
			MPI_Send(sizeGeneral, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			for (int j = 0; j < line; j++) {
				arrForGeneral[j] = arr[(i * line) + j];
			}
			MPI_Send(arrForGeneral, line, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
		}	


		MPI_Send(sizeRemainder, 1, MPI_INT, procSize - 1, 0, MPI_COMM_WORLD);
		int dummyVar = 0;
		for (int i = (line * (procSize - 1)); i < size; i++) {
			arrForRemainder[dummyVar] = arr[i];
			dummyVar++;
		}
		MPI_Send(arrForRemainder, remainder, MPI_FLOAT, procSize - 1, 0, MPI_COMM_WORLD);


		float* newArray = malloc(size*sizeof(float));
		for(int i = 0; i < line; i++){
				newArray[i] = (*func)(arr[i]);
		}

		float* cameArray1;
		float* cameArray2 = malloc(remainder*sizeof(float));
		int counter = line;
		for (int i = 1; i < procSize-1; i++) {
			cameArray1 = malloc(line*sizeof(float));
			MPI_Recv(cameArray1, line, MPI_FLOAT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for(int j = 0; j < line; j++){
				newArray[counter] = cameArray1[j];
				counter++;
			}
			free(cameArray1);
		}
		int lastProc = procSize - 1;
		MPI_Recv(cameArray2, remainder, MPI_FLOAT, lastProc, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(int j = 0; j < remainder; j++){
			newArray[counter] = cameArray2[j];
			counter++;
		}

		free(cameArray2);
		return (newArray);
    }
    else if(rank < procSize){
    	int* arrNo = (int* )malloc(1*sizeof(int));
		MPI_Recv(arrNo, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int sizeArr = arrNo[0];

		float* procArray = malloc(sizeArr*sizeof(float));
		MPI_Recv(procArray, sizeArr, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for(int i = 0; i < sizeArr; i++){
			procArray[i] = (*func)(procArray[i]);
		}
		MPI_Send(procArray, sizeArr, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
		free(procArray);
		free(arrNo);
    }
}
