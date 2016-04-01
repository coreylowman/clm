#ifndef CLM_ARRAY_LIST_H_
#define CLM_ARRAY_LIST_H_

typedef struct ClmArrayList {
  void **data;
  void (*free_element)(void *element);
  int capacity;
  int length;
} ClmArrayList;

ClmArrayList *clm_array_list_new(void (*free_element)(void *element));
void clm_array_list_free(void *data);

void clm_array_list_push(ClmArrayList *array, void *data);

void clm_array_list_foreach(ClmArrayList *array, void (*func)(void *data));
void clm_array_list_foreach_2(ClmArrayList *array, int level,
                              void (*func)(void *data, int l));

#endif