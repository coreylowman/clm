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

void *clm_array_list_get(ClmArrayList *array,int i);

void clm_array_list_push(ClmArrayList *array, void *data);
void clm_array_list_insert_array_at(ClmArrayList *array, int index, ClmArrayList *toAdd);

void clm_array_list_pop(ClmArrayList *array);
void clm_array_list_remove_at(ClmArrayList *array, int index);
void clm_array_list_remove_all_after(ClmArrayList *array, int start);

void clm_array_list_foreach(ClmArrayList *array,void (*func)(void *data));
void clm_array_list_foreach_2(ClmArrayList *array, int level, void (*func)(void *data,int l));

#endif