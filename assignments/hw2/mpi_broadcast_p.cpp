#include <iostream>
#include <mpi.h>

int main(int argc, char **argv) {
  // Initialize MPI
  MPI_Init(&argc, &argv);

  // Get the rank of the process
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Get the total number of ranks
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  int data = -1;
  if (rank == 0) {
    data = 42;
  }

  int parent = (rank - 1) / 2;
  int child = 2 * rank + 1;

  if (rank != 0) {
    MPI_Recv(&data, 1, MPI_INT, parent, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  } else {
    MPI_Send(&data, 1, MPI_INT, child, 0, MPI_COMM_WORLD);
  }

  std::cout << "Rank " << rank << " data = " << data << std::endl;

  // Finalize MPI
  MPI_Finalize();
}