#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX(a, b) a > b ? a : b

uint64_t joltage_output(const char *line, size_t line_len, size_t n) {
  size_t pos[n];

  pos[0] = 0;

  for (size_t m = 0; m < n; ++m) {
    // for m == 0 and n == 2 -> -1 from end
    // for m == 1 and n == 2 -> 0 from end
    // for  m == 0 and n == 3 -> -2 from end
    // for  m == 1 and n == 3 -> -1 from end
    // for  m == 2 and n == 3 -> 0 from end
    size_t limit = line_len - (n - m - 1);
    // printf(
    //     "Searching for digit %zu until pos %zu, selected max=%c,
    //     pos=%02zu\n", m, limit, line[pos[m]], pos[m]);
    // no i=pos[m] -> it already was marked as biggest right to properly
    // initialize array I avoided comparing it agains 0 or some other INT_MIN
    for (size_t i = pos[m] + 1; i < limit; ++i) {
      // printf("for i=%02zu   looking at=%c,   selected max=%c,   pos=%02zu\n",
      // i,
      //        line[i], line[pos[m]], pos[m]);
      if (line[pos[m]] < line[i]) {
        // printf("new max %c\n", line[i]);
        pos[m] = i;
      }
    }

    pos[m + 1] = pos[m] + 1;
  }

  uint64_t res = 0;

  for (size_t i = 0; i < n; ++i) {
    res *= 10;
    res += line[pos[i]] - '0';
  }

  printf("Answer %lld\n", res);

  return res;
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

  while ((line = fgetln(file, &len)) != NULL) {
    len--; // line contain whole text with \n at the end

    res1 += joltage_output(line, len, 2);
    res2 += joltage_output(line, len, 12);
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);

  fclose(file);
  return 0;
}
