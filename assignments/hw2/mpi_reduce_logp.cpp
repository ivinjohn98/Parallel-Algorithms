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

  int data = std::rand();
  int data_lchild = -1;
  int data_rchild = -1;

  int parent = (rank - 1) / 2;
  int lchild = 2 * rank + 1;
  int rchild = 2 * rank + 2;

  int data_max = data;
  if (lchild < world_size) {
    MPI_Recv(&data_lchild, 1, MPI_INT, lchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (data_lchild > data_max) {
      data_max = data_lchild;
    }
  }
  if (rchild < world_size) {
    MPI_Recv(&data_rchild, 1, MPI_INT, rchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (data_rchild > data_max) {
      data_max = data_rchild;
    }
  }

  if (rank != 0)
  {
    MPI_Send(&data_max, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
  }

  std::cout << "Rank " << rank << " data = " << data << " data max = " << data_max << std::endl;

  // Finalize MPI
  MPI_Finalize();
}