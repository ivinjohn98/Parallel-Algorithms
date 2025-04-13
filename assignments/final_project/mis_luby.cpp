#include <ygm/comm.hpp>
#include <ygm/container/map.hpp>
#include <ygm/container/set.hpp>
#include <vector>
#include <iostream>
#include <set>
#include <limits>
#include <algorithm>
#include <random>

// Struct to store vertex information
struct vert_info {
  template <typename Archive>
  void serialize(Archive & ar) {
    ar(edges, is_removed, is_belong_to_mis, random_value, neighbor_random_values, edge_weights);
  }

  std::vector<int> edges;
  bool is_removed = false;
  bool is_belong_to_mis = false;
  double random_value = -1.0;
  std::vector<double> neighbor_random_values;
  std::vector<int> edge_weights;
  
};

using graph_type = ygm::container::map<int, vert_info>;

template <typename T>
std::string vec_to_string(const std::vector<T> &vec) {
  std::stringstream ss;
  ss << "[";
  for (size_t i = 0; i < vec.size(); ++i) {
    ss << vec[i];
    if (i != vec.size() - 1) ss << ", ";
  }
  ss << "]";
  return ss.str();
}

void print_graph(graph_type &graph, ygm::comm &world) {
  graph.for_all([&world](int src, vert_info &vi) {
    world.cout("Vertex ", src);
    world.cout("  is_removed: ", vi.is_removed);
    world.cout("  is_belong_to_mis: ", vi.is_belong_to_mis);
    world.cout("  random_value: ", vi.random_value);
    world.cout("  edges: ", vec_to_string(vi.edges));
    world.cout("  neighbor_random_values: ", vec_to_string(vi.neighbor_random_values));
    world.cout("  edge_weights: ", vec_to_string(vi.edge_weights));
    world.cout("===========");
  });
}

// Function to add an edge to the graph
void add_edge(graph_type &graph, int src, int dest, int weight) {
  auto inserter = [](int src, vert_info &vi, int dest, int weight) {
    vi.edges.push_back(dest);
    vi.edge_weights.push_back(weight);
  };
  
  graph.async_visit(src, inserter, dest, weight);
  graph.async_visit(dest, inserter, src, weight);
}

void add_edge_with_random_value(graph_type &graph, int src, int dest, int weight, double src_random_value, double dest_random_value) {
  auto inserter = [](int src, vert_info &vi, int dest, int weight, double random_value_src, double random_value_dest) {
    if (vi.random_value == -1.0) {
      vi.random_value = random_value_src;
    }
    vi.edges.push_back(dest);
    vi.neighbor_random_values.push_back(random_value_dest);
    vi.edge_weights.push_back(weight);
  };

  graph.async_visit(src, inserter, dest, weight, src_random_value, dest_random_value);
  graph.async_visit(dest, inserter, src, weight, dest_random_value, src_random_value);
}


// Function to remove an edge from the graph
void remove_edge(graph_type &graph, int src, int dest) {
  auto deleter = [](int v, vert_info &vi, int dest) {
    std::vector<int> new_edges;
    std::vector<double> new_neighbor_random_values;
    for (int i = 0; i < vi.edges.size(); i++) {
      int neighbor = vi.edges[i];
      double neighbor_random_value = vi.neighbor_random_values[i];
      if (vi.edges[i] != dest) {
        new_edges.push_back(neighbor);
        new_neighbor_random_values.push_back(neighbor_random_value);
      }
    }
    vi.edges = new_edges;
    vi.neighbor_random_values = new_neighbor_random_values;
  };
  
  graph.async_visit(src, deleter, dest);
  graph.async_visit(dest, deleter, src);
}

void assign_random_values(graph_type &graph, ygm::comm &world) {
  graph.for_all([](const int key, vert_info &vi) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
    vi.random_value = prob_dist(gen);
  });
}

void propagate_random_values(graph_type &graph) {
  graph.for_all([&graph](const int key, vert_info &vi) {
    for (auto neighbor : vi.edges) {
      double my_rand = vi.random_value;
      graph.async_visit(neighbor, [my_rand](int, vert_info &u_vi) {
        u_vi.neighbor_random_values.push_back(my_rand);
      });
    }
  });
}

std::vector<int> mis_luby(graph_type &graph, ygm::comm &world) {
  std::vector<int> local_mis;
  bool has_active = true;
  static std::vector<int> local_removed;
  static std::vector<int> src_vertices;
  static std::vector<int> dest_vertices;

  while (has_active) {
    // Phase 1: Select MIS candidates
    graph.for_all([&](int v, vert_info &vi) {
      if (!vi.is_removed) {
        double min_neighbor_value = vi.random_value;
        for (auto neighbor_value : vi.neighbor_random_values) {
          min_neighbor_value = std::min(min_neighbor_value, neighbor_value);
        }
    
        if (vi.random_value <= min_neighbor_value) {
          vi.is_belong_to_mis = true;
          local_mis.push_back(v);
          local_removed.push_back(v);
          for (auto neighbor : vi.edges) {
            local_removed.push_back(neighbor);
          }
        }
      }
    });
    world.barrier();
    
    // Phase 2: Mark removed vertices and clean neighbor lists
    for (auto neighbor : local_removed) {
      auto vertex_remover = [&](int v, vert_info& vi) {
        vi.is_removed = true;
        for (auto dest : vi.edges) {
          src_vertices.push_back(v);
          dest_vertices.push_back(dest);
        }
      };
      graph.async_visit(neighbor, vertex_remover);
    }
    
    world.barrier();

    // Remove edges to removed vertices
    for (int i=0; i<src_vertices.size(); i++) {
      //std::cout<< "about to remove src: " << src_vertices[i] << ", dest: " << dest_vertices[i] << std::endl;
      //optimiaztion can be done for the below code - just one direction enough for the remove edge function.
      //remove_edge(graph, dest_vertices[i], src_vertices[i]);
      remove_edge(graph, src_vertices[i], dest_vertices[i]);
    }
    
    local_removed.clear();
    src_vertices.clear();
    dest_vertices.clear();
    
    // Phase 3: Check if any active vertices remain
    bool local_active = false;
    graph.for_all([&](int v, vert_info &vi) {
      if (!vi.is_removed) {
        local_active = true;
      }
    });
    
    // Reduce to get global activity status
    has_active = world.all_reduce(local_active, std::logical_or<bool>());
  }
  
  world.barrier();

  return local_mis;
}


int generate_connected_random_graph(graph_type &graph, int num_vertices, int num_edges, int max_weight, ygm::comm &world) {
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
        add_edge(graph, u, v, weight);
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
            add_edge(graph, u, v, weight);
        }
    }
    return sum_weights;
}

int main(int argc, char **argv) {
  ygm::comm world(&argc, &argv);

  int num_vertices = std::stoi(argv[1]);
  int num_edges = std::stoi(argv[2]);
  int max_weight = std::stoi(argv[3]);

  graph_type graph(world);

  if (world.rank0()) {
    std::unordered_map<int, double> vertex_to_random_value_map;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
    for (int i = 0; i < num_vertices; ++i) {
      vertex_to_random_value_map[i] = prob_dist(gen);
    }
    // Just add edges
    add_edge_with_random_value(graph, 0, 1, 4, vertex_to_random_value_map[0], vertex_to_random_value_map[1]);
    add_edge_with_random_value(graph, 0, 3, 8, vertex_to_random_value_map[0], vertex_to_random_value_map[3]);
    add_edge_with_random_value(graph, 1, 3, 11, vertex_to_random_value_map[1], vertex_to_random_value_map[3]);
    add_edge_with_random_value(graph, 1, 2, 3, vertex_to_random_value_map[1], vertex_to_random_value_map[2]);
    add_edge_with_random_value(graph, 2, 4, 2, vertex_to_random_value_map[2], vertex_to_random_value_map[4]);
    add_edge_with_random_value(graph, 3, 4, 7, vertex_to_random_value_map[3], vertex_to_random_value_map[4]);
    add_edge_with_random_value(graph, 4, 5, 6, vertex_to_random_value_map[4], vertex_to_random_value_map[5]);
    add_edge_with_random_value(graph, 3, 5, 1, vertex_to_random_value_map[3], vertex_to_random_value_map[5]);
  }
  
  world.barrier();
  
  print_graph(graph, world);
  world.barrier();
  
  double start_time = MPI_Wtime();
  std::vector<int> local_solution = mis_luby(graph, world);
  world.barrier();
  double end_time = MPI_Wtime();
  
  world.barrier();

  for (int v : local_solution) {
    world.cout("MIS vertex: ", v);
  }

  return 0;
}

