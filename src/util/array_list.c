#include <stdlib.h>

#include "array_list.h"

ArrayList *array_list_new(void (*free_element)(void *element)) {
  ArrayList *self = malloc(sizeof(*self));
  self->length = 0;
  self->capacity = 16;
  self->data = malloc(self->capacity * sizeof(*(self->data)));
  int i;
  for (i = 0; i < self->capacity; i++) {
    self->data[i] = NULL;
  }
  self->free_element = free_element;
  return self;
}

void array_list_free(void *data) {
  if (data == NULL)
    return;

  ArrayList *self = (ArrayList *)data;
  int i;
  for (i = self->length - 1; i >= 0; i--) {
    self->free_element(self->data[i]);
  }
  free(self->data);
  free(self);
}

void array_list_push(ArrayList *self, void *data) {
  if (self->length == self->capacity) {
    int i = self->capacity;
    self->capacity = 2 * self->capacity;
    self->data = realloc(self->data, self->capacity * sizeof(*(self->data)));
    for (; i < self->capacity; i++) {
      self->data[i] = NULL;
    }
  }
  self->data[self->length] = data;
  self->length += 1;
}

void array_list_foreach(ArrayList *self, void (*func)(void *data)) {
  if (self == NULL)
    return;
  int i = 0;
  for (; i < self->length; i++) {
    func(self->data[i]);
  }
}

void array_list_foreach_2(ArrayList *self, int level,
                          void (*func)(void *data, int l)) {
  if (self == NULL)
    return;
  int i = 0;
  for (; i < self->length; i++) {
    func(self->data[i], level);
  }
}
