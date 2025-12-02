#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

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

  int res1 = 0;
  int res2 = 0;

  int pos = 50;
  int max_value = 100;

  char *line;
  size_t len;

  char dir;
  int num;

  while ((line = fgetln(file, &len)) != NULL) {
    sscanf(line, "%c%d", &dir, &num);
    if (dir == 'L') {
      num *= -1;
    }

    div_t r = div(pos + num, max_value);
    res2 += abs(r.quot) + (dir == 'L' && (r.rem <= 0) - (pos == 0));
    pos = r.rem < 0 ? max_value + r.rem : r.rem;

    if (pos == 0)
      res1++;
  }

  printf("Result for part 1: %d\n", res1);
  printf("Result for part 2: %d\n", res2);

  fclose(file);
  return 0;
}
