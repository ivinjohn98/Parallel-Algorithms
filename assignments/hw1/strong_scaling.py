import subprocess
import matplotlib.pyplot as plt
import numpy as np

exe_path = "./openmp_prefixsum.exe"
thread_counts = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
execution_times = []

for P in thread_counts:
    result = subprocess.run(
        ["powershell", f"$env:OMP_NUM_THREADS={P}; .\\{exe_path} 100000000"],
        capture_output=True, text=True
    )

    for line in result.stdout.splitlines():
        if "Elapsed time (t)=" in line:
            time_taken = float(line.split("=")[1].strip())
            execution_times.append(time_taken)
            break

log_thread_counts = np.log2(thread_counts)
log_execution_times = np.log2(execution_times)

plt.figure(figsize=(10, 6))
plt.plot(log_thread_counts, log_execution_times, marker='o', color='b', linestyle='-', markersize=8)
plt.title("log(Elapsed Time) vs. log(Number of Threads)", fontsize=16)
plt.xlabel("log(Number of Threads, P)", fontsize=14)
plt.ylabel("log(Elapsed Time, t)", fontsize=14)
plt.grid(True)
plt.show()
