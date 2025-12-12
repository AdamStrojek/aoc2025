#ifndef DA_H
#define DA_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*
 * All Dynamic Arrays need to match pattern:
 */
// typedef struct {
//   uint64_t *items;
//   size_t len;
//   size_t capacity;
// } da_t;

#define da_reserve(da, new_capacity)                                           \
  do {                                                                         \
    if ((new_capacity) > (da)->capacity) {                                     \
      if ((da)->capacity == 0)                                                 \
        (da)->capacity = 1;                                                    \
      while ((new_capacity) > (da)->capacity) {                                \
        (da)->capacity *= 2;                                                   \
      }                                                                        \
      (da)->items =                                                            \
          realloc((da)->items, (da)->capacity * sizeof(*(da)->items));         \
    }                                                                          \
  } while (0)

#define da_append(da, ...)                                                     \
  da_reserve((da), (da)->len + 1);                                             \
  (da)->items[(da)->len++] = (__VA_ARGS__)

#define da_remove(da, index) ((da)->items[index] = (da)->items[--(da)->len])

#define da_pop_last(da) ((da)->items[--(da)->len])

#define da_last(da) ((da)->items[(da)->len - 1])

#define da_foreach(da, index) for (size_t index = 0; index < (da)->len; ++index)

#define da_item(da, index, item_var_name)                                      \
  auto item_var_name = (da)->items[(index)]

#endif // DA_H
