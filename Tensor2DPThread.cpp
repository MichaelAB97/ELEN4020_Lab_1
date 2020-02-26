#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <time.h> 
#include <chrono>

using namespace std;
using namespace std::chrono;

#define num_threads 2

int nxt_operation_index  = 0;

pthread_mutex_t updateIndexLock;

// Helps with access with some of the matrices, structure for the thread.
typedef struct{
    int operation_index, ID;
    int *matrixA, *matrixB, *matrixC; //matrices it will operate on
    int N; // matrix size
}ThreadDataIndex;

// This function dynamically generates an NxN 2D Matrix
int* GenerateMatrix(int N)
{
    int dimension = N*N;

    int seed = rand()%N;// % 10;
    srand(seed);

    int *matrix =  (int* )calloc(dimension, sizeof(int));
    int *element_ptr = matrix;

    for (int i = 0; i < dimension; ++i)
    {
        *element_ptr = rand()%N;// % 10 + 0;
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

//Memory Allocation
int* allocateMatrix(int N)
{
    int dimension = N*N;
    int* result = (int*)calloc(dimension, sizeof(int));
    return result;
}

//Pass this function to each thread
void *ElementMultiplier(void *arg)
{
    ThreadDataIndex* thread_data = (ThreadDataIndex*)arg;
    int N = thread_data->N;
    int Operations = N*N*N;
    int dimension = N*N;
    while(true)
    {
        int m = thread_data->operation_index;
        int i = m/dimension;
        int j = m%N;
        int k = (m/N)%N;
            
        int indexA[2] = {i, k}; 
        int indexB[2] = {k, j};
            
        int elementA = getElement(thread_data->matrixA, indexA, N);
        int elementB = getElement(thread_data->matrixB, indexB, N);
            
        int indexC[2] = {i, j};
        int* total = thread_data->matrixC+getElementPosition2D(indexC, N);
        *total+=(elementA*elementB);

        pthread_mutex_lock(&updateIndexLock);
        if(nxt_operation_index < Operations)
        {
            thread_data->operation_index = nxt_operation_index;
            ++nxt_operation_index;
        }else thread_data->operation_index = Operations;
        pthread_mutex_unlock(&updateIndexLock);

        if(thread_data->operation_index == Operations) break;
    }
    pthread_exit((void*)0);
} 

void run2DMultiplier(int*matrixA, int*matrixB, int*matrixC, int N)
{
    int rc;

    //create threads based on the number of threads
    pthread_t threads[num_threads];
    ThreadDataIndex threads_data[num_threads]; // Create a struct of threads

    // First operation for each thread will be defined in the for loop below
    //So if one of the threads finish earlier they will go onto the operation
    nxt_operation_index = (int)num_threads; //Updates the index for the threads so it know the next operation need


    //Initializing the information in the struct for each thread
    // Threads are sharing memory. They identified by their individual 
    //ID's but they still all access the same data in each of the matrices

    for(int i=0; i < num_threads; ++i)
    {
        
        threads_data[i].ID = i;
        threads_data[i].operation_index = i;
        threads_data[i].matrixA = matrixA;
        threads_data[i].matrixB = matrixB;
        threads_data[i].matrixC = matrixC;
        threads_data[i].N = N;

        //Creating thread, passing in the address and the function for multiplying the element
        //and the information for each thread.
        rc = pthread_create(&threads[i], NULL, ElementMultiplier, &threads_data[i]);
        //pthread_create - returns 1 if theres an error with creating the threads & 0 if not

        if(rc)
        {
            cout<< "ERROR creating thread."<<endl;
            exit(-1);
        }
    }
    //scheduling the joining of the threads.
    for(int j = 0; j< num_threads; j++) pthread_join(threads[j],NULL);
}

//Rank 2 Multiplication

int* rank2TensorMult(int *matrixA, int *matrixB, int N)
{
    int dimension = N*N;
    int* result = allocateMatrix(dimension);
    int operation_num = dimension*N;

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

    return result;
}

int main()
{
    int N = 2;
    int* matrixA = GenerateMatrix(N);
    int* matrixB = GenerateMatrix(N);
    int* matrixC = allocateMatrix(N);

    cout << "Matrix A" << endl;
    DisplayMatrix(matrixA, N);
    cout << "Matrix B" << endl;
    DisplayMatrix(matrixB, N);


    //Starting the steady clock
    std::chrono::time_point<std::chrono::steady_clock> startClock, endClock;
    startClock = std::chrono::steady_clock::now();

    run2DMultiplier(matrixA, matrixB, matrixC, N);

    //Pause the steady clock
    endClock = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = duration_cast<duration<double>>(endClock - startClock);

    cout << "Matrix C" << endl;
    DisplayMatrix(matrixC,N);
    cout << "Elapsed Time in Seconds: " << elapsedTime.count() << endl;

    return 0;
}