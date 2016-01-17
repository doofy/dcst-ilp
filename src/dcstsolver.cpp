#include "dcstsolver.hpp"
#include "edge.hpp"
#include "node.hpp"
#include "graph.hpp"
#include "scip_exception.hpp"

#include <vector>
#include <sstream>

DCSTSolver::DCSTSolver(Graph& graph) {
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
  std::vector<Edge> &edges = graph.edges;
  for (std::vector<Edge>::iterator edge = edges.begin(); edge != edges.end(); edge++) {
    // create variable
    SCIP_VAR* var;
    std::ostringstream varname;
    varname << "x" << counter;
    // TODO we should probably use SCIPcreateVarBasic here
/*
    SCIP_CALL_EXC(SCIPcreateVar(scip, // problem instance
          &var, // reference
          varname.str().c_str(), // name
          0.0, 1.0, // upper and lower bounds
          edge->cost, // TODO ?objective function value TODO?
          SCIP_VARTYPE_INTEGER, // SCIP_VARTYPE_INTEGER vs SCIP_VARTYPE_CONTINUOUS
          TRUE, // TODO ?should var's column be present in the initial root LP?
          FALSE, // TODO ?is var's column removable from the LP (due to aging or cleanup)?
          NULL,
          NULL,
          NULL,
          NULL,
          NULL)); // user data for this specific variable, or NULL

    // add variable
    SCIP_CALL_EXC(SCIPaddVar(scip, var));
*/
    counter++;
  }

  // add pricers
  // TODO do we need pricing?
  //SCIP_CALL_EXC(SCIPactivatePricer(scip, ...));

  // create constraints
  //SCIP_CALL_EXC(SCIPcreateConsLinear(scip, &cons, ...));

  // add constraints
  //SCIP_CALL_EXC(SCIPaddConsLinear(scip, cons));
}

DCSTSolver::~DCSTSolver() {
  //SCIPreleaseCons();
  //SCIPreleaseVar();
}

void DCSTSolver::solve() {
  SCIP_CALL_EXC(SCIPsolve(scip));
}

