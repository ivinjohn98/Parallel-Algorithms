#include <chrono>
#include <iostream>
#include <omp.h>
#include <thread>

int main() {

  //Strong Scaling Example
  size_t num_sleeps = 16;
  for (int p = 1; p < 20; p *= 2) {
    double start_time = omp_get_wtime();
#pragma omp parallel for num_threads(p)
    for (size_t i = 0; i < num_sleeps; ++i) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
    } // implied thread barrier here
    double end_time = omp_get_wtime();
    std::cout << "Num Threads = " << p
              << " Elapsed time = " << end_time - start_time 
              << std::endl;
  }

  #pragma parallel num_threads(4)
  {
    #pragma omp barrier
    double start_time = omp_get_wtime();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    #pragma omp barrier
    double end_time = omp_get_wtime();
  }


}