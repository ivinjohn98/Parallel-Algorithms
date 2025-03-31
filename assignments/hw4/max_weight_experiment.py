import matplotlib.pyplot as plt
import numpy as np

# | # Processors | # Vertices | # Edges  | Max Edge Weight | Delta (200) | Average Degree | Time (Measured) |
# |--------------|------------|----------|-----------------|-------------|----------------|-----------------|
# | 16           | 1,000      | 8,000    | 100             | 100         | 16             | 0.210673        |
# | 16           | 1,000      | 8,000    | 400             | 100         | 16             | 0.809575        |
# | 16           | 1,000      | 8,000    | 700             | 100         | 16             | 1.38209         |
# | 16           | 1,000      | 8,000    | 1000            | 100         | 16             | 1.92522         |
# | 16           | 1,000      | 8,000    | 1300            | 100         | 16             | 2.57236         |
# | 16           | 1,000      | 8,000    | 1600            | 100         | 16             | 3.17349         |
# | 16           | 1,000      | 8,000    | 1900            | 100         | 16             | 3.76076         |
# | 16           | 1,000      | 8,000    | 2200            | 100         | 16             | 4.33165         |
# | 16           | 1,000      | 8,000    | 2500            | 100         | 16             | 4.87797         |
# | 16           | 1,000      | 8,000    | 2800            | 100         | 16             | 5.55718         |
# | 16           | 1,000      | 8,000    | 3100            | 100         | 16             | 6.06727         |

execution_data = [
    {'delta-stepping':[0.210673, 0.809575, 1.38209, 1.92522, 2.57236, 3.17349, 3.76076, 
         4.33165, 4.87797, 5.55718, 6.06727]}
]

thread_counts = [100, 400, 700, 1000, 1300, 1600, 1900, 2200, 2500, 2800, 3100]

for i, execution_times in enumerate(execution_data, start=1):
    plt.figure(figsize=(12, 6))
    
    for label, times in execution_times.items():
        plt.plot(thread_counts, times, marker='o', linestyle='-', markersize=8, label=label)

    plt.title("Elapsed Time vs. Max Edge Weight", fontsize=16)
    plt.xlabel("Max Edge Weight, w", fontsize=14)
    plt.ylabel("Elapsed Time, t", fontsize=14)
    plt.grid(True)
    plt.legend()

    filename = f"max_edge_weight_experiment_{i}.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
