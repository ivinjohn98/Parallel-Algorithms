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

2 nodes 48 cores.

| # Processors | # Vertices | # Edges  | Max Edge Weight | Delta (200) | Average Degree  | Time (Measured) |
|--------------|------------|----------|-----------------|-------------|-----------------|-----------------|
| 1            | 31         | 248      | 1,000           | 200         | 16              | 0.00559459      |
| 2            | 62         | 496      | 1,000           | 200         | 16              | 0.0238602       |
| 4            | 125        | 1,000    | 1,000           | 200         | 16              | 0.0678698       |
| 8            | 250        | 2,000    | 1,000           | 200         | 16              | 0.177056        |
| 16           | 500        | 4,000    | 1,000           | 200         | 16              | 0.514986        |
| 32           | 1,000      | 8,000    | 1,000           | 200         | 16              | 1.54407         |

![Weak Scaling Delta-Stepping plot](weak_scaling.png)

# Strong Scaling Experiments and discussion

| # Processors | # Vertices | # Edges  | Max Edge Weight | Delta (200) | Average Degree  | Time (Measured) |
|--------------|------------|----------|-----------------|-------------|-----------------|-----------------|
| 1            | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.0870622       |
| 2            | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.28799         |
| 4            | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.480869        |
| 8            | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.68234         |
| 16           | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.990479        |
| 32           | 1,000      | 8,000    | 1,000           | 200         | 16              | 1.5392          |

![Strong Scaling Delta-Stepping plot](strong_scaling.png)


# Average degree sensitivity experiments and discussion

| # Processors | # Vertices | # Edges | Max Edge Weight | Delta (200) | Average Degree | Time (Measured) |
|--------------|------------|---------|-----------------|-------------|----------------|-----------------|
| 16           | 100        | 100     | 1,000           | 200         | 1              | 0.0643401       |
| 16           | 100        | 400     | 1,000           | 200         | 4              | 0.07408         |
| 16           | 100        | 700     | 1,000           | 200         | 7              | 0.102071        |
| 16           | 100        | 1000    | 1,000           | 200         | 10             | 0.140195        |
| 16           | 100        | 1300    | 1,000           | 200         | 13             | 0.169399        |
| 16           | 100        | 1600    | 1,000           | 200         | 16             | 0.208059        |
| 16           | 100        | 1900    | 1,000           | 200         | 19             | 0.23961         |
| 16           | 100        | 2200    | 1,000           | 200         | 22             | 0.282678        |
| 16           | 100        | 2500    | 1,000           | 200         | 25             | 0.319986        |
| 16           | 100        | 2800    | 1,000           | 200         | 28             | 0.358302        |
| 16           | 100        | 3100    | 1,000           | 200         | 31             | 0.387412        |
| 16           | 100        | 3400    | 1,000           | 200         | 34             | 0.43785         |
| 16           | 100        | 3700    | 1,000           | 200         | 37             | 0.461746        |
| 16           | 100        | 4000    | 1,000           | 200         | 40             | 0.496364        |
| 16           | 100        | 4300    | 1,000           | 200         | 43             | 0.52875         |
| 16           | 100        | 4600    | 1,000           | 200         | 46             | 0.574309        |
| 16           | 100        | 4900    | 1,000           | 200         | 49             | 0.610889        |
| 16           | 100        | 4950    | 1,000           | 200         | 49.5(complete) | 0.610755        |

![Average Degree vs Time Delta-Stepping plot](degree_experiment.png)

# Maximum edge weight experiments and discussion

| # Processors | # Vertices | # Edges  | Max Edge Weight | Delta (200) | Average Degree | Time (Measured) |
|--------------|------------|----------|-----------------|-------------|----------------|-----------------|
| 16           | 1,000      | 8,000    | 100             | 100         | 16             | 0.210673        |
| 16           | 1,000      | 8,000    | 400             | 100         | 16             | 0.809575        |
| 16           | 1,000      | 8,000    | 700             | 100         | 16             | 1.38209         |
| 16           | 1,000      | 8,000    | 1000            | 100         | 16             | 1.92522         |
| 16           | 1,000      | 8,000    | 1300            | 100         | 16             | 2.57236         |
| 16           | 1,000      | 8,000    | 1600            | 100         | 16             | 3.17349         |
| 16           | 1,000      | 8,000    | 1900            | 100         | 16             | 3.76076         |
| 16           | 1,000      | 8,000    | 2200            | 100         | 16             | 4.33165         |
| 16           | 1,000      | 8,000    | 2500            | 100         | 16             | 4.87797         |
| 16           | 1,000      | 8,000    | 2800            | 100         | 16             | 5.55718         |
| 16           | 1,000      | 8,000    | 3100            | 100         | 16             | 6.06727         |

![Maximum edge weight vs Time Delta-Stepping plot](max_weight_experiment.png)