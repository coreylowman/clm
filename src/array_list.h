#ifndef ARRAY_LIST_H_
#define ARRAY_LIST_H_

typedef struct ArrayList {
  void **data;
  void (*free_element)(void *element);
  int capacity;
  int length;
} ArrayList;

ArrayList *array_list_new(void (*free_element)(void *element));
void array_list_free(void *data);

void array_list_push(ArrayList *self, void *data);

void array_list_foreach(ArrayList *self, void (*func)(void *data));
void array_list_foreach_2(ArrayList *self, int level,
                          void (*func)(void *data, int l));

#endif
