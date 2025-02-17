#include <iostream>
#include <mpi.h>
#include <vector>

int main(int argc, char **argv)
{
  // Initialize MPI
  MPI_Init(&argc, &argv);

  // Get the rank of the process
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Get the total number of ranks
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int data = rank;
  std::vector<int> data_gathered(world_size);

  double start_time = MPI_Wtime();
  MPI_Allgather(&data, 1, MPI_INT, data_gathered.data(), 1, MPI_INT, MPI_COMM_WORLD);
  double end_time = MPI_Wtime();

  if (rank == 2) {
    std::cout << "Rank 2 gathered data: ";
    for (int num : data_gathered)
    {
      std::cout << num << " ";
    }
    std::cout << std::endl;
  }
  if (rank == 0)
  {
    std::cout << "Number of elements (N) = " << world_size << std::endl;
    std::cout << "Number of Threads (P) = " << world_size << std::endl;
    std::cout << "Elapsed time (t) = " << end_time - start_time << std::endl;
  }

  // Finalize MPI
  MPI_Finalize();
}
