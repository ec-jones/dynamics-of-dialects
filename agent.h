#include "category.h"

typedef struct _node {
   Category *tree;
   struct _node **neighbours;
	int degree, capacity, time_stamp;

   float h, t; //High pdf and Transition
   int name_mod;
} Agent;

Agent *create_agent(void);
extern int split_count;
void split(Agent *agent, float x, float y);
void negotiate(Agent *spk, Agent *lst, float x, float y, int time);
Agent *clone_agent(Agent *agent);
void delete_agent(Agent *agent);