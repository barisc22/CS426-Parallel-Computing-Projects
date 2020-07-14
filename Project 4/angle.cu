#include <stdio.h>
#include <iostream>
#include <string>
#include <time.h> 
#include <math.h>
#include <sys\timeb.h>
#include <ctime>

#define PI 3.1415926
using namespace std;

__host__ void outer_compute (int *arr1, int *arr2, int* out, float* sum1, float* sum2, int N, int blockSize);
__global__ void addKernel(int* d_arr1, int* d_arr2, int* d_out, float* d_sum1, float* d_sum2, int N, int blockSize);
__device__ int calculate(int val1, int val2);


__device__ int calculate(int val1, int val2)
{
	return val1*val2;
}

__global__ void addKernel(int* d_arr1, int* d_arr2, int* d_dot_out, float* d_sum1, float* d_sum2, int N, int blockSize)
{
	int i;
	d_dot_out[threadIdx.x] = 0;
	d_sum1[threadIdx.x] = 0;
	d_sum2[threadIdx.x] = 0;
	for(i = 0; i <= N/blockSize && (i*blockSize + threadIdx.x) < N; i++)
	{
		int val1 = d_arr1[i*blockSize + threadIdx.x];
		int val2 = d_arr2[i*blockSize + threadIdx.x];
		d_dot_out[threadIdx.x] += calculate(val1, val2);
		d_sum1[threadIdx.x] += (val1*val1);
		d_sum2[threadIdx.x] += (val2*val2);
	}
}


__host__ void outer_compute (int *arr1, int *arr2, int* dot_out, float* sum1, float* sum2, int N, int blockSize) {
	int *d_arr1, *d_arr2, *d_dot_out;
	float *d_sum1, *d_sum2;
	cudaMalloc((void **) &d_arr1, N*sizeof(int));
	cudaMalloc((void **) &d_arr2, N*sizeof(int));	

	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start);
	cudaMemcpy(d_arr1, arr1, N*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_arr2, arr2, N*sizeof(int), cudaMemcpyHostToDevice);
	cudaEventRecord(stop);
    cudaEventSynchronize(stop);
	float milliseconds = 0;
	cudaEventElapsedTime(&milliseconds, start, stop);
	printf("Time for the Host to Device transfer : %f ms\n", milliseconds);

	cudaMalloc((void **) &d_dot_out, blockSize*sizeof(int));
	cudaMalloc((void **) &d_sum1, blockSize*sizeof(float));
	cudaMalloc((void **) &d_sum2, blockSize*sizeof(float));

	dim3 dimGrid(1, 1);
	dim3 dimBlock(blockSize);

	//Compute
	cudaEventRecord(start);
	addKernel<<<dimGrid,dimBlock>>> (d_arr1, d_arr2, d_dot_out, d_sum1, d_sum2, N, blockSize);
	cudaThreadSynchronize();
	cudaEventRecord(stop);
    cudaEventSynchronize(stop);
	milliseconds = 0;
	cudaEventElapsedTime(&milliseconds, start, stop);
	printf("Time for the kernel execution : %f ms\n", milliseconds);
	
	cudaEventRecord(start);
	cudaMemcpy(dot_out, d_dot_out, blockSize*sizeof(int), cudaMemcpyDeviceToHost);
	cudaMemcpy(sum1, d_sum1, blockSize*sizeof(float), cudaMemcpyDeviceToHost);
	cudaMemcpy(sum2, d_sum2, blockSize*sizeof(float), cudaMemcpyDeviceToHost);
	cudaEventRecord(stop);
    cudaEventSynchronize(stop);
	milliseconds = 0;
	cudaEventElapsedTime(&milliseconds, start, stop);
	printf("Time for the Device to Host transfer : %f ms\n", milliseconds);
	cudaFree(d_arr1); 
	cudaFree(d_arr2); 
	cudaFree(d_dot_out);
	cudaFree(d_sum1);
	cudaFree(d_sum2);
}

int main(int argc, char *argv[])
{
	srand(time(0)); 
	int N, blockSize;
    N = atoi(argv[1]);
    blockSize = atoi(argv[2]);

    int* arr1 = (int*)malloc(N * sizeof(int));
	int* arr2 = (int*)malloc(N * sizeof(int));
	int* dot_out = (int*)malloc(blockSize * sizeof(int));
	float* sum1 = (float*)malloc(blockSize * sizeof(float));
	float* sum2 = (float*)malloc(blockSize * sizeof(float));

    printf("\n\nInfo\n-----------------\n");
    printf("Number of Elements: %d\n", N);
    printf("Number of threads per block: %d\n", blockSize);
    printf("Number of blocks will be created: %d\n", N/blockSize);

	printf("Time\n-----------------\n");
	clock_t begin = clock();
	if(argv[3] == NULL){
		for(int i = 0; i < N; i++){
	        arr1[i] = rand()%100;
	        arr2[i] = rand()%100; 
	    }
	}else{
	    char* filename = argv[3];
	    int i = 0;
	    char line[10];	 
	    FILE* fp = fopen(filename, "r");
	    if (fp == NULL){
	        printf("Could not open file %s",filename);
	        return 0;
	    }
	    while (fgets(line, sizeof(line), fp)){
	    	if(i <= N && i > 0){
	    		arr1[i-1]= atoi(line);
	    	}
	    	else if(i > N){
	    		arr2[(i-N)-1]= atoi(line);
	    	}
	        i++;
	    }
	    fclose(fp);
	}


    clock_t end = clock();
  	double elapsed_secs = double(end - begin);
  	printf("Time for the array generation : %f ms\n", elapsed_secs);

	begin = clock();
	int cpu_dot_total = 0;
	float cpu_vector_a = 0;
	float cpu_vector_b = 0;
	float cpu_sum = 0;
	for(int i = 0; i < N; i++){
		cpu_dot_total += arr1[i] * arr2[i];
		cpu_vector_a += arr1[i]*arr1[i];
		cpu_vector_b += arr2[i]*arr2[i];
	}
	cpu_vector_a = sqrt(cpu_vector_a);
	cpu_vector_b = sqrt(cpu_vector_b);
	cpu_sum = cpu_vector_a*cpu_vector_b;
    float cpu_angle = acos(cpu_dot_total/cpu_sum)*180/PI;
    end = clock();
  	elapsed_secs = double(end - begin);
  	printf("Time for the CPU function : %f ms\n", elapsed_secs);

    /*
    for(int i = 0; i < N; i++){
    	if(i%blockSize == 0){
    		printf("Here is the 0\n");
    	}
    	printf("Arr1 : %d\n", arr1[i]);
    }

    for(int i = 0; i < N; i++){
    	if(i%blockSize == 0){
    		printf("Here is the 0\n");
    	}
    	printf("Arr2 : %d\n", arr2[i]);
    }
*/

	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start);
    outer_compute(arr1, arr2, dot_out, sum1, sum2, N, blockSize);
    int dot_total = 0;
    float sum1_total = 0;
    float sum2_total = 0;
    float sum = 0;
    float angle = 0;
    for(int i = 0; i < blockSize; i++){
    	//printf("Hop1: %d\n", dot_out[i]);
    	dot_total += dot_out[i];
    	//printf("Hop2: %f\n", sum1[i]);
    	sum1_total += sum1[i];
    	//printf("Hop3: %f\n", sum2[i]);
    	sum2_total += sum2[i];
    }
    sum1_total = sqrt(sum1_total);
    sum2_total = sqrt(sum2_total);
    sum = sum1_total*sum2_total;
    angle = acos(dot_total/sum)*180/PI;
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
	float milliseconds = 0;
	cudaEventElapsedTime(&milliseconds, start, stop);
	printf("Total execution time for GPU : %f ms\n", milliseconds);
    printf("Results\n-----------------\n");
    printf("The CPU angle is: %.3f\n", cpu_angle);
    printf("The GPU angle is: %.3f", angle);
    return 0;
}

