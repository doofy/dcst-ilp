#ifndef DCSTSOLVER_H
#define DCSTSOLVER_H

#include "graph.hpp"

#include <scip/scip.h>

class DCSTSolver {
  private:
    SCIP* scip;

  public:
    DCSTSolver(Graph&);
    ~DCSTSolver();
    void solve(void);
    void display(Graph&);
    SCIP_RETCODE prepare(Graph&);
};
#endif
