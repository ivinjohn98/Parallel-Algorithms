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
  std::vector<int> recv_buffer;
  data_gattered.push_back(data);
  
  for (size_t i = 0; i < world_size - 1; ++i) { // P-1 iterations
    int gathered_size = data_gattered.size();
    MPI_Send(data_gattered.data(), gathered_size, MPI_INT, nextP, 0, MPI_COMM_WORLD);
    recv_buffer.resize(gathered_size);
    MPI_Recv(recv_buffer.data(), gathered_size, MPI_INT, nextP, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    data_gattered.insert(data_gattered.end(), recv_buffer.begin(), recv_buffer.end());
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