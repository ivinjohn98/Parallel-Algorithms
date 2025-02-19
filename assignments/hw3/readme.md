# CSCE 626 -- Homework 3 

## Due Monday Feb 24th by midnight.

## Objectives
* Answer Question 1 below.
* Extend the mpi pivot sort we created during class (see code_examples) to do the following:
    * Add a step to balance the sorted array across each rank (N/P elements per rank)
    * Add the capability to oversample pivots with a parameter 'k' for k pivot samples per rank.
* Setup week and strong scaling experiments up to 128 processors
* Setup experiment with 'k' to oversample from 1 to 100
* Your minimum number of elements for your weak scaling experiments should be 1 Million per rank.


## Deliverables and Rubric
* 20pts for answer to question below
* 20pts for correct sorted array balancing
* 20pts for correct pivot oversampling
* 10pts for Strong scaling experiment and report
* 10pts for Weak scaling experiment and report
* 20pts for k-oversampling experiment and report


## Student Report
*  Question 1:   You are given a large array of N elements on P CREW processors (e.g., OpenMP), where N >> P.    Describe an algorithm in pseudocode based on a parallel merge sort and analyze its complexity on a CREW.   You may use any sequential sort you wish (e.g., Quicksort) for sorting sequential N/P chunks of the array.  You are not required to describe the fastest possible, but are required to anayze the time complexity your pseudcode correctly.


The data is assumed to be evenly distributed across  P processors. Else, one processors can broadcast partitioned data across in O(N)-time to P processors. Here P has to be a power of 2. 

```plaintext
1.  h = P / 2
2.  Each processor sorts its local (N / P) elements using merge sort.  // O((N / P) log (N / P)) time
3.  while (h >= 1) do { // O(log P)
4.      if (rank % (2 * h) >= h) then {
5.          send local sorted data to (rank - h) // O(N/h)
6.      }
7.      else {
8.          receive sorted data from (rank + h) // O(N/h)
9.          merge local sorted data with received sorted data // O(2(N/h))
10.     }
11.     h = h / 2
12. }
```

### Time Complexity Analysis

#### **1. Local Sorting:**  
Each processor sorts N/P elements using merge sort, which takes: O((N/P) log(P)).

#### **2. Merging Steps:**  
At each merging step, each processor merges two sorted arrays of increasing size. The merging work across all processors is:

O(N + N/2 + N/4 + N/8 + ... + N/P) = O(N)

Since there are log(P) merging rounds, the total merging time is: O(N log(P))

#### **3. Overall Complexity:**
Combining both sorting and merging steps:

O((N/P) log(N/P) + N log(P))

This is the total time complexity for parallel merge sort.


*  Append the rest of your experimental report here...
