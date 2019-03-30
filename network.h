#include "agent.h"

typedef struct {
   int n;
   Agent **nodes;
   double **weights;
   double dmin;
   int split_count;
} Network;

Network *create_network(int n, double dmin);
double drand(void);
void make_complete(Network *network, double p);
void make_linear(Network *network, double h, double t);
void make_isolate(Network *network, double h, double t);
void reconnect(Network *network, double h, double t);
void watts_strogatz(Network *network, double h, double t, int K, double beta);
void step(Network *network, int F, bool update_weight, bool rand, double l1, double l2);
Network *clone_network(Network *network);
void delete_network(Network *network);