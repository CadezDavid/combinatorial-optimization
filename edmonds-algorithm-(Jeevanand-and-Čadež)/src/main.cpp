#include <fstream>  // For reading input files.
#include <iostream> // For writing to the standard output.

#include "edmonds.hpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Expected one argument, but found " << argc - 1 << std::endl;
    return EXIT_FAILURE;
  }

  std::fstream input_file_graph{argv[1]};
  ED::Graph graph = ED::GraphEdmonds::read_dimacs(input_file_graph);

  std::vector<ED::NodeId> mu = graph.edmonds();

  ED::GraphEdmonds matching_subgraph = ED::GraphEdmonds(graph.num_nodes());
  for (ED::NodeId i = 0; i < mu.size(); i++)
    if (i < mu[i])
      matching_subgraph.add_edge(i, mu[i]);
  std::cout << matching_subgraph << std::endl;

  std::cout << std::flush;
  return EXIT_SUCCESS;
}
