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

  //EREW O(log(P))
  int global_max = -1;
  MPI_Allreduce(&my_max, &global_max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

  
  std::cout << "global_max = " << global_max << std::endl;


  // Finalize MPI
  MPI_Finalize();
}