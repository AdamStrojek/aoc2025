#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  long res1 = 0;
  long res2 = 0;

  long range_start, range_end;
  char drop;

  size_t primes[] = {2, 3, 5, 7};

  while (fscanf(file, "%ld-%ld%c", &range_start, &range_end, &drop) > 0) {
    for (long i = range_start; i <= range_end; ++i) {
      char num[255];
      sprintf(num, "%ld", i);
      size_t len = strlen(num);
      for (int p = 0; p < 4; ++p) {
        if (len % primes[p] == 0) {
          size_t part_len = len / primes[p];
          bool valid = true;
          for (size_t x = 0; x < primes[p]; ++x) {
            if (strncmp(num, num + part_len * x, len / primes[p]) != 0) {
              valid = false;
              break;
            }
          }
          if (valid) {
            if (p == 0) {
              res1 += i;
            }
            res2 += i;
            break;
          }
        }
      }
    }
  }

  printf("Result for part 1: %ld\n", res1);
  printf("Result for part 2: %ld\n", res2);

  fclose(file);
  return 0;
}
