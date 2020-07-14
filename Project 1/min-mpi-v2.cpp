#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream> 
#include <stddef.h>
using namespace std;

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	int rank;
	int sizeS;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &sizeS);

	if (rank == 0) {
		ifstream inFile;
		int* arr;
		int count = 0;
		int x;
		double start = MPI_Wtime();
		inFile.open("input.txt");
		if (!inFile) {
			cerr << "Unable to open file input.txt";
			exit(1);
		}
		while (inFile >> x) {
			count = count + 1;
		}
		arr = new int[count];
		int i = 0;
		inFile.close();
		inFile.open("input.txt");
		while (inFile >> x) {
			arr[i] = x;
			i++;
		}
		inFile.close();

		int generalNo = count / sizeS;

		int* sizeHop = new int[1];
		int* sizeHop1 = new int[1];

		int* hop = new int[generalNo];

		sizeHop[0] = generalNo;

		int remainder = count - (generalNo * (sizeS - 1));
		if (count > (generalNo * (sizeS - 1))) {
			sizeHop1[0] = remainder;
		}
		else {
			sizeHop1[0] = generalNo;
		}
		int* hop1 = new int[remainder];
		int dummyVar = 0;
		int counter = 0;
		for (int i = 1; i < sizeS - 1; i++) {
			for (int j = 0; j < generalNo; j++) {
				hop[j] = arr[i * generalNo + j];
				counter++;
			}
			MPI_Send(sizeHop, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(hop, generalNo, MPI_INT, i, 0, MPI_COMM_WORLD);
		}

		MPI_Send(sizeHop1, 1, MPI_INT, sizeS - 1, 0, MPI_COMM_WORLD);
		for (int i = (generalNo * (sizeS - 1)); i < count; i++) {
			hop1[dummyVar] = arr[i];
			dummyVar++;
		}
		MPI_Send(hop1, remainder, MPI_INT, sizeS - 1, 0, MPI_COMM_WORLD);
		
		int localMin = 1000000;
		for (int i = 0; i < generalNo; i++) {
			if (localMin > arr[i]) {
				localMin = arr[i];
			}
		}
		cout << "Local Min is for rank " << rank << " is: " << localMin << endl;

		int min;
		MPI_Allreduce(&localMin, &min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
		cout << "The min is: " << min << endl;

		double end = MPI_Wtime();
		printf("Our timers precision is %.20f seconds\n", MPI_Wtick());
		printf("Parallel Min FunctionV2 took %.5f seconds\n", end - start);
		 
	}
	else if (rank >= 1) {
		int arrNo[1];
		MPI_Recv(arrNo, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int sizeArr = arrNo[0];
		int* papo = new int[sizeArr];
		MPI_Recv(papo, sizeArr, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		int localMin = 1000000;
		for (int i = 0; i < sizeArr; i++) {
			if (localMin > papo[i]) {
				localMin = papo[i];
			}
		}
		cout << "Local Min is for rank " << rank << " is: " << localMin << endl;

		int min;
		MPI_Allreduce(&localMin, &min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
		cout << "The min is: " << min << endl;
		
	}
	MPI_Finalize();
}	