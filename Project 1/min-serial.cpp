// Serials.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <time.h>
#include <sys\timeb.h> 

using namespace std;    
int main(int argc, char* argv[])
{
	ifstream inFile;
	int* arr;
	int sum = 0;
	int diff;
	int x;
	struct timeb start, end;
	ftime(&start);
	inFile.open("input.txt");
	if (!inFile) {
		cerr << "Unable to open file datafile.txt";
		exit(1);   // call system to stop
	}
	while (inFile >> x) {
		sum = sum + 1;
	}
	arr = new int[sum];
	int i = 0;
	inFile.close();
	inFile.open("input.txt");
	while (inFile >> x) {
		arr[i] = x;
		i++;
	}
	inFile.close();
	int min = 1000000;
	for (int j = 0; j < sum; j++) {
		if (min > arr[j]) {
			min = arr[j];
		}
	}
	ftime(&end);
	diff = (int)(1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
	printf("The global min is: %d\n", min);
	printf("\nOperation took %u milliseconds\n", diff);
}