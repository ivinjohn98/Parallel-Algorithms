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
  uniformly generated random weighted edges with an average degree of 16
* Design experiments such that you evaluate the sensitivity on 
  the maximum edge weight as well as the average degree of the graph

## Helpful Resources
* https://www.cs.utexas.edu/~pingali/CS395T/2013fa/papers/delta-stepping.pdf
* https://jshun.csail.mit.edu/6886-s19/lectures/lecture23-1.pdf


## Deliverables and Rubric
* 20pts for a correct implementation of Delta Stepping SSSP
* 20pts for weak scaling experiments and discussion
* 20pts for strong scaling experiments and discussion
* 20pts for average degree sensitivity experiments and discussion
* 20pts for maximum edge weight experiments and discussion


## Student Report
*  Append your report here...


# Weak Scaling Experiments and discussion

| # Processors | # Vertices | # Edges  | Max Edge Weight | Delta (200) | Average Degree | Time (Measured) |
|--------------|------------|----------|-----------------|-------------|-----------------|-----------------|
| 1            | 31         | 248      | 1,000           | 200         | 16              | TBD             |
| 2            | 62         | 496      | 1,000           | 200         | 16              | TBD             |
| 4            | 125        | 1,000    | 1,000           | 200         | 16              | TBD             |
| 8            | 250        | 2,000    | 1,000           | 200         | 16              | TBD             |
| 16           | 500        | 4,000    | 1,000           | 200         | 16              | TBD             |
| 32           | 1,000      | 8,000    | 1,000           | 200         | 16              | TBD             |

# Strong Scaling Experiments and discussion

| # Processors | # Vertices | # Edges  | Max Edge Weight | Delta (200) | Average Degree | Time (Measured) |
|--------------|------------|----------|-----------------|-------------|-----------------|-----------------|
| 1            | 1,000      | 8,000    | 1,000           | 200         | 16              | TBD             |
| 2            | 1,000      | 8,000    | 1,000           | 200         | 16              | TBD             |
| 4            | 1,000      | 8,000    | 1,000           | 200         | 16              | TBD             |
| 8            | 1,000      | 8,000    | 1,000           | 200         | 16              | TBD             |
| 16           | 1,000      | 8,000    | 1,000           | 200         | 16              | TBD             |
| 32           | 1,000      | 8,000    | 1,000           | 200         | 16              | TBD             |


# Average degree sensitivity experiments and discussion

| # Processors | # Vertices | # Edges | Max Edge Weight | Delta (200) | Average Degree | Time (Measured) |
|--------------|------------|---------|-----------------|-------------|----------------|-----------------|
| 16           | 1,000      | 2,000   | 1,000           | 200         | 4              | TBD             |
| 16           | 1,000      | 4,000   | 1,000           | 200         | 8              | TBD             |
| 16           | 1,000      | 8,000   | 1,000           | 200         | 16             | TBD             |
| 16           | 1,000      | 16,000  | 1,000           | 200         | 32             | TBD             |
| 16           | 1,000      | 32,000  | 1,000           | 200         | 64             | TBD             |


# Maximum edge weight experiments and discussion

| # Processors | # Vertices | # Edges  | Max Edge Weight | Delta (200) | Average Degree | Time (Measured) |
|--------------|------------|----------|-----------------|-------------|----------------|-----------------|
| 16           | 1,000      | 8,000    | 100             | 200         | 16             | TBD             |
| 16           | 1,000      | 8,000    | 200             | 200         | 16             | TBD             |
| 16           | 1,000      | 8,000    | 500             | 200         | 16             | TBD             |
| 16           | 1,000      | 8,000    | 1,000           | 200         | 16             | TBD             |
| 16           | 1,000      | 8,000    | 2,000           | 200         | 16             | TBD             |
| 16           | 1,000      | 8,000    | 5,000           | 200         | 16             | TBD             |
