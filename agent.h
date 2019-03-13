#include "category.h"

typedef struct _node {
   Category *tree;
   float h, t;
   int name_mod, time_stamp;
} Agent;

Agent *create_agent(void);
bool negotiate(Agent *spk, Agent *lst, float x, float y, int time, int *split_count);
float overlap(Agent *agent_x, Agent *agent_y, float min, float max, bool env);
Agent *clone_agent(Agent *agent);
void delete_agent(Agent *agent);