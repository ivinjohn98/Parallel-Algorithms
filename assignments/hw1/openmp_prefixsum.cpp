#include <cmath> // for std::log2
#include <iostream>
#include <memory>
#include <numeric> // for std::partial_sum
#include <omp.h>

int main()
{
  size_t N = 50;
  std::unique_ptr<int[]> data = std::make_unique<int[]>(N);
  std::unique_ptr<int[]> prefix_sum = std::make_unique<int[]>(N);

#pragma omp parallel for
  for (size_t i = 0; i < N; ++i)
  {
    data[i] = 1;
  }

  // Algorithm starts here!
  // Initialization phase
  size_t P;
#pragma omp parallel
  {
    P = omp_get_num_threads();
  }
  std::unique_ptr<int[]> block_sums = std::make_unique<int[]>(P);
  std::unique_ptr<int[]> block_sums_tmp = std::make_unique<int[]>(P);

  // Step 1: O(N/P) algorithm
#pragma omp parallel num_threads(P)
  {
    size_t tid = omp_get_thread_num();
    size_t chunk_size = (N + P - 1) / P; // Divide N among P threads
    size_t start = tid * chunk_size;
    size_t end = std::min(start + chunk_size, N);

    if (start < N)
    {
      prefix_sum[start] = data[start];
      for (size_t i = start + 1; i < end; ++i)
      {
        prefix_sum[i] = prefix_sum[i - 1] + data[i];
      }
      block_sums[tid] = prefix_sum[end - 1]; // Store the last element of each block
    }
  }

  // Step 2: O(log (P)) Algorithm
  for (size_t outer = 0; outer < std::log2(P); ++outer)
  {
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i)
    {
      if (i < (1 << outer))
      { // (1 << outer) is 2^outer
        block_sums_tmp[i] = block_sums[i];
      }
      else
      {
        block_sums_tmp[i] = block_sums[i] + block_sums[i - (1 << outer)];
      }
    }
    std::swap(block_sums, block_sums_tmp);
  }

  // Step 3: Final computation (O(N/P))
#pragma omp parallel
  {
    size_t tid = omp_get_thread_num();
    size_t chunk_size = (N + P - 1) / P;
    size_t start = tid * chunk_size;
    size_t end = std::min(start + chunk_size, N);

    if (tid > 0 && start < N)
    {
      size_t offset = block_sums[tid - 1];
      for (size_t i = start; i < end; ++i)
      {
        prefix_sum[i] += offset;
      }
    }
  }

  // Printing the result
  for (size_t i = 0; i < N; ++i)
  {
    std::cout << prefix_sum[i] << " ";
  }
  std::cout << std::endl;
}