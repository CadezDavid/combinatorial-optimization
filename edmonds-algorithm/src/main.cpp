#include <chrono>
#include <fstream>  // For reading input files.
#include <iostream> // For writing to the standard output.

#include "edmonds.cpp"
#include "graph.cpp"

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Expected one argument, but found " << argc - 1 << std::endl;
    return EXIT_FAILURE;
  }

  std::fstream input_file_graph{argv[1]};
  ED::Graph const graph = ED::Graph::read_dimacs(input_file_graph);

  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  std::vector<ED::NodeId> mu = edmonds(graph);

  std::chrono::duration<double> elapsed =
      std::chrono::steady_clock::now() - begin;
  std::cout << "Time elapsed (sec) = " << elapsed.count() << std::endl;

  int nu = 0;
  for (ED::NodeId i = 0; i < mu.size(); i++)
    if (mu[i] != i)
      nu++;
  nu = nu / 2;
  std::cout << "Found matching of size: " << nu << std::endl;

  std::cout << std::flush;
  return EXIT_SUCCESS;
}
