#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/bag.hpp>
#include <vector>

// Struct to store vertex information
struct vert_info {
  template <class Archive>
  void serialize( Archive & ar )
  {
    ar(dist, light_edges, heavy_edges);
  }
  
  int dist = std::numeric_limits<int>::max();
  std::vector<int> light_edges;
  std::vector<int> light_edges_weights;
  std::vector<int> heavy_edges;
  std::vector<int> heavy_edges_weights;
};

using graph_type = ygm::container::map<int, vert_info>;

// Function to add an edge to the graph
void add_edge(graph_type &graph, int src, int dest, int weight, int delta) {
  auto inserter = [](int src, vert_info &vi, int dest, int weight, int delta) {
    if (weight <= delta) {
      vi.light_edges.push_back(dest);
      vi.light_edges_weights.push_back(weight);
      vi.dist = 42;
    }
    else {
      vi.heavy_edges.push_back(dest);
      vi.heavy_edges_weights.push_back(weight);
      vi.dist = 42;
    }
  };
  graph.async_visit(src, inserter, dest, weight, delta); // Asynchronously add the edge to the graph
  graph.async_visit(dest, inserter, src, weight, delta);
}

void relax(std::vector<std::vector<int>> &buckets, int v, int new_dist, int delta, graph_type &graph, ygm::comm &world) {
  // Define the relaxer lambda to match async_visit signature
  auto relaxer = [new_dist, delta](int v, vert_info &vi, std::vector<std::vector<int>> buckets) {
    std::cout << "so far good2" << std::endl;
    if (new_dist < vi.dist) {
      int new_bucket = new_dist / delta;
      int old_bucket = vi.dist / delta;
      if (new_bucket != old_bucket) {
        // Find and remove the vertex from the old bucket
        auto &old_bucket_list = buckets[old_bucket];
        auto position = std::find(old_bucket_list.begin(), old_bucket_list.end(), v);
        if (position != old_bucket_list.end()) {
            old_bucket_list.erase(position);
        }
        // Add the vertex to the new bucket
        buckets[new_bucket].push_back(v);
      }
      // Update the distance
      int s = 0;
      //vi.dist = new_dist;
    }
  };

  // Call async_visit, passing the relaxer as the visitor function
  graph.async_visit(v, relaxer, buckets);
}

void delta_stepping(std::vector<std::vector<int>> &buckets, graph_type &graph, int source, int delta, ygm::comm &world) {
  relax(buckets, source, 0, delta, graph, world);
  /*graph.async_visit(source, [](int, vert_info &vi) {
    vi.dist = 0;  // Distance of source vertex is 0
  });*/
}

int main(int argc, char **argv) {
  ygm::comm world(&argc, &argv);
  world.welcome();

  graph_type graph(world);
  int delta = 3;
  
  std::vector<std::vector<int>> buckets((42/delta) + 1);
  if(world.rank0()) {
    add_edge(graph, 0, 1, 4, delta);
    add_edge(graph, 0, 3, 8, delta);
    add_edge(graph, 1, 3, 11, delta);
    add_edge(graph, 1, 2, 3, delta);
    add_edge(graph, 2, 4, 2, delta);
    add_edge(graph, 3, 4, 7, delta);
    add_edge(graph, 4, 5, 6, delta);
    add_edge(graph, 3, 5, 1, delta);
    
    for (int i = 0; i < 6; i++) {
      buckets[buckets.size()-1].push_back(i);
    }
  }
  
  // Synchronize to make sure all edges are added before printing
  /*world.barrier();
  graph.for_all([&world](int src, vert_info& vi){
    for(auto weight : vi.light_edges_weights) {
      world.cout(src, " weight - ", weight);
    }
  });
  
  world.barrier();
  graph.for_all([&world](int src, vert_info& vi){
    for(auto dest : vi.light_edges) {
      world.cout(src, " -> ", dest);
    }
  });*/
  
  if (world.rank0()) {
    delta_stepping(buckets, graph, 0, delta, world);  // Run from source vertex 0
  }
  
  world.barrier();
  
  // Print results
  graph.for_all([&world](int src, vert_info &vi){
    world.cout(src, " -> dist: ", vi.dist, "\n");
  });

  return 0;
}

