#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

// Name node
typedef struct _name {
   int value, time_stamp;
   struct _name *next;
} Name;

// Name constructor
// if value = -1 new name
// if value = -2 new name is 0 mod 3
// if value = -3 new name is 1 mod 3
// if value = -4 new name is 2 mod 3
Name *create_name(int value, int time_stamp) {
   static int new = 0;
   static int new_0 = 0;
   static int new_1 = 1;
   static int new_2 = 2;
   switch (value) {
      case -1:
         value = ++new;
         break;
      case -2:
         value = new_0;
         new_0 += 3;
         break;
      case -3:
         value = new_1;
         new_1 += 3;
         break;
      case -4:
         value = new_2;
         new_2 += 3;
         break;  
   }

   Name *name = malloc(sizeof(Name));
   assert(name != NULL);
   
   *name = (Name){value, time_stamp, NULL};
   return name;
}

// Check if value is in list
bool contain_name(Name *name, int value, int time) {
   while (name != NULL) {
      if (name->value == value && (time == -1 || name->time_stamp >= time)) {
         return true;
      }
      else {
         name = name->next;
      }
   }
   return false;
}

// Clone a name list (set tail to last name cloned)
Name *clone_name(Name *src, Name **tail) {
   if (src == NULL) {
      *tail = NULL;
      return NULL;
   }
   Name *name = create_name(src->value, src->time_stamp);
   name->next = clone_name(src->next, tail);
   if(name->next == NULL) {
      *tail = name;
   }
   return name;
}

// Deconstructor
void delete_name(Name *name) {
   if (name != NULL) {
      delete_name(name->next);
      free(name);
   }
}