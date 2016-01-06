#ifndef DCSTSOLVER_H
#define DCSTSOLVER_H

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

class DCSTSolver {
  private:
    SCIP* scip;

  public:
    DCSTSolver();
    ~DCSTSolver();
    void solve(void);
};
#endif
