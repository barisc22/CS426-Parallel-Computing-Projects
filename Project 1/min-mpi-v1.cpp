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

		const int generalNo = count / (sizeS - 1);
		int* hop = new int[generalNo];
		int* sizeHop = new int[1];
		int* sizeHop1 = new int[1];
		sizeHop[0] = generalNo;
		int remainder = count - (generalNo * (sizeS - 2));
		if (count > (generalNo * (sizeS - 1))) {
			sizeHop1[0] = remainder;
		}
		else {
			sizeHop1[0] = generalNo;
		}
		int* hop1 = new int[remainder];
		int dummyVar = 0;

        for (int i = 0; i < sizeS-2; i++) {
            for (int j = 0; j < generalNo; j++) {
                hop[j] = arr[i*generalNo + j];
            }
            MPI_Send(sizeHop, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
			MPI_Send(hop, generalNo, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        }
        MPI_Send(sizeHop1, 1, MPI_INT, sizeS-1, 0, MPI_COMM_WORLD);
		for (int i = (generalNo * (sizeS - 2)); i < count; i++) {
			hop1[dummyVar] = arr[i];
			dummyVar++;
		}
        MPI_Send(hop1, remainder, MPI_INT, sizeS-1, 0, MPI_COMM_WORLD);

		int* minArr = new int[sizeS-1];
		for (int i = 1; i < sizeS; i++) {
			int tempMin[1];
			MPI_Recv(tempMin, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			minArr[i-1] = tempMin[0];
		}
		int min = 10000;
		for (int i = 0; i < sizeS - 1; i++) {
			if (min > minArr[i]) {
				min = minArr[i];
			}
		}
		cout << "The global min is: " << min << endl;
		double end = MPI_Wtime();
		printf("Timers precision is %.20f seconds\n", MPI_Wtick());
		printf("Parallel Min Function took %.5f seconds\n", end - start);
    }
    else if (rank >= 1 ) {
        int arrNo[1];
        MPI_Recv(arrNo, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int sizeArr = arrNo[0];
        int* papo = new int[sizeArr];
        MPI_Recv(papo, sizeArr, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		int min = 1000000;
		for (int i = 0; i < sizeArr; i++) {
			if (min > papo[i]) {
				min = papo[i];
			}
		}
		cout << "The local min for the rank " << rank << " is " <<  min << endl;
		MPI_Send((void*)&min, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}