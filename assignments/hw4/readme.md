# CSCE 626 -- Homework 4 

## Due Wednesday March 19th by midnight.

## How to build with cmake
``` bash
# from hw4 directory
module load GCC OpenMPI CMake
mkdir build
cd build
cmake ..
#salloc with desired number of processors
salloc -n 8 -pshort -t 50:00 --mem-per-cpu=2G
mpirun bfs_example
```


## Objectives
* Using either raw MPI or YGM, implement Delta Stepping SSSP
* Perform both weak and strong scaling experiments using
  uniformly generated random weighted edges
* Design experiments such that you evaluate the sensitivity on 
  the maximum edge weight as well as the average degree of the graph

## Helpful Resources
* https://www.cs.utexas.edu/~pingali/CS395T/2013fa/papers/delta-stepping.pdf
* https://jshun.csail.mit.edu/6886-s19/lectures/lecture23-1.pdf


## Deliverables and Rubric
* 20pts for a correct implmentation of Delta Stepping SSSP
* 20pts for weak scaling experiments and discussion
* 20pts for strong scaling experiments and dicussion
* 20pts for average degree sensitivity experiments and discussion
* 20pts for maximum edge weight experiments and discussion


## Student Report
*  Append your report here...
