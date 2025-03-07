#include <ygm/comm.hpp>

int main(int argc, char** argv) {
 
  ygm::comm world(&argc, &argv);
  world.welcome();

  // NOTE:   You may either use raw MPI or YGM to for this assignment
  //
  // WARNING:   This is a challenging assignment, start early!
  //
  // TODO:   
  // * Take as arguments # vertices (N),  # edges (M) & Max edge weight (W)
  // * Generate M edges uniformly at random between the N vertices
  //   with edge weights sampled from [1,W)
  // * Store edges in a 1D partitioned adjacency list using either a 
  //   ygm::container::map    or    ygm::container::array
  // * Implement Delta Stepping BFS

  //
  // OPTIONAL:   You may also elect to read in an edge list from a file
  //             and you could use ygm::io::csv_parser or ygm::io::line_parser
  //             to do this.

}