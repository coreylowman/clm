#include <stdlib.h>

#include "array_list.h"

ArrayList *array_list_new(void (*free_element)(void *element)) {
  ArrayList *array = malloc(sizeof(*array));
  array->length = 0;
  array->capacity = 16;
  array->data = malloc(array->capacity * sizeof(*(array->data)));
  int i;
  for (i = 0; i < array->capacity; i++) {
    array->data[i] = NULL;
  }
  array->free_element = free_element;
  return array;
}

void array_list_free(void *data) {
  if (data == NULL)
    return;

  ArrayList *array = (ArrayList *)data;
  int i;
  for (i = array->length - 1; i >= 0; i--) {
    array->free_element(array->data[i]);
  }
  free(array->data);
  free(array);
}

void array_list_push(ArrayList *array, void *data) {
  if (array->length == array->capacity) {
    int i = array->capacity;
    array->capacity = 2 * array->capacity;
    array->data =
        realloc(array->data, array->capacity * sizeof(*(array->data)));
    for (; i < array->capacity; i++) {
      array->data[i] = NULL;
    }
  }
  array->data[array->length] = data;
  array->length += 1;
}

void array_list_foreach(ArrayList *array, void (*func)(void *data)) {
  if (array == NULL)
    return;
  int i = 0;
  for (; i < array->length; i++) {
    func(array->data[i]);
  }
}

void array_list_foreach_2(ArrayList *array, int level,
                          void (*func)(void *data, int l)) {
  if (array == NULL)
    return;
  int i = 0;
  for (; i < array->length; i++) {
    func(array->data[i], level);
  }
}
