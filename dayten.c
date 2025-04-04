#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LENGTH 59

int read_file(const char *filename, int map[LENGTH][LENGTH]) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return EXIT_FAILURE;
  }
  char line[LENGTH + 2];
  int row = 0;
  while (row < LENGTH && fgets(line, sizeof(line), file) != NULL) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
      len--;
    }

    if (len < LENGTH) {
      fprintf(stderr, "Error: Line %d is too short.\n", row);
      fclose(file);
      return EXIT_FAILURE;
    }

    for (int col = 0; col < LENGTH; col++) {
      if (!isdigit(line[col])) {
        fprintf(stderr, "Error: Invalid character '%c' at row %d, col %d.\n", line[col], row, col);
        fclose(file);
        return EXIT_FAILURE;
      }
      map[row][col] = line[col] - '0';
    }
    row++;
  }

  if (row < LENGTH) {
    fprintf(stderr, "Error: File has fewer than %d lines.\n", LENGTH);
    fclose(file);
    return EXIT_FAILURE;
  }

  fclose(file);
  return EXIT_SUCCESS;
}

void dfs(int map[LENGTH][LENGTH], int r, int c, int visited[LENGTH][LENGTH], int found_nine[LENGTH][LENGTH]) {
  visited[r][c] = 1;
  if (map[r][c] == 9) {
    found_nine[r][c] = 1;
    return;
  }
  int dr[4] = {-1, 1, 0, 0};
  int dc[4] = {0, 0, -1, 1};
  int next_val = map[r][c] + 1;
  for (int d = 0; d < 4; d++) {
    int nr = r + dr[d];
    int nc = c + dc[d];
    if (nr >= 0 && nr < LENGTH && nc >= 0 && nc < LENGTH) {
      if (map[nr][nc] == next_val && !visited[nr][nc]) 
        dfs(map, nr, nc, visited, found_nine);
    }
  }
}

int calc_trailhead(int map[LENGTH][LENGTH], int start_r, int start_c) {
  int visited[LENGTH][LENGTH] = {0};
  int found_nine[LENGTH][LENGTH] = {0};
  dfs(map, start_r, start_c, visited, found_nine);
  int count = 0;
  for (int i = 0; i < LENGTH; i++) {
    for (int j = 0; j < LENGTH; j++) {
      if (found_nine[i][j])
        count++;
    }
  }
  return count;
}

int calc_trailhead_sum(int map[LENGTH][LENGTH]) {
  int total_trailhead_sum = 0;
  for (int i = 0; i < LENGTH; i++) {
    for (int j = 0; j < LENGTH; j++) {
      if (map[i][j] == 0) {
        total_trailhead_sum += calc_trailhead(map, i, j);
      }
    }
  }
  return total_trailhead_sum;
}

int count_paths(int map[LENGTH][LENGTH], int r, int c, int memo[LENGTH][LENGTH]) {
  // If we are at a 9, this is a valid complete trail.
  if (map[r][c] == 9) {
      return 1;
  }
  // Return cached result if already computed.
  if (memo[r][c] != -1) {
      return memo[r][c];
  }
  
  int total = 0;
  int dr[4] = {-1, 1, 0, 0};
  int dc[4] = {0, 0, -1, 1};
  int next_val = map[r][c] + 1;
  
  // Try all four cardinal directions.
  for (int d = 0; d < 4; d++) {
      int nr = r + dr[d];
      int nc = c + dc[d];
      if (nr >= 0 && nr < LENGTH && nc >= 0 && nc < LENGTH) {
          if (map[nr][nc] == next_val) {
              total += count_paths(map, nr, nc, memo);
          }
      }
  }
  
  memo[r][c] = total;
  return total;
}

int calc_trailhead_rating_sum(int map[LENGTH][LENGTH]) {
  int memo[LENGTH][LENGTH];
  for (int i = 0; i < LENGTH; i++) {
      for (int j = 0; j < LENGTH; j++) {
          memo[i][j] = -1;
      }
  }
  
  int total_rating_sum = 0;
  for (int i = 0; i < LENGTH; i++) {
      for (int j = 0; j < LENGTH; j++) {
          if (map[i][j] == 0) {
              total_rating_sum += count_paths(map, i, j, memo);
          }
      }
  }
  return total_rating_sum;
}
int main() {
  int map[LENGTH][LENGTH];
  if (read_file("dayten.txt", map) == EXIT_FAILURE) {
    perror("Error reading file");
    return EXIT_FAILURE;
  }

  int trailhead_sum = calc_trailhead_sum(map);
  int trailhead_rating_sum = calc_trailhead_rating_sum(map);
  printf("%d\n", trailhead_sum);
  printf("%d\n", trailhead_rating_sum);
  return 0;
}