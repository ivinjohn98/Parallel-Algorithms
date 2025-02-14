import subprocess

executables_list = [
    {"Impl reduce": "mpi_reduce_logp", "Std reduce": "mpi_reduce_logp_std"},
    {"Impl gather": "mpi_gather_logp", "Std gather": "mpi_gather_logp_std"},
    {"Impl broadcast O(P)": "mpi_broadcast_p", "Std broadcast": "mpi_broadcast_logp_std", "Impl broadcast O(log(P))": "mpi_broadcast_logp"},
    {"Impl all gather": "mpi_all_gather_p", "Std all gather": "mpi_all_gather_p_std"},
]

output_filename = "execution_times.txt"

with open(output_filename, "w") as f:
    for executables in executables_list:

        thread_counts = [1, 2, 3, 4, 5, 6, 7, 8]
        element_counts = [1, 2, 3, 4, 5, 6, 7, 8]
        
        execution_times = {key: [] for key in executables.keys()}
        for label, exe in executables.items():
            for P, E in zip(thread_counts, element_counts):
                result = subprocess.run(
                    ["mpirun", "-np", str(P), exe],
                    stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True
                )
                for line in result.stdout.splitlines():
                    if "Elapsed time (t) =" in line:
                        time_taken = float(line.split("=")[1].strip())
                        execution_times[label].append(time_taken)
                        break

        print(execution_times)
        f.write(str(execution_times) + "\n")

print(f"Execution times saved to {output_filename}")
