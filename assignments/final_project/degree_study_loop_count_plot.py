import matplotlib.pyplot as plt
import numpy as np

time_elapsed_B = [
    9,
    12,
    10,
    10,
    12,
    13
]

time_elapsed_A = [
    4,
    4,
    5,
    5,
    5,
    6
]

execution_data = [
    {
        'Algorithm A': time_elapsed_A,
        'Algorithm B': time_elapsed_B
    }
]

thread_counts = [1, 2, 4, 8, 16, 32]

for i, execution_times in enumerate(execution_data, start=1):
    plt.figure(figsize=(12, 6))
    
    for label, times in execution_times.items():
        plt.plot(thread_counts, times, marker='o', linestyle='-', markersize=8, label=label)

    plt.title("Number of iterations vs. Average Degree", fontsize=16)
    plt.xlabel("Average Degree, d", fontsize=14)
    plt.ylabel("Number of iterations", fontsize=14)
    plt.grid(True)
    plt.legend()

    filename = f"degree_study_loop_count.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")