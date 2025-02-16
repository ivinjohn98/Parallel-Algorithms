import os
import subprocess

cpp_files = [
    "mpi_all_gather_p.cpp",
    "mpi_all_gather_p_std.cpp",
    "mpi_broadcast_logp.cpp",
    "mpi_broadcast_logp_std.cpp",
    "mpi_broadcast_p.cpp",
    "mpi_gather_logp.cpp",
    "mpi_gather_logp_std.cpp",
    "mpi_reduce_logp.cpp",
    "mpi_reduce_logp_std.cpp"
]

for cpp_file in cpp_files:
    executable_name = cpp_file.replace(".cpp", "")
    command = f"mpicxx -o {executable_name} {cpp_file}"
    print(f"Executing: {command}")

    try:
        subprocess.run(command, shell=True, check=True)
        print(f"Successfully compiled {cpp_file} -> {executable_name}")
    except subprocess.CalledProcessError as e:
        print(f"Error compiling {cpp_file}: {e}")

