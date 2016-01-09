#include "dcstloader.hpp"
#include "scip_exception.hpp"

DCSTLoader::DCSTLoader() {
}

DCSTLoader::~DCSTLoader() {
}

SCIP_RETCODE DCSTLoader::addVarToEdges(SCIP* scip, Edge& edge, SCIP_VAR* var) {
  assert(scip != NULL);
  assert(edge != NULL);
  assert(var != NULL);

  /* add variable to forward edge and capture it for usage in graph */
  edge.var = var;
  SCIP_CALL_EXC(SCIPcaptureVar(scip, edge.var));

  return SCIP_OKAY;
}
