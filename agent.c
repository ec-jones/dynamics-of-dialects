#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "category.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct _node {
   Category *tree;
   float h, t;
   int name_mod, time_stamp;
} Agent;

// Create a new agent with uniform distribution
Agent *create_agent(void) {
   Agent *new = malloc(sizeof(Agent));
   assert(new != NULL);

   *new = (Agent){create_category(), 0.0, 0.0, -1, 0};
   return new;
}

// Split agent if x and y overlap, if so add new name to x's category
bool split(Agent *agent, float x, float y) {
   Category *cat = agent->tree;
   while (cat != NULL) {
      if (cat->split == -1) {
         inner_split(cat, x, y);
         push(cat->left, agent->name_mod, agent->time_stamp);
         push(cat->right, agent->name_mod, agent->time_stamp);
         return true;
      }
      else if (x < cat->split && y < cat->split) {
         cat = cat->left;
      }
      else if (x >= cat->split && y >= cat->split) {
         cat = cat->right;
      }
      else {
         break;
      }
   }
   return false;
}

// Main negationation loop, time is forgetting distance
bool negotiate(Agent *spk, Agent *lst, float x, float y, int time, int *split_count) {
   if (split(spk, x, y)) {
      ++*split_count;
   }

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

   return succ;
}

// Helper function for match_agent
float scale(float top, float bottom, float h, float t) {
   if (h*t == 1 || h*t == 0) {
      return top - bottom;
   }
   if (bottom > t) {
      return ((1.0 - (h*t) / 1.0 - t)) * (top - bottom);
   }
   else if (top > t) {
      float left  = h * (t - bottom),
            right = ((1.0 - (h*t) / 1.0 - t)) * (top - t);
      return left + right;
   }
   else {
      return h * (top - bottom);
   }
}

// Mesure how much of the perceptual space (within a window) the agents agree op
// If env weight the result by the average distribution
float overlap(Agent *agent_x, Agent *agent_y, float min, float max, bool env) {
   Category *x = left_most(agent_x->tree),
            *y = left_most(agent_y->tree);

   float acc = 0, last_top = 0;
   while (x != NULL && y != NULL && last_top < max) {
      Category **smaller = x->top < y->top ? &x : &y;

      float high = MIN(max, (*smaller)->top), low = MAX(min, last_top);
      if (low <= high) {
         if (x->head != NULL && y->head != NULL && peek(x) == peek(y)) {
            if (env) {
               float acc_x = scale(high, low, agent_x->h, agent_x->t),
                     acc_y = scale(high, low, agent_y->h, agent_y->t);
               acc += (acc_x + acc_y) / 2;
            }
            else {
               acc += high - low;
            }
         }
      }
      last_top = high;
      *smaller = (*smaller)->next;
   }
   float width_x = scale(max, min, agent_x->h, agent_x->t),
         width_y = scale(max, min, agent_y->h, agent_y->t),
         width = env ? (width_x + width_y) / 2 : (max - min);
   return acc / width;
}

// Clone agent with only its leaves
Agent *clone_agent(Agent *agent) {
   Agent *new = create_agent();
   *new = (Agent){clone_leaves(agent->tree), agent->h, agent->t, agent->name_mod, agent->time_stamp};
   return new;
}

// Deallocate agent
void delete_agent(Agent *agent) {
   if (agent != NULL) {
      delete_category(agent->tree);
      agent->tree = NULL;

      free(agent);
      agent = NULL;
   }
}