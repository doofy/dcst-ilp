#ifndef DCSTLOADER_H
#define DCSTLOADER_H

#include "graph.hpp"

#include <string>

#include <scip/scip.h>

class DCSTLoader {
  private:
    SCIP *scip;
    SCIP_CONS *cons;

  public:
    DCSTLoader(std::string, Graph&);
    ~DCSTLoader();
};
#endif
