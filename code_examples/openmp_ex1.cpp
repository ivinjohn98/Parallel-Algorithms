#include <iostream>
#include <omp.h>

int main() {
  std::cout << "Howdy from the main thread" << std::endl;

  int threads_i_want = 7;
// OpenMP is fork/join
#pragma omp parallel num_threads(threads_i_want)
  {
    //private thread variables 
    int thread_id = omp_get_thread_num(); 
    int num_threads = omp_get_num_threads();
#pragma omp critical
    {
      std::cout << "Howdy from " << thread_id << " of " << num_threads
                << std::endl;
    }
  }
}