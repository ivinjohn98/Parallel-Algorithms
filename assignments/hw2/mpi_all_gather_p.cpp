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

  int nextP = (rank + 1) % world_size;
  int prevP = (rank - 1 + world_size) % world_size;
  
  data_gattered.push_back(data);
  int next_data_to_send = data;
  int recv_buffer;
  
  for (size_t i = 0; i < world_size - 1; ++i) { // P-1 iterations
    MPI_Send(&next_data_to_send, 1, MPI_INT, nextP, 0, MPI_COMM_WORLD);
    MPI_Recv(&recv_buffer, 1, MPI_INT, prevP, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    data_gattered.push_back(recv_buffer);
    next_data_to_send =  recv_buffer;
  }
  if (rank == 2) {
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
