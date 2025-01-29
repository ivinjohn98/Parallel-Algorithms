#include <cmath> // for std::log2
#include <iostream>
#include <memory>
#include <numeric> // for std::partial_sum
#include <omp.h>

int main() {
  size_t size = 1000000;
  std::unique_ptr<int[]> data = std::make_unique<int[]>(size);
  std::unique_ptr<int[]> tmp_data = std::make_unique<int[]>(size);

#pragma omp parallel for
  for (size_t i = 0; i < size; ++i) {
    data[i] = 1;
  }

  //On a CREW O(N/P * lg(N))
  // Can we get this to O( N/P  + lg(P) )  ?
  double start_time = omp_get_wtime();
  for (size_t outer = 0; outer < std::log2(size); ++outer) {
#pragma omp parallel for
    for (size_t i = 0; i < size; ++i) {
      if (i < (1 << outer)) { // (1 << outer) is 2^outer
        tmp_data[i] = data[i];
      } else {
        tmp_data[i] = data[i] + data[i - (1 << outer)];
      }
    }
    std::swap(data, tmp_data);
  }
  double end_time = omp_get_wtime();

  std::cout << "Elapsed time = " << end_time - start_time << std::endl;

  // for (size_t i = 0; i < size; ++i) {
  //   std::cout << data[i] << " ";
  // }
  std::cout << std::endl;
}