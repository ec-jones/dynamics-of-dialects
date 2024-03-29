#include <stdlib.h>
#include <assert.h>
#include "name.h"

typedef struct _category {
   double split;
   Name *head, *tail;
   struct _category *left, *right;

   // for leaf traversal
   double top;
   struct _category *next, *prev;
} Category;

// Category constructor
Category *create_category(void) {
   Category *cat = malloc(sizeof(Category));
   assert(cat != NULL);

   *cat = (Category){-1.0, NULL, NULL, NULL, NULL, 1.0, NULL, NULL};
   return cat;
}

// Get the partition that x falls in
Category *get_category(Category *cat, double x) {
   while (cat->split != -1) {
      if (x < cat->split) {
         cat = cat->left;
      }
      else {
         cat = cat->right;
      }
   }
   return cat;
}

// Left most leaf of the tree
Category *left_most(Category *cat) {
   return get_category(cat, 0);
}

// Split a node
void inner_split(Category *cat, double x, double y) {
   cat->split = (x + y) / 2;

   cat->left = create_category();
   cat->right = create_category();

   cat->left->top = cat->split;
   cat->right->top = cat->top;

   cat->left->next = cat->right;
   cat->right->next = cat->next;

   cat->right->prev = cat->left;
   cat->left->prev = cat->prev;

   if (cat->next != NULL) {
      cat->next->prev = cat->right;
   }
   if (cat->prev != NULL) {
      cat->prev->next = cat->left;
   }

   cat->left->head = clone_name(cat->head, &cat->left->tail);
   cat->right->head = clone_name(cat->head, &cat->right->tail);
}

// Return the preferred name associated with a category
int peek(Category *cat) {
   return cat->head->value;
}

// Insert new most relevant name to front of list
void push(Category *cat, int value, int time_stamp) {
   if (cat->head == NULL) {
      cat->head = create_name(value, time_stamp);
      cat->tail = cat->head;
   }
   else {
      Name *new_head = create_name(value, time_stamp);
      new_head->next = cat->head;
      cat->head = new_head;
   }
}

// Insert new least relevant name to back of list
void enqueue(Category *cat, int value, int time_stamp) {
   if (cat->head == NULL) {
      cat->head = create_name(value, time_stamp);
      cat->tail = cat->head;
   }
   else {
      Name *new_tail = create_name(value, time_stamp);
      cat->tail->next = new_tail;
      cat->tail = new_tail;
   }
}

// The number of linguistic categories in a tree
int lingcat_categories(Category *cat) {
   cat = left_most(cat);

   int acc = 0;
   while(cat != NULL) {

      if (cat->head != NULL &&
            (cat->next == NULL ||
            (cat->next->head != NULL && peek(cat) != peek(cat->next)))) {
         acc++;
      }

      cat = cat->next;
   }
   return acc;
}

// Clone leaves
Category *clone_leaves(Category *cat) {
   if (cat == NULL) {
      return NULL;
   }
   cat = left_most(cat);
   Category *new = create_category();
   new->head = clone_name(cat->head, &new->tail);
   new->next = clone_leaves(cat->next);
   new->top = cat->top;
   return new;
}

// Deconstructor
void delete_category(Category *cat) {
   if (cat != NULL) {
      delete_category(cat->left);
      delete_category(cat->right);
      delete_name(cat->head);
      free(cat);
   }
}