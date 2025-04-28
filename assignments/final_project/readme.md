# CSCE 626 -- Final Project

## Due Friday May 2 at 0800
* Progress update due April 14 during class

## Objectives
* Pick an algorithm of your own interest to study (discuss with instructor for approval). You could 
pick a known parallel algorithm and implement your own version for experimental study.   You could 
pick a sequential algorithm and attempt to parallelize it and perform an experimental study.   You 
could attempt to prove additional theoretical guarantees about a known parallel algorithm.    The sky
is the limit (with approval!).
* Starting April 14th we will have a mid-project update during class, approximately 3min standup presentation
* April 28th and May 2nd we will have a final-project presentation, approximately 5min each.
* Your final report should be written in a conference-style report due May 2nd.

## Project ideas (No pressure picking these ideas, can pick your own!)
* Max cut heuristic of undirected graphs
* K-core decomposition of undirected graphs (synchronous vs asynchronous)
* Outer Product sparse matrix multiply
  * see: https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8327050
* Distributed filesystem traversal for large/deep directory structures
* Fully asynchronous SSSP (compared to HW4)


## Deliverables and Rubric
* 20 pts for your 3 min mid-project update (April 14th)
* 30 pts for your 5 min final project presentation (April 28th and May 2nd)
* 50 pts for your final project report (May 2nd)

## Final Report
*  Upload your final report and presentation slides to this folder as PDFs

strong scaling experiment:

mpirun -np 1 ./mis_luby 80000 640000
loop count: 17
time elapsed: 1.79732
mpirun -np 2 ./mis_luby 80000 640000
loop count: 16
time elapsed: 1.05519
mpirun -np 4 ./mis_luby 80000 640000
loop count: 17
time elapsed: 0.526511
mpirun -np 8 ./mis_luby 80000 640000
loop count: 16
time elapsed: 0.282303
mpirun -np 12 ./mis_luby 80000 640000
loop count: 17
time elapsed: 0.181693
mpirun -np 16 ./mis_luby 80000 640000
loop count: 15
time elapsed: 0.169975
mpirun -np 20 ./mis_luby 80000 640000
loop count: 16
time elapsed: 0.121998
mpirun -np 26 ./mis_luby 80000 640000
loop count: 17
time elapsed: 0.0898481
mpirun -np 32 ./mis_luby 80000 640000
loop count: 18
time elapsed: 0.0834425
mpirun -np 40 ./mis_luby 80000 640000
loop count: 16
time elapsed: 0.0668803
mpirun -np 48 ./mis_luby 80000 640000
loop count: 14
time elapsed: 0.0575341

weak scaling experiment:

mpirun -np 1 ./mis_luby 1250 10000
loop count: 12
time elapsed: 0.0279982
mpirun -np 2 ./mis_luby 2500 20000
loop count: 13
time elapsed: 0.0334465
mpirun -np 4 ./mis_luby 5000 40000
loop count: 12
time elapsed: 0.0388907
mpirun -np 8 ./mis_luby 10000 80000
loop count: 14
time elapsed: 0.0357847
mpirun -np 12 ./mis_luby 15000 120000
loop count: 12
time elapsed: 0.0377812
mpirun -np 16 ./mis_luby 20000 160000
loop count: 11
time elapsed: 0.0392735
mpirun -np 20 ./mis_luby 25000 200000
loop count: 14
time elapsed: 0.0401486
mpirun -np 26 ./mis_luby 32500 260000
loop count: 13
time elapsed: 0.0423377
mpirun -np 32 ./mis_luby 40000 320000
loop count: 13
time elapsed: 0.0411337
mpirun -np 40 ./mis_luby 50000 400000
loop count: 15
time elapsed: 0.0448627
mpirun -np 48 ./mis_luby 60000 480000
loop count: 14
time elapsed: 0.0454676


Averge degree experiment:

avg degree = 2
mpirun -np 32 ./mis_luby 40000 40000
loop count: 15
time elapsed: 0.015775
avg degree = 4
mpirun -np 32 ./mis_luby 40000 80000
loop count: 13
time elapsed: 0.0179834
avg degree = 8
mpirun -np 32 ./mis_luby 40000 160000
loop count: 16
time elapsed: 0.0253662
avg degree = 16
mpirun -np 32 ./mis_luby 40000 320000
loop count: 16
time elapsed: 0.0421277
avg degree = 32
mpirun -np 32 ./mis_luby 40000 640000
loop count: 12
time elapsed: 0.0809645
avg degree = 64
mpirun -np 32 ./mis_luby 40000 1280000
loop count: 16
time elapsed: 0.182926

Consistency experiment: (loop iterations)

mpirun -np 32 ./mis_luby 80000 640000

loop count: 14
time elapsed: 0.0819287
loop count: 16
time elapsed: 0.0818912
loop count: 13
time elapsed: 0.0819484
loop count: 18
time elapsed: 0.0829478
loop count: 13
time elapsed: 0.0812319
loop count: 14
time elapsed: 0.0825045
loop count: 21
time elapsed: 0.0839945
loop count: 11
time elapsed: 0.0801642
loop count: 15
time elapsed: 0.0805575

////////////////////////