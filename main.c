// TODO: tidy, local_match weights

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include "network.h"

// Parameters
typedef enum { NONE = 0, PERCAT = 1, LINGCAT = 2, MATCH = 4, BOUNDS = 8, LOCAL_MATCH = 16, MATCH_ENV = 32, LOCAL_MATCH_ENV = 64, MATCH_WINDOW = 128, SPLIT_POP = 256 } MODE;
Network *network, *save;
int N = 100;
unsigned long long int T = 100e7, dump_T = -1;
float d = 0.05;
MODE mode = PERCAT  | LINGCAT | MATCH;
FILE *p = NULL, *m = NULL, *l = NULL, *b = NULL, *lm = NULL, *me = NULL, *lme = NULL, *mw = NULL, *s = NULL;

// Open files
void open(char *out) {
   char buf[100];
   sprintf(buf, "./data/%s", out);
   mkdir("./data/", 0777);
   mkdir(buf, 0777);
   if (mode & PERCAT) {
      sprintf(buf, "./data/%s/percat.dat", out);
      p = fopen(buf, "w");
      assert(p != NULL);
   }
   if (mode & LINGCAT) {
      sprintf(buf, "./data/%s/lingcat.dat", out);
      l = fopen(buf, "w");
      assert(l != NULL);
   }
   if (mode & MATCH) {
      sprintf(buf, "./data/%s/match.dat", out);
      m = fopen(buf, "w");
      assert(m != NULL);
   }
   if (mode & BOUNDS) {
      sprintf(buf, "./data/%s/bounds.dat", out);
      b = fopen(buf, "w");
      assert(b != NULL);
   }
   if (mode & LOCAL_MATCH) {
      sprintf(buf, "./data/%s/local_match.dat", out);
      lm = fopen(buf, "w");
      assert(lm != NULL);
   }
   if (mode & LOCAL_MATCH_ENV) {
      sprintf(buf, "./data/%s/local_match_env.dat", out);
      lme = fopen(buf, "w");
      assert(lme != NULL);
   }
   if (mode & MATCH_ENV) {
      sprintf(buf, "./data/%s/match_env.dat", out);
      me = fopen(buf, "w");
      assert(me != NULL);
   }
   if (mode & MATCH_WINDOW) {
      sprintf(buf, "./data/%s/match_window.dat", out);
      mw = fopen(buf, "w");
      assert(mw != NULL);
   }
   if (mode & SPLIT_POP) {
      sprintf(buf, "./data/%s/split.dat", out);
      s = fopen(buf, "w");
      assert(s != NULL);
   }
}

// Close files
void close(void) {
   if (mode & PERCAT) {
      fclose(p);
   }
   if (mode & LINGCAT) {
      fclose(l);
   }
   if (mode & MATCH) {
      fclose(m);
   }
   if (mode & LOCAL_MATCH) {
      fclose(lm);
   }
   if (mode & MATCH_ENV) {
      fclose(me);
   }
   if (mode & LOCAL_MATCH_ENV) {
      fclose(lme);
   }
   if (mode & SPLIT_POP) {
      fclose(s);
   }
}

// Print the linguistic category boundaries to file
void dump_bounds(Category *cat, FILE *f) {
   cat = left_most(cat);

   while(true) {
      if (cat == NULL) {
         return;
      }
      else if (cat->next == NULL) {
         fprintf(f, "%f", cat->top);
         return;
      }
      else {
         if (cat->head != NULL && cat->next->head != NULL
             && peek(cat) != peek(cat->next)) {
            fprintf(f, "%f ", cat->top);
         }
         cat = cat->next;
      }
   }
}

// The number of linguistic categories in a tree
int lingcat_categories(Category *cat) {
   cat = left_most(cat);

   int acc = 1;
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
        acc++;
      }
      cat = cat->next;
   }
   return acc;
}

// Write data to files
void write(unsigned long long int t) {
   if (mode & PERCAT) {
      fprintf(p, "%lld %f\n", t, ((float)split_count + N) / N);
   }
   if (mode & LINGCAT) {
      float acc = 0;
      for (int i = 0; i < N; i++) {
         acc += lingcat_categories(network->nodes[i]->tree);
      }
      fprintf(l, "%lld %f\n", t, acc / N);
   }
   if (mode & MATCH) {
      float acc = 0;
      for (int i = 0; i < N; i++) {
         for (int j = i + 1; j < N; j++) {
            acc += match_node(network->nodes[i], network->nodes[j], 0, 1, false);
         }
      }
      fprintf(m, "%lld %f\n", t, 2.0f * acc / (N * (N - 1)));
   }
   // if (mode & LOCAL_MATCH) {
   //    int deg_acc = 0;
   //    float acc = 0;
   //    for (int i = 0; i < N; i++) {
   //       int deg = network->nodes[i]->degree;
   //       deg_acc += deg;
   //       for (int j = 0; j < deg; j++) {
   //          acc += match_node(network->nodes[i], network->nodes[i]->neighbours[j], 0, 1, false);
   //       }
   //    }
   //    fprintf(lm, "%lld %f\n", t, acc / deg_acc);
   // }
   if (mode & MATCH_ENV) {
      float acc = 0;
      for (int i = 0; i < N; i++) {
         for (int j = i + 1; j < N; j++) {
            acc += match_node(network->nodes[i], network->nodes[j], 0, 1, true);
         }
      }
      fprintf(me, "%lld %f\n", t, 2.0f * acc / (N * (N - 1)));
   }
   // if (mode & LOCAL_MATCH_ENV) {
   //    int deg_acc = 0;
   //    float acc = 0;
   //    for (int i = 0; i < N; i++) {
   //       int deg = network->nodes[i]->degree;
   //       deg_acc += deg;
   //       for (int j = 0; j < deg; j++) {
   //          acc += match_node(network->nodes[i], network->nodes[i]->neighbours[j], 0, 1, true);
   //       }
   //    }
   //    fprintf(lme, "%lld %f\n", t, acc / deg_acc);
   // }
   if (mode & BOUNDS && t >= dump_T) {
      mode = mode & ~BOUNDS ;
      for (int i = 0; i < N; i++) {
         dump_bounds(network->nodes[i]->tree, b);
         fprintf(b, "\n");
      }
      fclose(b);
   }
   if (mode & MATCH_WINDOW && t >= dump_T) {
      mode = mode & ~MATCH_WINDOW;
      const float width = 0.1;
      for (float bottom = 0; bottom <= 2.0; bottom += 0.1) {
         if (bottom + width > 1.0) {
            break;
         }
         float acc = 0;
         for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
               acc += match_node(network->nodes[i], network->nodes[j], bottom, bottom + width, false);
            }
         }
         fprintf(mw, "%f %f\n", bottom + (width / 2.0), 2.0f * acc / (N * (N - 1)));
      }
   }
}

// Get two stimuli using the parameters
void get_stimuli(float *a, float *b, float h, float t) {
   *a = frand();
   *b = frand();
   while (fabs(*a - *b) <= d) {
      *b = frand();
   }

   if (h*t != 0.0 && h*t != 1.0) {
      if (*a <= h * t) {
         *a /= h;
      } else {
         *a -= h * t;
         *a *= (1.0 - t) / (1.0 - (h*t));
         *a += t;
      }

      if (*b <= h * t) {
         *b /= h;
      } else {
         *b -= h * t;
         *b *= (1.0 - t) / (1.0 - (h*t));
         *b += t;
      }
   }
}

// Dump network
void dump_network(Network *network, int T) {
   char buf[100];
   sprintf(buf, "./data/communities3/dump_%d.dat", T);
   FILE *n = fopen(buf, "w");
   assert(n != NULL);

   for (int i = 0; i < N ; i++) {
      for (int j = 0; j < N; j++) {
         fprintf(n, "%d %d %f\n", i, j, network->weights[i][j]);
      }
   }
}

// Get an agents index and an index from its adj list
void get_agents(Network *network, int *n, int *m) {
   *n = frand() * N;
   float cum[N];
   cum[0] = network->weights[*n][0];
   for (int i = 1; i < N; i++) {
      cum[i] = cum[i - 1] + network->weights[*n][i];
   }
   float p = frand() * cum[N - 1];
   int i = 0;
   for (; i < N && p > cum[i]; i++) { }
   *m = i;
}

int A = 0, B = 0, C = 0;
void names(void) {
   A = 0, B = 0, C = 0;
   for (int i = 0; i < N; i++) {
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
void compare(void) {
   names();
   fprintf(s, "A: %d\n", A);
   const float width = 0.1;
   for (float bottom = 0; bottom <= 2.0; bottom += 0.1) {
      if (bottom + width >= 1.0) {
         break;
      }
      float acc = 0;
      for (int i = 0; i < N; i++) {
         for (int j = 0; j < N / 2; j++) {
            acc += match_node(network->nodes[i], save->nodes[j], bottom, bottom + width, true);
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
            acc += match_node(network->nodes[i], save->nodes[j], bottom, bottom + width, true);
         }
      }
      fprintf(s, "%f %f\n", bottom + (width / 2), 2 * acc/ (N * N));
   }
   fprintf(s, "C: %d\n", C);
}

// Entry point
int main(int argc, char **argv) {
   time_t t;
   srand((unsigned) time(&t));
   clock_t begin = clock();

   open("communities3");
   network = create_network(N);
   make_complete(network, 0.5);

   long double step = 100.0l;
   long double step2 = 100.0l;
   for (unsigned long long int i = 0; i < T; i++) {
      int n, m;
      get_agents(network, &n, &m);
      Agent *spk = network->nodes[n],
            *lst = network->nodes[m];

      float a, b;
      get_stimuli(&a, &b, spk->h, spk->t);

      if (negotiate(spk, lst, a, b, -1)) {
         network->weights[n][m] = pow(network->weights[n][m], 0.5);
         network->weights[m][n] = pow(network->weights[m][n], 0.5);
      }
      else {
         network->weights[n][m] = pow(network->weights[n][m], 1.5);
         network->weights[m][n] = pow(network->weights[m][n], 1.5);
      }

      if (i >= step) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         step *= 1.1;
         printf("Running: %llu, %Lf%%\n", i, (long double)i / (long double)T * 100.0l);
         write(i);
      }
      if (i >= step2) {
         step2 *= 2;
         dump_network(network, i);
      }
   }

   delete_network(network);
   close();

   clock_t end = clock();
   double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

   printf("Done in: %f seconds.", time_spent);
   getchar();
   return 0;
}
