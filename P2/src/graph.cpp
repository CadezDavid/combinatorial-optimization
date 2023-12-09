#include "graph.hpp" // always include corresponding header first

/**
 * Note this included everything from the header similar to copy-pasting it
 * here, including our two classes, the function declarations and all the
 * includes. In this file we will actually implement the in- and output routines
 * though, so we need to include the actual implementation of std::istream and
 * std::ostream.
 */
#include <iostream>

/**
 * We are also going to use stringstream in order to treat a line,
 * which we have already read from the input, like an input stream.
 */
#include <sstream>

/**
 * The execption header is used to terminate our program
 * in the case of unexpected input,
 * which is often the best way to handle such input.
 * More complex programs may want to catch exceptions in
 * surrouding code and either try to recover or help debug them.
 */
#include <stdexcept>

// Anonymous name spaces may be used to show the reader
// that a function will only be used in the current file.
namespace {

// Using a function for converting the DIMACS node ids to and from our node ids
// makes the in and output code more understandable.
MMC::NodeId from_dimacs_id(MMC::size_type dimacs_node_id) {
  if (dimacs_node_id <= 0) {
    throw std::runtime_error(
        "Non-positive DIMACS node id can not be converted.");
  }
  return dimacs_node_id - 1;
}

MMC::size_type to_dimacs_id(MMC::NodeId node_id) { return node_id + 1; }

// Returns the first line which is not a comment, i.e. does not start with c.
std::string read_next_non_comment_line(std::istream &input) {
  std::string line;
  do {
    if (!std::getline(input, line)) {
      throw std::runtime_error("Unexpected end of DIMACS stream.");
    }
  } while (line[0] == 'c');
  return line;
}

} // end of anonymous namespace

namespace MMC {
/////////////////////////////////////////////
//! \c Node definitions
/////////////////////////////////////////////

void Node::add_outgoing_halfedge(HalfEdgeId const id) {
  _outgoing_halfedges.push_back(id);
}

/////////////////////////////////////////////
//! \c Graph definitions
/////////////////////////////////////////////

// Whenever reasonably possible you should prefer to use `:`
// to initalize the members of your class, instead of
// assigning values to them after they were default initialized.
// Note you should initialize them in the same order
// they were declare in back in the class body!
Graph::Graph(NodeId const num_nodes) : _nodes(num_nodes) {}

void Graph::add_edge(NodeId const node1_id, NodeId const node2_id,
                     EdgeWeight const weight) {
  // It is ok if your program crashes for garbage input,
  // but it should be an explicit, deliberate choice, e.g. like this.
  if (node1_id == node2_id) {
    throw std::runtime_error("MMC::Graph class does not support loops!");
  }
  for (EdgeId e : node(node1_id).outgoing_halfedges()) {
    if (halfedge(e).target() == node2_id) {
      return;
    }
  }

  HalfEdgeId const halfedge1_id = _halfedges.size();
  HalfEdgeId const halfedge2_id = halfedge1_id + 1;
  _nodes[node1_id].add_outgoing_halfedge(halfedge1_id);
  _nodes[node2_id].add_outgoing_halfedge(halfedge2_id);
  _halfedges.emplace_back(node2_id, halfedge2_id);
  _halfedges.emplace_back(node1_id, halfedge1_id);
  _edge_weights.emplace_back(weight);
}

Graph Graph::read_dimacs(std::istream &input) {
  // Unfortunatley the common std input functions require us to first declare
  // our variables and assign them the correct values only later.
  // Because we want to avoid unitizalized variables, we use a new syntax
  // added in c++17 to call the constuctor with no arguments,
  // often called the default constructor: We write {} behind the variable name.

  // When parsing the DIMACS format, there are some words we are not interested
  // in. We read them into this variable and never use the afterwards.
  std::string unused_word{};

  // As we need to watch out for comments, we first need to read the input by
  // line. In order to split non-comment lines into multiple variables we use a
  // std::stringstream.
  std::stringstream first_buffering_stream{};

  // Note if you do not plan to modify a variable, always declare it as
  // constant. This does not only prevent you from doing so accidently, but also
  // helps anybody reading your code understand what you are doing, as there are
  // less possiblities what can happen.
  std::string const first_line = read_next_non_comment_line(input);

  size_type num_nodes{};
  size_type num_edges{};
  first_buffering_stream << first_line;
  first_buffering_stream >> unused_word >> unused_word >> num_nodes >>
      num_edges;

  // Now we successively add edges to our graph;
  Graph graph(num_nodes);
  for (size_type i = 1; i <= num_edges; ++i) {
    // This works just as parsing the first line!
    std::stringstream ith_buffering_stream{};
    std::string const ith_line = read_next_non_comment_line(input);
    size_type dimacs_node1{};
    size_type dimacs_node2{};
    EdgeWeight weight{};
    ith_buffering_stream << ith_line;
    ith_buffering_stream >> unused_word >> dimacs_node1 >> dimacs_node2 >>
        weight;
    graph.add_edge(from_dimacs_id(dimacs_node1), from_dimacs_id(dimacs_node2),
                   weight);
  }

  return graph;
}

std::ostream &operator<<(std::ostream &output, Graph const &graph) {
  // We use std::endl to write new lines here.
  // If you prefer the new line character, \n on linux, that one works fine,
  // too.
  output << "p edge " << graph.num_nodes() << " " << graph.num_edges()
         << std::endl;

  for (EdgeId edge_id{0}; edge_id < graph.num_edges(); ++edge_id) {
    output << "e " << to_dimacs_id(graph.halfedge(2 * edge_id + 1).target())
           << " " << to_dimacs_id(graph.halfedge(2 * edge_id + 0).target())
           << " " << graph.edge_weight(edge_id) << std::endl;
  }

  // Streams sometimes buffer their output.
  // Once one is done with some output routine, it can make sense to flush them,
  // which clears the buffer and writes the remaining output.
  output << std::flush;
  return output;
}

} // namespace MMC
