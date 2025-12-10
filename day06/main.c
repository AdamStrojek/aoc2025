#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "da.h"

typedef struct {
  char op;
  // Position of op based on starting position
  size_t pos;
} op_t;

typedef struct {
  op_t *items;
  size_t len;
  size_t capacity;
} op_list_t;

typedef struct {
  char **items;
  size_t len;
  size_t capacity;
} data_t;

int load_data(char *filename, data_t *data) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Error opening file");
    return -1;
  }

  char *line;
  size_t len;

  while ((line = fgetln(file, &len)) != NULL) {
    len = line[len - 1] == '\n' ? len - 1 : len; // Removing \n
    char *item = malloc(len + 1); // Adding 1 for null-termination
    strncpy(item, line, len);
    da_append(data, item);
  }

  fclose(file);
  return data->len;
}

void process_ops(const char *op_data, op_list_t *list) {
  char ch;
  size_t pos = 0;

  while ((ch = *(op_data++)) != 0) {
    if (!isspace(ch)) {
      // If not space dumping operator
      auto tmp = (op_t){.op = ch, .pos = pos};
      da_append(list, tmp);
    }

    ++pos;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return 1;
  }

  char *filename = argv[1];

  data_t data = {0};
  op_list_t list = {0};
  load_data(filename, &data);
  process_ops(data.items[data.len - 1], &list);
  // We no longer need operators list, it has been processed
  free(da_pop_last(&data));

  uint64_t res1 = 0;
  uint64_t res2 = 0;

  da_foreach(&list, i) {
    auto *item = list.items + i;
    uint64_t result = (item->op == '*');

    da_foreach(&data, j) {
      uint64_t x;
      if (sscanf(data.items[j] + item->pos, "%llu", &x) > 0) {
        printf("Found %lld\n", x);
        if (item->op == '*') {
          result *= x;
        } else {
          result += x;
        }
      } else {
        assert(false && "Failed reading digit!");
      }
    }
    printf("Op: %c, pos: %lu result: %llu\n", list.items[i].op,
           list.items[i].pos, result);
    res1 += result;
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);

  return 0;
}
