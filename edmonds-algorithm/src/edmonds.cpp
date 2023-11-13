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
  int M = 0;

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
    //   break;

    for (ED::NodeId u : graph.node(v).neighbors()) {
      if (root[u] == -1) {
        outer.push_back(mu[u]);
        phi[u] = v;
        root[u] = root[v];
        root[mu[u]] = root[v];
      } else if ((mu[u] != u && phi[mu[u]] == mu[u]) || ro[u] == ro[v]) {
        continue;
      } else if (root[u] != root[v]) {
        assert(root[v] != -1);
        vec p_u, p_v = {};

        for (ED::NodeId z = mu[v]; z != mu[z]; z = mu[phi[z]]) {
          p_v.push_back(z);
          p_v.push_back(phi[z]);
        }
        for (ED::NodeId z = mu[u]; z != mu[z]; z = mu[phi[z]]) {
          p_u.push_back(z);
          p_u.push_back(phi[z]);
        }
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

        for (ED::NodeId i = 0; i < graph.num_nodes(); i++)
          assert(mu[mu[i]] == i);

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
        M++;
        break;
      } else {
        assert(root[u] == root[v]);
        vec p_v = {ro[v]};
        vec p_u = {ro[u]};

        while (p_u.back() != root[u]) {
          p_u.push_back(ro[phi[mu[p_u.back()]]]);
        }

        while (p_v.back() != root[v]) {
          p_v.push_back(ro[phi[mu[p_v.back()]]]);
        }

        while (p_v.size() > 1 && p_u.size() > 1 &&
               p_v[p_v.size() - 2] == p_u[p_u.size() - 2]) {
          p_u.pop_back();
          p_v.pop_back();
        }
        assert(p_u.back() == p_v.back());
        assert(p_v.size() < 2 || p_u.size() < 2 ||
               (p_v[p_v.size() - 2] != p_u[p_u.size() - 2]));
        ED::NodeId r = p_u.back();

        if (ro[v] != r) {
          assert(p_v.size() > 1);
          for (ED::NodeId z = mu[v]; ro[phi[z]] != r; z = mu[phi[z]])
            phi[phi[z]] = z;
          phi[v] = u;
        }

        if (ro[u] != r) {
          assert(p_u.size() > 1);
          for (ED::NodeId z = mu[u]; ro[phi[z]] != r; z = mu[phi[z]])
            phi[phi[z]] = z;
          phi[u] = v;
        }

        for (ED::NodeId z = 0; z < graph.num_nodes(); z++)
          if (root[z] == root[u] &&
              (std::find(p_v.begin(), p_v.end(), ro[phi[z]]) != p_v.end() ||
               std::find(p_u.begin(), p_u.end(), ro[phi[z]]) != p_u.end()))
            ro[z] = r;
      }
    }
    if (M % 500 == 0)
      std::cout << "Matching: " << M << std::endl;
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

  int nu = 0;
  for (ED::NodeId i = 0; i < mu.size(); i++)
    if (mu[i] != i)
      nu++;
  nu = nu / 2;
  std::cout << "Maximum matching is of size: " << nu << std::endl;

  std::cout << std::flush;
  return EXIT_SUCCESS;
}
