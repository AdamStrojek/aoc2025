#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint64_t start, end;
} range;

bool range_in(range *r, uint64_t number) {
  return r->start <= number && number <= r->end;
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

  range fresh[200] = {0};
  size_t fresh_len = 0;

  // Reading ranges
  while ((line = fgetln(file, &len)) != NULL) {
    if (len == 1 && line[0] == '\n') {
      // No new
      break;
    }

    sscanf(line, "%llu-%llu", &fresh[fresh_len].start, &fresh[fresh_len].end);

    printf("New range: %llu-%llu\n", fresh[fresh_len].start,
           fresh[fresh_len].end);
    ++fresh_len;
  }

  // Reading products
  uint64_t product_id = 0;
  while ((line = fgetln(file, &len)) != NULL) {

    sscanf(line, "%llu", &product_id);

    printf("New product: %llu\n", product_id);

    for (size_t i = 0; i < fresh_len; ++i) {
      if (range_in(fresh + i, product_id)) {
        res1 += 1;
        break;
      }
    }
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);

  fclose(file);
  return 0;
}
