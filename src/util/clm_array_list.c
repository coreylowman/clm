#include <stdlib.h>
#include "clm_array_list.h"

ClmArrayList *clm_array_list_new(void (*free_element)(void *element)){
    ClmArrayList *array = malloc(sizeof(*array));
    array->length = 0;
    array->capacity = 16;
    array->data = malloc(array->capacity * sizeof(*(array->data)));
    int i;
    for(i = 0;i < array->capacity;i++){
        array->data[i] = NULL;
    }
    array->free_element = free_element;
	return array;
}

void clm_array_list_free(void *data){
	if (data == NULL) return;

    ClmArrayList *array = (ClmArrayList *)data;
    int i;
    for(i = array->length - 1;i >= 0;i--){
        array->free_element(array->data[i]);
    }
    free(array->data);
    free(array);
}

void *clm_array_list_get(ClmArrayList *array,int i){
    return array->data[i];
}

void clm_array_list_push(ClmArrayList *array, void *data){
    if(array->length == array->capacity){
        int i = array->capacity;
        array->capacity = 2 * array->capacity;
        array->data = realloc(array->data,array->capacity * sizeof(*(array->data)));
        for(;i < array->capacity;i++){
            array->data[i] = NULL;
        }
    }
    array->data[array->length] = data;
    array->length += 1;
}

void clm_array_list_insert_array_at(ClmArrayList *array, int index, ClmArrayList *toAdd){
    if(index > array->length){
        return;
    }
    int i,amountFree = array->capacity - array->length;
    if(toAdd->length > amountFree){
        i = array->capacity;
        array->capacity = array->length + toAdd->length;
        array->data = realloc(array->data,array->capacity * sizeof(*(array->data)));
        for(;i < array->capacity;i++){
            array->data[i] = NULL;
        }
    }
    for(i = array->length - 1;i >= index;i--){
        array->data[i + toAdd->length] = array->data[i];
    }
    for(i = 0;i < toAdd->length;i++){
        array->data[i + index] = toAdd->data[i];
    }

    for(i = 0;i < toAdd->length;i++){
        toAdd->data[i] = NULL;
    }
    free(toAdd->data);
    free(toAdd);

    array->length = array->length + toAdd->length;
}

void clm_array_list_pop(ClmArrayList *array){
    array->free_element(array->data[array->length - 1]);
    array->data[array->length - 1] = NULL;
    array->length = array->length - 1;
}

void clm_array_list_remove_at(ClmArrayList *array, int index){
    if(index >= array->length) return;

    int i;
    array->free_element(array->data[index]);
    for(i = index + 1;i < array->length;i++){
        array->data[i - 1] = array->data[i];
    }
    array->data[array->length - 1] = NULL;
    array->length = array->length - 1;
}

void clm_array_list_remove_all_after(ClmArrayList *array, int start){
    if(start >= array->length) return;

    int i;
    for(i = array->length - 1; i >= start;i--){
        array->free_element(array->data[i]);
        array->data[i] = NULL;
    }
    array->length = start;
}

void clm_array_list_foreach(ClmArrayList *array,void (*func)(void *data)){
	if (array == NULL) return;
	int i = 0;
    for(;i < array->length;i++){
        func(array->data[i]);
    }
}

void clm_array_list_foreach_2(ClmArrayList *array, int level, void (*func)(void *data,int l)){
    if (array == NULL) return;
    int i = 0;
    for(;i < array->length;i++){
        func(array->data[i], level);
    }
}