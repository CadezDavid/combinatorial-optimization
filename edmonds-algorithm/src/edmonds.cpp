#include <algorithm>
#include <chrono>
#include <deque>
#include <numeric>

#include "graph.cpp"

namespace ED {

/**
 * Shrinks the blossom, created by an edge u-v.
 * @param nodes v and u, which lie in the same tree, meaning root[v] == root[u]
 * @return void
 */
void Graph::shrink(NodeId v, NodeId u) {
  // These will be bases of blossoms on paths from v and u to their root
  // (which we are assuming to be the same!).
  std::vector<NodeId> p_v = {ro[v]};
  std::vector<NodeId> p_u = {ro[u]};

  while (p_u.back() != root[u]) {
    p_u.push_back(mu[p_u.back()]); // Add inner blossom (by assumption its own
                                   // base)
    p_u.push_back(ro[phi[p_u.back()]]); // Add base of outer blossom
  }

  while (p_v.back() != root[v]) {
    p_v.push_back(mu[p_v.back()]); // Add inner blossom (by assumption its own
                                   // base)
    p_v.push_back(ro[phi[p_v.back()]]); // Add base of outer blossom
  }

  // This loop removes items at the end of p_v and p_u that are the same.
  while (p_v.size() > 1 && p_u.size() > 1 &&
         p_v[p_v.size() - 2] == p_u[p_u.size() - 2]) {
    p_u.pop_back();
    p_v.pop_back();
  }
  // Last item in p_v and p_u are assumed to be the same, and that is also their
  // first common item.
  NodeId r = p_u.back();

  if (ro[v] != r) {
    for (NodeId z = mu[v]; ro[phi[z]] != r; z = mu[phi[z]])
      phi[phi[z]] = z;
    phi[v] = u;
  }

  if (ro[u] != r) {
    for (NodeId z = mu[u]; ro[phi[z]] != r; z = mu[phi[z]])
      phi[phi[z]] = z;
    phi[u] = v;
  }

  // Change the blossom base of all those elements whose current blossom base
  // lies on either of paths v - r and u - r. Change it to r.
  for (NodeId z = 0; z < num_nodes(); z++)
    if (root[z] == root[u] &&
        (std::find(p_v.begin(), p_v.end(), ro[z]) != p_v.end() ||
         std::find(p_u.begin(), p_u.end(), ro[z]) != p_u.end()))
      ro[z] = r;
}

/**
 * Grows the forest, by adding the vertex u to the tree of v.
 * @param nodes v and u
 * @return void
 */
void Graph::grow(NodeId v, NodeId u) {
  phi[u] = v;
  root[u] = root[v];
  root[mu[u]] = root[v];
}

/**
 * Augments the matching, stored in mu, by augmenting the path
 * root[v]-v-u-root[u].
 * @param nodes v and u
 * @return void
 */
void Graph::augment(NodeId v, NodeId u) {
  NodeId z = u;
  NodeId w = mu[u];
  while (w != root[u]) {
    z = w;
    w = mu[phi[w]];
    mu[phi[z]] = z;
    mu[z] = phi[z];
    z = mu[w];
  }
  z = v;
  w = mu[v];
  while (w != root[v]) {
    z = w;
    w = mu[phi[w]];
    mu[phi[z]] = z;
    mu[z] = phi[z];
    z = mu[w];
  }
  mu[v] = u;
  mu[u] = v;
}

/**
 * Edmonds blossom algorithm for computing maximum matching in a general graph.
 * @param the graph itself
 * @return a vector mu of length num_nodes and whose values represent the pair
 * of a given index.
 */
std::vector<NodeId> Graph::edmonds() {
  // A dequeue that will hold all outer vertices that have to be checked.
  // Sometimes we add to the front, sometimes to the back, which we decided
  // based on intuition how quickly should the vertex be checked. But in the end
  // it does not matter on which end you put new vertices (in the algorithm the
  // order of checking is not specified).
  std::deque<NodeId> outer = {};

  // Fill all vectors with increasing values, starting with 0. This corresponds
  // to functions being identities at the start.
  std::iota(mu.begin(), mu.end(), 0);
  std::iota(phi.begin(), phi.end(), 0);
  std::iota(ro.begin(), ro.end(), 0);
  std::iota(root.begin(), root.end(), 0);

  // Greedy matching
  for (NodeId i = 0; i < num_nodes(); i++) {
    for (NodeId j : node(i).neighbors()) {
      if (mu[i] == i && mu[j] == j) {
        mu[i] = j;
        mu[j] = i;
        break;
      }
    }
  }

  // Outer ones are the ones that are unmatched vertices, they will be bases of
  // the trees.
  for (NodeId i = 0; i < num_nodes(); i++) {
    if (mu[i] == i)
      outer.push_back(i);
  }

  NodeId v;
  while (!outer.empty()) {
    v = outer.back();
    outer.pop_back();

    for (NodeId u : node(v).neighbors()) {
      if (is_inner(u) || ro[u] == ro[v]) {
        continue;
      } else if (is_out_of_forest(u)) {
        grow(v, u);
        outer.push_back(mu[u]); // mu[u] is a new outer vertex, so we add it to
                                // stack/queue
      } else if (root[v] != root[u]) {
        augment(v, u);
        // Here we do not clear all trees, but only the ones we augmented. But
        // we do add all outer vertices back on the dequeue.
        outer.clear();
        for (NodeId i = 0; i < num_nodes(); i++) {
          if (root[i] == root[u] || root[i] == root[v]) {
            phi[i] = i;
            ro[i] = i;
          } else if (is_outer(i)) {
            outer.push_front(i);
          }
        }
        break;
      } else { // in this case u and v are outer vertices of the same tree (but
               // in different blossoms)
        shrink(v, u);
      }
    }
  }

  return mu;
}
inline bool Graph::is_outer(NodeId u) {
  return mu[u] == u || phi[mu[u]] != mu[u];
}
inline bool Graph::is_inner(NodeId u) {
  return phi[mu[u]] == mu[u] && phi[u] != u;
}
inline bool Graph::is_out_of_forest(NodeId u) {
  return phi[u] == u && phi[mu[u]] == mu[u] && mu[u] != u;
}

} // namespace ED
