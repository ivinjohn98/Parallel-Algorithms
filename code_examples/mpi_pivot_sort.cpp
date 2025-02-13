#include <iostream>
#include <mpi.h>
#include <random>
#include <vector>
#include <algorithm>

int world_rank = -1;
int world_size = -1;
void print_in_order(const std::vector<int>& local_data) {
  // Assume local_data.size() could be different on each rank.
  int ps_send = local_data.size();
  int ps_recv = 0;

  // MPI provided function for Prefix Sum.
  MPI_Exscan(&ps_send, &ps_recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  //std::cout << "PS: " << world_rank << " " << ps_recv << std::endl;

  for(int token = 0;  token < world_size; ++token) {
    if(token == world_rank){
      for(size_t i = 0; i < local_data.size(); ++i) {
        std::cout << world_rank << ": index " << ps_recv + i 
                  << " = " << local_data[i] << std::endl;
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);   // O(lg(p))
  }
}



int main(int argc, char **argv) {
  // Initialize MPI
  MPI_Init(&argc, &argv);

  // Get the rank of the process
 
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  // Get the total number of ranks

  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  std::cout << "Howdy from " << world_rank << " of " << world_size << std::endl;



  std::mt19937 gen(world_rank); 
  std::uniform_int_distribution<> distrib(0, 100);
  
  std::vector<int> local_data;
  for(int i = 0; i<5; ++i) {
    local_data.push_back(distrib(gen));
  }


  print_in_order(local_data);


  // Sample P pivots
  std::uniform_int_distribution<> sampler(0, local_data.size()-1);
  int local_sampled_pivot = local_data[sampler(gen)];

  std::vector<int> pivots(world_size, 0);
  
  MPI_Allgather(&local_sampled_pivot, 1, MPI_INT,
    pivots.data(), 1, MPI_INT, MPI_COMM_WORLD);

  std::sort(pivots.begin(), pivots.end());
  pivots.pop_back();

  if(world_rank == 2) {
    std::cout << "PIVOTS = ";
    for(int p : pivots) {
      std::cout << p << " ";
    }
    std::cout << std::endl;
  }

  std::vector< std::vector<int> > send_bufs(world_size);
  


  for(int val : local_data) {
    auto lbi = std::lower_bound(pivots.begin(), pivots.end(), val);
    int dest_rank = std::distance(pivots.begin(), lbi);

    send_bufs.at(dest_rank).push_back(val);
  }

  if(world_rank == 0) {
    for(int dr = 0; dr < world_size; ++dr) {
      std::cout << "Sending to " << dr << ": ";
      for(int val : send_bufs[dr]) {
        std::cout << val << " ";
      }
      std::cout << std::endl;
    }
  }


  //TO BE CONTINUED




  // Finalize MPI
  MPI_Finalize();
}