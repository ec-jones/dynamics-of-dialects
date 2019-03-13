#include <stdlib.h>
#include <assert.h>
#include "agent.h"

// Undirected network of agents
typedef struct {
   int n;
   Agent **nodes;
   float **weights;
} Network;

// Generate float in range [0, 1)
float frand(void) {
   float f = ((float)rand()) / (((float)(RAND_MAX)) + 1.0);
   return f >= 1.0 ? 0.0 : f;
}

// Create a network of n isolated nodes
Network *create_network(int n) {
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

   *network = (Network){n, nodes, weights};
   return network;
}

// Check if network already contains edge
bool contains_edge(Network *network, int i, int j) {
   return network->weights[i][j] != 0;
}

// Insert undirect edge with weight p
void edge_insert(Network *network, int i, int j, float p) {
   assert(i != j); // No self loops
   network->weights[i][j] = p;
   network->weights[j][i] = p;
}

// Connect all nodes in a new network
void make_complete(Network *network, float p) {
   for (int i = 0; i < network->n; i++) {
      for (int j = i + 1; j < network->n; j++) {
         edge_insert(network, i, j, p);
      }
   }
}

// Assign environment to agent i, mod if different name categories
void assign_environment(Network *network, int i, float h, float t, bool mod) {
   if (h == 0 || h*t == 1) {
      network->nodes[i]->h = 1;
      network->nodes[i]->t = 1;
   }
   else {
      if (i < network->n / 2) {
         network->nodes[i]->h = h;
         network->nodes[i]->t = t;
      }
      else {
         network->nodes[i]->h = (1.0f - h*t) / (1.0f - t);
         network->nodes[i]->t = 1.0f - t;
      }
   }
   if (mod) {
      if (i < network->n / 2) {
         network->nodes[i]->name_mod = -2;
      }
      else {
         network->nodes[i]->name_mod = -3;
      }
   }
}

// Make new network linearly connected
void make_linear(Network *network, float h, float t) {
   for (int i = 0; i < network->n; i++) {
      assign_environment(network, i, h, t, false);
      if (i < network->n - 1) {
         edge_insert(network, i, i + 1, 1.0);
      }
   }
}

// Make two isolated populations (naemcat = -2, -3)
void make_isolate(Network *network, float h, float t) {
   for (int i = 0; i < network->n / 2; i++) {
      assign_environment(network, i, h, t, true);
      for (int j = 0; j < network->n / 2; j++) {
         if (i == j) {
            continue;
         }
         edge_insert(network, i, j, 1.0);
      }
   }
   for (int i = network->n / 2; i < network->n; i++) {
      assign_environment(network, i, h, t, true);
      for (int j = network->n / 2; j < network->n; j++) {
         if (i == j) {
            continue;
         }
         edge_insert(network, i, j, 1.0);
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
         edge_insert(network, i, j, 1.0);
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
         edge_insert(network, i, j, 1.0);
      }
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

   *new = (Network){n, nodes, weights};
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