#include <cstdlib>
#include <iostream>

#include "dcstsolver.hpp"
#include "scip_exception.hpp"

int main (int argc, char** argv) {
  try {
    DCSTSolver solver;
    solver.solve();
    //solver.disp();
  } catch(const SCIPException& exc) {
    std::cerr << exc.what() << std::endl;
    exit((int) exc.getRetcode());
  }
  return EXIT_SUCCESS;
}

