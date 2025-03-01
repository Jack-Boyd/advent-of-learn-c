#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROWS 140
#define COLS 140

int read_file(const char* filename, char grid[ROWS][COLS + 1]) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Unable to open file");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < ROWS; i++) {
    if (fgets(grid[i], COLS + 2, file) == NULL) {
      fprintf(stderr, "Unexpected end of file at row %d\n", i);
      fclose(file);
      return EXIT_FAILURE;
    }

    size_t len = strlen(grid[i]);
    if (len > 0 && grid[i][len - 1] == '\n') {
      grid[i][len - 1] = '\0';
    }
  }
  fclose(file);
  return 0;
}

int traverse_horizontal(char grid[ROWS][COLS + 1], int i, int j, bool forwards) {
  const char pattern[] = "MAS";
  int step = forwards ? 1 : -1;
  bool complete = true;
  for (int k = 0; k < 3; k++) {
    if (grid[i][j + step * (k + 1)] != pattern[k]) {
      complete = false;
      break;
    }
  }
  return complete ? 1 : 0;
}

int traverse_vertical(char grid[ROWS][COLS + 1], int i, int j, bool forwards) {
  const char pattern[] = "MAS";
  int step = forwards ? 1 : -1;
  bool complete = true;
  for (int k = 0; k < 3; k++) {
    if (grid[i + step * (k + 1)][j] != pattern[k]) {
      complete = false;
      break;
    }
  }
  return complete ? 1 : 0;
}

int traverse_inc_diagonal(char grid[ROWS][COLS + 1], int i, int j, bool forwards) {
  const char pattern[] = "MAS";
  int step = forwards ? 1 : -1;
  bool complete = true;
  for (int k = 0; k < 3; k++) {
    if (grid[i + (step * -1) * (k + 1)][j + step * (k + 1)] != pattern[k]) {
      complete = false;
      break;
    }
  }
  return complete ? 1 : 0;
}

int traverse_dec_diagonal(char grid[ROWS][COLS + 1], int i, int j, bool forwards) {
  const char pattern[] = "MAS";
  int step = forwards ? 1 : -1;
  bool complete = true;
  for (int k = 0; k < 3; k++) {
    if (grid[i + step * (k + 1)][j + step * (k + 1)] != pattern[k]) {
      complete = false;
      break;
    }
  }
  return complete ? 1 : 0;
}

int traverse(char grid[ROWS][COLS + 1], int i, int j) {
  if (i < 0 || i > ROWS || j < 0 || j > COLS + 1) return 0;
  
  int complete_words = 0;
  if (grid[i][j + 1] == 'M' || grid[i][j - 1] == 'M') {
    if (grid[i][j + 1] == 'M')
      complete_words += traverse_horizontal(grid, i, j, true);
    if (grid[i][j - 1] == 'M')
      complete_words += traverse_horizontal(grid, i, j, false);
  }
  
  if (grid[i + 1][j] == 'M' || grid[i - 1][j] == 'M') {
    if (grid[i + 1][j] == 'M')
      complete_words += traverse_vertical(grid, i, j, true);
    if (grid[i - 1][j] == 'M')
      complete_words += traverse_vertical(grid, i, j, false);
  }

  if (grid[i - 1][j + 1] == 'M' || grid[i + 1][j - 1] == 'M') {
    if (grid[i - 1][j + 1] == 'M')
      complete_words += traverse_inc_diagonal(grid, i, j, true);
    if (grid[i + 1][j - 1] == 'M')
      complete_words += traverse_inc_diagonal(grid, i, j, false);
  }
  
  if (grid[i + 1][j + 1] == 'M' || grid[i - 1][j - 1] == 'M') {
    if (grid[i + 1][j + 1] == 'M')
      complete_words += traverse_dec_diagonal(grid, i, j, true);
    if (grid[i - 1][j - 1] == 'M')
      complete_words += traverse_dec_diagonal(grid, i, j, false);
  } 

  return complete_words;
}


int count_xs(char grid[ROWS][COLS + 1], int i, int j) {
  bool left = false, right = false;
  if (grid[i + 1][j - 1] == 'M' || grid[i - 1][j + 1] == 'M') {
    if ((grid[i + 1][j - 1] == 'M' && grid[i - 1][j + 1] == 'S') ||
      (grid[i + 1][j - 1] == 'S' && grid[i - 1][j + 1] == 'M')) {
      left = true;
    }
  }
  if (grid[i + 1][j + 1] == 'M' || grid[i - 1][j - 1] == 'M') {
    if ((grid[i + 1][j + 1] == 'M' && grid[i - 1][j - 1] == 'S') ||
      (grid[i + 1][j + 1] == 'S' && grid[i - 1][j - 1] == 'M')) {
      right = true;
    } 
  }

  return (left && right) ? 1 : 0;
}

int main() {
  char grid[ROWS][COLS + 1] = {0};
  if (read_file("dayfour.txt", grid) != 0) {
    return EXIT_FAILURE;
  }

  int words = 0;
  int xs = 0;
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      if (grid[i][j] == 'X') {
        words += traverse(grid, i, j);
      }
      if (grid[i][j] == 'A') {
        xs += count_xs(grid, i, j);
      }
    }
  }
  printf("%d\n", words);
  printf("%d\n", xs);
  return 0;
}