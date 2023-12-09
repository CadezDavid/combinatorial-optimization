#include "min-join.hpp"

std::tuple<std::vector<std::vector<MMC::NodeId>>,
           std::vector<std::vector<double>>>
all_shortest_paths(MMC::Graph &graph, double gamma) {
  std::vector<std::vector<MMC::NodeId>> paths = std::vector(
      graph.num_nodes(), std::vector<MMC::NodeId>(graph.num_nodes()));
  std::vector<std::vector<double>> distances =
      std::vector(graph.num_nodes(),
                  std::vector<double>(graph.num_nodes(),
                                      std::numeric_limits<double>::max()));

  for (MMC::NodeId v{0}; v < graph.num_nodes(); v++) {
    auto ancestor = std::vector<MMC::NodeId>(graph.num_nodes());
    auto distance = std::vector<double>(
        graph.num_nodes(), std::numeric_limits<MMC::EdgeWeight>::max());
    distance[v] = 0;

    std::deque<MMC::NodeId> stack = {v};
    ancestor[v] = v;

    MMC::NodeId u, z;
    double w;
    while (!stack.empty()) {
      u = stack.front();
      stack.pop_front();

      for (MMC::EdgeId edge : graph.node(u).outgoing_halfedges()) {
        z = graph.halfedge(edge).target();
        w = std::abs(graph.halfedge_weight(edge) - gamma);
        if ((distance[z] > distance[u] + w) && (z != ancestor[u])) {
          stack.push_back(z);
          distance[z] = distance[u] + w;
          ancestor[z] = u;
        }
      }
    }
    for (MMC::NodeId u{0}; u < graph.num_nodes(); u++) {
      distances[u][v] = distance[u];
      paths[u][v] = ancestor[u];
    }
  }
  return {paths, distances};
}

MMC::Graph min_join(MMC::Graph &graph, double gamma) {

  MMC::NodeId sum = 0;
  std::vector<MMC::NodeId> conv{}; // convert indices in k_t to nodeid in graph
  for (MMC::NodeId u{0}; u < graph.num_nodes(); u++) {
    int count = 0;
    for (MMC::EdgeId e : graph.node(u).outgoing_halfedges()) {
      if (graph.halfedge_weight(e) < gamma)
        count++;
    }
    if (count % 2 == 1) {
      conv.push_back(u);
      sum++;
    }
  }

  auto [paths, distances] = all_shortest_paths(graph, gamma);

  PerfectMatching solver{static_cast<int>(sum), static_cast<int>(sum)};
  for (MMC::NodeId u{0}; u < sum; u++)
    for (MMC::NodeId v{u + 1}; v < sum; v++)
      solver.AddEdge(u, v, distances[conv[u]][conv[v]]);

  // for (MMC::EdgeId edge_id{0}; edge_id < k_t.num_edges(); ++edge_id) {
  //   solver.AddEdge(k_t.halfedge(2 * edge_id + 1).target(),
  //                  k_t.halfedge(2 * edge_id + 0).target(),
  //                  k_t.edge_weight(edge_id));
  // }

  solver.Solve();

  std::vector<MMC::EdgeId> t_join{};
  for (MMC::NodeId u{0}; u < sum; u++) {
    for (MMC::NodeId v{u + 1}; v < sum; v++) {
      if (solver.GetSolution(u * sum + v - 1)) {
        std::vector<MMC::EdgeId> cur_path{};
        MMC::NodeId u_g = conv[u];
        MMC::NodeId v_g = conv[v];
        while (u_g != v_g) {
          for (MMC::EdgeId t : graph.node(u_g).outgoing_halfedges())
            if (graph.halfedge(t).target() == paths[u_g][v_g])
              cur_path.push_back(t / 2);

          u_g = paths[u_g][v_g];
        }
        std::vector<MMC::EdgeId> sym_dif;
        std::sort(t_join.begin(), t_join.end());
        std::sort(cur_path.begin(), cur_path.end());
        std::set_symmetric_difference(t_join.begin(), t_join.end(),
                                      cur_path.begin(), cur_path.end(),
                                      std::back_inserter(sym_dif));
        t_join = sym_dif;
      }
    }
  }

  MMC::Graph solution{graph.num_nodes()};
  for (MMC::NodeId v{0}; v < graph.num_nodes(); v++) {
    for (MMC::EdgeId e : graph.node(v).outgoing_halfedges()) {
      if (((std::find(t_join.begin(), t_join.end(), e / 2) != t_join.end()) &&
           graph.halfedge_weight(e) >= gamma) ||
          ((std::find(t_join.begin(), t_join.end(), e / 2) == t_join.end()) &&
           graph.halfedge_weight(e) < gamma))
        solution.add_edge(v, graph.halfedge(e).target(),
                          graph.halfedge_weight(e));
    }
  }

  return solution;
}
