
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <time.h> 

using namespace std;

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
    int dimension = N*N*N;

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

//Memory Allocation
int* allocateMatrix(int N)
{
    int dimension = N*N*N;
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

//Rank 3 Multiplication
 void rank3DMultiplier(int *matrixA, int *matrixB, int *matrixC, int N, int number_of_elements)
{
	int startPos = 0;

		int elements2D = N*N;
		for(int i=0;i<N;++i)
		{
            run2DMultiplier(matrixA+startPos,matrixB+startPos, matrixC+startPos,N);
			//int* result_2D = rank2TensorMult(matrixA+startPos, matrixB+startPos, N);
			 										          
			// Copy elements:
			/*for(int i = 0; i<elements2D; i++)
			{
				*temp = *result_2D;
				temp++;
				result_2D++;
			}*/
			
			// Change starting position (Start at the beginning of the layer):
		 	startPos+= elements2D;
		 	
		 	// = result_2D-elements2D;//reset position to beginning of the new layer array
		 	//free(result_2D);// free memory
		}

	//}
	//else	
	//{
		//printf("Memory allocation failed.");
	//}

		
}

//Rank 3 Multiplication
int *rank3TensorMult(int *matrixA, int *matrixB, int N, int number_of_elements)
{
	int startPos = 0;
	int* result  = allocateMatrix(number_of_elements);
	int* temp = result;

	if (temp != NULL)
	{ 
		int elements2D = N*N;
		for(int i=0;i<N;++i)
		{
			 int* result_2D = rank2TensorMult(matrixA+startPos, matrixB+startPos, N);
			 										          
			// Copy elements:
			for(int i = 0; i<elements2D; i++)
			{
				*temp = *result_2D;
				temp++;
				result_2D++;
			}
			
			// Change starting position (Start at the beginning of the layer):
		 	startPos+= elements2D;
		 	
		 	result_2D = result_2D-elements2D;//reset position to beginning of the new layer array
		 	free(result_2D);// free memory
		}

	}
	else	
	{
		printf("Memory allocation failed.");
	}

	return result;	
}

//Display 3D Matrix
void Display3DMatrix(int *matrix, int N)
{
    int dimension = N*N*N;
	if(matrix != NULL)
	{
		cout << endl;
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
    int N = 3;
    int number_of_elements = N*N*N;
    int* matrixA = GenerateMatrix(N);
    int* matrixB = GenerateMatrix(N);
    int* matrixC = allocateMatrix(N);

    cout << "Matrix A" << endl;
    Display3DMatrix(matrixA, N);
    cout << "Matrix B" << endl;
    Display3DMatrix(matrixB, N);

    
    //run2DMultiplier(matrixA, matrixB, matrixC, N);
    rank3DMultiplier(matrixA, matrixB, matrixC, N, number_of_elements);
    cout << "Matrix C" << endl;
    Display3DMatrix(matrixC,N);

   


    cout << "Matrix D" << endl;
    int* matrixD =  rank3TensorMult(matrixA, matrixB, N, N*N) ;// ElementMultiplier(matrixA, matrixB, N);
    Display3DMatrix(matrixD,N);

    

    return 0;
}