# CSCE 626 -- Homework 1  

## Due Sunday Feb 9th by midnight.

## Objectives
* Implement a O(lg(P) + N/P) version of Prefix Sum in OpenMP
* Using the O(log(N) * N/P) version we implemented in class (see code_examples directory), setup an experimental comparison between the two versions and also the sequential std::partial_sum
* Setup both Weak and Strong scaling experiments to compare the 3 versions of Prefix Sum
  * Strong Scaling:    Set the input size to 100 million and vary the number of threads between 1 - #cores
  * Weak Scaling:   Starting at 10 million with 1 thread, increase both N and P simultaneously (e.g., double them)
* Document your experiments by writing a brief report on your experimental study


## Deliverables and Rubric
* 50pts for a correct O(lg(P) + N/P) implementation of Prefix Sum in OpenMP 
* 25pts for your report on your Weak Scaling study
* 25pts for your report on your Strong Scaling study
* If you're inspired to experiment with other variations, please do so!    These are just the basic requirements for the assignment.



## Submission Instructions
* Fork this repository, it should default to private (double check), and then manually add me (rogerpearce) and our TA(krishnanprash)
* Commit and push your code into this directory of your fork
* Commit and push your report and figures to this file and directory


## Student Report
*  Write your report here and add any images to this directory 
