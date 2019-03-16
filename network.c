#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "agent.h"

// Undirected network of agents
typedef struct {
   int n;
   Agent **nodes;
   float **weights;
   float dmin;
   int split_count;
} Network;

// Create a network of n isolated nodes
Network *create_network(int n, float dmin) {
   Network *network = malloc(sizeof(Network));
   assert(network != NULL);

   Agent **nodes = malloc(n * sizeof(Agent*));
   assert(nodes != NULL);
   for (int i = 0; i < n; i++) {
      nodes[i] = create_agent();
   }

   float **weights = malloc(n * sizeof(float*));
   assert(weights != NULL);

   for (int i = 0; i < n; i++) {
      weights[i] = malloc(n * sizeof(float));
      assert(weights[i] != NULL);

      for (int j = 0; j < n; j++) {
         weights[i][j] = 0;
      }
   }

   *network = (Network){n, nodes, weights, dmin, 0};
   return network;
}

// Generate float in range [0, 1)
float frand(void) {
   float f = ((float)rand()) / (((float)(RAND_MAX)) + 1.0);
   return f >= 1.0 ? 0.0 : f;
}

// Check if network already contains edge
bool contains_edge(Network *network, int i, int j) {
   return network->weights[i][j] != 0.0;
}

// Insert undirect edge with weight p
void set_weight(Network *network, int i, int j, float w) {
   assert(i != j); // no self loops
   network->weights[i][j] = w;
   network->weights[j][i] = w;
}

// Assign environment to agent i, mod if different name categories
void assign_environment(Network *network, int i, float h, float t, bool mod) {
   if (i < network->n / 2) {
      network->nodes[i]->name_mod = mod ?  -2 : -1;
      network->nodes[i]->h = h;
      network->nodes[i]->t = t;
   }
   else {
      network->nodes[i]->name_mod = mod ?  -3 : -1;
      network->nodes[i]->h = (1.0 - h*t) / (1.0 - t);
      network->nodes[i]->t = 1.0 - t;
   }

   if (h == 0.0 || h*t == 1.0) {
      network->nodes[i]->h = 1.0;
      network->nodes[i]->t = 1.0;
   }
}

// Connect all nodes in a new network
void make_complete(Network *network, float p) {
   for (int i = 0; i < network->n; i++) {
      for (int j = i + 1; j < network->n; j++) {
         set_weight(network, i, j, p);
      }
   }
}

// Make new network linearly connected
void make_linear(Network *network, float h, float t) {
   for (int i = 0; i < network->n; i++) {
      assign_environment(network, i, h, t, false);

      if (i < network->n - 1) {
         set_weight(network, i, i + 1, 1.0);
      }
   }
}

// Make two isolated populations (name_mod = -2, -3)
void make_isolate(Network *network, float h, float t) {
   for (int i = 0; i < network->n; i++) {
      assign_environment(network, i, h, t, true);

      for (int j = 0; j < network->n; j++) {
         if ((i <  network->n / 2  && j >= network->n / 2) ||
             (i >= network->n / 2  && j <  network->n / 2) ||
             (i == j)) {
            continue;
         }
         set_weight(network, i, j, 1.0);
      }
   }
}

// Connect isolated populations (name_mod = -4)
void reconnect(Network *network, float h, float t) {
   for (int i = 0; i < network->n; i++) {
      network->nodes[i]->h = h;
      network->nodes[i]->t = t;
      network->nodes[i]->name_mod = -4;
      for (int j = i + 1; j < network->n; j++) {
         set_weight(network, i, j, 1.0);
      }
   }
}

// Make new network into a small-world network using the Watts–Strogatz model
void watts_strogatz(Network *network, float h, float t, int K, float beta) {
   for (int i = 0; i < network->n; i++) {
      assign_environment(network, i, h, t, false);

      for (int k = 0; k < K / 2; k++) {
         int j = (i + 1 + k) % network->n;
         if (frand() <= beta) {
            j = frand() * network->n;
            while (j == i || contains_edge(network, i, j)) {
               j = (j + 1) % network->n;
            }
         }
         set_weight(network, i, j, 1.0);
      }
   }
}

// Sample from (h, t)-dist with inverse cummulative frequency transform
float resample(float x, float h, float t) {
   if (h*t != 0.0 && h*t != 1.0) {
      if (x <= h * t) {
         x /= h;
      } else {
         x -= h * t;
         x *= (1.0 - t) / (1.0 - (h*t));
         x += t;
      }
   }
   return x;
}

// Sigmoid like function
float smootherstep(float x) {
   const float eps = 0.000001;

   x = x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
   if (x <= eps) {
      return eps;
   }
   else if (x >= 1 - eps) {
      return 1 - eps;
   }
   else {
      return x;
   }
}

// Make one communication and optionally update weights, if rand the outcome of the communication is random
void step(Network *network, bool update_weight, bool rand, float l1, float l2) {
   int N = network->n;

   int n = frand() * N; 
   Agent *spk = network->nodes[n];

   float cum[N]; // cummulative weights
   cum[0] = network->weights[n][0];
   for (int i = 1; i < N; i++) {
      cum[i] = cum[i - 1] + network->weights[n][i];
   }

   float p = frand() * cum[N - 1];

   int m = 0;
   while (m < N && p > cum[m]) {
      m++;
   }
   if (network->weights[n][m] == 0.0) {
      printf("n:%d m:%d\n", n, m);
      printf("cum[n, m-1]:%f\n", cum[m - 1]);
      printf("cum[n, m+1]:%f\n", cum[m + 1]);
      printf("p:%f\n", p);
   }
   Agent *lst = network->nodes[m];

   // generate stimuli
   float a = frand(), b = frand();
   while (fabs(a - b) <= network->dmin) {
      b = frand();
   }
   a = resample(a, spk->h, spk->t);
   b = resample(b, spk->h, spk->t);

   bool succ = rand ? frand() > 0.5 : negotiate(spk, lst, a, b, -1, &network->split_count);
   if (update_weight) {
      network->weights[n][m] = smootherstep(network->weights[n][m] + (succ ? l1 : -l2));
      network->weights[m][n] = smootherstep(network->weights[m][n] + (succ ? l1 : -l2));
   }
}

// Clones a network with only agent's leaves
// Used to compare to past versions
Network *clone_network(Network *network) {
   int n = network->n;

   Agent **nodes = malloc(n * sizeof(Agent*));
   assert(nodes != NULL);
   for (int i = 0; i < n; i++) {
      nodes[i] = clone_agent(network->nodes[i]);
   }

   float **weights = malloc(n * sizeof(float*));
   assert(weights != NULL);
   for (int i = 0; i < n; i++) {
      weights[i] = malloc(n * sizeof(float));
      assert(weights[i] != NULL);
      for (int j = 0; j < n; j++) {
         weights[i][j] = network->weights[i][j];
      }
   }
   
   Network *new = malloc(sizeof(Network));
   assert(new != NULL);

   *new = (Network){n, nodes, weights, network->dmin, 0};
   return new;
}

// Deallocate network safely
void delete_network(Network *network) {
   if (network != NULL) {
      for (int i = 0; i < network->n; i++) {
         delete_agent(network->nodes[i]);
         network->nodes[i] = NULL;

         free(network->weights[i]);
         network->weights[i] = NULL;
      }
      free(network->weights);
      network->weights = NULL;
      free(network);
   }
}