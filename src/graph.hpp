#ifndef GRAPH_H
#define GRAPH_H

#include "node.hpp"
#include "edge.hpp"

#include <vector>
#include <string>

class Graph {
  private:

  public:
    Graph();
    ~Graph();
    std::string toString();
    std::string name;
    std::vector<Node> nodes;
    std::vector<Edge> edges;
};
#endif
