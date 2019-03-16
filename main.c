#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include "network.h"

// Create a formatted director
void dir(char *format, ...) {
   char buf[100];

   va_list argptr;
   va_start(argptr, format);
   vsnprintf(buf, sizeof(buf), format, argptr);
   va_end(argptr);

   mkdir(buf, 0777);
}

// Open a formatted file path
FILE *open(char *format, ...) {
   char buf[100];

   va_list argptr;
   va_start(argptr, format);
   vsnprintf(buf, sizeof(buf), format, argptr);
   va_end(argptr);

   FILE *f = fopen(buf, "w");
   assert(f != NULL);
   return f;
}

// Write the number of perceptual categories to a file
void write_percat(FILE *f, Network *network, unsigned long long int t) {
   fprintf(f, "%lld %f\n", t, (((float)network->split_count) + ((float)network->n)) / ((float)network->n));
}

// Write the number of linguistic categories to a file
void write_lingcat(FILE *f, Network *network, unsigned long long int t) {
   float acc = 0;
   for (int i = 0; i < network->n; i++) {
      acc += lingcat_categories(network->nodes[i]->tree);
   }
   fprintf(f, "%lld %f\n", t, acc / network->n);
}

// Write the overlap to a file
void write_overlap(FILE *f, Network *network, unsigned long long int t, bool env, bool local) {
   float acc = 0, acc_weight = 0;
   for (int i = 0; i < network->n; i++) {
      for (int j = i + 1; j < network->n; j++) {
         float weight = local ? network->weights[i][j] : 1;
         acc += weight * overlap(network->nodes[i], network->nodes[j], 0, 1, env);
         acc_weight += weight;
      }
   }
   fprintf(f, "%lld %f\n", t, acc / acc_weight);
}

// Write the network weights to a file
void write_weights(FILE *f, Network *network) {
   for (int i = 0; i < network->n; i++) {
      for (int j = 0; j < network->n; j++) {
         fprintf(f, "%d %d %f\n", i, j, network->weights[i][j]);
      }
   }
}

// Extract the number of names mod 3
void names(Network *network, int *A, int *B, int *C) {
   *A = 0, *B = 0, *C = 0;
   for (int i = 0; i < network->n; i++) {
      Category *cat = left_most(network->nodes[i]->tree);

      while(cat != NULL) {

         if (cat->head != NULL &&
               (cat->next == NULL ||
               (cat->next->head != NULL && peek(cat) != peek(cat->next)))) {
            
            switch (peek(cat) % 3) {
               case 0:
                  ++*A;
                  break;
               case 1:
                  ++*B;
                  break;
               case 2:
                  ++*C;
                  break;
            }
         }

         cat = cat->next;
      }
   }
}

// Compare a saved population to the current network
void write_compare(FILE *s, Network *network, Network *save) {
   int A, B, C;
   names(network, &A, &B, &C);

   fprintf(s, "A: %d\n", A);
   for (int i = 0; i < 10; i++) {
      float min = i * 0.1, max = min + 0.1;

      float acc = 0, acc_weight = 0;
      for (int i = 0; i < network->n; i++) {
         for (int j = 0; j < save->n / 2; j++) {
            acc_weight++;
            acc += overlap(network->nodes[i], save->nodes[j], min, max, true);
         }
      }
      fprintf(s, "%f %f", (min + max) / 2.0, acc / acc_weight);
   }

   fprintf(s, "B: %d\n", B);
   for (int i = 0; i < 10; i++) {
      float min = i * 0.1, max = min + 0.1;

      float acc = 0, acc_weight = 0;
      for (int i = 0; i < network->n; i++) {
         for (int j = save->n / 2; j < save->n; j++) {
            acc_weight++;
            acc += overlap(network->nodes[i], save->nodes[j], min, max, true);
         }
      }
      fprintf(s, "%f %f", (min + max) / 2.0, acc / acc_weight);
   }

   fprintf(s, "C: %d\n", C);
}

void contact(char *path, int r) {
   dir("data/%s_%d", path, r);
   FILE *l = open("data/%s_%d/lingcat.dat", path, r);
   FILE *lo = open("data/%s_%d/local_overlap_env.dat", path, r);
   FILE *o = open("data/%s_%d/overlap_env.dat", path, r);
   FILE *s = open("data/%s_%d/split.dat", path, r);

   Network *network = create_network(100, 0.05);
   make_isolate(network, 0.0, 0.0);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, false, false, 0.0, 0.0);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / 100e7 * 100.0l);

         write_lingcat(l, network, t);
         write_overlap(o, network, t, true, false);
         write_overlap(lo, network, t, true, true);
      }
   }

   Network *save = clone_network(network);
   make_complete(network, 1);

   interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, false, false, 0.0, 0.0);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / 100e7 * 100.0l);

         write_lingcat(l, network, t);
         write_overlap(o, network, t, true, false);
         write_overlap(lo, network, t, true, true);
      }
   }

   write_compare(s, network, save);

   delete_network(save);
   delete_network(network);

   fclose(s);
   fclose(o);
   fclose(lo);
   fclose(l);
}
//////////////////////////////////////////////////////////////////////////////////////////////

// Study 3
void communities(char *path, bool rand, float l1, float l2) {
   dir("data/%s_%.3f_%.3f", path, l1, l2);
   dir("data/%s_%.3f_%.3f/dump", path, l1, l2);
   FILE *o = open("data/%s_%.3f_%.3f/overlap.dat", path, l1, l2);
   FILE *l = open("data/%s_%.3f_%.3f/local_overlap.dat", path, l1, l2);

   Network *network = create_network(100, 0.05);
   make_complete(network, 0.5);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, true, rand, l1, l2);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / 100e7 * 100.0l);

         FILE *n = open("data/%s_%.3f_%.3f/dump/%d.dat", path, l1, l2, t);
         write_weights(n, network);
         fclose(n);

         write_overlap(o, network, t, false, false);
         write_overlap(l, network, t, false, true);
      }
   }

   delete_network(network);

   fclose(o);
   fclose(l);
}

// Entry point
int main(int argc, char **argv) {
   time_t t;
   srand((unsigned) time(&t));
   clock_t begin = clock();
   dir("data");

   // study3 grid
   float l1 = 0.0;
   for (int i = 0; i < 4; i++) {
      l1 += 0.15;
      float l2 = 0.0;
      for (int j = 0; j < 4; j++) {
         l2 += 0.15;
         communities("communities", false, l1, l2);
         communities("communities_rand", true, l1, l2);
      }
   }

   clock_t end = clock();
   double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
   printf("Done in: %f seconds.", time_spent);
   getchar();
   return 0;
}
