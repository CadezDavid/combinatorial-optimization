#include <algorithm>
#include <chrono>
#include <deque>

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
  std::vector<NodeId> p_u, p_v = {};

  for (NodeId z = mu[v]; z != mu[z]; z = mu[phi[z]]) {
    p_v.push_back(z);
    p_v.push_back(phi[z]);
  }
  for (NodeId z = mu[u]; z != mu[z]; z = mu[phi[z]]) {
    p_u.push_back(z);
    p_u.push_back(phi[z]);
  }
  for (NodeId i = 0; i < p_v.size(); i += 2) {
    mu[p_v[i]] = p_v[i + 1];
    mu[p_v[i + 1]] = p_v[i];
  }
  for (NodeId i = 0; i < p_u.size(); i += 2) {
    mu[p_u[i]] = p_u[i + 1];
    mu[p_u[i + 1]] = p_u[i];
  }
  mu[v] = u;
  mu[u] = v;
}

std::vector<NodeId> Graph::edmonds() {
  std::deque<NodeId> outer = {};

  for (NodeId i = 0; i < num_nodes(); i++) {
    outer.push_back(i);
    mu[i] = i;
    phi[i] = i;
    ro[i] = i;
    root[i] = i;
  }

  NodeId v;
  while (!outer.empty()) {
    v = outer.back();
    outer.pop_back();

    for (NodeId u : node(v).neighbors()) {
      if (phi[u] == u && phi[mu[u]] == mu[u] && mu[u] != u) {
        grow(v, u);
        outer.push_back(mu[u]);
      } else if ((mu[u] != u && phi[mu[u]] == mu[u]) || ro[u] == ro[v]) {
        continue;
      } else if (root[u] != root[v]) {
        augment(v, u);

        outer.clear();
        for (NodeId i = 0; i < num_nodes(); i++) {
          if (root[i] == root[u] || root[i] == root[v]) {
            phi[i] = i;
            ro[i] = i;
          } else if (mu[i] == i || phi[mu[i]] != mu[i]) {
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
} // namespace ED
