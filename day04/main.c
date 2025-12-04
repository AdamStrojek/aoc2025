#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int locations[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                       {0, 1},   {1, -1}, {1, 0},  {1, 1}};

#define X 0
#define Y 1

// Returns numer of adjacted paper rolls
// Skips if current field is not @ (returns 9)
int check_adjacent(char *grid, size_t row_size, size_t x, size_t y) {
  if (x > row_size || y > row_size) {
    return 0;
  }

  if (grid[y * row_size + x] != '@') {
    // printf("%dx%d %c\n", y, x, grid[y * row_size + x]);
    return 9;
  }

  int rolls = 0;

  for (int i = 0; i < 8; ++i) {
    int pos_x = x + locations[i][X];
    int pos_y = y + locations[i][Y];

    if (pos_x < 0 || pos_y < 0 || (size_t)pos_x >= row_size ||
        (size_t)pos_y >= row_size) {
      continue;
      // if (y == 4 && x == 9)
      //   printf("Out of range %dx%d\n", pos_x, pos_y);
      // Out of range continue;
    }

    // if (y == 4 && x == 9)
    //   printf("For %02ldx%02ld, checking %2dx%2d: %c\n", x, y, pos_x, pos_y,
    //          grid[pos_y * row_size + pos_x]);
    if (grid[pos_y * row_size + pos_x] == '@')
      rolls++;
  }
  return rolls;
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
  size_t row = 0;

  char *grid = NULL;
  size_t size = 0;

  while ((line = fgetln(file, &len)) != NULL) {
    len -= 1; // line contain whole text with \n at the end, we don't care about

    if (grid == NULL) {
      grid = malloc(len * len);
      size = len;
    }

    memcpy(grid + size * row, line, size);
    ++row;
  }

  printf("%ldx%ld\n", size, row);

  for (size_t y = 0; y < row; ++y) {
    for (size_t x = 0; x < size; ++x) {

      int ret = check_adjacent(grid, size, x, y);

      // if (ret == 9) {
      //   printf(" ");
      // } else
      //   printf("%d", ret);

      if (ret < 4)
        res1++;
    }
    // printf("x\n");
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);

  free(grid);
  fclose(file);
  return 0;
}
