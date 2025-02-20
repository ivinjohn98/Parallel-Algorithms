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

  MPI_Barrier(MPI_COMM_WORLD);

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

  int k = std::stoi(argv[1]);
  int number_of_elements_per_proc = std::stoi(argv[2]);

  // std::cout << "Howdy from " << world_rank << " of " << world_size << std::endl;

  std::mt19937 gen(world_rank); 
  std::uniform_int_distribution<> distrib(0, 10000000);

  std::vector<int> local_data;
  for(int i = 0; i< number_of_elements_per_proc; ++i) {
    local_data.push_back(distrib(gen));
  }

  // print_in_order(local_data);

  //
  // Sample P pivots   O(1)
  // TODO Homework 3 -- Sample k pivots per process
  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();
  std::uniform_int_distribution<> sampler(0, local_data.size()-1);
  std::vector<int> local_sampled_pivots;
  for (int i=0; i< k; i++) {
    int local_sampled_pivot = local_data[sampler(gen)];
    local_sampled_pivots.push_back(local_sampled_pivot);
  }

  std::vector<int> k_pivots(world_size * k, 0);
  
  //
  // Sharing of pivots O(P)

  MPI_Allgather(local_sampled_pivots.data(), k, MPI_INT, 
    k_pivots.data(), k, MPI_INT, MPI_COMM_WORLD);
  //
  // Assume Quicksort:   O(P*lg(P))   
  std::sort(k_pivots.begin(), k_pivots.end());

  //
  // TODO Homework 3 -- Pick "best" P-1 pivots from oversampled
  std::vector<int> pivots;
  int step = (k * world_size) / world_size;  // Evenly space the selection
  for (int i = step; i < k * world_size; i += step) {
      pivots.push_back(k_pivots[i]);
  }

  // if(world_rank == 2) {
  //   std::cout << "PIVOTS = ";
  //   for(int p : pivots) {
  //     std::cout << p << " ";
  //   }
  //   std::cout << std::endl;
  // }

  std::vector< std::vector<int> > send_bufs(world_size);
  
  //
  // Creating send buckets:  O(N/P * lg(P))
  for(int val : local_data) {
    auto lbi = std::lower_bound(pivots.begin(), pivots.end(), val);
    int dest_rank = std::distance(pivots.begin(), lbi);

    send_bufs.at(dest_rank).push_back(val);
  }

  // if(world_rank == 0) {
  //   for(int dr = 0; dr < world_size; ++dr) {
  //     std::cout << "Sending to " << dr << ": ";
  //     for(int val : send_bufs[dr]) {
  //       std::cout << val << " ";
  //     }
  //     std::cout << std::endl;
  //   }
  // }

  std::vector< std::vector<int> > recv_bufs(world_size);
  std::vector< int > size_recv(world_size, 0);
  std::vector< MPI_Request > size_recv_req(world_size);
  // Sets up P MPI_Irecvs to get future send size O(P)
  for(int i = 0; i < world_size; ++i) {
    MPI_Irecv(&size_recv[i], 1, MPI_INT, i,
              0, MPI_COMM_WORLD, &size_recv_req[i]);
  }
  // Send future send size to all ranks O(P)
  for(int i = 0; i < world_size; ++i) {
    int to_rank = (world_rank + i) % world_size;
    int to_send = send_bufs[to_rank].size();
    MPI_Send(&to_send, 1, MPI_INT, to_rank, 0, MPI_COMM_WORLD);
  }
  MPI_Waitall(size_recv_req.size(), size_recv_req.data(), 
    MPI_STATUS_IGNORE);

  // Post Irecv for every sending rank O(P)
  std::vector< MPI_Request > data_recv_req(world_size);
  for(int i = 0; i < world_size; ++i) {
    recv_bufs.at(i).resize(size_recv.at(i));
    MPI_Irecv(recv_bufs.at(i).data(), size_recv.at(i), MPI_INT, i,
              0, MPI_COMM_WORLD, &data_recv_req[i]);
  }
  // Send data to every rank O(P)
  for(int i = 0; i < world_size; ++i) {
    int to_rank = (world_rank + i) % world_size;
    MPI_Send(send_bufs[to_rank].data(), send_bufs[to_rank].size(), 
             MPI_INT, to_rank, 0, MPI_COMM_WORLD);
  }
  MPI_Waitall(data_recv_req.size(), data_recv_req.data(), 
    MPI_STATUS_IGNORE);

  //  If balanced:   O(N/P)
  std::vector<int> sorted_data;
  for(int i = 0; i < world_size; ++i) {
    for(int val : recv_bufs[i]){
      sorted_data.push_back(val);
    }
  }

  // If baalanced:   O(N/P * lg(N/P))
  MPI_Barrier(MPI_COMM_WORLD);
  std::sort(sorted_data.begin(), sorted_data.end());
  MPI_Barrier(MPI_COMM_WORLD);

  // print_in_order(sorted_data);
  // MPI_Barrier(MPI_COMM_WORLD);

  //OVERALL SO FAR, if N>>P & data is balanced:   O(N/P * lg(N/P))

  //TODO Homework 3 -- balance sorted_data vector

  // My code starts here:
  int ps_send = sorted_data.size();
  int ps_recv = 0;

  // MPI provided function for Prefix Sum.
  MPI_Exscan(&ps_send, &ps_recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

  int ideal_N_by_P = number_of_elements_per_proc * world_size / world_size;

  std::vector< std::vector<int> > send_bufs_balance(world_size);

  // Creating send buckets:  O(N/P * lg(P))
  for (size_t i = 0; i < sorted_data.size(); ++i) {
    send_bufs_balance.at((ps_recv + i)/ideal_N_by_P).push_back(sorted_data[i]);
  }

  std::vector< std::vector<int> > recv_bufs_balance(world_size);
  std::vector< int > size_recv_balance(world_size, 0);
  std::vector< MPI_Request > size_recv_req_balance(world_size);
  // Sets up P MPI_Irecvs to get sorted data size O(P)
  for(int i = 0; i < world_size; ++i) {
    MPI_Irecv(&size_recv_balance[i], 1, MPI_INT, i,
              0, MPI_COMM_WORLD, &size_recv_req_balance[i]);
  }

  // Send sorted data size to all ranks O(P)
  for(int i = 0; i < world_size; ++i) {
    int to_rank = (world_rank + i) % world_size;
    int to_send = send_bufs_balance[to_rank].size();
    MPI_Send(&to_send, 1, MPI_INT, to_rank, 0, MPI_COMM_WORLD);
  }

  MPI_Waitall(size_recv_req_balance.size(), size_recv_req_balance.data(), 
    MPI_STATUS_IGNORE);

  // Post Irecv for every sending rank O(P)
  std::vector< MPI_Request > data_recv_req_balance(world_size);
  for(int i = 0; i < world_size; ++i) {
    recv_bufs_balance.at(i).resize(size_recv_balance.at(i));
    MPI_Irecv(recv_bufs_balance.at(i).data(), size_recv_balance.at(i), MPI_INT, i,
              0, MPI_COMM_WORLD, &data_recv_req_balance[i]);
  }
  // Send data to every rank O(P)
  for(int i = 0; i < world_size; ++i) {
    int to_rank = (world_rank + i) % world_size;
    MPI_Send(send_bufs_balance[to_rank].data(), send_bufs_balance[to_rank].size(), 
             MPI_INT, to_rank, 0, MPI_COMM_WORLD);
  }

  MPI_Waitall(data_recv_req_balance.size(), data_recv_req_balance.data(), 
    MPI_STATUS_IGNORE);

  std::vector<int> sorted_data_balance;
  for(int i = 0; i < world_size; ++i) {
    for(int val : recv_bufs_balance[i]) {
      sorted_data_balance.push_back(val);
    }
  }
  
  // my code ends here.

  MPI_Barrier(MPI_COMM_WORLD);
  double end_time = MPI_Wtime();
  
  print_in_order(sorted_data_balance);

  if (world_rank == 0) {
    std::cout << "time elapsed: " << end_time - start_time << " , k_value: " << k << std::endl;
  }
  // Finalize MPI
  MPI_Finalize();
}