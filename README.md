2020-Team-014-Lab-1
Build Process for Ubuntu 18.04: OpenMP

To Compile rank2TensorMultOpenMP.cpp:

g++ -fopenmp -lgomp rank2TensorMultOpenMP.cpp -o OpenMP2D

To run use:
1. export OMP_NUM_THREADS=8 # <number of threads to use>
2. ./OpenMP2D

-----------------------------------------------------------------------------------------------

To Compile rank3TensorMultOpenMP.cpp:

g++ -fopenmp -lgomp rank3TensorMultOpenMP.cpp -o OpenMP3D

To run use:
1. export OMP_NUM_THREADS=8 # <number of threads to use>
2. ./OpenMP3D

------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------

Build Process for Ubuntu 18.04: PThreads

To Compile rank2TensorMultPThread.cpp:

g++ -o Tensor2DPT rank2TensorMultPThread.cpp -lpthread

To run use:
./Tensor2DPT

-------------------------------------------------------------------------------------------------

To Compile rank3TensorMultPThread.cpp:
g++ -o Tensor3DPT rank3TensorMultPThread.cpp -lpthread

To run use:
./Tensor3DPT
