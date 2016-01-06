#include "dcstsolver.hpp"
#include "scip_exception.hpp"

DCSTSolver::DCSTSolver() {
  // initialize scip
  SCIP_CALL_EXC(SCIPcreate(&scip));

  // load default plugins linke separators, heuristics, etc.
  SCIP_CALL_EXC(SCIPincludeDefaultPlugins(scip));

  // disable scip output to stdout
  //SCIP_CALL_EXC(SCIPsetMessagehdlr(scip, NULL));

  // create an empty problem
  SCIP_CALL_EXC(SCIPcreateProb(scip, "dcst", NULL, NULL, NULL, NULL, NULL, NULL, NULL));

  // set the objective sense to maximize, default is minimize
  SCIP_CALL_EXC(SCIPsetObjsense(scip, SCIP_OBJSENSE_MINIMIZE));
}

DCSTSolver::~DCSTSolver() {
}

void DCSTSolver::solve() {
  SCIP_CALL_EXC(SCIPsolve(scip));
}

