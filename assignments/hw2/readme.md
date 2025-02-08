# CSCE 626 -- Homework 2  

## Due Sunday Feb 16th by midnight.

## Objectives (All using MPI)
* Implement an O(P) version of Broadcast
* Implement an O(lg(P)) version of Broadcast
* Implmenet an O(lg(P)) version of Reduce
* Implement an O(lg(P)) version of Gather
* Implmentt an O(P) version of AllGather using a ring algorithm
* Setup weak scaling experiments and compare to MPI built in versions.
* Document your experiments by writing a brief report on your experimental study
* Answer why your performance is (likely) lower than MPI's built in


## Deliverables and Rubric
* 10pts for each correct implmentation (5 in total)
* 10pts for 4 scaling experiments comparing to (MPI_Bcast, MPI_Reduce, MPI_Gather, MPI_Allgather)
* 10pts for discussion of the performance differences between your implmentations and MPI's built in versions.

## Helpful Notes using Grace
* Load GCC and MPI with `module load GCC OpenMPI`
* Launch interactive jobs with:
```
  salloc -n 8 -pshort -t 10:00 --mem-per-cpu=2G
  mpirun <yourexe>
```



## Submission Instructions
* Fork this repository, it should default to private (double check), and then manually add me (rogerpearce) and our TA(krishnanprash)
* Commit and push your code into this directory of your fork
* Commit and push your report and figures to this file and directory


## Student Report
*  Write your report here and add any images to this directory 
