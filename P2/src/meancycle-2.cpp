#include <deque>
#include <fstream>  // For reading input files.
#include <iostream> // For writing to the standard output.

#include "graph.hpp"
#include "min-join.hpp"

double weight(MMC::Graph &graph, double gamma) {
  MMC::EdgeWeight w = 0;
  for (MMC::EdgeId e{0}; e < graph.num_edges(); e++) {
    w += graph.edge_weight(e) - gamma;
  }
  return w;
}

bool is_acyclic(MMC::Graph &graph) {
  std::vector<MMC::NodeId> seen(graph.num_nodes(), false);
  std::vector<MMC::NodeId> ancestor(graph.num_nodes());

  std::deque<MMC::NodeId> stack{MMC::NodeId(0)};
  ancestor[0] = 0;

  MMC::NodeId v;
  while (!stack.empty()) {
    v = stack.back();
    stack.pop_back();

    MMC::NodeId u;
    for (MMC::EdgeId edge : graph.node(v).outgoing_halfedges()) {
      u = graph.halfedge(edge).target();
      if (!seen[u]) {
        stack.push_back(u);
        ancestor[u] = v;
      } else if (ancestor[v] != u) {
        return false;
      }
    }
    seen[v] = true;
  }
  return true;
}

MMC::Graph min_mean_weight_cycle(MMC::Graph &graph) {
  if (is_acyclic(graph)) {
    MMC::Graph empty = MMC::Graph(graph.num_nodes());
    return empty;
  }

  double gamma = graph.edge_weight(0);
  for (MMC::EdgeId e{0}; e < graph.num_edges(); e++)
    gamma = std::max(static_cast<double>(graph.edge_weight(e)), gamma);

  MMC::Graph t_join{graph.num_nodes()};
  while (1) {
    t_join = min_join(graph, gamma);
    std::cout << t_join << std::endl;
    if (weight(t_join, gamma) >= 0)
      return t_join;
    gamma += weight(t_join, gamma) / t_join.num_edges();
  }

  MMC::Graph empty = MMC::Graph(graph.num_nodes());
  return empty;
}

int main(int argc, char **argv) {

  if (argc != 3) {
    std::cout
        << "Expected exactly two arguments (paths to input and output graph)!"
        << std::endl;
    return EXIT_FAILURE;
  }

  std::ifstream input_file_graph{argv[1]};
  MMC::Graph graph = MMC::Graph::read_dimacs(input_file_graph);

  MMC::Graph cycle = min_mean_weight_cycle(graph);

  std::ofstream output_file_graph{argv[2]};
  output_file_graph << cycle;

  return EXIT_SUCCESS;
}
