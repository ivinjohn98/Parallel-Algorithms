import matplotlib.pyplot as plt
import numpy as np


time_elapsed_B = [
    2.01279,
    1.11487,
    0.718965,
    0.30063,
    0.189272,
    0.17902,
    0.125913,
    0.0909839,
    0.084753,
    0.0672113,
    0.0590458
]

time_elapsed_A = [
    1.18285,
    0.673947,
    0.410731,
    0.178222,
    0.112292,
    0.100331,
    0.0797346,
    0.061563,
    0.0553522,
    0.0435775,
    0.0390177
]

execution_data = [
    {
        'Algorithm A': time_elapsed_A,
        'Algorithm B': time_elapsed_B
    }
]

thread_counts = [1, 2, 4, 8, 12, 16, 20, 26, 32, 40, 48]
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
