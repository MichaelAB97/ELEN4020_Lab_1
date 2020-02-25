// Rank 2 Tensor Contraction using PThreads
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <chrono> 
#define THREADS_NUM 2

using namespace std;
using namespace std::chrono;

// This function Dynamically generates an NxN 2D Matrix to store the NxN elements
int* GenerateMatrix(int N)
{
    int dimension = N*N;
    int seed = rand() % 10;
    srand(seed);

    int *matrix =  (int* )calloc(dimension, sizeof(int));
    int *element_ptr = matrix;

    for (int i = 0; i < dimension; ++i)
    {
        *element_ptr = rand() % 20 + 1;
        element_ptr++;
    }
    
    return matrix;
}

//This function return the position of the 1D array element
int getElementPosition2D(int coords[2], int N)
{
    int row = coords[0];
    int col = coords[1];
    int position = (row*N) + col;
    return position;
}

//This function returns the 2D element
int getElement(int* matrix_ptr, int index[2], int N)
{
    return *(matrix_ptr + getElementPosition2D(index,N));
}

//Displays the matrix
void DisplayMatrix(int* matrix, int N)
{
    int dimension = N*N;

    for (int i = 0; i < dimension; ++i)
    {
        if (i % N == 0)
        {
            cout << endl;
        }
        cout << *matrix << " ";
        ++matrix;   
    }
    cout << endl;
}

//Alocating memeory to store the resultant matrix
int* allocateMatrix(int N)
{
    int dimension = N*N;
    int* result = (int*)calloc(dimension, sizeof(int));
    return result;
}

//Rank 2 Matrix Multiplication
int* rank2TensorMult(int *matrixA, int *matrixB, int N)
{   
    int dimension = N*N;
	int* result = allocateMatrix(dimension);
	int operation_num = dimension*N;

    if(result == NULL) 
    {
        cerr << "\nError: Matrix Muliplication has failed " << endl; //Error message
    }
    else
    {
        for(int m=0; m < operation_num; m++)
	    {
		    int i = m/dimension;
		    int j = m%N;
            int k = (m/N)%N;
			
    		int indexA[2] = {i, k}; 
	    	int indexB[2] = {k, j};
			
		    int elementA = getElement(matrixA, indexA, N);
		    int elementB = getElement(matrixB, indexB, N);
			
    		int indexC[2] = {i, j};
	    	int* total = result+getElementPosition2D(indexC, N);
		    *total+=(elementA*elementB);	
	    }
    }
	return result;
}



int main()
{
    int N = 3; // Size of the matrix
    int* matrixA = GenerateMatrix(N);
    int* matrixB = GenerateMatrix(N);

    cout << "Matrix A";
    DisplayMatrix(matrixA, N);
    cout << "\nMatrix B";
    DisplayMatrix(matrixB, N);

    //Starting the steady clock
    std::chrono::time_point<std::chrono::steady_clock> startClockPThreads, endClockPThreads;
    startClockPThreads = std::chrono::steady_clock::now();

    int* matrixC = rank2TensorMult(matrixA, matrixB, N);

    endClockPThreads = std::chrono::steady_clock::now();
    std::chrono::duration<double> PThreadsTime = duration_cast<duration<double>>(endClockPThreads - startClockPThreads);

    cout << "\nMatrix C: " << endl;
    DisplayMatrix(matrixC,N);
    cout << "PThreads Elapsed Time in Seconds: " << PThreadsTime.count() << endl;

    return 0;
}