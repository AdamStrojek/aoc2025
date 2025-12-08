#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Source - https://stackoverflow.com/a
// Posted by ephemient, modified by community. See post 'Timeline' for change
// history Retrieved 2025-12-07, License - CC BY-SA 2.5

int fpeek(FILE *stream) {
  int c;

  c = fgetc(stream);
  ungetc(c, stream);

  return c;
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

  uint64_t data[1100] = {0};
  char ops[1100] = {0};
  size_t n = 0;

  // Go to last line
  if (file) {
    fseek(file, 0, SEEK_END);
    long pos = ftell(file);
    char c;
    while (pos > 0 && (c = fgetc(file)) != '\n') {
      fseek(file, --pos, SEEK_SET);
    }
    // Now read the line forward from here
    fseek(file, pos + 1, SEEK_SET);

    while ((c = fgetc(file)) != EOF) {
      if (isspace(c))
        continue;
      data[n] = c == '*'; // starting value for data, when mult set to 1
      ops[n++] = c;
    }
  }

  // for (size_t i = 0; i < n; ++i) {
  //   printf("Op: %c\n", ops[i]);
  // }

  // Reset and start from beggining
  fseek(file, 0, SEEK_SET);
  // char ch;

  // while (isdigit(fpeek(file))) {
  //   do {
  //     // Scanning single line
  //     uint64_t num;
  //     fscanf(file, "%llu", &num);
  //     printf("Number scanned %llu\n", num);
  //   } while (fgetc(file) != '\n');
  //   printf("Next line\n");
  // }

  while ((line = fgetln(file, &len)) != NULL) {
    // len -= 1; // line contain whole text with \n at the end, we don't care
    // about

    size_t data_pos = 0;
    char *p = line;
    char *end = line + len;

    while (p < end) {
      // Skip whitespace
      while (p < end && isspace(*p))
        p++;
      if (p >= end)
        break;

      char *parse_end;
      uint64_t numb = strtoimax(p, &parse_end, 10);

      switch (ops[data_pos]) {
      case '+':
        data[data_pos] += numb;
        break;
      case '*':
        data[data_pos] *= numb;
        break;
      }

      printf("Resp %lld\n", data[data_pos]);

      p = parse_end;
      data_pos++;
    }

    if (!isspace(*p)) {
      // Line not ended, because we encountered non-number, we are in operations
      // part
      break;
    }
  }

  for (size_t i = 0; i < n; ++i) {
    res1 += data[i];
  }

  printf("Result for part 1: %lld\n", res1);
  printf("Result for part 2: %lld\n", res2);

  fclose(file);
  return 0;
}
