#include "agent.h"

typedef struct {
   int n;
   Agent **adj_list;
} Network;

float frand(void);

Network *create_network(int n);
void make_complete(Network *network);
void make_linear(Network *network, float h, float t);
void make_isolate(Network *network, float h, float t);
void reconnect(Network *network, float h, float t);
void watts_strogatz(Network *network, float h, float t, int K, float beta);
float match_node(Agent *node_x, Agent *node_y, float min, float max, bool env);
Network *clone_network(Network *network);
void delete_network(Network *network);