#include "category.h"

typedef struct _node {
   Category *tree;
   double h, t;
   int name_mod, time_stamp;
} Agent;

Agent *create_agent(void);
bool negotiate(Agent *spk, Agent *lst, double x, double y, int time, int *split_count);
double overlap(Agent *agent_x, Agent *agent_y, double min, double max, bool env);
Agent *clone_agent(Agent *agent);
void delete_agent(Agent *agent);