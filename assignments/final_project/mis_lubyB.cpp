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
    ar(edges, is_removed, is_belong_to_mis, neighbor_coins, neighbor_degrees);
  }

  std::vector<int> edges;
  bool is_removed = false;
  bool is_belong_to_mis = false;
  std::vector<bool> neighbor_coins;
  std::vector<int> neighbor_degrees;
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
    ss << "  edges: " << vec_to_string(vi.edges) << "\n";
    ss << "  neighbor_degrees: " << vec_to_string(vi.neighbor_degrees) << "\n";
    ss << "===========";

    world.cout(ss.str());
  });
}

void add_vertex_to_graph(graph_type &graph, int src) {
  auto vertex_inserter = [](int src, vert_info &vi) {
    vi.is_belong_to_mis = false;
    vi.is_removed = false;
  };
  graph.async_visit(src, vertex_inserter);
}

void add_edge_to_graph(graph_type &graph, int src, int dest) {
  auto edge_inserter = [](int src, vert_info &vi, int dest) {
    vi.edges.push_back(dest);
    vi.neighbor_degrees.push_back(0); // adding default value for now
    vi.neighbor_coins.push_back(false);
  };

  graph.async_visit(src, edge_inserter, dest);
  graph.async_visit(dest, edge_inserter, src);
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

void modify_edge_coin_state_values(graph_type &graph, int src, int dest, bool src_state) {
  auto edge_random_value_modifier = [](int src, vert_info &vi, int dest, bool src_state) {
    for (int i=0; i < vi.edges.size(); i++) {
      if (vi.edges[i] == dest) {
        vi.neighbor_coins[i] = src_state;
      }
    }
  };

  graph.async_visit(dest, edge_random_value_modifier, src, src_state);
}

// Function to remove an edge from the graph
void remove_edge(graph_type &graph, int src, int dest) {
  auto deleter = [](int v, vert_info &vi, int dest) {
    std::vector<int> new_edges;
    std::vector<int> new_neighbor_degrees;
    std::vector<bool> new_neighbor_coins;
    for (int i = 0; i < vi.edges.size(); i++) {
      int neighbor = vi.edges[i];
      double neighbor_coin = vi.neighbor_coins[i];
      int neighbor_degree = vi.neighbor_degrees[i];
      if (vi.edges[i] != dest) {
        new_edges.push_back(neighbor);
        new_neighbor_degrees.push_back(neighbor_degree);
        new_neighbor_coins.push_back(neighbor_coin);
      }
    }
    vi.edges = new_edges;
    vi.neighbor_coins = new_neighbor_coins;
    vi.neighbor_degrees = new_neighbor_degrees;
  };
  
  graph.async_visit(src, deleter, dest);
  graph.async_visit(dest, deleter, src);
}

static std::vector<int> local_removed;
static std::vector<int> src_vertices;
static std::vector<int> dest_vertices;

std::vector<int> mis_luby(graph_type &graph, ygm::comm &world) {
  std::vector<int> local_mis;
  bool has_active = true;
  
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
  
  int loop_count = 0;

  while (has_active) {

    // Phase 0: Propagate degree values across edges!
    graph.for_all([&](int v, vert_info &vi) {
      for (int i=0; i < vi.edges.size(); i++) {
        modify_edge_degree_values(graph, v, vi.edges[i], vi.edges.size());
      }
    });
    
    world.barrier();

    // Phase 0: biased coin flip for each vertex!
    graph.for_all([&](int v, vert_info &vi) {
      if (!vi.is_removed) {
        if (vi.edges.size() == 0) {
          vi.is_belong_to_mis = true;
        } else {
          double coin_prob = 1.0 / (2 * vi.edges.size());
          double random_value = prob_dist(gen);

          // If random_value < coin_prob, then the vertex gets added to the MIS
          if (random_value < coin_prob) {
            vi.is_belong_to_mis = true;
          }
        }
      }
    });

    world.barrier();
    
    graph.for_all([&](int v, vert_info &vi) {
      for (int i=0; i < vi.edges.size(); i++) {
        modify_edge_coin_state_values(graph, v, vi.edges[i], vi.is_belong_to_mis);
      }
    });
    
    world.barrier();

    // Phase 0: if I and my neighbor are selected, and my neighor has higher degree than me. then I need to remove myself
    // Phase 1: Select MIS candidates
    graph.for_all([&](int v, vert_info &vi) {
      if (!vi.is_removed) {
        if (vi.is_belong_to_mis) {
          for (int i=0; i < vi.edges.size(); i++) {
            if (vi.neighbor_coins[i] == true) {
              if (vi.neighbor_degrees[i] == vi.edges.size()) {
                if (vi.edges[i] > v) {
                  vi.is_belong_to_mis = false;
                }
              }
              if (vi.neighbor_degrees[i] > vi.edges.size()) {
                vi.is_belong_to_mis = false;
              }
            }
          }
          if (vi.is_belong_to_mis) {
            local_mis.push_back(v);
            local_removed.push_back(v);
            for (auto neighbor : vi.edges) {
              local_removed.push_back(neighbor);
            }
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

    // Remove edges
    for (int i=0; i<src_vertices.size(); i++) {
      //optimiaztion can be done for the below code - just one direction enough for the remove edge function.
      remove_edge(graph, dest_vertices[i], src_vertices[i]);
    }
    
    world.barrier();
    
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
    
    loop_count++;
  }
  
  world.barrier();
  
  if (world.rank0()) {
    std::cout << "loop count: " << loop_count << std::endl;
  }

  return local_mis;
}

void generate_connected_random_graph(graph_type &graph, int num_vertices, int num_edges, ygm::comm &world) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> vertex_dist(0, num_vertices - 1);
    
    std::vector<std::pair<int, int>> edges;

    // Create an initial spanning tree (Ensuring connectivity)
    std::vector<int> shuffled_vertices(num_vertices);
    for (int i = 0; i < num_vertices; i++) shuffled_vertices[i] = i;
    std::shuffle(shuffled_vertices.begin(), shuffled_vertices.end(), gen);

    for (int i = 1; i < num_vertices; i++) {
        int u = shuffled_vertices[i - 1];
        int v = shuffled_vertices[i];
        edges.push_back({u, v});
        add_edge_to_graph(graph, u, v);
    }

    // Add remaining edges randomly (avoid duplicates with a simple check)
    while (edges.size() < num_edges) {
        int u = vertex_dist(gen);
        int v = vertex_dist(gen);
        if (u != v && 
          std::find(edges.begin(), edges.end(), std::make_pair(u, v)) == edges.end() &&
          std::find(edges.begin(), edges.end(), std::make_pair(v, u)) == edges.end()) 
        {
            edges.push_back({u, v});  // Now it's safe to add
            add_edge_to_graph(graph, u, v);
        }
    }
}

void generate_random_graph(graph_type &graph, int num_vertices, int num_edges, ygm::comm &world) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> vertex_dist(0, num_vertices - 1);
    
    std::vector<std::pair<int, int>> edges;

    // Add remaining edges randomly (avoid duplicates with a simple check)
    while (edges.size() < num_edges) {
        int u = vertex_dist(gen);
        int v = vertex_dist(gen);
        if (u != v && 
          std::find(edges.begin(), edges.end(), std::make_pair(u, v)) == edges.end() &&
          std::find(edges.begin(), edges.end(), std::make_pair(v, u)) == edges.end()) 
        {
            edges.push_back({u, v});  // Now it's safe to add
            add_edge_to_graph(graph, u, v);
        }
    }
}

int main(int argc, char **argv) {
  ygm::comm world(&argc, &argv);

  int num_vertices = std::stoi(argv[1]);
  int num_edges = std::stoi(argv[2]);

  graph_type graph(world);
  
  if (world.rank0()) {
    for (int i = 0; i < num_vertices; ++i) {
      // Intialize vertices
      add_vertex_to_graph(graph, i);
    }
    
    // Just add edges
    //generate_connected_random_graph(graph, num_vertices, num_edges, world);
    generate_random_graph(graph, num_vertices, num_edges, world);
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