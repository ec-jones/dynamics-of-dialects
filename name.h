#include <stdbool.h>

typedef struct _name {
  int value, time_stamp;
  struct _name *next;
} Name;

Name *create_name(int value, int time_stamp);
bool contain_name(Name *name, int value, int time);
extern Name *tail;
Name *clone_name(Name *src);
void delete_name(Name *name);
