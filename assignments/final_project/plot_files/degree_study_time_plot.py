import matplotlib.pyplot as plt
import numpy as np

time_elapsed_B = [
    0.0151653,
    0.0191667,
    0.0254801,
    0.0422729,
    0.0805394,
    0.178861
]


time_elapsed_A = [
    0.0113172,
    0.0134361,
    0.0193441,
    0.0327667,
    0.0606435,
    0.133709
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

    plt.title("Elapsed Time vs. Average Degree", fontsize=16)
    plt.xlabel("Average Degree, d", fontsize=14)
    plt.ylabel("Elapsed Time, t", fontsize=14)
    plt.grid(True)
    plt.legend()

    filename = f"degree_study.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")