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
  std::vector<int> neighbor_degrees;
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
    std::stringstream ss;
    ss << "Vertex " << src << "\n";
    ss << "  is_removed: " << vi.is_removed << "\n";
    ss << "  is_belong_to_mis: " << vi.is_belong_to_mis << "\n";
    ss << "  random_value: " << vi.random_value << "\n";
    ss << "  edges: " << vec_to_string(vi.edges) << "\n";
    ss << "  neighbor_random_values: " << vec_to_string(vi.neighbor_random_values) << "\n";
    ss << "  edge_weights: " << vec_to_string(vi.edge_weights) << "\n";
    ss << "  neighbor_degrees: " << vec_to_string(vi.neighbor_degrees) << "\n";
    ss << "===========";

    world.cout(ss.str());
  });
}

void add_vertex_to_graph(graph_type &graph, int src, double random_value) {
  auto vertex_inserter = [](int src, vert_info &vi, double random_value) {
    vi.random_value = random_value;
  };
  graph.async_visit(src, vertex_inserter, random_value);
}

void add_edge_with_random_value(graph_type &graph, int src, int dest, int weight, double src_random_value, double dest_random_value) {
  auto edge_inserter = [](int src, vert_info &vi, int dest, int weight, double random_value_src, double random_value_dest) {
    vi.edges.push_back(dest);
    vi.neighbor_random_values.push_back(random_value_dest);
    vi.edge_weights.push_back(weight);
    vi.neighbor_degrees.push_back(0); // adding default value for now
  };

  graph.async_visit(src, edge_inserter, dest, weight, src_random_value, dest_random_value);
  graph.async_visit(dest, edge_inserter, src, weight, dest_random_value, src_random_value);
}

void modify_vertex_random_value(graph_type &graph, int src, double src_random_value) {
  auto vertex_random_value_modifier = [](int src, vert_info &vi, double random_value_src) {
    vi.random_value = random_value_src;
  };

  graph.async_visit(src, vertex_random_value_modifier, src_random_value);
}

void modify_edge_degree_values(graph_type &graph, int src, int dest, int src_degree_value) {
  auto edge_random_value_modifier = [](int src, vert_info &vi, int dest, int dest_degree) {
    for (int i=0; i < vi.edges.size(); i++) {
      if (vi.edges[i] == dest) {
        vi.neighbor_degrees[i] = dest_degree;
      }
    }
  };

  graph.async_visit(dest, edge_random_value_modifier, src, src_degree_value);
}

void modify_edge_random_value(graph_type &graph, int src, int dest, double src_random_value) {
  auto edge_random_value_modifier = [](int dest, vert_info &vi, int src, double random_value_src) {
    for (int i=0; i < vi.edges.size(); i++) {
      if (vi.edges[i] == src) {
        vi.neighbor_random_values[i] = random_value_src;
      }
    }
  };

  graph.async_visit(dest, edge_random_value_modifier, src, src_random_value);
}

// Function to remove an edge from the graph
void remove_edge(graph_type &graph, int src, int dest) {
  auto deleter = [](int v, vert_info &vi, int dest) {
    std::vector<int> new_edges;
    std::vector<double> new_neighbor_random_values;
    std::vector<int> new_neighbor_degrees;
    for (int i = 0; i < vi.edges.size(); i++) {
      int neighbor = vi.edges[i];
      double neighbor_random_value = vi.neighbor_random_values[i];
      int neighbor_degree = vi.neighbor_degrees[i];
      if (vi.edges[i] != dest) {
        new_edges.push_back(neighbor);
        new_neighbor_random_values.push_back(neighbor_random_value);
        new_neighbor_degrees.push_back(neighbor_degree);
      }
    }
    vi.edges = new_edges;
    vi.neighbor_random_values = new_neighbor_random_values;
    vi.neighbor_degrees = new_neighbor_degrees;
  };
  
  graph.async_visit(src, deleter, dest);
  graph.async_visit(dest, deleter, src);
}

static std::vector<int> local_removed;
static std::vector<int> src_vertices;
static std::vector<int> dest_vertices;
static std::vector<int> src_collect_suspect_neighbors;
static std::vector<int> dest_collect_suspect_neighbors;
static std::vector<int> position_to_remove;

std::vector<int> mis_luby(graph_type &graph, ygm::comm &world) {
  std::vector<int> local_mis;
  bool has_active = true;
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::bernoulli_distribution coin_flip(0.5);

  while (has_active) {

    // Phase 0: Propagate degree values across edges!
    graph.for_all([&](int v, vert_info &vi) {
      for (int i=0; i < vi.edges.size(); i++) {
        modify_edge_degree_values(graph, v, vi.edges[i], vi.edges.size());
      }
    });
    
    world.barrier();

    // Phase 0: coin flip for each vertex!
    graph.for_all([&](int v, vert_info &vi) {
      int coin_value = coin_flip(gen);
      if (coin_value == 1) {
        vi.is_belong_to_mis = true;
        for (int i = 0; i < vi.edges.size(); i++) {
          // Forcing dest to have higher priority that src - these are our suspects
          // if the suspect is in mis, the v should not be in mis
          if (vi.neighbor_degrees[i] == vi.edges.size()) {
            if (vi.edges[i] > v) {
              src_collect_suspect_neighbors.push_back(v);
              dest_collect_suspect_neighbors.push_back(vi.edges[i]);
            }
          }
          if (vi.neighbor_degrees[i] > vi.edges.size()) {
            src_collect_suspect_neighbors.push_back(v);
            dest_collect_suspect_neighbors.push_back(vi.edges[i]);
          }
        }
      }
    });
    
    //world.cout("Everything good here:1");
    world.barrier();

    // Phase 0: Filtering neighors if that are not part of mis
    for (int i=0; i< dest_collect_suspect_neighbors.size(); i++) {
      // dest always has higher priority than src here!
      auto mis_neighbor_remover = [&](int dest, vert_info& vi, int src, int position) {
        if (!vi.is_belong_to_mis) {
          position_to_remove.push_back(position);
        }
      };
      graph.async_visit(dest_collect_suspect_neighbors[i], mis_neighbor_remover, src_collect_suspect_neighbors[i], i);
    }
    
    //world.cout("Everything good here:2");
    world.barrier();

    std::sort(position_to_remove.rbegin(), position_to_remove.rend());
    for (int pos : position_to_remove) {
      if (pos < dest_collect_suspect_neighbors.size()) {
        dest_collect_suspect_neighbors.erase(dest_collect_suspect_neighbors.begin() + pos);
        src_collect_suspect_neighbors.erase(src_collect_suspect_neighbors.begin() + pos);
      }
    }
    
    //world.cout("Everything good here:3");
    world.barrier();

    // Phase 0: All remaining dest are mis, therefore src cannot be mis
    for (int i=0; i< src_collect_suspect_neighbors.size(); i++) {
      auto mis_remover = [&](int src, vert_info& vi) {
        vi.is_belong_to_mis = false;
        vi.is_removed = false;
      };
      graph.async_visit(src_collect_suspect_neighbors[i], mis_remover);
    }
    
    //world.cout("Everything good here:4");
    world.barrier();
    
    // Phase 1: Select MIS candidates
    graph.for_all([&](int v, vert_info &vi) {
      if (!vi.is_removed) {
        if (vi.is_belong_to_mis) {
          local_mis.push_back(v);
          local_removed.push_back(v);
          for (auto neighbor : vi.edges) {
            local_removed.push_back(neighbor);
          }
        }
      }
    });

    //world.cout("Everything good here:5");
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
    
    //world.cout("Everything good here:6");
    world.barrier();

    // Remove edges
    for (int i=0; i<src_vertices.size(); i++) {
      //optimiaztion can be done for the below code - just one direction enough for the remove edge function.
      remove_edge(graph, dest_vertices[i], src_vertices[i]);
    }
    
    world.barrier();
    
    local_removed.clear();
    src_vertices.clear();
    dest_vertices.clear();
    src_collect_suspect_neighbors.clear();
    dest_collect_suspect_neighbors.clear();
    position_to_remove.clear();
    
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

int generate_connected_random_graph(graph_type &graph, int num_vertices, int num_edges, int max_weight, ygm::comm &world, std::unordered_map<int, double>& vertex_to_random_value_map) {
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
        add_edge_with_random_value(graph, u, v, weight, vertex_to_random_value_map[u], vertex_to_random_value_map[v]);
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
            add_edge_with_random_value(graph, u, v, weight, vertex_to_random_value_map[u], vertex_to_random_value_map[v]);
        }
    }
    return sum_weights;
}

int generate_random_graph(graph_type &graph, int num_vertices, int num_edges, int max_weight, ygm::comm &world, std::unordered_map<int, double>& vertex_to_random_value_map) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> vertex_dist(0, num_vertices - 1);
    std::uniform_int_distribution<int> weight_dist(1, max_weight);
    
    std::vector<std::pair<int, int>> edges;
    int sum_weights = 0;

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
            add_edge_with_random_value(graph, u, v, weight, vertex_to_random_value_map[u], vertex_to_random_value_map[v]);
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
      // Intialize vertices
      add_vertex_to_graph(graph, i, vertex_to_random_value_map[i]);
    }
    
    // Just add edges
    //generate_connected_random_graph(graph, num_vertices, num_edges, max_weight, world, vertex_to_random_value_map);
    generate_random_graph(graph, num_vertices, num_edges, max_weight, world, vertex_to_random_value_map);
  }
  
  world.barrier();
  
  //print_graph(graph, world);
  //world.barrier();
  
  double start_time = MPI_Wtime();
  std::vector<int> local_solution = mis_luby(graph, world);
  world.barrier();
  double end_time = MPI_Wtime();
  
  world.barrier();

  if (world.rank0()) {
    std::cout << "time elapsed: " << end_time - start_time << std::endl;
  }
  /*for (int v : local_solution) {
    world.cout("MIS vertex: ", v);
  }*/

  return 0;
}