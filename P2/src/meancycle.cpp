#include <deque>
#include <fstream>  // For reading input files.
#include <iostream> // For writing to the standard output.
#include <numeric>

#include "graph.hpp"
#include "min-join.hpp"

double weight(MMC::Graph &graph, double gamma) {
  MMC::EdgeWeight w = 0;
  for (MMC::EdgeId e{0}; e < graph.num_edges(); e++) {
    w += graph.edge_weight(e) - gamma;
  }
  return w;
}

std::vector<MMC::EdgeId> find_cycle(MMC::Graph &graph) {
  std::vector<MMC::NodeId> seen(graph.num_nodes(), false);
  std::vector<MMC::EdgeId> ancestor(graph.num_nodes(),
                                    std::numeric_limits<MMC::EdgeId>::max());

  std::deque<MMC::NodeId> stack{};
  for (MMC::NodeId root{0}; root < graph.num_nodes(); root++) {
    if (seen[root] || graph.node(root).outgoing_halfedges().empty())
      continue;
    else
      stack.push_back(root);

    MMC::EdgeId e = graph.node(root).outgoing_halfedges()[0];
    ancestor[root] = graph.halfedge(e).inverse();

    MMC::NodeId v;
    while (!stack.empty()) {
      v = stack.back();
      stack.pop_back();

      MMC::NodeId u;
      for (MMC::EdgeId edge : graph.node(v).outgoing_halfedges()) {
        u = graph.halfedge(edge).target();
        if (!seen[u]) {
          stack.push_back(u);
          ancestor[u] = graph.halfedge(edge).inverse();
        } else if (ancestor[v] != edge) {
          std::vector<MMC::EdgeId> cycle = {graph.halfedge(edge).inverse()};
          while (u != v) {
            cycle.push_back(ancestor[v]);
            v = graph.halfedge(ancestor[v]).target();
          }
          return cycle;
        }
      }
      seen[v] = true;
    }
  }
  return {};
}

MMC::Graph min_mean_weight_cycle(MMC::Graph &graph) {
  if (find_cycle(graph).empty()) {
    MMC::Graph empty = MMC::Graph(graph.num_nodes());
    return empty;
  }

  double gamma = graph.edge_weight(0);
  for (MMC::EdgeId e{0}; e < graph.num_edges(); e++)
    gamma = std::max(static_cast<double>(graph.edge_weight(e)), gamma);

  MMC::Graph t_join = min_join(graph, gamma);
  double change;
  while (weight(t_join, gamma) < 0) {
    change = weight(t_join, gamma) / t_join.num_edges();
    gamma += change;
    t_join = min_join(graph, gamma);
    std::cout << t_join << std::endl;
  }
  t_join = min_join(graph, gamma - change);
  std::vector<MMC::EdgeId> cycle = find_cycle(t_join);
  MMC::Graph graph_cycle(graph.num_nodes());
  MMC::NodeId u, v;
  for (MMC::EdgeId edge : cycle) {
    u = t_join.halfedge(edge).target();
    v = t_join.halfedge(t_join.halfedge(edge).inverse()).target();
    graph_cycle.add_edge(u, v, t_join.halfedge_weight(edge));
  }
  return graph_cycle;
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
