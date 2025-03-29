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
  std::vector<std::set<int>> buckets;
  
  vert_info() {}  // Default constructor

  void initialize_buckets(int sum_weights, int delta) {
    buckets.resize((sum_weights / delta) + 1);
  }
};

using graph_type = ygm::container::map<int, vert_info>;

void intialize_vertex_info(graph_type &graph, int sum_weights, int delta) {
  graph.for_all([&](int src, vert_info &vi) {
    vi.dist = sum_weights;
    vi.initialize_buckets(sum_weights, delta);
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
  }
  
  world.barrier();
  sum_weights = world.all_reduce_sum(sum_weights);
  world.barrier();
  
  intialize_vertex_info(graph, sum_weights, delta);
  world.barrier();
  
  int bucket_index = ((sum_weights / delta) + 1) - 1;
  for (int v = 0; v < num_vertices; v++) {
    add_src_to_bucket(graph, v, bucket_index);
  }
  world.barrier();
  
  delta_stepping(graph, 0, delta, world);  // Run from source vertex 0
  
  world.barrier();
  
  // Print results
  graph.for_all([&world](int src, vert_info &vi){
    world.cout(src, " -> dist: ", vi.dist);
  });

  return 0;
}
