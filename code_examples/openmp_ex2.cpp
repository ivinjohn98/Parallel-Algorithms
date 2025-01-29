#include <iostream>
#include <omp.h>
#include <vector>

// Find max integer in vector

int main() {

  std::vector<int> vec = {8, 6, 7, 5, 3, 0, 9};

  // shared variable
  int max_found = -1;

#pragma omp parallel
  {
    // private variable
    int my_max = -1;

    // CREW runtime is O(N/P)
#pragma omp  for
    for (size_t i = 0; i < vec.size(); ++i) {
      my_max = std::max(my_max, vec[i]);
    }
    // CREW runtime is O(P)
#pragma omp critical
    { max_found = std::max(max_found, my_max); }
  } //parallelism joins here

  std::cout << "Max found = " << max_found << std::endl;
}