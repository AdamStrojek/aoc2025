#include <assert.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  uint64_t start, end;
} range;

bool range_in(range *r, uint64_t number) {
  return r->start <= number && number <= r->end;
}

// return >= 0 -> found range
// return == -1 -> no matching range
int ranges_pos(range *ranges, size_t len, uint64_t number) {
  for (size_t i = 0; i < len; ++i) {
    if (range_in(ranges + i, number)) {
      return i;
    }
  }
  return -1;
}

#define MIN(a, b) (a <= b ? a : b)
#define MAX(a, b) (a >= b ? a : b)

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

  range fresh[200] = {0};
  size_t fresh_len = 0;

  // Reading ranges
  while ((line = fgetln(file, &len)) != NULL) {
    if (len == 1 && line[0] == '\n') {
      // No new
      break;
    }

    uint64_t start, end;
    sscanf(line, "%llu-%llu", &start, &end);
    assert(start <= end);

    int pos_start = ranges_pos(fresh, fresh_len, start);
    int pos_end = ranges_pos(fresh, fresh_len, end);

    // New range is inside of two different ranges. We need to merge them into
    // single range
    if (pos_start >= 0 && pos_end >= 0 && pos_start != pos_end) {
      fresh[pos_start].start =
          MIN(fresh[pos_start].start, fresh[pos_end].start);
      fresh[pos_start].end = MAX(fresh[pos_start].end, fresh[pos_end].end);
      // Move last element to pos_end to delete it by replacing
      fresh[pos_end] = fresh[--fresh_len];
      continue;
    }

    // New range is partially inside of other single range, so we can extend
    // existing one
    if (pos_start >= 0 || pos_end >= 0) {
      // pos_start == pos_end or one of them is -1
      int pos = MAX(pos_start, pos_end);
      if (start < fresh[pos].start) {
        fresh[pos].start = start;
      }

      if (end > fresh[pos].end) {
        fresh[pos].end = end;
      }
      continue;
    }

    range r = {
        .start = start,
        .end = end,
    };

    bool replaced = false;

    // New range is contains other smaller range that already existis
    for (size_t pos = 0; pos <= fresh_len; ++pos) {
      if (range_in(&r, fresh[pos].start) && range_in(&r, fresh[pos].end)) {
        fresh[pos] = r;
        replaced = true;
        // TODO: what if there are more smaller ranges?
        // 5-6
        // 10-11
        // 2-20
        // ->
        // 2-20
        break;
      }
    }

    // Otherwise just add new range
    if (!replaced) {
      fresh[fresh_len] = r;
      ++fresh_len;
    }
  }

  // PART 1
  uint64_t product_id = 0;
  while ((line = fgetln(file, &len)) != NULL) {
    // Reading products
    sscanf(line, "%llu", &product_id);
    if (ranges_pos(fresh, fresh_len, product_id) >= 0) {
      res1 += 1;
    }
  }

  // PART 2
  for (size_t i = 0; i < fresh_len; ++i) {
    res2 += fresh[i].end - fresh[i].start + 1;
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);

  fclose(file);
  return 0;
}
