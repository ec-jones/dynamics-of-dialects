#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>
#include "network.h"

#define study0
#define study1
#define study2
#define study3

#define JOIN(N) {for (int __i = 0; __i < (N); __i++){pthread_join(threads[__i], NULL);}}

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
   fprintf(f, "%lld %f\n", t, (((double)network->split_count) + ((double)network->n)) / ((double)network->n));
}

// Write the number of linguistic categories to a file
void write_lingcat(FILE *f, Network *network, unsigned long long int t) {
   double acc = 0;
   for (int i = 0; i < network->n; i++) {
      acc += lingcat_categories(network->nodes[i]->tree);
   }
   fprintf(f, "%lld %f\n", t, acc / network->n);
}

// Write the overlap to a file
void write_overlap(FILE *f, Network *network, unsigned long long int t, bool env, bool local) {
   double acc = 0, acc_weight = 0;
   for (int i = 0; i < network->n; i++) {
      for (int j = i + 1; j < network->n; j++) {
         double weight = local ? network->weights[i][j] : 1;
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
void names(Network *network, double *A, double *B, double *C) {
   *A = 0.0, *B = 0.0, *C = 0.0;
   for (int i = 0; i < network->n; i++) {
      Category *cat = left_most(network->nodes[i]->tree);
      double bottom = 0.0;
      while(cat != NULL) {
         if (cat->head != NULL) {
            int name = peek(cat);
            switch (name % 3) {
               case 0:
                  *A += cat->top - bottom;
                  break;
               case 1:
                  *B += cat->top - bottom;
                  break;
               case 2:
                  *C += cat->top - bottom;
                  break;
            }
         }
         bottom = cat->top;
         cat = cat->next;
      }
   }
   *A /= network->n;
   *B /= network->n;
   *C /= network->n;
}

// Compare a saved population to the current network
void write_compare(FILE *s, Network *network, Network *save) {
   double A, B, C;
   names(network, &A, &B, &C);

   fprintf(s, "A: %f\n", A);
   for (int i = 0; i < 20; i++) {
      double min = i * 0.05, max = min + 0.05;

      double acc = 0, acc_weight = 0;
      for (int i = 0; i < network->n; i++) {
         for (int j = 0; j < save->n / 2; j++) {
            acc_weight++;
            acc += overlap(network->nodes[i], save->nodes[j], min, max, true);
         }
      }
      fprintf(s, "%f %f\n", (min + max) / 2.0, acc / acc_weight);
   }

   fprintf(s, "B: %f\n", B);
   for (int i = 0; i < 20; i++) {
      double min = i * 0.05, max = min + 0.05;

      double acc = 0, acc_weight = 0;
      for (int i = 0; i < network->n; i++) {
         for (int j = save->n / 2; j < save->n; j++) {
            acc_weight++;
            acc += overlap(network->nodes[i], save->nodes[j], min, max, true);
         }
      }
      fprintf(s, "%f %f\n", (min + max) / 2.0, acc / acc_weight);
   }

   fprintf(s, "C: %f\n", C);
}

// The core model with default parameters
void *core_model(void *arg) {
   int i = *(int*)arg;
   dir("study0/run_%d", i);
   FILE *p = open("study0_%d/percat.dat", i);
   FILE *l = open("study0_%d/lingcat.dat", i);
   FILE *o = open("study0_%d/overlap.dat", i);

   Network *network = create_network(100, 0.01);
   make_complete(network, 1);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, -1, false, false, 0.0, 0.0);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / 100e7 * 100.0l);

         write_percat(p, network, t);
         write_lingcat(l, network, t);
         write_overlap(o, network, t, true, false);
      }
   }

   delete_network(network);

   fclose(o);
   fclose(l);
   fclose(p);
   return 0;
}

struct linear_network_args {int i; double h; double t;};
void *linear_network(void *args) {
   int i = ((struct linear_network_args*)args)->i;
   int h = ((struct linear_network_args*)args)->h;
   int t = ((struct linear_network_args*)args)->t;
   dir("study1/linear_network/h=%ft=%f/run_%d", h, t, i);
   FILE *p = open("study1/linear_network/h=%ft=%f/run_%d/percat.dat", h, t, i);
   FILE *l = open("study1/linear_network/h=%ft=%f/run_%d/lingcat.dat", h, t, i);
   FILE *o = open("study1/linear_network/h=%ft=%f/run_%d/overlap.dat", h, t, i);
   FILE *lo = open("study1/linear_network/h=%ft=%f/run_%d/local_overlap.dat", h, t, i);

   Network *network = create_network(100, 0.01);
   make_linear(network, h, t);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, -1, false, false, 0.0, 0.0);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / 100e7 * 100.0l);

         write_percat(p, network, t);
         write_lingcat(l, network, t);
         write_overlap(o, network, t, true, false);
         write_overlap(lo, network, t, true, true);
      }
   }

   delete_network(network);

   fclose(lo);
   fclose(o);
   fclose(l);
   fclose(p);

   return 0;
}

struct ring_lattice_args {int K; int i;};
void *ring_lattice(void *args) {
   int K = ((struct ring_lattice_args*)args)->K;
   int i = ((struct ring_lattice_args*)args)->i;

   dir("study1/ring_lattice/K=%d/", K);
   dir("study1/ring_lattice/K=%d/run_%d", K, i);
   FILE *p = open("study1/ring_lattice/K=%d/run_%d/percat.dat", K, i);
   FILE *l = open("study1/ring_lattice/K=%d/run_%d/lingcat.dat", K, i);
   FILE *o = open("study1/ring_lattice/K=%d/run_%d/overlap.dat", K, i);
   FILE *lo = open("study1/ring_lattice/K=%d/run_%d/local_overlap.dat", K, i);

   Network *network = create_network(100, 0.01);
   watts_strogatz(network, 2.0, 0.5, K, 0.0);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, -1, false, false, 0.0, 0.0);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / 100e7 * 100.0l);

         write_percat(p, network, t);
         write_lingcat(l, network, t);
         write_overlap(o, network, t, true, false);
         write_overlap(lo, network, t, true, true);
      }
   }

   delete_network(network);

   fclose(lo);
   fclose(o);
   fclose(l);
   fclose(p);

   return 0;
}

struct beta_args {double beta; int i;};
void *beta_simulation(void *args) {
   double beta = ((struct beta_args*)args)->beta;
   int i = ((struct beta_args*)args)->i;
   if (beta > 1.0) {
      beta = 1.0;
   }
   printf("%f %d\n", beta, i);
   dir("study1/small_world_network/beta_%f", beta);
   dir("study1/small_world_network/beta_%f/run_%d", beta, i);
   FILE *o = open("study1/small_world_network/beta_%f/run_%d/overlap.dat", i, beta);
   FILE *lo = open("study1/small_world_network/beta_%f/run_%d/local_overlap.dat", i, beta);
   FILE *m = open("study1/small_world_network/beta_%f/run_%d/matrix.dat", i, beta);

   Network *network = create_network(100, 0.05);
   watts_strogatz(network, 4.0, 0.2, 6, beta);
   write_weights(m, network);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, -1, false, false, 0.0, 0.0);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / 100e7 * 100.0l);

         write_overlap(o, network, t, true, false);
         write_overlap(lo, network, t, true, true);
      }
   }

   delete_network(network);

   fclose(lo);
   fclose(o);
   fclose(m);

   return 0;
}

// Study 2
struct contact_args {int mode; int i;};
void *contact(void *arg) {
   int mode = ((struct contact_args*)arg)->mode;
   int i    = ((struct contact_args*)arg)->i;
   assert(mode >= 0 && mode < 3);
   const char *name;
   switch (mode) {
      case 0:
         name = "uniform";
         break;
      case 1:
         name = "non_uniform_uniform";
         break;
      case 2:
         name = "non_uniform_non_uniform";
         break;
   }
   dir("study2/%s", name);
   dir("study2/%s/run_%d", name, i);
   FILE *l = open("study2/%s/run_%d/lingcat.dat", name, i);
   FILE *lo = open("study2/%s/run_%d/local_overlap.dat", name, i);
   FILE *o = open("study2/%s/run_%d/overlap.dat", name, i);
   FILE *s = open("study2/%s/run_%d/split.dat", name, i);

   Network *network = create_network(100, 0.01);
   make_isolate(network, 4.0, 0.2);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, -1, false, false, 0.0, 0.0);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / 100e7 * 100.0l);

         write_lingcat(l, network, t);
         write_overlap(o, network, t, true, false);
         write_overlap(lo, network, t, true, true);
      }
   }

   Network *save = clone_network(network);
   reconnect(network, 4.0, 0.2);

   interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, -1, false, false, 0.0, 0.0);

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


   return 0;
}

// Study 3
struct communities_args {bool live; double l1, l2; int i;};
void *communities(void *args) {
   double l1 = ((struct communities_args*)args)->l1;
   double l2 = ((struct communities_args*)args)->l2;
   int i = ((struct communities_args*)args)->i;
   bool live = ((struct communities_args*)args)->live;
   const char *name = live ? "live" : "null";
   dir("study3/%s_model/l1=%fl2=%f/", name, l1, l2);
   dir("study3/%s_model/l1=%fl2=%f/run_%d", name, l1, l2, i);
   dir("study3/%s_model/l1=%fl2=%f/run_%d/matrix", name, l1, l2, i);
   FILE *o = open("study3/%s_model/l1=%fl2=%f/run_%d/overlap.dat", name, i, l1, l2);
   FILE *l = open("study3/%s_model/l1=%fl2=%f/run_%d/local_overlap.dat", name, i, l1, l2);
   FILE *c = open("study3/%s_model/l1=%fl2=%f/run_%d/local_overlap.dat", name, i, l1, l2);

   Network *network = create_network(100, 0.01);
   make_complete(network, 0.5);

   long double interval = 100.0;
   for (unsigned long long int t = 0; t < 100e7; t++) {
      step(network, -1, true, true, l1, l2);

      if (t >= interval) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         interval *= 1.1;
         printf("Running: %Lf%%\n", t / 100e7 * 100.0l);

         FILE *n = open("study3/%s_model/l1=%fl2=%f/run_%d/matrix/%d.dat", name, l1, l2, i, t);
         write_weights(n, network);
         fclose(n);

         write_overlap(o, network, t, true, false);
         write_overlap(l, network, t, true, true);
         write_lingcat(c, network, t);
      }
   }

   delete_network(network);

   fclose(c);
   fclose(l);
   fclose(o);

   return 0;
}

// Entry point
int main(int argc, char **argv) {
   // reorginise dir
   #if defined(study0)
   {
      dir("study0"); 
      pthread_t threads[10];
      int thread_args[10];
      for (int i = 0; i < 10; i++) {
         thread_args[i] = i;
         pthread_create(&threads[i], NULL, core_model, &thread_args[i]);
      }
      JOIN(10);
   }
   #endif

   #if defined(study1)
   {
      dir("study1");
      pthread_t threads[100];

      dir("study1/linear_network");
      struct linear_network_args linear_network_args[100];
      int k = 0;
      for (int i = 0; i < 10; i++) {
         linear_network_args[k] = (struct linear_network_args) {i, 0, 0};
         pthread_create(&threads[k], NULL, linear_network, &linear_network_args[i]);
         k++;

         for (double t = 0.1; t < 1.0; t += 0.1) {
            double h = 1.0 / t;
            linear_network_args[k] = (struct linear_network_args) {i, h, t};
            pthread_create(&threads[k], NULL, linear_network, &linear_network_args[i]);
            k++;

            if (t < 0.5) {
               h = 0.5 / t;
               linear_network_args[k] = (struct linear_network_args) {i, h, t};
               pthread_create(&threads[k], NULL, linear_network, &linear_network_args[i]);
               k++;
            }
         }
      }
      JOIN(k);

      dir("study1/ring_lattice");
      struct ring_lattice_args ring_lattice_args[40];
      k = 0;
      for (int i = 0; i < 10; i++) {
         ring_lattice_args[k] = (struct ring_lattice_args){2, i};
         pthread_create(&threads[k], NULL, ring_lattice, &ring_lattice_args[i]);
         k++;
         ring_lattice_args[k] = (struct ring_lattice_args){4, i};
         pthread_create(&threads[k], NULL, ring_lattice, &ring_lattice_args[k]);
         k++;
         ring_lattice_args[k] = (struct ring_lattice_args){6, i};
         pthread_create(&threads[k], NULL, ring_lattice, &ring_lattice_args[k]);
         k++;
         ring_lattice_args[k] = (struct ring_lattice_args){8, i};
         pthread_create(&threads[k], NULL, ring_lattice, &ring_lattice_args[k]);
         k++;
      }
      JOIN(k);
      
      dir("study1/small_world_network");
      struct beta_args beta_args[100];
      for (int i = 0; i < 100; i++) {
         double beta = i % 10 == 0 ? 0 : 0.0001 * pow(2.8, i % 10);
         beta_args[i] = (struct beta_args){beta, i / 10};
         pthread_create(&threads[i], NULL, beta_simulation, &beta_args[i]);
      }
      JOIN(100);
   }
   #endif

   #if defined(study2)
   {
      dir("study2");
      pthread_t threads[30];
      struct contact_args contact_args[30];
      int k = 0;
      for (int i = 0; i < 10; i++) {
         for (int j = 0; j < 3; j++) {
            contact_args[k] = (struct contact_args) {j, i};
            pthread_create(&threads[k], NULL, contact, &contact_args[k]);
            k++;
         }
      }

      JOIN(k);
   }
   #endif

   #if defined(study3)
   {
      dir("study3"); 
      pthread_t threads[90];

      dir("study3/null_model");
      struct communities_args thread_args[90];
      int thread = 0;
      for (int k = 0; k < 10; k++) {
         double l1 = 0.15;
         for (int i = 0; i < 3; i++) {
            l1 += 0.15;
            double l2 = 0.0;
            for (int j = 0; j < 3; j++) {
               l2 += 0.15;
               thread_args[thread] = (struct communities_args) {false, l1, l2, k};
               pthread_create(&threads[thread], NULL, communities, &thread_args[thread]);
               thread++;
            }
         }
      }
      JOIN(90);

      dir("study3/live_model");
      thread = 0;
      for (int k = 0; k < 10; k++) {
         double l1 = 0;
         for (int i = 0; i < 3; i++) {
            l1 += 0.15;
            double l2 = 0.0;
            for (int j = 0; j < 3; j++) {
               l2 += 0.15;
               thread_args[thread] = (struct communities_args) {true, l1, l2, k};
               pthread_create(&threads[thread], NULL, communities, &thread_args[thread]);
               thread++;
            }
         }
      }
      JOIN(90);
   }
   #endif

   return 0;
}
