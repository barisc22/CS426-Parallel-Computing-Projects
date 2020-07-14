#include <omp.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

int main(int argc, char* argv[]) 
{
	//Reading the txt file, calculating the number of rows in the txt file before opening the arrays.
    int nthreads, tid;
    char* token;
    int number_of_threads = atoi(argv[1]);
    omp_set_num_threads(number_of_threads);
    int num_of_iterations = atoi(argv[2]);
    int print_flag = atoi(argv[3]);
    char* file_name = argv[4];
    int* sizes = (int*)malloc(3 * sizeof(int));
	FILE* file = fopen(file_name, "r");
    char line[50];
    int counter  = 0;
    int counter2 = 0;
    while (fgets(line, sizeof(line), file)) {
        if(counter == 0){
        	token = strtok(line, " ");
	      	while( token != NULL ) {
		      sizes[counter2] = atoi(token);
		      token = strtok(NULL, " ");
		      counter2++;
	        }
        }
        counter++;
    }
    fclose(file);

    //Creating the x vector.
    float* x = (float*)malloc(sizes[0] * sizeof(float));
    for(int i = 0; i < sizes[0]; i++){
   		x[i] = 1;
    } 

    //Reading the file again and putting the content into array string_arr.
	char** string_arr = malloc(counter * sizeof(char*));
	
    int* row_ptr = (int*)malloc(sizes[0] * sizeof(int));
    int* col_ind = (int*)malloc(counter * sizeof(int));
    float* values = (float*)malloc(counter * sizeof(int));
    int line_count = counter-1; 
	file = fopen(file_name, "r");
    int flag = 0;
    counter = 0;
    while (fgets(line, sizeof(line), file)) {
    	if(counter > 0){
	    	string_arr[counter-1] = malloc(sizeof(line) * sizeof(char));
	    	for(int i = 0; i < sizeof(line); i++)
	       		string_arr[counter-1][i] = line[i];
	       	
       	}
       	counter++;
    }
    fclose(file);

    //Partitioning the string_arr into rows, columns and numbers. 
    flag = 0;
    counter2 = 0;
    int* rows = (int*)malloc(counter * sizeof(int));
    int* columns = (int*)malloc(counter * sizeof(int));
    float* numbers = (float*)malloc(counter * sizeof(int));

    for(int i = 0; i < line_count; i++){
	   	token = strtok(string_arr[i], " ");
	  	while( token != NULL ) {
			if(counter2 == 0)
				rows[i] = atoi(token);
		    else if(counter2 == 1)
		      	columns[i] = atoi(token);
		    else if(counter2 == 2)
		      	numbers[i] = atof(token);
		    token = strtok(NULL, " ");
		    counter2++;
	    }
	    counter2 = 0;
    }
    int flag2 = 0;
    for(int i = 0; i < counter; i++){
        if(rows[i] == sizes[0]){
            flag2 = 1;
        }
    }
    //Rearranging the columns and numbers arrays into col_ind and values arrays.
    counter2 = 0;
    int old_counter = 1;
    for(int i = 0; i < sizes[0]; i++){
    	if(old_counter == counter){
    		row_ptr[i] = counter2 + 1;
    	}else if (flag2 == 0 && i == sizes[0]-1){
            row_ptr[i] = counter-2;
        }else{
    		row_ptr[i] = counter2;
    	}
    	old_counter = counter2;
    	for(int j = 0; j < counter; j++){
    		if(rows[j] == i+1){
    			col_ind[counter2] = columns[j];
                values[counter2] = numbers[j];
    			counter2++;
    		}
    	}

    }
    //Printing the all arrays content if desired.
    float sum = 0;
    float multiply = 0;
    float* temp = (float*)malloc(sizes[0] * sizeof(float));
    if(print_flag == 1){
        printf("\n------------------Printing Row Number + Row_Ptr + Column_Ind[Row_Ptr] + Values[Row_Ptr]------------------\n");
        for(int i = 0; i < sizes[0]; i++){
            printf("%d + ", i);
            printf(" %d + ", row_ptr[i]);
            printf(" %d + ", col_ind[row_ptr[i]]);
            printf(" %f\n", values[row_ptr[i]]);
        }
        printf("\n------------------Printing Column_Ind + Values------------------\n");
        for(int i = 0; i < line_count; i++){
            printf(" %d + ", col_ind[i]);
            printf(" %f\n", values[i]); 
        }
    }
    //Parallel part
    int k = 0;
    double start; 
    double end; 
    start = omp_get_wtime();
    #pragma omp parallel shared(temp, x) private(k, sum, multiply) 
    {
        for(k = 0; k < num_of_iterations; k++){
            sum = 0;
            int x_counter = 0;
            #pragma omp for private(sum, multiply) 
            for(int i = 0; i < sizes[0]; i++) {
                if(i == sizes[0] - 1){
                   for(int j = row_ptr[i]; j < line_count; j++){
                        multiply = values[j] * x[col_ind[x_counter]-1];
                        sum = sum + multiply;
                        x_counter++;
                        if(j == line_count -1)
                            temp[i] = sum;
                    }
                    sum = 0; 
                }
                else
                {
                    for(int j = row_ptr[i]; j < row_ptr[i+1]; j++){
                        multiply = values[j] * x[col_ind[x_counter]-1];
                        sum = sum + multiply;
                        x_counter++;
                        if(j == row_ptr[i + 1] -1)
                            temp[i] = sum;
                    }
                    sum = 0;
                }
            }

            #pragma omp barrier
            tid = omp_get_thread_num(); 
            if(print_flag == 1 && tid == 0){
                for(int i = 0; i < sizes[0]; i++){
                    printf("Iteration %d + %f\n", k, temp[i]);
                }
            }
            #pragma omp for
            for(int p = 0; p < sizes[0]; p++){
                x[p] = temp[p];
            }
        }
    }
    end = omp_get_wtime(); 
    printf("Thread count: %d, Iterations: %d,  Number of Rows: %d took %f seconds\n", number_of_threads, num_of_iterations, sizes[0], end - start);  
}