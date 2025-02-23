import matplotlib.pyplot as plt
import numpy as np



execution_data = [
    {'k pivot sorting':[97.6393, 50.252, 15.7718, 14.2543, 7.06163, 3.72435, 2.00855, 1.10429]}
]

thread_counts = [1, 2, 4, 8, 16, 32, 64, 128]
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

    filename = f"execution_plot_{i}.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
