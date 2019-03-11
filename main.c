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
MODE mode = LINGCAT | LOCAL_MATCH_ENV | MATCH_ENV | SPLIT_POP;
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
void close() {
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

// Write data to files
void write(unsigned long long int t) {
   if (mode & PERCAT) {
      fprintf(p, "%lld %f\n", t, ((float)split_count + N) / N);
   }
   if (mode & LINGCAT) {
      float acc = 0;
      for (int i = 0; i < N; i++) {
         acc += lingcat_categories(network->adj_list[i]->tree);
      }
      fprintf(l, "%lld %f\n", t, acc / N);
   }
   if (mode & MATCH) {
      float acc = 0;
      for (int i = 0; i < N; i++) {
         for (int j = i + 1; j < N; j++) {
            acc += match_node(network->adj_list[i], network->adj_list[j], 0, 1, false);
         }
      }
      fprintf(m, "%lld %f\n", t, 2.0f * acc / (N * (N - 1)));
   }
   if (mode & LOCAL_MATCH) {
      int deg_acc = 0;
      float acc = 0;
      for (int i = 0; i < N; i++) {
         int deg = network->adj_list[i]->degree;
         deg_acc += deg;
         for (int j = 0; j < deg; j++) {
            acc += match_node(network->adj_list[i], network->adj_list[i]->neighbours[j], 0, 1, false);
         }
      }
      fprintf(lm, "%lld %f\n", t, acc / deg_acc);
   }
   if (mode & MATCH_ENV) {
      float acc = 0;
      for (int i = 0; i < N; i++) {
         for (int j = i + 1; j < N; j++) {
            acc += match_node(network->adj_list[i], network->adj_list[j], 0, 1, true);
         }
      }
      fprintf(me, "%lld %f\n", t, 2.0f * acc / (N * (N - 1)));
   }
   if (mode & LOCAL_MATCH_ENV) {
      int deg_acc = 0;
      float acc = 0;
      for (int i = 0; i < N; i++) {
         int deg = network->adj_list[i]->degree;
         deg_acc += deg;
         for (int j = 0; j < deg; j++) {
            acc += match_node(network->adj_list[i], network->adj_list[i]->neighbours[j], 0, 1, true);
         }
      }
      fprintf(lme, "%lld %f\n", t, acc / deg_acc);
   }
   if (mode & BOUNDS && t >= dump_T) {
      mode = mode & ~BOUNDS ;
      for (int i = 0; i < N; i++) {
         dump_bounds(network->adj_list[i]->tree, b);
         fprintf(b, "\n");
      }
      fclose(b);
   }
   if (mode & MATCH_WINDOW && t >= dump_T) {
      mode = mode & ~MATCH_WINDOW;
      const float width = 0.1;
      for (float bottom = 0; bottom <= 2.0; bottom += 0.1) {
         if (bottom + width >= 1.0) {
            break;
         }
         float acc = 0;
         for (int i = 0; i < N; i++) {
            for (int j = i + 1; j < N; j++) {
               acc += match_node(network->adj_list[i], network->adj_list[j], bottom, bottom + width, false);
            }
         }
         fprintf(mw, "%f %f\n", bottom + (width / 2), 2.0f * acc / (N * (N - 1)));
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

   if (h != 0 && h*t != 1) {
      if (*a <= h * t) {
         *a /= h;
      } else {
         *a -= h * t;
         *a *= (1 - t) / (1 - (h*t));
         *a += t;
      }

      if (*b <= h * t) {
         *b /= h;
      } else {
         *b -= h * t;
         *b *= (1 - t) / (1 - (h*t));
         *b += t;
      }
   }
}

// Get two agents
void get_agents(Agent **spk, Agent **lst, Network *network) {
   int n = frand() * N;
   int m = frand() * network->adj_list[n]->degree;

   *spk = network->adj_list[n];
   *lst = network->adj_list[n]->neighbours[m];
}

int A = 0, B = 0, C = 0;
void names() {
   for (int i = 0; i < N; i++) {
      Category *cat = left_most(network->adj_list[i]->tree);
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
void compare() {
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
            acc += match_node(network->adj_list[i], save->adj_list[j], bottom, bottom + width, true);
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
            acc += match_node(network->adj_list[i], save->adj_list[j], bottom, bottom + width, true);
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

   open("contact");
   network = create_network(N);
   make_isolate(network, 0, 0);

   long double step = 100.0l;
   for (unsigned long long int i = 0; i < T; i++) {
      Agent *spk, *lst;
      get_agents(&spk, &lst, network);

      float a, b;
      get_stimuli(&a, &b, spk->h, spk->t);

      negotiate(spk, lst, a, b, -1);
      if (i >= step) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         step *= 1.1;
         printf("Running: %llu, %Lf%%\n", i, (long double)i / (long double)T * 100.0l);
         write(i);
      }
   }

   save = clone_network(network);
   reconnect(network);
   step = 100.0l;

   for (unsigned long long int i = 0; i < T; i++) {
      Agent *spk, *lst;
      get_agents(&spk, &lst, network);

      float a, b;
      get_stimuli(&a, &b, spk->h, spk->t);

      negotiate(spk, lst, a, b, -1);
      if (i >= step) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
         step *= 1.1;
         printf("Running: %llu, %Lf%%\n", i, (long double)i / (long double)T * 100.0l);
         write(i);
      }
   }

   compare();

   delete_network(network);
   delete_network(save);
   close();

   clock_t end = clock();
   double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

   printf("Done in: %f seconds.", time_spent);
   getchar();
   return 0;
}
