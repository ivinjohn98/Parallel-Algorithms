#include <iostream>
#include <numeric> // for std::partial_sum
#include <vector>
#include <cmath> // for std::log2
#include <omp.h>


int main() {
  //
  // Sequential initialization
  std::vector<int> data = {8,6,7,5,3,0,9};
  std::vector<int> stl_partial_sum = data;
  std::vector<int> my_prefix_sum = data;
  std::vector<int> my_tmp(data.size(),0);

  //
  // Sequential algorithm O( n * lg(n) )
  for (size_t outer = 0; outer < std::log2(my_prefix_sum.size()); ++outer) {
    for (size_t i = 0; i < my_prefix_sum.size(); ++i) {
      if (i < (1 << outer)) { // (1 << outer) is 2^outer
        my_tmp[i] = my_prefix_sum[i];
      } else {
        my_tmp[i] = my_prefix_sum[i] + my_prefix_sum[i - (1 << outer)];
      }
    }
    for (size_t i = 0; i < my_prefix_sum.size(); ++i) {
      my_prefix_sum[i] = my_tmp[i];
    }
  }

  //
  // Verification with known sequential algorithm O(N)
  std::partial_sum(stl_partial_sum.begin(), stl_partial_sum.end(), stl_partial_sum.begin());

  for (int v : data) {
    std::cout << v << " ";
  }
  std::cout << std::endl;

  for (int v : my_prefix_sum) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
  for (int v : stl_partial_sum) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
}