import matplotlib.pyplot as plt
import numpy as np

time_elapsed_B = [
    0.028282,
    0.0328746,
    0.0391673,
    0.0400215,
    0.0375201,
    0.039075,
    0.0406142,
    0.0417945,
    0.0435124,
    0.0452421,
    0.0459878
]

time_elapsed_A = [
    0.0198141,
    0.0281727,
    0.0285605,
    0.028054,
    0.0297758,
    0.0286033,
    0.0316772,
    0.0327137,
    0.031814,
    0.0335555,
    0.0337457
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
        plt.plot(log_thread_counts, times, marker='o', linestyle='-', markersize=8, label=label)

    plt.title("Elapsed Time vs. log(Number of Threads) and Element count", fontsize=16)
    plt.xlabel("log(Number of Threads, P)", fontsize=14)
    plt.ylabel("Elapsed Time, t", fontsize=14)
    plt.grid(True)
    plt.legend()

    filename = f"weak_scaling.png"
    plt.savefig(filename)
    plt.close()

    print(f"Plot saved as {filename}")
