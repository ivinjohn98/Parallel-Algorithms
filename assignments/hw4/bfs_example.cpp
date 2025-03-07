#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/bag.hpp>
#include <vector>

struct vert_info {

  template <class Archive>
  void serialize( Archive & ar )
  {
    ar(bfs_level, adj);
  }

  int bfs_level = std::numeric_limits<int>::max();
  std::vector<int> adj;

};

using graph_type = ygm::container::map<int, vert_info >;


void add_edge(graph_type& graph,
              int src, int dest) {

  auto inserter = [](int src, vert_info& vi, int dest) {
    vi.adj.push_back(dest);
  };

  graph.async_visit(src, inserter, dest);
  graph.async_visit(dest, inserter, src);
}


int main(int argc, char** argv) {
 
  ygm::comm world(&argc, &argv);
  world.welcome();

  //key must have std::hash<key>
  //value must be compatible with Cereal
  graph_type graph(world);

  if(world.rank0()) {
    add_edge(graph, 1, 2);
    add_edge(graph, 1, 3);
    add_edge(graph, 2, 4);
    add_edge(graph, 3, 4);
    add_edge(graph, 4, 5);
  }

  ygm::container::map<int, int> bfs_level(world);

  graph.for_all([&world, &bfs_level](int src, vert_info& vi){
    for(auto dest : vi.adj) {
      world.cout(src, " -> ", dest);
    }
  });


  ygm::container::bag<int> cur_level(world);
  ygm::container::bag<int> next_level(world);
  static int cur_bfs_level = 0;
  if(world.rank0()){
    cur_level.async_insert(1);
  }
  
  static ygm::container::bag<int>& s_next_level = next_level;
  static graph_type& s_graph = graph;
  static ygm::comm& s_world = world;


  do {
    // For each vertex in the current level
    cur_level.for_all([](int vert) {
      auto bfs_visitor = [](int vert, vert_info& vi) {

        if(cur_bfs_level < vi.bfs_level) {
          vi.bfs_level = cur_bfs_level;
          for(int neighbor : vi.adj) {
            s_next_level.async_insert(neighbor);
          }
        }
      };
      //Visit the vertex and check if it has been visited before
      s_graph.async_visit(vert, bfs_visitor);
    });

    // world.cout0("Printing next_level of size: ", next_level.size());
    // next_level.for_all([](int vert) {
    //   s_world.cout(vert);
    // });

    s_world.barrier();
    cur_level.swap(next_level);
    next_level.clear();
    ++cur_bfs_level;
    //Continue if there is new vertices to
  } while( cur_level.size() > 0);


  graph.for_all([](int vert, const vert_info& vi){
    s_world.cout(vert, " is on level ", vi.bfs_level);
  });


  return 0;
}