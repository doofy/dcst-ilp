#include "dcstloader.hpp"
#include "scip_exception.hpp"

#include "graph.hpp"

#include <iostream>
#include <fstream>
#include <string>

DCSTLoader::DCSTLoader(std::string filename, Graph& graph) {
  std::ifstream file(filename.c_str());
  std::string name;
  int nodecount;
  int edgecount;
  if (file.good()) {
    file >> name;
    file >> nodecount;
    file >> edgecount;
    graph.name = name;
    for (int i = 0; i < nodecount; i++) {
      int degree;
      file >> degree;
      Node newnode(degree);
      graph.nodes.push_back(newnode);
    }
    for (int i = 0; i < edgecount; i++) {
      int from;
      int to;
      int cost;
      int partOfSolution;
      file >> from;
      file >> to;
      file >> cost;
      file >> partOfSolution;
      graph.nodes[from].edges.push_back(i);
      graph.nodes[to].edges.push_back(i);
      Edge newedge(from, to, cost, partOfSolution);
      graph.edges.push_back(newedge);
    }
    std::cerr << "loading instance " << name << " successful" << std::endl;
  }
  else {
    std::cerr << "file not good" << std::endl;
  }
  file.close();
}

DCSTLoader::~DCSTLoader() {
}
/*
SCIP_RETCODE DCSTLoader::addVarToEdges(SCIP* scip, Edge& edge, SCIP_VAR* var) {
  assert(scip != NULL);
  assert(edge != NULL);
  assert(var != NULL);

  // add variable to forward edge and capture it for usage in graph
  edge.var = var;
  SCIP_CALL_EXC(SCIPcaptureVar(scip, edge.var));

  return SCIP_OKAY;
}
*/
