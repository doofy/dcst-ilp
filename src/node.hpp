#ifndef NODE_H
#define NODE_H

#include "edge.hpp"

#include <vector>

class Node {
  private:

  public:
    Node(int);
    ~Node();
    int degree;
    std::vector<int> edges;
};
#endif
