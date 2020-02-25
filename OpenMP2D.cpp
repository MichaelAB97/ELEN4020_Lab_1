#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

int main ()
{   
    int N = 10;
    int matA[N][N] = {};
    int matB[N][N] = {};
    int matC[N][N] = {};

    //Populating A and B arrays with random values between 0 and 50 and 50 and 100 respectively
    for (int i=0; i!=N; i++)
    {
        for (int j=0; j!=N; j++)
        {
            matA[i][j] = rand() % N+1;
            matB[i][j] = rand() % N+1;
        }
    }

    //Starting the steady clock
    std::chrono::time_point<std::chrono::steady_clock> startClock, endClock;
    startClock = std::chrono::steady_clock::now();

    //Multiplying the 2 arrays and storing it in matrix C 
    #pragma omp parallel shared(matA,matB,matC)
    {
        #pragma omp for schedule(static)
            for (int i=0; i<N; i++)
            {
                for (int j=0;j<N; j++)
                {
                    matC[i][j] = matC[i][j] + (matA[i][j]*matB[i][j]);
                }
            }
    }

    endClock = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = duration_cast<duration<double>>(endClock - startClock);

    //Displaying 2D Arrays
    cout << "matA: " << endl;
    for (int i=0; i!=N; i++)
    {
        for (int j=0; j!=N; j++)
        {
            cout << matA[i][j] << " ";
        }
        cout << endl; 
    }

    cout << "matB: " << endl;
    for (int i=0; i!=N; i++)
    {
        for (int j=0; j!=N; j++)
        {
            cout << matB[i][j] << " ";
        }
        cout << endl; 
    }

    cout << endl;

    cout << "matC: " << endl;
    for (int i=0; i!=N; i++)
    {
        for (int j=0; j!=N; j++)
        {
            cout << matC[i][j] << " ";
        }
        cout << endl; 
    }

    cout << "Elapsed Time in Seconds: " << elapsedTime.count() << endl;

    return 0;
}