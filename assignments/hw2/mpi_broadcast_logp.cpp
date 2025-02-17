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
  // The process with rank 0 has data 42
  if (rank == 0)
  {
    data = 42;
  }

  // Synchronize all processes before starting the communication
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime(); // start time

  // tree node positions calculated here
  int parent = (rank - 1) / 2;
  int lchild = 2 * rank + 1;
  int rchild = 2 * rank + 2;

  // If the process is not root -> receive data from the parent
  if (rank != 0)
  {
    MPI_Recv(&data, 1, MPI_INT, parent, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }
  // If the left child exists -> send data to the left child
  if (lchild < world_size)
  {
    MPI_Send(&data, 1, MPI_INT, lchild, 0, MPI_COMM_WORLD);
  }
  // If the right child exists -> send data to the right child
  if (rchild < world_size)
  {
    MPI_Send(&data, 1, MPI_INT, rchild, 0, MPI_COMM_WORLD);
  }
  
  // Synchronize all processes again after communication is done
  MPI_Barrier(MPI_COMM_WORLD);

  double end_time = MPI_Wtime(); // end time

  if (rank == 0)
  {
    std::cout << "Number of elements (N) = " << world_size << std::endl;
    std::cout << "Number of Threads (P) = " << world_size << std::endl;
    std::cout << "Elapsed time (t) = " << end_time - start_time << std::endl;
  }

  std::cout << "Rank " << rank << " data = " << data << std::endl;

  // Finalize MPI
  MPI_Finalize();
}