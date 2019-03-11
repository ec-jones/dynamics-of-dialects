#include "agent.h"

typedef struct {
   Agent **adj_list;
   int n;
} Network;

float frand(void);

Network *create_network(int n);
bool contains_edge(Network *network, int i, int j);
void edge_insert(Network *network, int i, int j);
void make_complete(Network *network);
void make_linear(Network *network, float h, float t);
void make_isolate(Network *network, float h, float t);
void reconnect(Network *network);
bool contains_edge(Network *network, int i, int j);
void watts_strogatz(Network *network, float h, float t, int K, float beta);
float match_node(Agent *x, Agent *y, float min, float max, bool env);
void delete_network(Network *network);
Network *clone_network(Network *network);
