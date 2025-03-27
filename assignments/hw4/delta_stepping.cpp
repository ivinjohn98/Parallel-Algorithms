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
  std::vector<std::set<int>> buckets;
  
  vert_info() : buckets(42 / 3 + 1) {}  // Initialize in constructor
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

void add_src_to_bucket(graph_type &graph, int src, int bucket_index) {
  auto inserter = [](int src, vert_info &vi, int bucket_index) {
    vi.buckets[bucket_index].insert(src);
  };
  graph.async_visit(src, inserter, bucket_index);
}

void relax(int v, int new_dist, int delta, graph_type &graph, ygm::comm &world) {
  // Define the relaxer lambda to match async_visit signature
  auto relaxer = [](int v, vert_info &vi, int new_dist, int delta) {
    if (new_dist < vi.dist) {
      int new_bucket = new_dist / delta;
      int old_bucket = vi.dist / delta;
      if (new_bucket != old_bucket) {
        // Find and remove the vertex from the old bucket
        vi.buckets[old_bucket].erase(v);
        // Add the vertex to the new bucket
        vi.buckets[new_bucket].insert(v);
      }
      // Update the distance with the new estimate.
      vi.dist = new_dist;
    }
  };

  // Call async_visit, passing the relaxer as the visitor function
  graph.async_visit(v, relaxer, new_dist, delta);
}

void delta_stepping(graph_type &graph, int source, int delta, ygm::comm &world) {
  relax(source, 0, delta, graph, world);
  int current_bucket_index = 0;
  while (current_bucket_index < (42/3)) {
    std::set<int> light_relax;
    world.barrier();
    while (true) {
      bool local_bucket_found = false;
      graph.for_all([&current_bucket_index, &local_bucket_found](int v, vert_info &vi) {
        if (!vi.buckets[current_bucket_index].empty()) {
          local_bucket_found = true;
        }
      });
      world.barrier();
      bool global_bucket_found = world.all_reduce(local_bucket_found, std::logical_or<bool>());
      if (!global_bucket_found)
        break;
      
      graph.for_all([&](int src, vert_info &vi) {
        if (vi.buckets[current_bucket_index].contains(src)) {
          for (size_t i = 0; i < vi.light_edges.size(); i++) {
            int neighbor = vi.light_edges[i];
            int weight = vi.light_edges_weights[i];
            relax(neighbor, vi.dist + weight, delta, graph, world);
          }
          light_relax.insert(src);
          vi.buckets[current_bucket_index].erase(src);
        }
      });
      world.barrier();
    }
    
    /*world.barrier();
    world.cout("Here we go:");
    graph.for_all([&world](int src, vert_info &vi){
      world.cout(src, " -> dist: ", vi.dist);
    });*/
    world.barrier();
    
    graph.for_all([&](int v, vert_info &vi) {
      if (light_relax.contains(v)) {
        for (size_t i = 0; i < vi.heavy_edges.size(); i++) {
          int neighbor = vi.heavy_edges[i];
          int weight = vi.heavy_edges_weights[i];
          relax(neighbor, vi.dist + weight, delta, graph, world);
        }
      }
    });
    current_bucket_index++;
  }
}

int main(int argc, char **argv) {
  ygm::comm world(&argc, &argv);
  world.welcome();

  graph_type graph(world);
  int delta = 3;
  
  if(world.rank0()) {
    add_edge(graph, 0, 1, 4, delta);
    add_edge(graph, 0, 3, 8, delta);
    add_edge(graph, 1, 3, 11, delta);
    add_edge(graph, 1, 2, 3, delta);
    add_edge(graph, 2, 4, 2, delta);
    add_edge(graph, 3, 4, 7, delta);
    add_edge(graph, 4, 5, 6, delta);
    add_edge(graph, 3, 5, 1, delta);
  }
  
  world.barrier();
  
  
  for (int src = 0; src < 6; src++) {
    add_src_to_bucket(graph, src, ((42/delta) + 1) - 1);
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
  
  world.barrier();
  
  delta_stepping(graph, 0, delta, world);  // Run from source vertex 0
  
  world.barrier();
  
  // Print results
  graph.for_all([&world](int src, vert_info &vi){
    world.cout(src, " -> dist: ", vi.dist);
  });

  return 0;
}
