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
  std::vector<int> data_gattered;
  
  // Synchronize all processes to start communication at the same time
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime(); // start time

  // calculate positions in the brinary tree
  int parent = (rank - 1) / 2;
  int lchild = 2 * rank + 1;
  int rchild = 2 * rank + 2;

  std::vector<int> recv_buffer;
  if (lchild < world_size)
  {
    int lsize;
    // Receive the size of data from the left child
    MPI_Recv(&lsize, 1, MPI_INT, lchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    recv_buffer.resize(lsize); 
    // Receive the actual data from the left child
    MPI_Recv(recv_buffer.data(), lsize, MPI_INT, lchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    data_gattered.insert(data_gattered.end(), recv_buffer.begin(), recv_buffer.end());
  }
  if (rchild < world_size)
  {
    int rsize;
    // Receive the size of data from the right child
    MPI_Recv(&rsize, 1, MPI_INT, rchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    recv_buffer.resize(rsize);
    // Receive the actual data from the right child
    MPI_Recv(recv_buffer.data(), rsize, MPI_INT, rchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    data_gattered.insert(data_gattered.end(), recv_buffer.begin(), recv_buffer.end());
  }
  data_gattered.push_back(data);

  // If the process is not the root -> send the gathered data back to the parent
  if (rank != 0)
  {
    int gathered_size = data_gattered.size();
    // Send the size of the gathered data to the parent
    MPI_Send(&gathered_size, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
    // Send the actual gathered data to the parent
    MPI_Send(data_gattered.data(), gathered_size, MPI_INT, parent, 0, MPI_COMM_WORLD);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  
  if (rank == 0)
  {
    
    double end_time = MPI_Wtime(); // end time
    std::cout << "Number of elements (N) = " << world_size << std::endl;
    std::cout << "Number of Threads (P) = " << world_size << std::endl;
    std::cout << "Elapsed time (t) = " << end_time - start_time << std::endl;
    std::cout << "Rank 0 gathered data: ";
    for (int num : data_gattered)
    {
      std::cout << num << " ";
    }
    std::cout << std::endl;
  }

  // Finalize MPI
  MPI_Finalize();
}
