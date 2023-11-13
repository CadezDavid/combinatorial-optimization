#include <algorithm>
#include <assert.h>
#include <deque>
#include <fstream>  // For reading input files.
#include <iostream> // For writing to the standard output.
#include <numeric>
#include <tuple>

#include "graph.cpp"

using vec = std::vector<ED::NodeId>;

vec edmonds(ED::Graph graph) {
  vec mu(graph.num_nodes());
  vec phi(graph.num_nodes());
  vec ro(graph.num_nodes());
  vec root(graph.num_nodes());
  std::deque<ED::NodeId> outer = {};

  for (ED::NodeId i = 0; i < graph.num_nodes(); i++) {
    outer.push_front(i);
    mu[i] = i;
    phi[i] = i;
    ro[i] = i;
    root[i] = i;
  }

  ED::NodeId v;
  while (!outer.empty()) {
    v = outer.front();
    outer.pop_front();
    // if (root[v] == -1)
    //   continue;

    ED::NodeId w = v;
    while (w != mu[w])
      w = phi[mu[w]];
    assert(w == root[v]);
    // std::cout << "Looking at an outer vertex " << v << std::endl;

    for (ED::NodeId u : graph.node(v).neighbors()) {
      // std::cout << "Found neighbour " << u << std::endl;
      if (root[u] == -1) {
        assert(mu[u] != v && mu[v] != u);
        // std::cout << "Growing the tree (with root " << root[v] << ") by
        // adding " << u << std::endl;
        assert(mu[u] != v && mu[v] != u);
        assert(phi[mu[u]] == mu[u]);
        outer.push_back(mu[u]);
        phi[u] = v;
        root[u] = root[v];
        root[mu[u]] = root[v];
      } else if ((mu[u] != u && phi[mu[u]] == mu[u]) || ro[u] == ro[v]) {
        // std::cout << "Found inner vertex " << u << std::endl;
        continue;
      } else if (root[u] != root[v]) {
        assert(mu[u] != v && mu[v] != u);
        // std::cout << "Augmenting trees of " << u << " and " << v <<
        // std::endl; std::cout << "With roots " << root[u] << " and " <<
        // root[v]
        //           << " respectively" << std::endl;
        vec p_u, p_v = {};

        for (ED::NodeId z = mu[v]; z != mu[z]; z = mu[phi[z]]) {
          p_v.push_back(z);
          p_v.push_back(phi[z]);
        }
        assert(p_v.empty() || phi[p_v.back()] == root[v]);
        assert(p_v.size() % 2 == 0);
        for (ED::NodeId z = mu[u]; z != mu[z]; z = mu[phi[z]]) {
          p_u.push_back(z);
          p_u.push_back(phi[z]);
        }
        assert(p_u.empty() || phi[p_u.back()] == root[u]);
        assert(p_u.size() % 2 == 0);

        for (ED::NodeId i = 0; i < p_v.size(); i += 2) {
          mu[p_v[i]] = p_v[i + 1];
          mu[p_v[i + 1]] = p_v[i];
        }
        for (ED::NodeId i = 0; i < p_u.size(); i += 2) {
          mu[p_u[i]] = p_u[i + 1];
          mu[p_u[i + 1]] = p_u[i];
        }
        mu[v] = u;
        mu[u] = v;
        assert(mu[root[u]] != root[u]);
        assert(mu[root[v]] != root[v]);

        // Reset all trees
        outer.clear();
        // ED::NodeId root_v = root[v];
        // ED::NodeId root_u = root[u];
        for (ED::NodeId i = 0; i < graph.num_nodes(); i++) {
          // if (root[i] == root_v || root[i] == root_u) {
          root[i] = -1;
          if (mu[i] == i) { // i not matched
            outer.push_back(i);
            root[i] = i;
          }
          phi[i] = i;
          ro[i] = i;
          // }
        }
        break;
      } else {
        if (mu[u] != v && mu[v] != u)
          continue;
        // std::cout << "Shrinking a blossom " << u << " and " << v <<
        // std::endl; std::cout << "Their roots are " << root[u] << " and " <<
        // root[v]
        //           << std::endl;
        vec p_u, p_v = {};

        for (ED::NodeId z = v; z != mu[z]; z = phi[mu[z]]) {
          if (ro[z] == z)
            p_v.push_back(z);
        }
        for (ED::NodeId z = u; z != mu[z]; z = phi[mu[z]]) {
          if (ro[z] == z)
            p_u.push_back(z);
        }
        p_u.push_back(root[u]);
        p_v.push_back(root[v]);

        // Define r
        while (p_v.size() > 1 && p_u.size() > 1 &&
               p_v.end()[-2] == p_u.end()[-2]) {
          p_v.pop_back();
          p_u.pop_back();
        }
        assert(p_v.back() == p_u.back());
        ED::NodeId r = p_u.back();
        // std::cout << "Their first shared blossom base is " << r << std::endl;

        // Change phi
        if (ro[u] != r) {
          ED::NodeId z = mu[u];
          while (phi[z] != r) {
            if (ro[phi[z]] != r)
              phi[phi[z]] = z;
            z = mu[phi[z]];
          }
        }
        if (ro[v] != r) {
          ED::NodeId z = mu[v];
          while (phi[z] != r) {
            if (ro[phi[z]] != r)
              phi[phi[z]] = z;
            z = mu[phi[z]];
          }
        }

        if (ro[u] != r) {
          phi[u] = v;
        }
        if (ro[v] != r) {
          phi[v] = u;
        }

        for (ED::NodeId w = 0; w < graph.num_nodes(); w++) {
          if (root[w] == root[u] &&
              (std::find(p_v.begin(), p_v.end(), ro[w]) != p_v.end() ||
               std::find(p_u.begin(), p_u.end(), ro[w]) != p_u.end()))
            ro[w] = r;
        }
      }
    }
    // std::cout << "Okay that was all from " << v << std::endl;
    // for (ED::NodeId i = 0; i < mu.size(); i++)
    //   std::cout << " " << mu[i] << "  ";
    // std::cout << std::endl;

    // int nu = 0;
    // for (ED::NodeId i = 0; i < mu.size(); i++)
    //   if (mu[i] != i)
    //     nu++;
    // nu = nu / 2;
    // std::cout << "Matching is of size " << nu << std::endl;

    // std::cout << std::endl;
  }
  return mu;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Expected the file name as an argument, but found " << argc - 1
              << std::endl;
    return EXIT_FAILURE; // return 1 would do the same, but is way too easy to
                         // mix up!
  }

  std::fstream input_file_graph{argv[1]};
  ED::Graph const graph = ED::Graph::read_dimacs(input_file_graph);

  vec mu = edmonds(graph);

  std::cout << "I finished!" << std::endl;

  // ED::Graph greedy_matching_as_graph{graph.num_nodes()};
  // for (ED::NodeId node_id = 0; node_id < graph.num_nodes(); ++node_id) {
  //   if (greedy_matching_as_graph.node(node_id).neighbors().empty()) {
  //     for (ED::NodeId neighbor_id : graph.node(node_id).neighbors()) {
  //       if (greedy_matching_as_graph.node(neighbor_id).neighbors().empty())
  //       {
  //         greedy_matching_as_graph.add_edge(node_id, neighbor_id);
  //         break; // Do not add more edges incident to this node!
  //       }
  //     }
  //   }
  // }

  // for (ED::NodeId i = 0; i < mu.size(); i++)
  //   std::cout << " " << mu[i] << "  ";
  // std::cout << std::endl;

  int nu = 0;
  for (ED::NodeId i = 0; i < mu.size(); i++)
    if (mu[i] != i)
      nu++;
  nu = nu / 2;
  std::cout << "Matching is of size " << nu << std::endl;

  std::cout << std::flush;
  return EXIT_SUCCESS;
}
