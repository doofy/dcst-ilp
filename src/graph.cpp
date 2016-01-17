#include "graph.hpp"

#include <sstream>

Graph::Graph() {
}

Graph::~Graph() {
}

std::string Graph::toString() {
  std::ostringstream s;
  s << name << std::endl;
  s << nodes.size() << std::endl;
  s << edges.size() << std::endl;
  for (std::vector<Node>::iterator node = nodes.begin(); node != nodes.end(); node++) {
    s << node->degree << std::endl;
  }
  for (std::vector<Edge>::iterator edge = edges.begin(); edge != edges.end(); edge++) {
    s << edge->from << " " << edge->to << " " << edge->cost  << " " << edge->partOfSolution << std::endl;
  }
  return s.str();
}
