#include <stdbool.h>

typedef struct _name {
   int value, time_stamp;
   struct _name *next;
} Name;

Name *create_name(int value, int time_stamp);
bool contain_name(Name *name, int value, int time);
Name *clone_name(Name *src, Name **tail);
void delete_name(Name *name);