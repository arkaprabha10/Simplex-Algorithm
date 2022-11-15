# Simplex-Algorithm
The simplex method is an approach to solve linear programming models by using slack variables, tables, and pivot variables as a means to find the optimal solution to an optimization problem. Here, we represent the naive serial implementation of the standard simplex algorithm and its parallel counterpart implemented using OpenMP over a shared memory architecture.

Any Linear programming problem can be modelled into the following standard form:

    maximize Z = CX
    subject to AX = B  where X >= 0

For our implementation we have used the following file format while taking the input:

        |  A B |
        | -C 0 |

Filename should be with respect to shape of A i.e., with A of dimension 50x60 keep filename name_50x60.txt

Number of rows in the input file matrix (nL) = 50 + 1 =51

Number of columns in the input file matrix (nC) = 50 + 60 + 1 =121

## Serial version: Algorithm/serial_final.cpp
To compile: 
    
    g++ -std=c++11 serial_final.cpp -o s

To execute serial code for name_50x60.txt:   
    
    ./s name_50x60.txt


## Parallel version: Algorithm/openmp_final.cpp
Here, in addition to the filename we also need to specify the number of threads and the chunk size for scheduling as the command line arguments

To compile: 

    g++ -std=c++11 openmp_final.cpp -fopenmp -o p

To execute parallel code for name_50x60.txt with 4 threads and chunkSize=8:   
    
    ./p name_50x60.txt 4 8
