import matplotlib.pyplot as plt
import numpy as np


# | # Processors | # Vertices | # Edges  | Max Edge Weight | Delta (200) | Average Degree  | Time (Measured) |
# |--------------|------------|----------|-----------------|-------------|-----------------|-----------------|
# | 1            | 31         | 248      | 1,000           | 200         | 16              | 0.00559459      |
# | 2            | 62         | 496      | 1,000           | 200         | 16              | 0.0238602       |
# | 4            | 125        | 1,000    | 1,000           | 200         | 16              | 0.0678698       |
# | 8            | 250        | 2,000    | 1,000           | 200         | 16              | 0.177056        |
# | 16           | 500        | 4,000    | 1,000           | 200         | 16              | 0.514986        |
# | 32           | 1,000      | 8,000    | 1,000           | 200         | 16              | 1.54407         |

execution_data = [
    {'delta-stepping':[0.00559459, 0.0238602, 0.0678698, 0.177056, 0.514986, 1.54407]}
]

thread_counts = [1, 2, 4, 8, 16, 32]
log_thread_counts = np.log2(thread_counts) 

for i, execution_times in enumerate(execution_data, start=1):
    plt.figure(figsize=(12, 6))
    
    for label, times in execution_times.items():
        plt.plot(log_thread_counts, times, marker='o', linestyle='-', markersize=8, label=label)

    plt.title("Elapsed Time vs. log(Number of Threads) and Element count", fontsize=16)
    plt.xlabel("log(Number of Threads, P)", fontsize=14)
    plt.ylabel("Elapsed Time, t", fontsize=14)
    plt.grid(True)
    plt.legend()

    filename = f"weak_scaling_{i}.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
