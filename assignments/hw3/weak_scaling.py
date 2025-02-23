import matplotlib.pyplot as plt
import numpy as np



execution_data = [
    {'k pivot sorting':[0.637168, 0.694559, 0.79019, 0.817929, 0.865297, 0.985766, 0.962246, 1.06661]}
]

thread_counts = [1, 2, 4, 8, 16, 32, 64, 128]
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

    filename = f"execution_plot_{i}.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
