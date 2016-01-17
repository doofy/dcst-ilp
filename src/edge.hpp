#ifndef EDGE_H
#define EDGE_H

#include <scip/scip.h>

class Edge {
  private:

  public:
    Edge(int, int, int, int);
    ~Edge();
    int from;
    int to;
    int cost;
    float partOfSolution;
    SCIP_VAR* var;
};
#endif
