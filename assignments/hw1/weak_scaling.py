import subprocess
import matplotlib.pyplot as plt
import numpy as np

exe_path = "./openmp_prefixsum.exe"

thread_counts = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
element_counts = [10000000, 20000000, 30000000, 40000000, 50000000, 60000000, 70000000, 80000000, 90000000, 100000000, 110000000, 120000000]
execution_times = []
log_thread_counts = []

for P, E in zip(thread_counts, element_counts):
    result = subprocess.run(
        ["powershell", f"$env:OMP_NUM_THREADS={P}; .\\{exe_path} {E}"],
        capture_output=True, text=True
    )

    for line in result.stdout.splitlines():
        if "Elapsed time (t)=" in line:
            time_taken = float(line.split("=")[1].strip())
            execution_times.append(time_taken)
            log_thread_counts.append(np.log2(P))
            break

plt.figure(figsize=(12, 6))
plt.plot(log_thread_counts, execution_times, marker='o', color='b', linestyle='-', markersize=8)
plt.title("Elapsed Time vs. log(Number of Threads) and Element count", fontsize=16)
plt.xlabel("log(Number of Threads, P)", fontsize=14)
plt.ylabel("Elapsed Time, t", fontsize=14)
plt.grid(True)
plt.show()
