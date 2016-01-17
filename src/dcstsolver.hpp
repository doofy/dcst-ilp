#ifndef DCSTSOLVER_H
#define DCSTSOLVER_H

#include "graph.hpp"

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

class DCSTSolver {
  private:
    SCIP* scip;

  public:
    DCSTSolver(Graph&);
    ~DCSTSolver();
    void solve(void);
};
#endif
