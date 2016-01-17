#include <cstdlib>
#include <iostream>
#include <string>

#include "dcstsolver.hpp"
#include "dcstloader.hpp"
#include "scip_exception.hpp"

int main (int argc, char** argv) {
  std::string path;
  if (argc > 1) {
    path = argv[1];
  }
  else {
    std::cerr << "No input file give" << std::endl;
    return EXIT_FAILURE;
  }
  Graph graph;
  try {
    DCSTLoader loader(path, graph);
    std::cout << graph.toString() << std::endl;
    //DCSTSolver solver(graph);
    //solver.solve();
    //solver.disp();
  } catch(const SCIPException& exc) {
    std::cerr << exc.what() << std::endl;
    exit((int) exc.getRetcode());
  }
  return EXIT_SUCCESS;
}

