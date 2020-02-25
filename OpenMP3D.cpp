#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

int main ()
{
    int N = 5;

    cout << N << endl;
    int matA3D[N][N][N] = {};
    int matB3D[N][N][N] = {};
    int matC3D[N][N][N] = {};


   //Populating the A and B arrays with random values between 0 and 99
    for (int i=0; i<N; i++)
    {   
        for (int j=0; j<N; j++)
        {
            for (int k=0; k<N; k++)
            {
                matA3D[i][j][k] = rand() % 100;
                matB3D[i][j][k] = rand() % 100;
            }

        }
    }

    std::chrono::time_point<std::chrono::steady_clock> startClock, endClock;
    startClock = std::chrono::steady_clock::now();

  //Multiplying the 2 arrays and storing it in C
    #pragma omp parallel shared(matA3D, matB3D, matC3D)
    {
        #pragma omp for schedule(static)
        for (int i=0; i<N; i++)
        {
            for (int j=0; j<N; j++)
            {
                for (int k=0; k<N; k++)
                {
                    matC3D[i][j][k] =  matC3D[i][j][k] + (matA3D[i][j][k]* matB3D[i][j][k]);
                }
            }
        }
    }

    endClock = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedTime = duration_cast<duration<double>>(endClock - startClock);

    //Displaying matrices
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            for (int k=0; k<N; k++)
            {
                cout << " " << matA3D[i][j][k] << " ";
            }

            cout << endl;
        }
        cout << endl;
    }

    cout << "B Matrix: " << endl;
    
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            for (int k=0; k<N; k++)
            {
                cout << " " << matB3D[i][j][k] << " ";
            }

            cout << endl;
        }
        cout << endl;
    }

    cout << "C Matrix: " << endl;
    
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            for (int k=0; k<N; k++)
            {
                cout << " " << matC3D[i][j][k] << " ";
            }

            cout << endl;
        }
        cout << endl;
    }

    return 0;
}