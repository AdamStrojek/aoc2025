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

  long range_start, range_end;
  char drop;

  while (fscanf(file, "%ld-%ld%c", &range_start, &range_end, &drop) > 0) {
    int range_len = range_end - range_start;
    printf("Range: %ld - %ld, len: %d\n", range_start, range_end, range_len);
    for (long i = range_start; i <= range_end; ++i) {
      char num[255];
      sprintf(num, "%ld", i);
      size_t len = strlen(num);
      size_t len_half = len / 2;
      if (len % 2 == 0 && strncmp(num, num + len_half, len_half) == 0) {
        printf("%s len: %lu\n", num, strlen(num));
        res1 += i;
      }
    }
  }

  printf("Result for part 1: %ld\n", res1);

  fclose(file);
  return 0;
}
