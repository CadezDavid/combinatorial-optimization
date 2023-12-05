#include <iostream> // For writing to the standard output.
#include <fstream> // For reading input files.

#include "graph.hpp"
#include "../blossom5-v2.05.src/PerfectMatching.h"

int main(int argc, char** argv) {
   // If you don't want to write MMC:: all the time,
   // using a namespace in the implementation of some function is perfectly ok.
   using namespace MMC;

   if (argc != 3) {
      std::cout << "Expected exactly two arguments (paths to input and output graph)!" << std::endl;
      return EXIT_FAILURE; // return 1 would do the same, but is way too easy to mix up!
   }

   std::ifstream input_file_graph{argv[1]};
   Graph const graph = Graph::read_dimacs(input_file_graph);
   // These checks are only for the example, your MMC solver has to handle non-complete graphs and graphs with odd
   // vertex count!
   if (graph.num_edges() == graph.num_nodes() * (graph.num_nodes() - 1)) {
      std::cout << "Expected input graph to be complete graph!" << std::endl;
      return EXIT_FAILURE;
   } else if (graph.num_nodes() % 2 != 0) {
      std::cout << "Expected input graph to have even number of vertices!" << std::endl;
      return EXIT_FAILURE;
   } else {
      PerfectMatching solver{static_cast<int>(graph.num_nodes()), static_cast<int>(graph.num_edges())};
      for (EdgeId edge_id{0}; edge_id < graph.num_edges(); ++edge_id)
      {
         solver.AddEdge(
            graph.halfedge(2*edge_id + 1).target(),
            graph.halfedge(2*edge_id + 0).target(),
            graph.edge_weight(edge_id)
         );
      }
 
      solver.Solve();

      Graph solution{graph.num_nodes()};
      for (EdgeId edge_id{0}; edge_id < graph.num_edges(); ++edge_id) {
        if (solver.GetSolution(edge_id)) {
          solution.add_edge(
             graph.halfedge(2*edge_id + 1).target(),
             graph.halfedge(2*edge_id + 0).target(),
             graph.edge_weight(edge_id)
          );
        }
      }
      std::ofstream output_file_graph{argv[2]};
      output_file_graph << solution;

      return EXIT_SUCCESS; 
   }

}

