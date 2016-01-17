#include "dcstsolver.hpp"
#include "edge.hpp"
#include "node.hpp"
#include "graph.hpp"
#include "scip_exception.hpp"

#include <vector>
#include <sstream>
#include <iostream>

#include <scip/scipdefplugins.h>

DCSTSolver::DCSTSolver(Graph& graph) {
  // initialize scip
  SCIP_CALL_EXC(SCIPcreate(&scip));

  // set time limit
  SCIP_CALL_EXC(SCIPsetRealParam(scip, "limits/time", 300));

  // load default plugins linke separators, heuristics, etc.
  SCIP_CALL_EXC(SCIPincludeDefaultPlugins(scip));

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
    SCIP_CALL_EXC(SCIPcreateVarBasic(scip, // problem instance
          &var, // reference
          varname.str().c_str(), // name
          0.0, 1.0, // upper and lower bounds
          edge->cost, // TODO ?objective function value TODO?
          SCIP_VARTYPE_INTEGER)); // SCIP_VARTYPE_INTEGER vs SCIP_VARTYPE_CONTINUOUS

    edge->var = var;

    // add variable
    SCIP_CALL_EXC(SCIPaddVar(scip, var));
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

void DCSTSolver::display(Graph& graph) {
  // get the best found solution from scip
  SCIP_SOL *solution = SCIPgetBestSol(scip);

  // when SCIP did not succeed then sol is NULL
  if (solution == NULL) {
    std::cerr << "no solution found" << std::endl;
    return;
  }
/*
  std::vector<Edge> &edges = graph.edges;
  for (std::vector<Edge>::iterator edge = edges.begin(); edge != edges.end(); edge++) {
    if (SCIPgetSolVal(scip, solution, edge->var) > 0.5 ) {
      edge->partOfSolution = 1;
    }
    else {
      edge->partOfSolution = 0;
    }
  }
*/
  std::vector<Edge> &edges = graph.edges;
  for (std::vector<Edge>::iterator edge = edges.begin(); edge != edges.end(); edge++) {
    edge->partOfSolution = SCIPgetSolVal(scip, solution, edge->var);
  }
}
