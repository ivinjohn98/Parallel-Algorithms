import matplotlib.pyplot as plt
import numpy as np

# | # Processors | # Vertices | # Edges | Max Edge Weight | Delta (200) | Average Degree | Time (Measured) |
# |--------------|------------|---------|-----------------|-------------|----------------|-----------------|
# | 16           | 100        | 100     | 1,000           | 200         | 1              | 0.0643401       |
# | 16           | 100        | 400     | 1,000           | 200         | 4              | 0.07408         |
# | 16           | 100        | 700     | 1,000           | 200         | 7              | 0.102071        |
# | 16           | 100        | 1000    | 1,000           | 200         | 10             | 0.140195        |
# | 16           | 100        | 1300    | 1,000           | 200         | 13             | 0.169399        |
# | 16           | 100        | 1600    | 1,000           | 200         | 16             | 0.208059        |
# | 16           | 100        | 1900    | 1,000           | 200         | 19             | 0.23961         |
# | 16           | 100        | 2200    | 1,000           | 200         | 22             | 0.282678        |
# | 16           | 100        | 2500    | 1,000           | 200         | 25             | 0.319986        |
# | 16           | 100        | 2800    | 1,000           | 200         | 28             | 0.358302        |
# | 16           | 100        | 3100    | 1,000           | 200         | 31             | 0.387412        |
# | 16           | 100        | 3400    | 1,000           | 200         | 34             | 0.43785         |
# | 16           | 100        | 3700    | 1,000           | 200         | 37             | 0.461746        |
# | 16           | 100        | 4000    | 1,000           | 200         | 40             | 0.496364        |
# | 16           | 100        | 4300    | 1,000           | 200         | 43             | 0.52875         |
# | 16           | 100        | 4600    | 1,000           | 200         | 46             | 0.574309        |
# | 16           | 100        | 4900    | 1,000           | 200         | 49             | 0.610889        |
# | 16           | 100        | 4950    | 1,000           | 200         | 49.5(complete) | 0.610755        |

execution_data = [
    {'delta-stepping':[0.0643401, 0.07408, 0.102071, 0.140195, 0.169399, 0.208059, 0.23961, 
         0.282678, 0.319986, 0.358302, 0.387412, 0.43785, 0.461746, 0.496364, 
         0.52875, 0.574309, 0.610889, 0.610755]}
]

thread_counts = [1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 49.5]

for i, execution_times in enumerate(execution_data, start=1):
    plt.figure(figsize=(12, 6))
    
    for label, times in execution_times.items():
        plt.plot(thread_counts, times, marker='o', linestyle='-', markersize=8, label=label)

    plt.title("Elapsed Time vs. Average Degree", fontsize=16)
    plt.xlabel("Average Degree, d", fontsize=14)
    plt.ylabel("Elapsed Time, t", fontsize=14)
    plt.grid(True)
    plt.legend()

    filename = f"degree_experiment_{i}.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
