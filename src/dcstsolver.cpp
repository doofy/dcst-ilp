#include "dcstsolver.hpp"
#include "edge.hpp"
#include "node.hpp"
#include "graph.hpp"
#include "scip_exception.hpp"
#include "DCUTConstraintHandler.hpp"

#include <vector>
#include <sstream>
#include <iostream>

#include <scip/scipdefplugins.h>

DCSTSolver::DCSTSolver(Graph& graph) {
  try {
    prepare(graph);
  } catch(SCIPException e) {
    std::cout << "prepare threw an exception: " << e.what() << std::endl;
  }
}

SCIP_RETCODE DCSTSolver::prepare(Graph& graph) {
  int counter;

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

  // add own constraint handler
  SCIP_CALL(SCIPincludeObjConshdlr(scip, new DCUTConstraintHandler(scip), TRUE));

  // build variables
  counter = 0;
  std::vector<Edge> &edges = graph.edges;
  for (std::vector<Edge>::iterator edge = edges.begin(); edge != edges.end(); edge++) {
    // create variable
    SCIP_VAR* newVar;
    std::ostringstream varname;
    varname << "edge" << counter << "_" << edge->from << "_" << edge->to;
    // TODO we should probably use SCIPcreateVarBasic here
    SCIP_CALL_EXC(SCIPcreateVarBasic(scip, // problem instance
          &newVar, // reference
          varname.str().c_str(), // name
          0.0, 1.0, // upper and lower bounds
          edge->cost, // objective function value
          SCIP_VARTYPE_INTEGER
          //SCIP_VARTYPE_CONTINUOUS
          ));

    edge->var = newVar;

    // add variable
    SCIP_CALL_EXC(SCIPaddVar(scip, newVar));
    counter++;
  }

  // add pricers
  // TODO do we need pricing?
  //SCIP_CALL_EXC(SCIPactivatePricer(scip, ...));

  // build constraints
  counter = 0;
  std::vector<Node> &nodes = graph.nodes;
  for (std::vector<Node>::iterator node = nodes.begin(); node != nodes.end(); node++) {
    // TODO lets hope we iterate in the same order as indices are assigned to the nodes
    SCIP_CONS *newCon;
    std::ostringstream conname;
    conname << "degree_con" << counter;
    SCIP_CALL_EXC(SCIPcreateConsLinear(scip, // problem instance
          &newCon, // created constraint
          conname.str().c_str(), // constraint name
          0, // number of nonzeros in the constraint
          NULL, // array with variables of constraint entries
          NULL, // array with coefficients of constraint entries
          1.0, // left hand side of constraint : at least one edge must connect this node to the tree
          node->degree, // right hand side of constraint : respect the degree constraint of this node
          TRUE, // should the LP relaxation of constraint be in the initial LP?
          TRUE, // should the constraint be separated during LP processing?
          TRUE, // should the constraint be enforced during node processing?
          TRUE, // should the constraint be checked for feasibility? TRUE for model constraints, FALSE for additional, redundant constraints.
          TRUE, // should the constraint be propagated during node processing?
          FALSE, // is constraint only valid locally? TODO branching
          FALSE, // is constraint modifiable (subject to column generation)?
          FALSE, // is constraint subject to aging? TODO own cuts
          FALSE, // should the relaxation be removed from the LP due to aging or cleanup? TODO user cuts
          FALSE //  should the constraint always be kept at the node where it was added, even if it may be moved to a more global node? TODO constraint that represents node data
          ));

    for (std::vector<int>::iterator edge = node->edges.begin(); edge != node->edges.end(); edge++) {
      // TODO lets hope we iterate in the same order as indices are assigned to the edges
      SCIP_CALL(SCIPaddCoefLinear(scip, newCon, edges[*edge].var, edges[*edge].cost));
    }

    // add constraints
    SCIP_CALL_EXC(SCIPaddCons(scip, newCon));
    counter++;
  }

  return SCIP_OKAY;
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
  std::cerr << "solution found" << std::endl;
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
