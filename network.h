#include "agent.h"

typedef struct {
   int n;
   Agent **nodes;
   float **weights;
   float dmin;
   int split_count;
} Network;

Network *create_network(int n, float dmin);
void make_complete(Network *network, float p);
void make_linear(Network *network, float h, float t);
void make_isolate(Network *network, float h, float t);
void reconnect(Network *network, float h, float t);
void watts_strogatz(Network *network, float h, float t, int K, float beta);
void step(Network *network, bool update_weight, bool rand);
Network *clone_network(Network *network);
void delete_network(Network *network);