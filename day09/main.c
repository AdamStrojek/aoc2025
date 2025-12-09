#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "da.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
  size_t x, y;
} tile_t;

typedef struct {
  tile_t *items;
  size_t len;
  size_t capacity;
} tile_list_t;

void tile_print(tile_t *t) { printf("%lux%lu", t->x, t->y); }

void tile_grid_print(tile_list_t *list, size_t max_x, size_t max_y) {
  for (size_t y = 0; y < max_y; ++y) {
    for (size_t x = 0; x < max_x; ++x) {
      bool exists = false;
      da_foreach(list, i) {
        if (list->items[i].x == x && list->items[i].y == y) {
          exists = true;
          break;
        }
      }

      printf(exists ? "#" : ".");
    }
    printf("\n");
  }
}

size_t tile_area(tile_t *a, tile_t *b) {
  size_t len_x = MAX(a->x, b->y) - MIN(a->x, b->x) + 1;
  size_t len_y = MAX(a->y, b->y) - MIN(a->y, b->y) + 1;

  return len_x * len_y;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return 1;
  }

  char *filepath = argv[1];
  FILE *file = fopen(filepath, "r");

  if (file == NULL) {
    perror("Error opening file");
    return 1;
  }

  uint64_t res1 = 0;
  uint64_t res2 = 0;

  char *line;
  size_t len;

  tile_list_t list = {0};

  while ((line = fgetln(file, &len)) != NULL) {
    // len -= 1; // last char is \n
    tile_t t = {0};
    sscanf(line, "%lu,%lu", &t.x, &t.y);

    da_append(&list, t);
  }

  printf("Board:\n");

  tile_grid_print(&list, 14, 9);

  printf("Calculating areas:\n");
  da_foreach(&list, i) {
    da_foreach(&list, j) {
      if (i >= j)
        continue;
      size_t area = tile_area(list.items + i, list.items + j);
      // Max area
      res1 = MAX(res1, area);
    }
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);

  fclose(file);
  return 0;
}
