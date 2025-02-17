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

  int data = rank;
  int data_lchild = -1;
  int data_rchild = -1;

  int parent = (rank - 1) / 2;
  int lchild = 2 * rank + 1;
  int rchild = 2 * rank + 2;

  int data_max = data;

  // Synchronize all processes to start communication at the same time
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime(); // start time
  // If the process has a left child, receive the max data from the left child
  if (lchild < world_size)
  {
    // Receive the max data from the left child process
    MPI_Recv(&data_lchild, 1, MPI_INT, lchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (data_lchild > data_max)
    {
      data_max = data_lchild;
    }
  }
  // If the process has a right child -> receive the max data from the right child
  if (rchild < world_size)
  {
    // Receive the max data from the right child process
    MPI_Recv(&data_rchild, 1, MPI_INT, rchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (data_rchild > data_max)
    {
      data_max = data_rchild;
    }
  }

  // If the process is not the root -> send the max data back to the parent
  if (rank != 0)
  {
    MPI_Send(&data_max, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
  }
  // Synchronize all processes to start communication at the same time
  MPI_Barrier(MPI_COMM_WORLD);
  double end_time = MPI_Wtime(); // end time

  if (rank == 0)
  {
    std::cout << "Number of elements (N) = " << world_size << std::endl;
    std::cout << "Number of Threads (P) = " << world_size << std::endl;
    std::cout << "Elapsed time (t) = " << end_time - start_time << std::endl;
  }

  std::cout << "Rank " << rank << " data = " << data << " data max = " << data_max << std::endl;

  // Finalize MPI
  MPI_Finalize();
}