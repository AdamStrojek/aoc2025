#include <stdint.h>
#include <stdio.h>

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

  uint64_t beam[256] = {0};
  size_t beam_len = 0;

  while ((line = fgetln(file, &len)) != NULL) {
    len -= 1; // last char is \n
    beam_len = beam_len > 0 ? beam_len : len;
    for (size_t i = 0; i < len; ++i) {
      if (line[i] == 'S') {
        beam[i] = 1;
        printf("S");
      } else if (line[i] == '.') {
        if (beam[i]) {
          printf("|");
        } else {
          printf(".");
        }
      } else if (line[i] == '^') {
        if (beam[i]) {
          printf("\b|^");
          res1 += 1;
          beam[i - 1] += beam[i];
          beam[i + 1] += beam[i];
          beam[i] = 0;
        } else {
          printf("^");
        }
      }
    }
    printf("\n");
  }

  printf("Result for part 1: %lld\n", res1);

  for (size_t i = 0; i < beam_len; ++i) {
    res2 += beam[i];
  }
  printf("Result for part 2: %lld\n", res2);

  fclose(file);
  return 0;
}
