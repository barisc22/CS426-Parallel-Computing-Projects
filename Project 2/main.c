#include "helper.h"
float square(float number);
bool isBigger(float number);
float add(float number1, float number2);
void main(int argc, char* argv[])
{
  	
  	int size = atoi(argv[1]);
  	
  	//To calculate the probability and also for loops
  	float sizeF = size/2;

    MPI_Init(&argc, &argv);
	int rank;
	int procSize;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &procSize);
	double start, end;
	//For random
	srand(time(NULL)+rank);	
	float* arr = (float* )malloc(size*sizeof(float));
	if(rank == 0){
		start = MPI_Wtime();
		for(int i = 0; i < size; i++){
			//Generate random number
			float x = (float)rand()/(float)(RAND_MAX/1);
    		arr[i] = x;
    		//Normalize the number
    		arr[i] = arr[i] - 0.5f;
		}
	}
	//Calling Map function to take square of the array
    float *mapArr = (float* )malloc(size*sizeof(float));
    mapArr = MPI_Map_Func(arr, size, square);
    //Pisagor to calculate distance
    int newSize = size/2;
    float *newMapArr = (float* )malloc(newSize*sizeof(float));
    for(int i = 0; i < size; i+=2){
		if(rank == 0){
			newMapArr[i/2] = sqrt(mapArr[i] + mapArr[i+1]);
		}
	}
	//Calling Filter function to check if the distance is bigger than r return true if small
	float *filterArr = malloc(newSize*sizeof(float));
    filterArr = MPI_Filter_Func(newMapArr, newSize, isBigger);

    //Calling Fold functiom to add the hit values
    float initial = isBigger(newMapArr[0]);
	float foldArr = MPI_Fold_Func(filterArr, newSize, initial, add);

	//Calculate the probability 

	if(rank == 0){
		end = MPI_Wtime();
		printf("Size : %d for Parallel took %.5f miliseconds\n", size, (end - start)*1000);
		printf("Probability is : %f\n", foldArr/sizeF);
	}
	MPI_Finalize();
}

float square(float number) 
{ 
    return number * number;
} 

float add(float number1, float number2) 
{ 
    return number1 + number2;
} 

bool isBigger(float number){
	if(number <= 0.5f){
		return true;
	}
	else if(number > 0.5f){
		return false;
	}
}
