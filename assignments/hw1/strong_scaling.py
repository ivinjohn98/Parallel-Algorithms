import subprocess
import matplotlib.pyplot as plt
import numpy as np

executables = {
    "O((N/P)log(P))": "./prefixsum_np_p.exe",
    "O(N/P+log(P))": "./prefixsum_np_plus_p.exe",
    "O(N)": "./prefixsum.exe"
}

thread_counts = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]

execution_times = {key: [] for key in executables}

for label, exe_path in executables.items():
    for P in thread_counts:
        result = subprocess.run(
            ["powershell", f"$env:OMP_NUM_THREADS={P}; .\\{exe_path} 100000000"],
            capture_output=True, text=True
        )

        for line in result.stdout.splitlines():
            if "Elapsed time (t) =" in line:
                time_taken = float(line.split("=")[1].strip())
                execution_times[label].append(time_taken)
                break

print(execution_times)

log_thread_counts = np.log2(thread_counts)

plt.figure(figsize=(10, 6))

for label, times in execution_times.items():
    log_execution_times = np.log2(times)
    plt.plot(log_thread_counts, log_execution_times, marker='o', linestyle='-', markersize=8, label=label)

plt.title("log(Elapsed Time) vs. log(Number of Threads)", fontsize=16)
plt.xlabel("log(Number of Threads, P)", fontsize=14)
plt.ylabel("log(Elapsed Time, t)", fontsize=14)
plt.grid(True)
plt.legend()
plt.show()
