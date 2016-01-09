#include "dcstsolver.hpp"
#include "scip_exception.hpp"

DCSTSolver::DCSTSolver() {
  // initialize scip
  SCIP_CALL_EXC(SCIPcreate(&scip));

  // set time limit
  SCIP_CALL_EXC(SCIPsetRealParam(scip, "limits/time", 300));

  // load default plugins linke separators, heuristics, etc.
  //SCIP_CALL_EXC(SCIPincludeDefaultPlugins(scip));

  // disable scip output to stdout
  //SCIP_CALL_EXC(SCIPsetMessagehdlr(scip, NULL));

  // create an empty problem
  SCIP_CALL_EXC(SCIPcreateProb(scip, "dcst", NULL, NULL, NULL, NULL, NULL, NULL, NULL));

  // set the objective sense to minimize
  SCIP_CALL_EXC(SCIPsetObjsense(scip, SCIP_OBJSENSE_MINIMIZE));

  // build variables
  int counter = 0;
  // TODO think about saving references to each var in their corresponding edge object
  for (Edge edge = edges.begin(); edge != edges.end(); edge++) {
    // create variable
    SCIP_VAR* var;
    std::ostringstream varname;
    varname << "x" << couter;
    // SCIP_VARTYPE_INTEGER vs SCIP_VARTYPE_CONTINUOUS
    SCIP_CALL_EXC(SCIPcreateVar(scip, &var, varname.str().c_str(), 0.0, 1.0, edge.cost(), SCIP_VARTYPE_INTEGER, TRUE, FALSE, NULL, NULL, NULL, NULL, NULL));

    // add variable
    SCIP_CALL_EXC(SCIPaddVar(scip, var));

    counter++;
  }

  // add pricers
  SCIP_CALL_EXC(SCIPactivatePricer(scip, ...));

  // create constraints
  SCIP_CONS * cons;
  SCIP_CALL_EXC(SCIPcreateConsLinear(scip, &cons, ...));

  // add constraints
  SCIP_CALL_EXC(SCIPaddConsLinear(scip, cons);
}

DCSTSolver::~DCSTSolver() {
  SCIPreleaseCons()
    SCIPreleaseVar()
}

void DCSTSolver::solve() {
  SCIP_CALL_EXC(SCIPsolve(scip));
}

