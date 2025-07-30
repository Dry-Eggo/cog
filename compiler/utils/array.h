#pragma once
#include <string.h>
/*
  Appends an element to an array of any type using a JArena
  Array must have the following structure:
  struct MyArr {
      Type*    items;
      size_t   count;
      size_t   capacity;
  };
*/
#define arr_append(arr, item, arena) \
do {\
    if ((arr)->capacity <= 0) {\
        (arr)->capacity = 255;\
        (arr)->items    = jarena_alloc((arena), (arr)->capacity);\
    }\
    if ((arr)->count >= (arr)->capacity) {\
        size_t newsz = (arr)->capacity*2;\
        printf("newsz: %zu. count: %zu. cap: %zu\n", newsz, (arr)->count, (arr)->capacity);\
        void* new_items = (void*)jarena_alloc((arena), newsz);\
        memcpy(new_items, (arr)->items, (arr)->count);\
        (arr)->items = new_items;\
        (arr)->capacity = newsz;\
    }\
    (arr)->items[(arr)->count++] = item;\
} while (0)
