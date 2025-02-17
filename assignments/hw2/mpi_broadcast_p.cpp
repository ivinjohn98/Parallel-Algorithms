#include <iostream>
#include <mpi.h>

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

  int data = -1;
  if (rank == 0)
  {
    data = 42;
  }

  // Parent process is the process with rank 0
  int parent = 0;

  // Synchronize all processes before starting the communication
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime(); // start time

  if (rank != 0)
  {
    // all other processes receive data from rank 0
    MPI_Recv(&data, 1, MPI_INT, parent, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  else
  {
    // Rank 0 sends data to all other processes
    for (int child = 1; child < world_size; child++)
      MPI_Send(&data, 1, MPI_INT, child, 0, MPI_COMM_WORLD);
  }

  // Synchronize all processes before starting the communication
  MPI_Barrier(MPI_COMM_WORLD);
  double end_time = MPI_Wtime(); // end time

  if (rank == 0)
  {
    std::cout << "Number of elements (N) = " << world_size << std::endl;
    std::cout << "Number of Threads (P) = " << world_size << std::endl;
    std::cout << "Elapsed time (t) = " << end_time - start_time << std::endl;
  }

  // std::cout << "Rank " << rank << " data = " << data << std::endl;

  // Finalize MPI
  MPI_Finalize();
}
