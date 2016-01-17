#ifndef EDGE_H
#define EDGE_H

class Edge {
  private:

  public:
    Edge(int, int, int, int);
    ~Edge();
    int from;
    int to;
    int cost;
    int partOfSolution;
};
#endif
