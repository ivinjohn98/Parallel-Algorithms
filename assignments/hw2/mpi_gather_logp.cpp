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

  int parent = (rank - 1) / 2;
  int lchild = 2 * rank + 1;
  int rchild = 2 * rank + 2;

  std::vector<int> recv_buffer;

  if (lchild < world_size)
  {
    int lsize;
    MPI_Recv(&lsize, 1, MPI_INT, lchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    recv_buffer.resize(lsize);
    MPI_Recv(recv_buffer.data(), lsize, MPI_INT, lchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    data_gattered.insert(data_gattered.end(), recv_buffer.begin(), recv_buffer.end());
  }
  if (rchild < world_size)
  {
    int rsize;
    MPI_Recv(&rsize, 1, MPI_INT, rchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    recv_buffer.resize(rsize);
    MPI_Recv(recv_buffer.data(), rsize, MPI_INT, rchild, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    data_gattered.insert(data_gattered.end(), recv_buffer.begin(), recv_buffer.end());
  }
  data_gattered.push_back(data);

  if (rank != 0)
  {
    int gathered_size = data_gattered.size();
    MPI_Send(&gathered_size, 1, MPI_INT, parent, 0, MPI_COMM_WORLD);
    MPI_Send(data_gattered.data(), gathered_size, MPI_INT, parent, 0, MPI_COMM_WORLD);
  }
  else
  {
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
