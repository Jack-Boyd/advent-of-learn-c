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

int count_visted_pos(const char map[ROWS][COLS + 1]) {
  int pos_row = -1, pos_col = -1;
  
  for (int i = 0; i < ROWS && pos_row == -1; i++) {
    for (int j = 0; (j < COLS + 1); j++) {
      if (map[i][j] == '^') {
        pos_row = i;
        pos_col = j;
        break;
      }
    }
  } 
  if (pos_row == -1) {
    perror("Error finding starting point");
    return EXIT_FAILURE;
  }

  bool visited[ROWS][COLS];
  memset(visited, 0, sizeof(visited));

  visited[pos_row][pos_col] = true;
  int visited_count = 1;
  int direction = 1;

  if (map[pos_row - 1][pos_col] == '#')
    direction = 2;

  int cur_row = pos_row, cur_col = pos_col;

  while (true) {
    switch (direction) {
      case 1: cur_row--; break;
      case 2: cur_col++; break;
      case 3: cur_row++; break;
      case 4: cur_col--; break;
    }
    if (cur_row < 1 || cur_row >= ROWS || cur_col < 1 || cur_col >= COLS) {
      break;
    }
    if (!visited[cur_row][cur_col]) {
      visited[cur_row][cur_col] = true;
      visited_count++;
    }
    int next_row = cur_row, next_col = cur_col;
    
    switch (direction) {
      case 1: next_row--; break;
      case 2: next_col++; break;
      case 3: next_row++; break;
      case 4: next_col--; break;
    }

    if (map[next_row][next_col] == '#')
      direction = (direction % 4) + 1;
  }
  
  return visited_count;
}

bool is_obstacle(int x, int y, int obst_x, int obst_y, const char map[ROWS][COLS+1]) {
  if (x == obst_x && y == obst_y)
      return true;
  if (x >= 0 && x < ROWS && y >= 0 && y < COLS && map[x][y] == '#')
      return true;
  return false;
}

bool simulate_new_block (const char map[ROWS][COLS + 1], int obst_x, int obst_y) {  
  int start_x = -1, start_y = -1;
  
  for (int i = 0; i < ROWS && start_x == -1; i++) {
    for (int j = 0; j < COLS + 1; j++) {
      if (map[i][j] == '^') {
        start_x = i;
        start_y = j;
        break;
      }
    }
  } 
  if (start_x == -1) {
    perror("Error finding starting point");
    return EXIT_FAILURE;
  }
  int pos_x = start_x, pos_y = start_y;
  int direction = 1;

  int next_x = pos_x - 1, next_y = pos_y;
  if (is_obstacle(next_x, next_y, obst_x, obst_y, map)) {
      direction = 2;
  }


  bool visited[ROWS][COLS][4];
  memset(visited, 0, sizeof(visited));
  while (true) {
    if (pos_x < 0 || pos_x >= ROWS || pos_y < 0 || pos_y >= COLS)
      return false;
    
    if (visited[pos_x][pos_y][direction - 1])
      return true;
    
    visited[pos_x][pos_y][direction - 1] = true;

    int candidate_x = pos_x, candidate_y = pos_y;
    if (direction == 1)
      candidate_x = pos_x - 1;
    else if (direction == 2)
      candidate_y = pos_y + 1;
    else if (direction == 3)
      candidate_x = pos_x + 1;
    else if (direction == 4)
      candidate_y = pos_y - 1;
    
   
    if (is_obstacle(candidate_x, candidate_y, obst_x, obst_y, map)) {
      direction = (direction % 4) + 1;
    } else {
      pos_x = candidate_x;
      pos_y = candidate_y;
    }
  }
  return false;
}

int main() {
  char map[ROWS][COLS + 1];
  if (read_file("daysix.txt", map) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  int visited_count = count_visted_pos(map);
  int new_block_count = 0;
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      if (map[i][j] == '.') {
        if (simulate_new_block(map, i, j)) {
          new_block_count++;
        }
      }
    }
  }

  printf("%d\n", visited_count);
  printf("%d\n", new_block_count);
  return 0;
}