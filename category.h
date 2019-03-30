#include "name.h"

typedef struct _category {
   double split;
   Name *head, *tail;
   struct _category *left, *right;

   double top;
   struct _category *next, *prev;
} Category;

Category *create_category(void);
Category *get_category(Category *cat, double x);
Category *left_most(Category *cat);
void inner_split(Category *cat, double x, double y);
int peek(Category *cat);
void push(Category *cat, int value, int time_stamp);
void enqueue(Category *cat, int value, int time_stamp);
int lingcat_categories(Category *cat);
Category *clone_leaves(Category *cat);
void delete_category(Category *cat);