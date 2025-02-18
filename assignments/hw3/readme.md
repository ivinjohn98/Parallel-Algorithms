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
*  Append the rest of your experimental report here...
