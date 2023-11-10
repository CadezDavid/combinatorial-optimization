// A so called include guard uses the preprocessor to make sure nothing happens
// when this header is include a second time. This becomes important if there
// are many headers including each other, as undirected cycles can usually not
// be avoided.
#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>

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
 * library. This namespace is intended to be used for Edmonds.
 * */
namespace ED {

/**
 * Using names for types has many advantages.
 * One of them is being able to switch type with very little effort.
 * For now, we are going to use unsigned (i.e. non negative)
 * 32 bit integers for all sizes and indices.
 * But if there was some large graph for which we need 64 bit indices,
 * we would only need to change the type once, right here!
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
   @class Node

   @brief A @c Node stores an array of neighbors (via their ids).

   @note The neighbors are not necessarily ordered, so searching for a specific
neighbor takes O(degree)-time.
**/
class Node {
public:
  /** @brief Create an isolated node (you can add neighbors later). **/
  Node() = default;

  /** @return The number of neighbors of this node. **/
  size_type degree() const;

  /** @return The array of ids of the neighbors of this node. **/
  std::vector<NodeId> const &neighbors() const;

private:
  // This allows each Graph to access private members of this class,
  // in our case the add_neighbor function
  friend class Graph;

  /**
     @brief Adds @c id to the list of neighbors of this node.
     @warning Does not check whether @c id is already in the list of neighbors
    (a repeated neighbor is legal, and models parallel edges).
     @warning Does not check whether @c id is the identity of the node itself
    (which would create a loop!).
  **/
  void add_neighbor(NodeId const id) { _neighbors.push_back(id); }

  std::vector<NodeId> _neighbors = {};
}; // class Node

/**
   @class Graph

   @brief A @c Graph stores an array of @c Node s, but no array of edges. The
list of edges is implicitly given by the fact that the nodes know their
neighbors.

   This class models undirected graphs only (in the sense that the method @c
add_edge(node1, node2) adds both @c node1 as a neighbor of @c node2 and @c node2
as a neighbor of @c node1). It also forbids loops, but parallel edges are legal.

   @warning Nodes are numbered starting at 0, as is usually done in programming,
    instead starting at 1, as is done in the DIMACS format that your program
should take as input! Be careful.
**/
class Graph {
public:
  /**
     @brief Creates a @c Graph with @c num_nodes isolated nodes.

     The number of nodes in the graph currently cannot be changed. You can only
  add edges between the existing nodes.
  **/
  Graph(NodeId const num_nodes) {
    for (int i = 0; i < num_nodes; i++) {
      _nodes.push_back(Node());
    }
  };

  /** @return The number of nodes in the graph. **/
  NodeId num_nodes() const;

  /** @return The number of edges in the graph. **/
  size_type num_edges() const;

  /**
     @return A reference to the id-th entry in the array of @c Node s of this
  graph.
  **/
  Node const &node(NodeId const id) const;

  /**
     @brief Adds the edge <tt> {node1_id, node2_id} </tt> to this graph.

     Checks that @c node1_id and @c node2_id are distinct and throws an
  exception otherwise. This method adds both @c node1_id as a neighbor of @c
  node2_id and @c node2_id as a neighbor of @c node1_id.

     @warning Does not check that the edge does not already exist, so this class
  can be used to model non-simple graphs.
  **/
  void add_edge(NodeId node1_id, NodeId node2_id) {
    _nodes[node1_id].add_neighbor(node2_id);
    _nodes[node2_id].add_neighbor(node1_id);
  };

  // Static functions are not called on an object of the class, but on the class
  // itself.
  /**
   * Reads a graph in DIMACS format from the given istream and returns that
   * graph.
   */
  static Graph read_dimacs(std::istream &str) {
    std::string a, b, c, d = {};
    str >> a >> b >> c >> d;
    NodeId num_nodes = std::stoi(c);

    ED::Graph graph = Graph(num_nodes);

    while (!(str >> a >> b >> c).eof()) {
      NodeId node1_id = std::stoi(b) - 1;
      NodeId node2_id = std::stoi(c) - 1;
      graph.add_edge(node1_id, node2_id);
    };

    return graph;
  };
  /**
    @brief Prints the graph to the given ostream in DIMACS format.
  **/
  friend std::ostream &operator<<(std::ostream &str, Graph const &graph) {
    std::cout << "p edge " << graph.num_nodes() << " " << graph.num_edges()
              << "\n";
    for (int i = 0; i < graph.num_nodes(); i++) {
      for (int j : graph.node(i).neighbors()) {
        std::cout << "e " << i << " " << j << "\n";
      }
    }
    return str;
  };

private:
  std::vector<Node> _nodes;
  size_type _num_edges;
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

inline size_type Node::degree() const { return neighbors().size(); }

inline std::vector<NodeId> const &Node::neighbors() const { return _neighbors; }

inline NodeId Graph::num_nodes() const { return _nodes.size(); }

inline size_type Graph::num_edges() const { return _num_edges; }

inline Node const &Graph::node(NodeId const id) const { return _nodes[id]; }
// END: Inline section

} // namespace ED

#endif /* GRAPH_HPP */
