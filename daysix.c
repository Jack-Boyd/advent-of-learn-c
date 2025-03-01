#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define ROWS 130
#define COLS 130

int read_file(const char* filename, char map[ROWS][COLS + 1]) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("File opening failed"); 
    return EXIT_FAILURE;
  }
  for (int i = 0; i < ROWS; i++) {
    if (fgets(map[i], COLS + 2, file) == NULL) {
      fprintf(stderr, "Unexpected end of file at row %d\n", i);
      fclose(file);
      return EXIT_FAILURE;
    }

    size_t len = strlen(map[i]);
    if (len > 0 && map[i][len - 1] == '\n') {
      map[i][len - 1] = '\0';
    }
  }
  fclose(file);
  return 0;
}

int count_visted_pos(char map[ROWS][COLS + 1]) {
  int **visited = malloc(ROWS * sizeof(int*));
  if (!visited) {
    perror("Memory allocation failed");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < ROWS; i++) {
    visited[i] = calloc(COLS, sizeof(int));
    if (!visited[i]) {
      for (int k = 0; k < i; k++) 
        free(visited[k]);
      free(visited);
      return EXIT_FAILURE;
    }
  }

  bool stop = false;
  int pos_x = 0, pos_y = 0;

  for (int i = 0; (i < ROWS) && !stop; i++) {
    for (int j = 0; (j < COLS + 1) && !stop; j++) {
      if (map[i][j] == '^') {
        pos_x = i;
        pos_y = j;
        stop = true;
      }
    }
  } 

  visited[pos_x][pos_y] = 1;
  int visited_count = 1;
  int direction = 1;

  if (map[pos_x - 1][pos_y] == '#')
    direction++;

  while (true) {
    switch (direction) {
      case 1:
        pos_x--;
        if (map[pos_x - 1][pos_y] == '#')
          direction++;
        break;
      case 2:
        pos_y++;
        if (map[pos_x][pos_y + 1] == '#')
          direction++;
        break;
      case 3:
        pos_x++;
        if (map[pos_x + 1][pos_y] == '#')
          direction++;
        break;
      case 4:
        pos_y--;
        if (map[pos_x][pos_y - 1] == '#')
          direction = 1;
        break;
      default:
        break;
    }
    if (visited[pos_x][pos_y] != 1) {
      visited[pos_x][pos_y] = 1;
      visited_count++;
    }
    if ((pos_x < 1 || pos_x >= ROWS) || (pos_y < 1 || pos_y >= COLS)) 
      break;
  }
  
  return visited_count;
}

int main() {
  char map[ROWS][COLS + 1];
  if (read_file("daysix.txt", map) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  int visited_count = count_visted_pos(map);
  
  printf("%d\n", visited_count);
  return 0;
}