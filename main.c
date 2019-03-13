// TODO: sprintf mkdir
//       merge write_overlap_window with compare and so forth

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "network.h"

#define T 100e7

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
void write_overlap(FILE *f, Network *network, unsigned long long int t, float min, float max, bool env, bool local) {
   float acc = 0, acc_weight = 0;
   for (int i = 0; i < network->n; i++) {
      for (int j = i + 1; j < network->n; j++) {
         float weight = local ? network->weights[i][j] : 1;
         acc += weight * overlap(network->nodes[i], network->nodes[j], min, max, env);
         acc_weight += weight;
      }
   }
   fprintf(f, "%lld %f\n", t, acc / acc_weight);
}

// Write an overlap window to a file
void write_overlap_window(FILE *f, Network *network, bool env, bool local) {
   const float width = 0.1;
   for (float bottom = 0; bottom <= 2.0; bottom += 0.1) {
      if (bottom + width > 1.0) {
         break;
      }
      float acc = 0, acc_weight = 0;
      for (int i = 0; i < network->n; i++) {
         for (int j = i + 1; j < network->n; j++) {
            float weight = local ? network->weights[i][j] : 1;
            acc += weight * overlap(network->nodes[i], network->nodes[j], bottom, bottom + width, env);
            acc_weight += weight;
         }
      }
      fprintf(f, "%f %f\n", bottom + (width / 2.0), acc / acc_weight);
   }
}

// Write the network weights to a file
void write_weights(FILE *f, Network *network) {
   for (int i = 0; i < network->n; i++) {
      for (int j = 0; j < network->n; j++) {
         fprintf(f, "%d %d %f\n", i, j, network->weights[i][j]);
      }
   }
}

int A = 0, B = 0, C = 0;
void names(Network *network) {
   A = 0, B = 0, C = 0;
   for (int i = 0; i < network->n; i++) {
      Category *cat = left_most(network->nodes[i]->tree);
      while(cat != NULL) {
         bool inc = false;
         if (cat->head == NULL) {
            cat = cat->next;
            continue;
         }
         else if (cat->next == NULL || cat->next->head == NULL) {
            inc = true;
         }
         else {
            inc = peek(cat) != peek(cat->next);
         }
         if (inc) {
            switch (peek(cat) % 3) {
               case 0:
                  A++;
                  break;
               case 1:
                  B++;
                  break;
               case 2:
                  C++;
                  break;
            }
         }
         cat = cat->next;
      }
   }
}

// Window overlap between poppulations
void write_compare(FILE *s, Network *network, Network *save) {
   int N = network->n;
   names(network);
   fprintf(s, "A: %d\n", A);
   const float width = 0.1;
   for (float bottom = 0; bottom <= 2.0; bottom += 0.1) {
      if (bottom + width >= 1.0) {
         break;
      }
      float acc = 0;
      for (int i = 0; i < N; i++) {
         for (int j = 0; j < N / 2; j++) {
            acc += overlap(network->nodes[i], save->nodes[j], bottom, bottom + width, true);
         }
      }
      fprintf(s, "%f %f\n", bottom + (width / 2), 2 * acc / (N * N));
   }
   fprintf(s, "B: %d\n", B);
   for (float bottom = 0; bottom <= 2.0; bottom += 0.1) {
      if (bottom + width >= 1.0) {
         break;
      }
      float acc = 0;
      for (int i = 0; i < N; i++) {
         for (int j = N / 2; j < N; j++) {
            acc += overlap(network->nodes[i], save->nodes[j], bottom, bottom + width, true);
         }
      }
      fprintf(s, "%f %f\n", bottom + (width / 2), 2 * acc/ (N * N));
   }
   fprintf(s, "C: %d\n", C);
}

// Study 2
void contact(char *path, int r) {
   FILE *l = open("data/%s_%d/lingcat.dat", path, r);
   FILE *lo = open("data/%s_%d/local_overlap_env.dat", path, r);
   FILE *o = open("data/%s_%d/overlap_env.dat", path, r);
   FILE *s = open("data/%s_%d/split.dat", path, r);

   Network *network = create_network(100, 0.05);
   make_isolate(network, 0.0, 0.0);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < T; t++) {
      step(network, true, false);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / T * 100.0l);

         write_overlap(o, network, t, 0, 1, true, false);
         write_overlap(lo, network, t, 0, 1, true, true);
      }
   }

   Network *clone = clone_network(network);
   make_complete(network, 1);

   interval = 100.0;
   for (unsigned long long int t = 0; t < T; t++) {
      step(network, false, false);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / T * 100.0l);

         write_overlap(o, network, t, 0, 1, true, false);
         write_overlap(lo, network, t, 0, 1, true, true);
      }
   }

   write_compare(s, network, clone);

   delete_network(clone);
   delete_network(network);

   fclose(s);
   fclose(o);
   fclose(lo);
   fclose(l);
}

// Study 3
void communities(char *path, bool rand) {
   FILE *o = open("data/%s/overlap.dat", path);
   FILE *l = open("data/%s/local_overlap.dat", path);

   Network *network = create_network(100, 0.05);
   make_complete(network, 0.5);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < T; t++) {
      step(network, true, rand);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / T * 100.0l);

         FILE *n = open("data/%s/dump/%d.dat", path, t);
         write_weights(n, network);
         fclose(n);

         write_overlap(o, network, t, 0, 1, false, false);
         write_overlap(l, network, t, 0, 1, false, true);
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
   mkdir("data", 0777);

   mkdir("data/contact_0", 0777);
   contact("contact", 0);

   clock_t end = clock();
   double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
   printf("Done in: %f seconds.", time_spent);
   getchar();
   return 0;
}
