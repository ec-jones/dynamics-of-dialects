#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "category.h"

typedef struct _node {
   Category *tree;
   struct _node **neighbours;
   int degree, capacity, time_stamp;

   float h, t; //High pdf and Transition
   int name_mod;
} Agent;

// Create a new (isolated) agent with uniform distribution
Agent *create_agent(void) {
   Agent *new = malloc(sizeof(Agent));
   assert(new != NULL);

   Agent **neighbours = malloc(4 * sizeof(Agent*));
   assert(neighbours != NULL);

   *new = (Agent){create_category(), neighbours, 0, 4, 0, 0.0, 0.0, -1};
   return new;
}

// Count the number of splits in the population
int split_count = 0;

// Split agent if x and y overlap
// if so add new name (-1) to x's new agent
void split(Agent *agent, float x, float y) {
   Category *cat = agent->tree;
   while (cat != NULL) {
      if (cat->split == -1) {
         inner_split(cat, x, y);
         push(cat->left, agent->name_mod, agent->time_stamp);
         push(cat->right, agent->name_mod, agent->time_stamp);
         split_count++;
         return;
      }
      else if (x < cat->split && y < cat->split) {
         cat = cat->left;
      }
      else if (x >= cat->split && y >= cat->split) {
         cat = cat->right;
      }
      else {
         return;
      }
   }
}

// Main negationation loop
void negotiate(Agent *spk, Agent *lst, float x, float y, int time) {
   split(spk, x, y);

   Category *spkx = get_category(spk->tree, x),
            *lstx = get_category(lst->tree, x),
            *lsty = get_category(lst->tree, y);


   spkx->head->time_stamp = spk->time_stamp;
   int mrn = peek(spkx);

   bool succ;
   if (time == -1) {
      succ = contain_name(lstx->head, mrn, -1)
         && (!contain_name(lsty->head, mrn, -1));
   }
   else { // Forgetting
      succ = contain_name(lstx->head, mrn, lst->time_stamp - time)
         && (!contain_name(lsty->head, mrn, lst->time_stamp - time));
   }

   if (succ) { // Success
      delete_name(spkx->head->next);
      spkx->head->next = NULL;
      spkx->tail = spkx->head;

      delete_name(lstx->head);
      lstx->head = NULL;
      lstx->tail = NULL;
      push(lstx, mrn, lst->time_stamp);
   }
   else { // Failure
      enqueue(lstx, mrn, lst->time_stamp);
   }

   spk->time_stamp++;
   lst->time_stamp++;
}

   // Clone agent's tree, h, t & name_mod
Agent *clone_agent(Agent *agent) {
   Agent *new = create_agent();
   new->tree = clone_tree(agent->tree);
   new->h = agent->h;
   new->t = agent->t;
   new->name_mod = agent->name_mod;
   return new;
}

// Deallocate agent
void delete_agent(Agent *agent) {
   if (agent != NULL) {
      delete_category(agent->tree);
      agent->tree = NULL;

      free(agent->neighbours);
      agent->neighbours = NULL;

      free(agent);
      agent = NULL;
   }
}