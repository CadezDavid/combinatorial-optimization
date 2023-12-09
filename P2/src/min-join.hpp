#include <algorithm>
#include <deque>
#include <fstream>  // For reading input files.
#include <iostream> // For writing to the standard output.
#include <tuple>

#include "../blossom5-v2.05.src/PerfectMatching.h"
#include "graph.hpp"

std::tuple<std::vector<std::vector<MMC::NodeId>>,
           std::vector<std::vector<MMC::EdgeWeight>>>
all_shortest_paths(MMC::Graph &graph);

MMC::Graph min_join(MMC::Graph &graph, double gamma);
