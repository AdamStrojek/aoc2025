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

  while ((line = fgetln(file, &len)) != NULL) {
    // len -= 1; // last char is \n
    int number;
    sscanf(line, "%d", &number);

    printf("Number: %d\n", number);

    res1 += number;
    res2 -= number;
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);

  fclose(file);
  return 0;
}
