#include <fstream>  // For reading input files.
#include <iostream> // For writing to the standard output.

#include "graph.hpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Expected the file name as an argument, but found " << argc - 1 << std::endl;
    return EXIT_FAILURE; // return 1 would do the same, but is way too easy to
                         // mix up!
  }

  std::fstream input_file_graph{argv[1]};
  ED::Graph const graph = ED::Graph::read_dimacs(input_file_graph);

  ED::Graph greedy_matching_as_graph{graph.num_nodes()};
  for (ED::NodeId node_id = 0; node_id < graph.num_nodes(); ++node_id) {
    if (greedy_matching_as_graph.node(node_id).neighbors().empty()) {
      for (ED::NodeId neighbor_id : graph.node(node_id).neighbors()) {
        if (greedy_matching_as_graph.node(neighbor_id).neighbors().empty()) {
          greedy_matching_as_graph.add_edge(node_id, neighbor_id);
          break; // Do not add more edges incident to this node!
        }
      }
    }
  }
  std::cout << greedy_matching_as_graph;
  return EXIT_SUCCESS;
}
