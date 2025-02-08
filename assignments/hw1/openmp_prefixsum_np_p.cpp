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

  //On a CREW O(N/P * lg(N))
  double start_time = omp_get_wtime();
  for (size_t outer = 0; outer < std::log2(N); ++outer) {
#pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
      if (i < (1 << outer)) { // (1 << outer) is 2^outer
        prefix_sum[i] = data[i];
      } else {
        prefix_sum[i] = data[i] + data[i - (1 << outer)];
      }
    }
    std::swap(data, prefix_sum);
  }
  double end_time = omp_get_wtime();

  // printing the time elapsed
  std::cout << "Number of elements (N) = " << N << std::endl;
  std::cout << "Number of Threads (P) = " << P << std::endl;
  std::cout << "Elapsed time (t) = " << end_time - start_time << std::endl;

  // for (size_t i = 0; i < size; ++i) {
  //   std::cout << data[i] << " ";
  // }
  // std::cout << std::endl;
}