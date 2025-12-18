#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "da.h"

typedef uint16_t lights_t;
typedef lights_t button_mask_t;

typedef struct {
  int *items;
  button_mask_t mask;
  size_t len;
  size_t capacity;
} button_t;

typedef struct {
  button_t *items;
  size_t len;
  size_t capacity;
} button_list_t;

button_mask_t button_to_mask(size_t lights_len, button_t *group) {
  da_foreach(group, i) {

    group->mask |= 1 << (lights_len - group->items[i] - 1);
  }
  return group->mask;
}

typedef struct {
  int *items;
  size_t len;
  size_t capacity;
} joltage_list_t;

typedef struct {
  lights_t lights;
  button_list_t buttons;
  joltage_list_t joltage;
} data_t;

typedef struct {
  data_t *items;
  size_t len;
  size_t capacity;
} data_list_t;

typedef struct {
  lights_t light;
  int *joltage;
  int lvl;
} lights_lvl_t;

typedef struct {
  lights_lvl_t *items;
  size_t len;
  size_t capacity;
} lights_queue_t;

#define SKIP_SPACE(p)                                                          \
  do {                                                                         \
    while (isspace((unsigned char)*(p))) {                                     \
      (p)++;                                                                   \
    }                                                                          \
  } while (0)

// ----------------------
// Print utilities
// ----------------------
void print_mask(uint16_t x) {
  uint16_t mask = 1u << (sizeof(x) * CHAR_BIT - 1);

  while (mask) {
    putchar((x & mask) ? '1' : '0');
    mask >>= 1;
  }
}

void print_data(data_list_t *list) {
  da_foreach(list, i) {
    auto item = list->items[i];
    printf("Light:\n");
    print_mask(item.lights);
    printf("\nButtons:\n01234567\n");
    da_foreach(&item.buttons, j) {
      auto b_item = item.buttons.items[j];
      print_mask(b_item.mask);
      printf("\n");
    }

    printf("joltage: {");
    da_foreach(&item.joltage, j) {
      auto j_item = item.joltage.items[j];
      printf("%d, ", j_item);
    }
    printf("}\n");
  }
}

// ----------------------
// Data Input
// ----------------------

// Args:
// - lights_len - it will be required to do proper bit mask
// Returns:
// -1 -> string doesnt start with '(', you are probably reading wrong section
int read_button_group(size_t lights_len, char *it, button_t *group) {
  auto start = it;
  if (*it != '(') {
    return -1;
  }
  while (*(it++) != ')') {
    int r = 0;
    while (isdigit(*it)) {
      r *= 10;
      r += *it - '0';
      it++;
    }
    // *group |= 1 << (lights_len - r - 1);
    da_append(group, r);
  }

  group->mask = button_to_mask(lights_len, group);

  return it - start;
}

int read_jitter(char *it, joltage_list_t *joltage) {
  auto start = it;
  if (*it != '{') {
    return -1;
  }
  while (*(it++) != '}') {
    int r = 0;
    while (isdigit(*it)) {
      r *= 10;
      r += *it - '0';
      it++;
    }
    da_append(joltage, r);
  }

  return it - start;
}

int read_data(const char *filename, data_list_t *list) {
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    perror("Error opening file");
    return -1;
  }

  char *line;
  size_t len;

  while ((line = fgetln(file, &len)) != NULL) {
    // Lights
    if (*line != '[') {
      return -1;
    }
    size_t lights_len = 0;
    lights_t lights = 0;
    char *it = line;
    while (*(it++) != ']') {
      lights_len += 1;
      lights <<= 1;
      if (*it == '#') {
        lights ^= 1;
      }
    }

    // TODO
    // Found:   00000000000000000000000000001100
    // Expeced: 00000000000000000000000000000110
    lights >>= 1;
    lights_len -= 1;

    line = it;

    SKIP_SPACE(it);

    data_t data = {
        .lights = lights,
    };

    while (true) {
      button_t b = {0};
      int ret = read_button_group(lights_len, it, &b);
      if (ret < 0) {
        break;
      }
      da_append(&data.buttons, b);

      it += ret;
      SKIP_SPACE(it);
    };

    read_jitter(it, &data.joltage);

    da_append(list, data);
  }
  fclose(file);

  return 0;
}

// ----------------------
// Data Processing
// ----------------------
#define LVL_LIMIT 10

// #if 0
int process_inner(lights_queue_t *q, lights_t expected, lights_t lights,
                  button_list_t *list, int lvl) {
  if (lvl > LVL_LIMIT) {
    // printf("Exceeded level ");
    // print_mask(lights);
    // printf("\n");
    return -1;
  }

  da_foreach(list, i) {
    auto item = list->items[i];
    lights_t new_light = lights ^ item.mask;
    da_append(q, (lights_lvl_t){.light = new_light, .lvl = lvl + 1});

    // printf("Apply mask:     ");
    // print_mask(list->items[i]);
    // printf("\nNew Light:      ");
    // print_mask(new_light);
    // printf("\n");

    if (new_light == expected) {
      // printf("I found Walle in %d!  ", lvl);
      // print_mask(new_light);
      // printf("\n");

      return lvl;
    }
  }

  return -1;
}

int process_list(data_t data, button_list_t *list) {
  lights_queue_t q = {0};

  // Initial state of the lights, all should be turned off
  int *joltage = malloc(sizeof(int) * data.joltage.len);
  memset(joltage, 0, sizeof(int) * data.joltage.len);
  da_append(&q, (lights_lvl_t){0, joltage, 1});

  // printf("Expected: ");
  // print_mask(expected);
  // printf("\n");

  size_t i = 0;
  while (i < q.len) {
    // printf("Processing %3lu, lvl:%d: ", i, q.items[i].lvl);
    // print_mask(q.items[i].light);
    // printf("\n");

    int ret =
        process_inner(&q, data.lights, q.items[i].light, list, q.items[i].lvl);

    if (ret != -1) {
      free(q.items->joltage);
      free(q.items);
      return ret;
    }
    i++;
  }
  free(q.items->joltage);
  free(q.items);
  return -1;
}
// #endif

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return 1;
  }
  char *filename = argv[1];
  data_list_t list = {0};

  read_data(filename, &list);

  print_data(&list);

  uint64_t res1 = 0;
  uint64_t res2 = 0;

  printf("Processing data...\n");

  da_foreach(&list, i) {
    da_item(&list, i, item);
    int ret = process_list(item, &item.buttons);
    if (ret > 0) {
      res1 += ret;
    } else {
      printf("Couldn't find soluton for line %ld, expected light ", i);
      print_mask(item.lights);
      printf("\n");
    }
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);

  return 0;
}
