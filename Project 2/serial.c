#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
void main(int argc, char* argv[])
{
    clock_t start, end;
    double cpu_time_used;
    //For random
  	srand(time(NULL));
  	int size = atoi(argv[1]);
    start = clock();
	float arr[size];
    //To calculate the probability and also for loops
	float sizeF = size/2;
    //Generate random variables
    for(int i = 0; i < size; i++){
    	float x = (float)rand()/(float)(RAND_MAX/1);
    	arr[i] = x;
    }
    
    //Normalize them by subtracting 0.5 and taking square of them
    float *normArr = malloc(size*sizeof(float));
    for(int i = 0; i < size; i++){
    	normArr[i] = arr[i] - 0.5f;
        normArr[i] = normArr[i] * normArr[i];
    }
    //Pisagor to calculate distance and checking if the distance is bigger than r. Add one to hit if it is smaller
    float *pisArr = malloc((size/2)*sizeof(float));
    int hit = 0;
    for(int i = 0; i < size; i+=2){
    	pisArr[i] = sqrt(normArr[i] + normArr[i+1]);
    	if(pisArr[i] <= 0.5000000f){
    		hit++;
    	}
    }
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Size : %d for Serial took %f miliseconds to execute \n", size, cpu_time_used*1000); 
    //Calculate the probability 
    float probability = (hit / sizeF);
    printf("Probability: %f\n", probability);
}