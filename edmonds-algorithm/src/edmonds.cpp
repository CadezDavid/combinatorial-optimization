#include <algorithm>
#include <chrono>
#include <deque>
#include <numeric>

#include "graph.cpp"

namespace ED {

void Graph::shrink(NodeId v, NodeId u) {
  std::vector<NodeId> p_v = {ro[v]};
  std::vector<NodeId> p_u = {ro[u]};

  while (p_u.back() != root[u])
    p_u.push_back(ro[phi[mu[p_u.back()]]]);

  while (p_v.back() != root[v])
    p_v.push_back(ro[phi[mu[p_v.back()]]]);

  while (p_v.size() > 1 && p_u.size() > 1 &&
         p_v[p_v.size() - 2] == p_u[p_u.size() - 2]) {
    p_u.pop_back();
    p_v.pop_back();
  }
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

  for (NodeId z = 0; z < num_nodes(); z++)
    if (root[z] == root[u] &&
        (std::find(p_v.begin(), p_v.end(), ro[phi[z]]) != p_v.end() ||
         std::find(p_u.begin(), p_u.end(), ro[phi[z]]) != p_u.end()))
      ro[z] = r;
}

void Graph::grow(NodeId v, NodeId u) {
  phi[u] = v;
  root[u] = root[v];
  root[mu[u]] = root[v];
}

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

std::vector<NodeId> Graph::edmonds() {
  std::deque<NodeId> outer = {};

  std::iota(mu.begin(), mu.end(), 0);
  std::iota(phi.begin(), phi.end(), 0);
  std::iota(ro.begin(), ro.end(), 0);
  std::iota(root.begin(), root.end(), 0);

  for (NodeId i = 0; i < num_nodes(); i++) {
    for (NodeId j : node(i).neighbors()) {
      if (mu[i] == i && mu[j] == j) {
        mu[i] = j;
        mu[j] = i;
        break;
      }
    }
  }
  for (NodeId i = 0; i < num_nodes(); i++) {
    if (mu[i] == i)
      outer.push_back(i);
  }

  NodeId v;
  while (!outer.empty()) {
    v = outer.back();
    outer.pop_back();

    for (NodeId u : node(v).neighbors()) {
      if (is_out_of_forest(u)) {
        grow(v, u);
        outer.push_back(mu[u]);
      } else if (is_inner(u) || ro[u] == ro[v]) {
        continue;
      } else if (root[v] != root[u]) {
        augment(v, u);
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
      } else {
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
