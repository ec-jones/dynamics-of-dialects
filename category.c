#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "name.h"

typedef struct _category {
  float split;
  Name *head, *tail;
  struct _category *left, *right;

  float top;
  struct _category *next, *prev;
} Category;

// Category constructor
Category *create_category(void) {
  Category *cat = (Category*)malloc(sizeof(Category));
  assert(cat != NULL);

  cat->split = -1;
  cat->head = NULL;
  cat->tail = NULL;
  cat->left = NULL;
  cat->right = NULL;

  cat->top = 1;
  cat->next = NULL;
  cat->prev = NULL;

  return cat;
}

// Return the most relevant name
// Cannot pass NULL
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

// Clone a name list (set tail to last name cloned)
Name *tail = NULL;
Name *clone(Name *src) {
  if (src == NULL) {
    return NULL;
  }
  Name *name = create_name(src->value, src->time_stamp);
  name->next = clone(src->next);
  name->time_stamp = src->time_stamp;
  tail = name;
  return name;
}

// Copy names from one Category to another
// Cannot pass NULL
void namecpy(Category *dest, Category *src) {
  dest->head = clone(src->head);
  dest->tail = tail;
  tail = NULL;
}

// Get the Category associated with x
Category *get_category(Category *cat, float x) {
  while(true) {
    assert(cat != NULL);
    if (cat->split == -1) {
      return cat;
    }
    else if (x < cat->split) {
      cat = cat->left;
    }
    else {
      cat = cat->right;
    }
  }
}

// Split a node
// Cannot pass NULL
void inner_split(Category *cat, float x, float y) {
  cat->split = (x + y) / 2;

  cat->left = create_category();
  cat->right = create_category();

  cat->left->top = cat->split;
  cat->right->top = cat->top;

  cat->left->next = cat->right;
  cat->left->prev = cat->prev;

  cat->right->next = cat->next;
  cat->right->prev = cat->left;

  if (cat->next != NULL) {
    cat->next->prev = cat->right;
  }
  if (cat->prev != NULL) {
    cat->prev->next = cat->left;
  }

  namecpy(cat->left, cat);
  namecpy(cat->right, cat);
}

// Left most leaf of the tree
Category *left_most(Category *cat) {
   while(true) {
      if (cat->split == -1 || cat == NULL) {
         return cat;
      }
      else {
         cat = cat->left;
      }
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

// Deconstructor
void delete_category(Category *cat) {
  if (cat != NULL) {
    delete_category(cat->left);
    delete_category(cat->right);
    delete_name(cat->head);
    free(cat);
  }
}

// Clone leaves
Category *clone_tree(Category *cat) {
  if (cat == NULL) {
    return NULL;
  }
  cat = left_most(cat);
  Category *new = create_category();
  new->head = clone(cat->head);
  new->tail = tail;
  new->next = clone_tree(cat->next);
  new->top = cat->top;
  return new;
}