#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <ygm/container/bag.hpp>
#include <vector>
#include <random>
#include <unordered_set>

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
  
  vert_info() {}  // Default constructor

};

using graph_type = ygm::container::map<int, vert_info>;
static std::vector<std::set<int>> local_buckets;
static std::set<int> light_relax;

static std::vector<int> local_bucket_neighbor;
static std::vector<int> local_bucket_neighbor_weight;
static std::vector<int> local_bucket_v;
static std::vector<int> local_bucket_dist;

void intialize_vertex_info(graph_type &graph, int sum_weights) {
  graph.for_all([&](int src, vert_info &vi) {
    vi.dist = sum_weights;
  });
}

void intialize_buckets(graph_type &graph, int sum_weights, int delta, int bucket_index) {
  graph.for_all([&](int src, vert_info &vi) {
    local_buckets.resize((sum_weights / delta) + 1);
    local_buckets[bucket_index].insert(src);
  });
}

// Function to add an edge to the graph
void add_edge(graph_type &graph, int src, int dest, int weight, int delta) {
  auto inserter = [](int src, vert_info &vi, int dest, int weight, int delta) {
    if (weight <= delta) {
      vi.light_edges.push_back(dest);
      vi.light_edges_weights.push_back(weight);
    }
    else {
      vi.heavy_edges.push_back(dest);
      vi.heavy_edges_weights.push_back(weight);
    }
  };
  graph.async_visit(src, inserter, dest, weight, delta); // Asynchronously add the edge to the graph
  graph.async_visit(dest, inserter, src, weight, delta);
}

void relax(int v, int new_dist, int delta, graph_type &graph, ygm::comm &world) {
  // Define the relaxer lambda to match async_visit signature
  auto relaxer = [&](int v, vert_info &vi, int new_dist, int delta) {
    if (new_dist < vi.dist) {
      int new_bucket = new_dist / delta;
      int old_bucket = vi.dist / delta;
      if (new_bucket != old_bucket) {
        // Find and remove the vertex from the old bucket
        local_buckets[old_bucket].erase(v);
        // Add the vertex to the new bucket
        local_buckets[new_bucket].insert(v);
      }
      // Update the distance with the new estimate.
      vi.dist = new_dist;
    }
  };

  // Call async_visit, passing the relaxer as the visitor function
  graph.async_visit(v, relaxer, new_dist, delta);
}

void delta_stepping(graph_type &graph, int source, int delta, ygm::comm &world, int sum_weights) {
  relax(source, 0, delta, graph, world);
  int current_bucket_index = 0;
  while (current_bucket_index < (sum_weights/delta)) {
    light_relax.clear();
    world.barrier();
    while (true) {
      bool local_bucket_found = false;
      if (!local_buckets[current_bucket_index].empty()) {
        local_bucket_found = true;
      }
      
      bool global_bucket_found = world.all_reduce(local_bucket_found, std::logical_or<bool>());
      if (!global_bucket_found)
        break;
      
      std::set<int> local_buckets_copy(local_buckets[current_bucket_index].begin(), local_buckets[current_bucket_index].end());
      
      /*world.cout("Contents of Local bucket: {");
      for (int v : local_buckets_copy) {
        world.cout(v, " ");
      }
      world.cout("}\n");*/
      
      for (int v : local_buckets_copy) {
        auto visitor_neighbor = [](int v, vert_info &vi, int current_bucket_index) {
          local_buckets[current_bucket_index].erase(v);
          light_relax.insert(v);
          for (size_t i = 0; i < vi.light_edges.size(); i++) {
            int neighbor = vi.light_edges[i];
            int weight = vi.light_edges_weights[i];
            local_bucket_v.push_back(v);
            local_bucket_neighbor.push_back(neighbor);
            local_bucket_neighbor_weight.push_back(weight);
            local_bucket_dist.push_back(vi.dist);
          }
        };
        graph.async_visit(v, visitor_neighbor, current_bucket_index);
      }
      
      for (size_t i = 0; i < local_bucket_v.size(); i++) {
        relax(local_bucket_neighbor[i], local_bucket_dist[i] + local_bucket_neighbor_weight[i], delta, graph, world);
      }
      
      local_bucket_neighbor.clear();
      local_bucket_neighbor_weight.clear();
      local_bucket_v.clear();
      local_bucket_dist.clear();
      
      world.barrier();
    }
    
    /*world.barrier();
    world.cout("Here we go:");
    graph.for_all([&world](int src, vert_info &vi){
      world.cout(src, " -> dist: ", vi.dist);
    });*/
    
    
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

int generate_connected_random_graph(graph_type &graph, int num_vertices, int num_edges, int max_weight, int delta, ygm::comm &world) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> vertex_dist(0, num_vertices - 1);
    std::uniform_int_distribution<int> weight_dist(1, max_weight);
    
    std::vector<std::pair<int, int>> edges;
    int sum_weights = 0;

    // Create an initial spanning tree (Ensuring connectivity)
    std::vector<int> shuffled_vertices(num_vertices);
    for (int i = 0; i < num_vertices; i++) shuffled_vertices[i] = i;
    std::shuffle(shuffled_vertices.begin(), shuffled_vertices.end(), gen);

    for (int i = 1; i < num_vertices; i++) {
        int u = shuffled_vertices[i - 1];
        int v = shuffled_vertices[i];
        int weight = weight_dist(gen);
        edges.push_back({u, v});
        sum_weights += weight;
        add_edge(graph, u, v, weight, delta);
    }

    // Add remaining edges randomly (avoid duplicates with a simple check)
    while (edges.size() < num_edges) {
        int u = vertex_dist(gen);
        int v = vertex_dist(gen);
        if (u != v && 
          std::find(edges.begin(), edges.end(), std::make_pair(u, v)) == edges.end() &&
          std::find(edges.begin(), edges.end(), std::make_pair(v, u)) == edges.end()) 
        {
            int weight = weight_dist(gen);
            edges.push_back({u, v});  // Now it's safe to add
            sum_weights += weight;
            add_edge(graph, u, v, weight, delta);
        }
    }
    return sum_weights;
}

int main(int argc, char **argv) {

  ygm::comm world(&argc, &argv);

  int num_vertices = std::stoi(argv[1]);
  int num_edges = std::stoi(argv[2]);
  int max_weight = std::stoi(argv[3]);
  int delta = std::stoi(argv[4]);

  graph_type graph(world);
  
  int sum_weights = 0;
  if (world.rank0()) {
    sum_weights = generate_connected_random_graph(graph, num_vertices, num_edges, max_weight, delta, world);
    /*add_edge(graph, 0, 1, 4, delta);
    add_edge(graph, 0, 3, 8, delta);
    add_edge(graph, 1, 3, 11, delta);
    add_edge(graph, 1, 2, 3, delta);
    add_edge(graph, 2, 4, 2, delta);
    add_edge(graph, 3, 4, 7, delta);
    add_edge(graph, 4, 5, 6, delta);
    add_edge(graph, 3, 5, 1, delta);
    sum_weights = 42;
    delta = 3;*/
  }
  
  world.barrier();
  sum_weights = world.all_reduce_sum(sum_weights);
  world.barrier();
  
  intialize_vertex_info(graph, sum_weights);
  
  
  int bucket_index = ((sum_weights / delta) + 1) - 1;
  intialize_buckets(graph, sum_weights, delta, bucket_index);
  world.barrier();
  
  /*world.cout("Contents of Local bucket ", bucket_index, ": {");
  for (int v : local_buckets[bucket_index]) {
    world.cout(v, " ");
  }
  world.cout("}\n");*/
  
  //world.barrier();
  graph.for_all([&world](int src, vert_info& vi){
    for(auto dest : vi.light_edges) {
      world.cout(src, " -> ", dest);
    }
  });
  
  
  world.barrier();
  double start_time = MPI_Wtime();
  
  delta_stepping(graph, 0, delta, world, sum_weights);  // Run from source vertex 0
  
  world.barrier();
  double end_time = MPI_Wtime();
  
  if (world.rank0()) {
    std::cout << "time elapsed: " << end_time - start_time << std::endl;
  }
  
  // Print results
  graph.for_all([&world](int src, vert_info &vi){
    world.cout(src, " -> dist: ", vi.dist);
  });

  return 0;
}
