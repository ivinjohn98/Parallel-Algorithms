import matplotlib.pyplot as plt
import numpy as np


# | # Processors | # Vertices | # Edges  | Max Edge Weight | Delta (200) | Average Degree  | Time (Measured) |
# |--------------|------------|----------|-----------------|-------------|-----------------|-----------------|
# | 1            | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.0870622       |
# | 2            | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.28799         |
# | 4            | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.480869        |
# | 8            | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.68234         |
# | 16           | 1,000      | 8,000    | 1,000           | 200         | 16              | 0.990479        |
# | 32           | 1,000      | 8,000    | 1,000           | 200         | 16              | 1.5392          |

execution_data = [
    {'delta-stepping':[0.0870622, 0.28799, 0.480869, 0.68234, 0.990479, 1.5392]}
]

thread_counts = [1, 2, 4, 8, 16, 32]
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

    filename = f"strong_scaling_{i}.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
