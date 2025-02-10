import subprocess
import matplotlib.pyplot as plt
import numpy as np

executables = {
    "O((N/P)log(P))": "./prefixsum_np_p.exe",
    "O(N/P + log(P))": "./prefixsum_np_plus_p.exe",
    "O(N)": "./prefixsum.exe"
}

thread_counts = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
# element_counts = [100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200]
element_counts = [10000000, 20000000, 30000000, 40000000, 50000000, 60000000, 70000000, 80000000, 90000000, 100000000, 110000000, 120000000]
log_thread_counts = []

execution_times = {key: [] for key in executables}

for label, exe_path in executables.items():
    for P, E in zip(thread_counts, element_counts):
        result = subprocess.run(
            ["powershell", f"$env:OMP_NUM_THREADS={P}; .\\{exe_path} {E}"],
            capture_output=True, text=True
        )

        for line in result.stdout.splitlines():
            if "Elapsed time (t) =" in line:
                time_taken = float(line.split("=")[1].strip())
                execution_times[label].append(time_taken)
                break

print(execution_times)

log_thread_counts = np.log2(thread_counts)

plt.figure(figsize=(12, 6))

for label, times in execution_times.items():
    plt.plot(log_thread_counts, times, marker='o', linestyle='-', markersize=8, label=label)

plt.title("Elapsed Time vs. log(Number of Threads) and Element count", fontsize=16)
plt.xlabel("log(Number of Threads, P)", fontsize=14)
plt.ylabel("Elapsed Time, t", fontsize=14)
plt.grid(True)
plt.legend()

plt.show()
