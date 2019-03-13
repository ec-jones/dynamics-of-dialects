#include "agent.h"

typedef struct {
   int n;
   Agent **nodes;
   float **weights;
} Network;

float frand(void);

Network *create_network(int n);
void make_complete(Network *network, float p);
void make_linear(Network *network, float h, float t);
void make_isolate(Network *network, float h, float t);
void reconnect(Network *network, float h, float t);
void watts_strogatz(Network *network, float h, float t, int K, float beta);
Network *clone_network(Network *network);
void delete_network(Network *network);