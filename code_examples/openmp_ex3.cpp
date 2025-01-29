#include <iostream>
#include <omp.h>
#include <vector>

// Find max integer in vector

int main() {

  std::vector<int> vec = {8, 6, 7, 5, 3, 0, 9};

  // shared variable
  int max_found = -1;
  int max_i_care_about = 5;

  //CREW runtime is O(N/P + log(P))
#pragma omp parallel for reduction(max : max_found)
  for (size_t i = 0; i < vec.size(); ++i) {
      max_found = std::max(max_found, vec[i]);
  }

  std::cout << "Max found = " << max_found << std::endl;
}