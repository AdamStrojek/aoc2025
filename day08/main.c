#include "da.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int64_t x, y, z;
} junction_t;

typedef struct {
  size_t a_idx, b_idx;
  uint64_t dist_sq;
} junction_pair_t;

typedef struct {
  // Index of item for faster search
  size_t *items;
  size_t len;
  size_t capacity;
} junction_list_t;

typedef struct {
  junction_list_t *items;
  size_t len;
  size_t capacity;
} junction_conn_t;

uint64_t junction_distance(junction_t a, junction_t b) {
  int64_t x = b.x - a.x;
  int64_t y = b.y - a.y;
  int64_t z = b.z - a.z;

  return x * x + y * y + z * z;
}

void junction_print_pair(junction_pair_t *p) {
  printf("[%3ld]-[%3ld] -> %llu\n", p->a_idx, p->b_idx, p->dist_sq);
}

void add_unique(junction_list_t *list, size_t new) {
  da_foreach(list, i) {
    if (list->items[i] == new) {
      return;
    }
  }

  da_append(list, new);
}

int comp(const void *a, const void *b) { return (*(size_t *)b - *(size_t *)a); }

int comp_pair(const void *a, const void *b) {
  junction_pair_t *pair_a = (junction_pair_t *)a;
  junction_pair_t *pair_b = (junction_pair_t *)b;
  if (pair_a->dist_sq == pair_b->dist_sq) {
    return 0;
  } else if (pair_a->dist_sq > pair_b->dist_sq) {
    return 1;
  }
  return -1;
}

#define JUNCTION_MAX 1000
#define PAIRS_MAX 500000

size_t read_from_file(const char *file_name, junction_t *points) {
  size_t points_len = 0;
  FILE *file = fopen(file_name, "r");

  if (file == NULL) {
    perror("Error opening file");
    exit(1);
  }

  char *line;
  size_t len;

  while ((line = fgetln(file, &len)) != NULL) {
    // len -= 1; // last char is \n
    junction_t point;
    sscanf(line, "%lld,%lld,%lld", &point.x, &point.y, &point.z);

    points[points_len++] = point;
  }

  fclose(file);

  printf("Scanned %lu points\n", points_len);
  return points_len;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return 1;
  }

  junction_t points[1000] = {0};
  size_t points_len = read_from_file(argv[1], points);

  printf("Calculating distances...\n");

  junction_pair_t *p = malloc(sizeof(junction_pair_t) * PAIRS_MAX);
  memset(p, 0, sizeof(junction_pair_t) * PAIRS_MAX);
  size_t p_len = 0;
  // Sorting pairs by distance
  for (size_t a_idx = 0; a_idx < points_len - 1; ++a_idx) {
    for (size_t b_idx = a_idx + 1; b_idx < points_len; ++b_idx) {
      uint64_t dist = junction_distance(points[a_idx], points[b_idx]);
      // printf("%ld:%ld    Distance: %llu\n", a_idx, b_idx, dist);
      p[p_len++] =
          (junction_pair_t){.a_idx = a_idx, .b_idx = b_idx, .dist_sq = dist};
    }
  }

  qsort(p, p_len, sizeof(p[0]), comp_pair);

  // for (size_t i = 0; i < p_len; ++i) {
  //   junction_print_pair(p + i);
  // }

  printf("Calculating...\n");

  uint64_t res1 = 1;
  uint64_t res2 = 0;

  junction_conn_t conn = {0};

  int part = 2;
  size_t max_i = part == 1 ? 100 : p_len;
  for (size_t i = 0; i < max_i; ++i) {
    junction_pair_t *pair = p + i;

    size_t match_a = -1;
    size_t match_b = -1;

    da_foreach(&conn, c_idx) {
      // Going over all already registered junction boxes and checking will
      // this pair match any item
      junction_list_t *junk_list = conn.items + c_idx;

      da_foreach(junk_list, a_idx) {
        size_t item_idx = junk_list->items[a_idx];
        if (item_idx == pair->a_idx) {
          add_unique(junk_list, pair->b_idx);
          match_a = c_idx;
          break;
        }
        if (item_idx == pair->b_idx) {
          add_unique(junk_list, pair->a_idx);
          match_b = c_idx;
          break;
        }
      }
    }

    if (match_a != -1 && match_b != -1 && match_a != match_b) {
      // Both junction boxes found in two separate lists, need to merge them
      junction_list_t *junk_list_a = conn.items + match_a;
      junction_list_t *junk_list_b = conn.items + match_b;
      // Update list A with all records from B
      da_foreach(junk_list_b, i) {
        add_unique(junk_list_a, junk_list_b->items[i]);
      }
      // Remove B from list of all connections
      da_remove(&conn, match_b);
    }

    if (match_a == -1 && match_b == -1) {
      // No matching junction box list found. Creating new connected junction
      // boxes list
      junction_list_t l = {0};
      da_append(&l, pair->a_idx);
      da_append(&l, pair->b_idx);
      da_append(&conn, l);
    }

    if (conn.items[0].len == 1000) {
      res2 = points[pair->a_idx].x * points[pair->b_idx].x;
      break;
    }
  }

  size_t *lens = malloc(conn.len * sizeof(size_t));

  da_foreach(&conn, c_idx) lens[c_idx] = conn.items[c_idx].len;

  qsort(lens, conn.len, sizeof(size_t), comp);

  for (size_t i = 0; i < 3; ++i) {
    // printf("Connection %ld has %lu items\n", i, lens[i]);
    res1 *= lens[i];
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);
  return 0;
}
