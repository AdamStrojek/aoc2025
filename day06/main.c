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
  size_t len;
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
      if (list->len > 0) {
        // Updating previous one
        da_last(list).len = pos - da_last(list).pos - 1;
      }
      // If not space dumping operator
      auto tmp = (op_t){.op = ch, .pos = pos};
      da_append(list, tmp);
    }

    ++pos;
  }

  // Workaround, we can't estimate how many numbers has last operation, but it
  // always will be max 4
  da_last(list).len = 4;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return 1;
  }

  char *filename = argv[1];

  data_t data = {0};
  op_list_t op_list = {0};
  load_data(filename, &data);
  process_ops(data.items[data.len - 1], &op_list);
  // We no longer need operators list, it has been processed and can be
  // discarded/freed
  free(da_pop_last(&data));

  uint64_t answer_part_1 = 0;
  uint64_t answer_part_2 = 0;

  da_foreach(&op_list, i) {
    auto item = op_list.items + i;
    uint64_t result_1 = (item->op == '*');
    uint64_t result_2 = result_1;

    // Calculating part 1
    uint64_t x;
    da_foreach(&data, j) {
      if (sscanf(data.items[j] + item->pos, "%llu", &x) > 0) {
        if (item->op == '*') {
          result_1 *= x;
        } else {
          result_1 += x;
        }
      } else {
        assert(false && "Failed reading digit!");
      }
    }

    // Calculating part 2
    size_t size = op_list.items[i].len * sizeof(uint64_t);
    uint64_t *y = malloc(size);
    memset(y, 0, size);
    for (size_t k = 0; k < op_list.items[i].len; k++) {
      da_foreach(&data, j) {
        auto line = data.items[j];
        auto d = line[item->pos + k];
        if (isdigit(d)) {
          y[k] *= 10;
          y[k] += d - '0';
        }
      }

      if (y[k] > 0) {
        if (item->op == '*') {
          result_2 *= y[k];
        } else {
          result_2 += y[k];
        }
      }
    }
    free(y);

    printf("Op: %c, pos: %lu, len: %lu, result 2: %llu\n", op_list.items[i].op,
           op_list.items[i].pos, op_list.items[i].len, result_2);
    answer_part_1 += result_1;
    answer_part_2 += result_2;
  }

  printf("Result for part 1: %lld\n", answer_part_1);
  printf("Result for part 2: %lld\n", answer_part_2);

  return 0;
}
