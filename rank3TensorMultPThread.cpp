//Rank 3 Tensor Contraction
//-------------------------------------------------------------------------------//
//The elements needed for each (NxN) matrix are first stored in a 1D array.
//-------------------------------------------------------------------------------//

#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <time.h> 
#include <chrono>

using namespace std;
using namespace std::chrono;

// Defining the number threads to be used in the program
#define num_threads 8


// Operation Counter
int nxt_operation_index  = 0;


// Mutex Lock
pthread_mutex_t updateIndexLock;


// Struct containing data the threads need in order to execute different operations
typedef struct{
    int operation_index, ID;
    int *matrixA, *matrixB, *matrixC; //matrices it will operate on
    int N; // matrix size
}ThreadDataIndex;


/* This function dynamically generates a pointer to the first element of the matrix and populates 
   the matrix with random numbers according to a randomly generated seed value between 0 and 100 */
int* GenerateMatrix(int N)
{
    int dimension = N*N*N;

    int seed = rand()%100;
    srand(seed);

    int *matrix =  (int* )calloc(dimension, sizeof(int));
    int *element_ptr = matrix;

    for (int i = 0; i < dimension; ++i)
    {
        *element_ptr = rand()%N;
        element_ptr++;
    }
    
    return matrix;
}

/* This function makes use of an elements coordinates in the 2D matrix and returns the
   element's position in the 1D matrix that was initially created. */
int getElementPosition2D(int coords[2], int N)
{
    int row = coords[0];
    int col = coords[1];
    int position = (row*N) + col;
    return position;
}

/* This function returns the value of the element by shifting the matrix element pointer
   by the position returned from the getElementPosition2D function */ 
int getElement(int* matrix_ptr, int coords[2], int N)
{
    return *(matrix_ptr + getElementPosition2D(coords,N));
}

//This Function displays the 2D matrix
void DisplayMatrix(int* matrix, int N)
{
    int dimension = N*N*N;

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

/* This function dynamically allocates memory for the resultant matrix C and returns the pointer
   to the first element of the resultant matrix*/
int* allocateMatrix(int N)
{
    int dimension = N*N*N;
    int* result = (int*)calloc(dimension, sizeof(int));
    return result;
}

/* This function assigns each thread an operation based on the operation index. 
   A mutex lock is used to ensure that the threads wait for the thread that is 
   currently accessing data to complete its operation before the next operation is executed.*/
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

/* Rank 2 Tensor Multiplication Function:
    this function creates threads and a struct of threads based on the number of threads defined.*/
void rank2TensorMultPThread(int*matrixA, int*matrixB, int*matrixC, int N)
{
    int rc;

    pthread_t threads[num_threads];
    ThreadDataIndex threads_data[num_threads]; 
    /* Updates the operation index for the threads so that if a thread has completed it's
       assigned operation, it will go and execute the next available operation */  

    nxt_operation_index = (int)num_threads; 

    /* Initializing the information in the struct for each thread and each thread is assigned an operation*/
    for(int i=0; i < num_threads; ++i)
    {
        
        threads_data[i].ID = i;
        threads_data[i].operation_index = i;
        threads_data[i].matrixA = matrixA;
        threads_data[i].matrixB = matrixB;
        threads_data[i].matrixC = matrixC;
        threads_data[i].N = N;

        /*Creating thread, passing in the address and the function for multiplying the element*/
        //pthread_create - returns 1 if theres an error with creating the threads & 0 if not
        rc = pthread_create(&threads[i], NULL, ElementMultiplier, &threads_data[i]);

        //Error condition: If the pthreads are created successfully, 0 is returned
        //If threads are not created successfully, an error message will be output and the program will terminate
        if(rc)
        {
            cout<< "ERROR creating thread."<<endl;
            exit(-1);
        }
    }
    //scheduling the joining of the threads.
    for(int j = 0; j< num_threads; j++)
    {
        pthread_join(threads[j],NULL);
    }
}


//Rank 3 Multiplication
 void rank3TensorMultPThread(int *matrixA, int *matrixB, int *matrixC, int N, int number_of_elements)
{
	int startPos = 0;

		int elements2D = N*N;
		for(int i=0;i<N;++i)
		{
            rank2TensorMultPThread(matrixA+startPos,matrixB+startPos, matrixC+startPos,N);
			
		 	startPos+= elements2D;
		 	
		}
		
}


//Display 3D Matrix
void Display3DMatrix(int *matrix, int N)
{
    int dimension = N*N*N;
	if(matrix != NULL)
	{
		for(int i=0; i< N*N*N; ++i)
		{
			if( i%(N*N) == 0)
			{
				cout << " \n\nLayer: " << (i/(N*N)+1);
			}
		
			if( i%N == 0) cout << endl;
	
			cout << *matrix << " ";
			++matrix;
		}
		cout << endl;
	}else cout << " The Matrix is empty";
}




int main()
{
    int N = 30;
    int number_of_elements = N*N*N;
    int* matrixA = GenerateMatrix(N);
    int* matrixB = GenerateMatrix(N);
    int* matrixC = allocateMatrix(N);

    cout << "Matrix A";
    Display3DMatrix(matrixA, N);
    cout << "\nMatrix B";
    Display3DMatrix(matrixB, N);

    //Starting the steady clock
    std::chrono::time_point<std::chrono::steady_clock> startClock, endClock;
    startClock = std::chrono::steady_clock::now();

    rank3TensorMultPThread(matrixA, matrixB, matrixC, N, number_of_elements);

    //Pause the steady clock
    endClock = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = duration_cast<duration<double>>(endClock - startClock);
    
    cout << "\nMatrix C";
    Display3DMatrix(matrixC,N);
    cout << "Elapsed Time in Seconds: " << elapsedTime.count() << endl;

    return 0;
}