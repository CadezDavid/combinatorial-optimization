// A so called include guard uses the preprocessor to make sure nothing happens
// when this header is include a second time. This becomes important if there
// are many headers including each other, as undirected cycles can usually not
// be avoided.
#ifndef GRAPH_HPP
#define GRAPH_HPP

/**
   @file graph.hpp

   @brief This file provides a simple class @c Graph to model unweighted
undirected graphs.
**/

/**
 * In and output in the standard library is done using streams.
 * In this header we only need to know std::istream and std::ostream are
 * classes, since we only declare the function which read write our graph from
 * an std::istream or to an std::ostream, so we only include the forward
 * declaration.
 */
#include <iosfwd>

/**
 * This header defined many different integer types,
 * enabling us to choose what integers we want to use.
 */
#include <cstdint>

/**
 * Limits are provided by the standard library to check
 * e.g. if some value can be represented in a certain integer type.
 */
#include <limits>

/**
 *  Vectors are implemented in the standard library as std::vector.
 *  They encapsulate an array of dynamic size,
 *  so that you don't have to know about the exact implementation.
 *  If you add an element in the end (aka push_back) but the dynamic array is
 * full, it will automatically be resized. See
 * https://en.cppreference.com/w/cpp/container/vector for documentation.
 */
#include <vector>

/**
 * Namespaces can be used in order to make sure different modules,
 * possibly implemented by different people don't have classes/functions/...
 * with the same name. If you want to refer to some symbol S,
 * which is defined in a namespace N from outside of that namespace,
 * you need to write N::S.
 * The most prominent example is std, the namespace used by the standard
 * library. This namespace is intended to be used for Minimum Mean Cycle.
 * */
namespace MMC {

/**
 * Using names for types has many advantages.
 * One of them is being able to switch type with very little effort.
 * For example,  you can use singed integers here instead
 * if you want to avoid having to type cast when useing the perfect matching
 * solver,
 */
using size_type = uint32_t;

/**
 * Another advantage of naming types is making your code more readable.
 * For example an Id is usually a light weight object (read: few bits)
 * which uniquely determines some object, in this case a node in our graph.
 * Note the same Id may be used by different graphs though!
 */
using NodeId = size_type;

/**
 * We introduce two directed edges or "half edges" for each undirected edge.
 */
using EdgeWeight = int32_t;
using EdgeId = size_type;
using HalfEdgeId = size_type;

/**
   @class Node

   @brief A @c Node stores an array of outgoing half edges (via their ids).

   @note The neighbors are not necessarily ordered, so searching for a specific
neighbor takes O(degree)-time.
**/
class Node {
public:
  /** @brief Create an isolated node (you can add neighbors later). **/
  Node() = default;

  /** @return The number of neighbors of this node. **/
  size_type degree() const;

  /** @return The array of ids of the outgoing half edges of this node. **/
  std::vector<HalfEdgeId> const &outgoing_halfedges() const;

  /** @brief Adds @c id to the list of outgoing half edges of this node. */
  void add_outgoing_halfedge(HalfEdgeId const id);

private:
  std::vector<HalfEdgeId> _outgoing_halfedges;
}; // class Node

/**
   @class HalfEdge

   @brief One of the two halfs of some undirected edge.

   @note The half edge does not save its source,
   since we can look up the target of the inverse half edge.
*/
class HalfEdge {
public:
  /** @brief Do not allow creation of  a unititialized @c HalfEdge. */
  HalfEdge() = delete;

  /** @brief Create a @c HalfEdge with specified @c target and @c inverse. */
  HalfEdge(NodeId target, HalfEdgeId inverse);

  /** @return The id of this edge leads towards. */
  NodeId target() const;

  /** @return The id of the corresponding half edge in the opposite direction.
   */
  HalfEdgeId inverse() const;

private:
  NodeId _target;
  HalfEdgeId _inverse;
}; // class HalfEdge

/**
   @class Graph

   This class models unweighted undirected graphs only.
   Each edge is represented by two half edges.
   Both nodes and half edges are indexed using contiguous integers starting at
0. It is guaranteed all pairs of corresponding half edges follow each other
directly. In particualar, you end up with the same number if you divide their
ids by 2.
**/
class Graph {
public:
  /**
     @brief Creates a @c Graph with @c num_nodes isolated nodes.

     The number of nodes in the graph currently cannot be changed. You can only
  add edges between the existing nodes.
  **/
  Graph(NodeId const num_nodes);

  /** @return The number of nodes in the graph. **/
  NodeId num_nodes() const;

  /** @return The number of edges in the graph. **/
  EdgeId num_edges() const;

  /** @return The number of half edges in the graph. Is twice the number of
   * edges. */
  HalfEdgeId num_halfedges() const;

  /** @return A reference to node with the specified id. */
  Node const &node(NodeId const id) const;

  /** @return A reference to the half edge of this graph with the specified id.
   */
  HalfEdge const &halfedge(HalfEdgeId const id) const;

  /** @return The weight of the specified edge. */
  EdgeWeight edge_weight(HalfEdgeId const id) const;

  /** @return The weight of the specfiied half edge (i.e. the weight of its
   * edge). */
  EdgeWeight halfedge_weight(HalfEdgeId const id) const;

  /**
     @brief Adds the edge <tt> {node1_id, node2_id} </tt> with specified weight.

     Checks that @c node1_id and @c node2_id are distinct and throws an
  exception otherwise.
     @warning Does not check that the edge does not already exist, so this class
  can be used to model non-simple graphs.
  **/
  void add_edge(NodeId node1_id, NodeId node2_id, EdgeWeight weight);

  // Static functions are not called on an object of the class, but on the class
  // itself.
  /**
   * @brief Reads a graph in DIMACS format from the given istream and returns
   * that graph.
   */
  static Graph read_dimacs(std::istream &str);

  /**
    @brief Prints the graph to the given ostream in DIMACS format.
  **/
  friend std::ostream &operator<<(std::ostream &str, Graph const &graph);

private:
  std::vector<Node> _nodes;
  std::vector<HalfEdge> _halfedges;
  std::vector<EdgeWeight> _edge_weights;
}; // class Graph

// Calling a function usually has some constant time overhead.
// The compiler is capable of "inlining" function calls,
// which means when your code calls this function,
// the compiler will instead insert the content of the function.
// This has no affect on your code, but will get rid of this overhead.
// The inline keywoard recommends the compiler to inline a function.
// If you use it for some, you must implement that function
// in the header! For readablility, we put all implementations
// of inline function into the following inline section.
// BEGIN: Inline section

inline size_type Node::degree() const { return outgoing_halfedges().size(); }

inline std::vector<EdgeId> const &Node::outgoing_halfedges() const {
  return _outgoing_halfedges;
}

inline HalfEdge::HalfEdge(NodeId const target, HalfEdgeId const inverse)
    : _target(target), _inverse(inverse) {}

inline NodeId HalfEdge::target() const { return _target; }

inline HalfEdgeId HalfEdge::inverse() const { return _inverse; }

inline NodeId Graph::num_nodes() const { return _nodes.size(); }

inline EdgeId Graph::num_edges() const { return _edge_weights.size(); }

inline HalfEdgeId Graph::num_halfedges() const { return _halfedges.size(); }

inline Node const &Graph::node(NodeId const id) const { return _nodes[id]; }

inline HalfEdge const &Graph::halfedge(HalfEdgeId const id) const {
  return _halfedges[id];
}

inline EdgeWeight Graph::edge_weight(EdgeId const id) const {
  return _edge_weights[id];
}

inline EdgeWeight Graph::halfedge_weight(HalfEdgeId const id) const {
  return _edge_weights[id / 2];
}
// END: Inline section

} // namespace MMC

#endif /* GRAPH_HPP */
