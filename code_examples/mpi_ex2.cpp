#include <iostream>
#include <mpi.h>
#include <vector>

int main(int argc, char **argv) {
  // Initialize MPI
  MPI_Init(&argc, &argv);

  // Get the rank of the process
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Get the total number of ranks
  int size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  std::vector<int> data = {rank * 1, rank * 2, rank * 3};

  //
  // EREW PRAM O(N/P)
  int my_max = -1;
  for (auto val : data) {
    my_max = std::max(val, my_max);
  }



  std::cout << "Rank " << rank << " my_max = " << my_max << std::endl;


  //
  // max value reduction in O(P) on EREW
  if (rank != 0) {
    // send my_max to rank 0
    MPI_Send(&my_max, 1, MPI_INT, 0, 42, MPI_COMM_WORLD);
  } else {
    // receive from other ranks
    for (int sender = 1; sender < size; ++sender) {
      int tmp;
      MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      my_max = std::max(my_max, tmp);
    }
  }

  if(rank == 0) {
    std::cout << "Max = " << my_max << std::endl;
  } else {
    std::cout << "Are we there yet?" <<std::endl;
  }

  // Finalize MPI
  MPI_Finalize();
}