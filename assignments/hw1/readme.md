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

* Weak Scaling Study:
  PS D:\Visual Studio Code\TAMU-Workspace\CSCE-626-S25\assignments\hw1> py .\weak_scaling.py
{'O((N/P)log(P))': [12.054, 12.828, 13.721, 13.801, 13.768, 13.758, 16.85, 17.615, 18.125, 20.349, 19.093, 21.221], 'O(N/P + log(P))': [0.487, 0.652, 0.669, 0.657, 0.745, 0.787, 0.798, 0.874, 0.924, 0.966, 1.033, 1.042], 'O(N)': [0.059, 0.113, 0.171, 0.233, 0.284, 0.342, 0.399, 0.458, 0.512, 0.57, 0.619, 0.638]}


* Strong Scaling Study: 
  PS D:\Visual Studio Code\TAMU-Workspace\CSCE-626-S25\assignments\hw1> py .\strong_scaling.py
{'O((N/P)log(P))': [137.887, 69.352, 46.324, 35.06, 30.307, 29.701, 26.23, 23.179, 20.698, 18.936, 21.18, 19.317], 'O(N/P+log(P))': [4.922, 3.264, 2.176, 1.628, 1.353, 1.183, 1.124, 1.065, 0.992, 0.913, 0.853, 0.797], 'O(N)': [0.553, 0.575, 0.571, 0.572, 0.566, 0.57, 0.57, 0.425, 0.57, 0.57, 0.572, 0.569]}