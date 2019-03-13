#include <stdio.h>
#include "name.h"

typedef struct _category {
	float split;
	Name *head, *tail;
	struct _category *left, *right;

	float top;
	struct _category *next, *prev;
} Category;

Category *create_category(void);
int peek(Category *cat);
void push(Category *cat, int value, int time_stamp);
void enqueue(Category *cat, int value, int time_stamp);
Category *get_category(Category *cat, float x);
Category *left_most(Category *cat);
void inner_split(Category *cat, float x, float y);
Category *clone_leaves(Category *cat);
void delete_category(Category *cat);