import matplotlib.pyplot as plt
import numpy as np

mpirun -np 32 ./mis_luby 40000 40000
mpirun -np 32 ./mis_luby 40000 80000
mpirun -np 32 ./mis_luby 40000 160000
mpirun -np 32 ./mis_luby 40000 320000
mpirun -np 32 ./mis_luby 40000 640000
mpirun -np 32 ./mis_luby 40000 1280000
mpirun -np 32 ./mis_luby 40000 2560000

execution_data = [
    {
        'Algorithm A':[2.01279, 1.11487, 0.718965, 0.30063, 0.189272, 0.17902, 0.125913, 0.0909839, 0.084753, 0.0672113, 0.0590458],
        'Algorithm B':[2.01279, 1.11487, 0.718965, 0.30063, 0.189272, 0.17902, 0.125913, 0.0909839, 0.084753, 0.0672113, 0.0590458]
    }
]

thread_counts = [2, 2, 4, 8, 12, 16, 20, 26, 32, 40, 48]
log_thread_counts = np.log2(thread_counts)

for i, execution_times in enumerate(execution_data, start=1):
    plt.figure(figsize=(12, 6))
    
    for label, times in execution_times.items():
        plt.plot(log_thread_counts, np.log2(times), marker='o', linestyle='-', markersize=8, label=label)

    plt.title("log(Elapsed Time) vs. log(Number of Threads)", fontsize=16)
    plt.xlabel("log(Number of Threads, P)", fontsize=14)
    plt.ylabel("log(Elapsed Time, t)", fontsize=14)
    plt.grid(True)
    plt.legend()

    filename = f"strong_scaling.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
