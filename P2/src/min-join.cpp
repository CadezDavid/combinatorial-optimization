#include "min-join.hpp"

std::tuple<std::vector<std::vector<MMC::NodeId>>,
           std::vector<std::vector<double>>>
all_shortest_paths(MMC::Graph &graph, double gamma) {
  std::vector<std::vector<MMC::NodeId>> paths(
      graph.num_nodes(), std::vector<MMC::NodeId>(graph.num_nodes()));
  std::vector<std::vector<double>> distances(
      graph.num_nodes(),
      std::vector<double>(graph.num_nodes(),
                          std::numeric_limits<double>::max()));

  for (MMC::NodeId v{0}; v < graph.num_nodes(); v++) {
    std::vector<MMC::NodeId> ancestor(graph.num_nodes());
    std::vector<double> distance(graph.num_nodes(),
                                 std::numeric_limits<double>::max());

    std::deque<MMC::NodeId> stack{v};
    ancestor[v] = v;
    distance[v] = 0;

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

  MMC::NodeId num_nodes = 0;
  std::vector<MMC::NodeId> conv{};
  for (MMC::NodeId u{0}; u < graph.num_nodes(); u++) {
    int count = 0;
    for (MMC::EdgeId e : graph.node(u).outgoing_halfedges()) {
      if (graph.halfedge_weight(e) < gamma)
        count++;
    }
    if (count % 2 == 1) {
      conv.push_back(u);
      num_nodes++;
    }
  }

  auto [paths, distances] = all_shortest_paths(graph, gamma);

  PerfectMatching solver{static_cast<int>(num_nodes),
                         static_cast<int>(num_nodes)};
  for (MMC::NodeId u{0}; u < num_nodes; u++)
    for (MMC::NodeId v{u + 1}; v < num_nodes; v++)
      solver.AddEdge(u, v, distances[conv[u]][conv[v]]);

  solver.Solve();

  int counter = 0;
  std::vector<MMC::EdgeId> t_join(graph.num_edges(), 0);
  for (MMC::NodeId u{0}; u < num_nodes; u++) {
    for (MMC::NodeId v{u + 1}; v < num_nodes; v++) {
      if (solver.GetSolution(counter)) {
        std::vector<MMC::EdgeId> cur_path{};
        MMC::NodeId u_g = conv[u];
        MMC::NodeId v_g = conv[v];
        while (u_g != v_g) {
          for (MMC::EdgeId t : graph.node(u_g).outgoing_halfedges())
            if (graph.halfedge(t).target() == paths[u_g][v_g])
              t_join[t / 2] = t_join[t / 2] ? 0 : 1;
          u_g = paths[u_g][v_g];
        }
      }
      counter++;
    }
  }

  MMC::Graph solution{graph.num_nodes()};
  for (MMC::NodeId v{0}; v < graph.num_nodes(); v++) {
    for (MMC::EdgeId e : graph.node(v).outgoing_halfedges()) {
      if (t_join[e / 2] != (graph.halfedge_weight(e) < gamma))
        solution.add_edge(v, graph.halfedge(e).target(),
                          graph.halfedge_weight(e));
    }
  }

  return solution;
}
