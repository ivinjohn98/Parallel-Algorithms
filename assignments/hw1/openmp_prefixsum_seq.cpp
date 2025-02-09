#include <cmath> // for std::log2
#include <iostream>
#include <memory>
#include <numeric> // for std::partial_sum
#include <omp.h>

int main(int argc, char* argv[])
{
  size_t N = std::stoi(argv[1]);
  std::unique_ptr<int[]> data = std::make_unique<int[]>(N);
  std::unique_ptr<int[]> prefix_sum = std::make_unique<int[]>(N);

#pragma omp parallel for
  for (size_t i = 0; i < N; ++i)
  {
    data[i] = 1;
  }

  // Algorithm starts here!
  // Initialization phase
  size_t P = omp_get_max_threads();
  size_t max_threads;

  //On a EREW O(N))
  double start_time = omp_get_wtime();
  for (size_t i = 1; i < N; ++i)
  {
    data[i] = data[i]+ data[i-1];
  }
  // std::partial_sum(data.get(), data.get() + N, data.get());
  double end_time = omp_get_wtime();

  // printing the time elapsed
  std::cout << "Number of elements (N) = " << N << std::endl;
  std::cout << "Number of Threads (P) = " << P << std::endl;
  std::cout << "Elapsed time (t) = " << end_time - start_time << std::endl;

  // for (size_t i = 0; i < N; ++i) {
  //   std::cout << data[i] << " ";
  // }
  // std::cout << std::endl;
}