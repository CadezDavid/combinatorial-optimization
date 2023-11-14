#include <algorithm>
#include <chrono>
#include <deque>

#include "graph.cpp"

using vector = std::vector<ED::NodeId>;

void shrink(vector &mu, vector &phi, vector &ro, vector &root,
            const ED::Graph &graph, ED::NodeId v, ED::NodeId u) {
  vector p_v = {ro[v]};
  vector p_u = {ro[u]};

  while (p_u.back() != root[u])
    p_u.push_back(ro[phi[mu[p_u.back()]]]);

  while (p_v.back() != root[v])
    p_v.push_back(ro[phi[mu[p_v.back()]]]);

  while (p_v.size() > 1 && p_u.size() > 1 &&
         p_v[p_v.size() - 2] == p_u[p_u.size() - 2]) {
    p_u.pop_back();
    p_v.pop_back();
  }
  ED::NodeId r = p_u.back();

  if (ro[v] != r) {
    for (ED::NodeId z = mu[v]; ro[phi[z]] != r; z = mu[phi[z]])
      phi[phi[z]] = z;
    phi[v] = u;
  }

  if (ro[u] != r) {
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

void grow(vector &mu, vector &phi, vector &root, ED::NodeId v, ED::NodeId u) {
  phi[u] = v;
  root[u] = root[v];
  root[mu[u]] = root[v];
}

void augment(vector &mu, vector &phi, ED::NodeId v, ED::NodeId u) {
  vector p_u, p_v = {};

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
}

vector edmonds(const ED::Graph graph) {
  vector mu(graph.num_nodes());
  vector phi(graph.num_nodes());
  vector ro(graph.num_nodes());
  vector root(graph.num_nodes());
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
    v = outer.back();
    outer.pop_back();

    for (ED::NodeId u : graph.node(v).neighbors()) {
      if (phi[u] == u && phi[mu[u]] == mu[u] && mu[u] != u) {
        grow(mu, phi, root, v, u);
        outer.push_back(mu[u]);
      } else if ((mu[u] != u && phi[mu[u]] == mu[u]) || ro[u] == ro[v]) {
        continue;
      } else if (root[u] != root[v]) {
        augment(mu, phi, v, u);

        outer.clear();
        for (ED::NodeId i = 0; i < graph.num_nodes(); i++) {
          if (root[i] == root[u] || root[i] == root[v]) {
            phi[i] = i;
            ro[i] = i;
          } else if (mu[i] == i || phi[mu[i]] != mu[i]) {
            outer.push_front(i);
          }
        }
        break;
      } else {
        shrink(mu, phi, ro, root, graph, v, u);
      }
    }
  }

  return mu;
}
