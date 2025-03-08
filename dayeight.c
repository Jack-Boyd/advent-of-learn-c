#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 50
#define COLS 50

typedef struct {
  int row;
  int col;
} Location;

int read_file(const char *filename, char map[ROWS][COLS + 1]) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
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

char* collect_frequencies(char map[ROWS][COLS + 1]) {
  char *frequencies = malloc(1);
  if (!frequencies) {
    perror("Memory allocation failed");
    return NULL;
  }
  frequencies[0] = '\0';  

  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      char ch = map[i][j];
      if (ch != '.' && strchr(frequencies, ch) == NULL) {
        size_t len = strlen(frequencies);
        char *temp = realloc(frequencies, len + 2);
        if (!temp) {
          perror("Memory allocation failed");
          free(frequencies);
          return NULL;
        }
        frequencies = temp;
        frequencies[len] = ch;
        frequencies[len + 1] = '\0';
      }
    }
  }
  return frequencies;
}

int calculate_signal_impact(char map[ROWS][COLS + 1], char *frequencies) {
  int signal_impact = 0;
  size_t freq_count = strlen(frequencies);

  int antinode_map[ROWS][COLS];
  memset(antinode_map, 0, sizeof(antinode_map));

  for (size_t i = 0; i < freq_count; i++) {
    char antenna = frequencies[i];

    Location* antennae_locations = NULL;
    int location_count = 0;
    for (int j = 0; j < ROWS; j++) {
      for (int k = 0; k < COLS; k++) {
        if (map[j][k] == antenna) {
          Location antennae_location;
          antennae_location.row = j;
          antennae_location.col = k;
          Location* tmp = realloc(antennae_locations, (location_count + 1) * sizeof(Location));
          if (!tmp) {
            perror("Memory allocation failed");
            free(antennae_locations);
            return -1;
          }
          antennae_locations = tmp;
          antennae_locations[location_count++] = antennae_location;
        }
      }
    }
    if (location_count >= 2) {
      for (int j = 0; j < location_count; j++) {
        int r = antennae_locations[j].row;
        int c = antennae_locations[j].col;
        if (antinode_map[r][c] == 0) {
          antinode_map[r][c] = 1;
          signal_impact++;
        }
      }
    } 
    for (int j = 0; j < location_count - 1; j++) {
      for (int k = j + 1; k < location_count; k++) {
        Location l1 = antennae_locations[j];
        Location l2 = antennae_locations[k];

        int row_diff = l2.row - l1.row;
        int col_diff = l2.col - l1.col;

        int antinode_row_1 = l1.row - row_diff;
        int antinode_col_1 = l1.col - col_diff;

        while (antinode_row_1 >= 0 && antinode_row_1 < ROWS 
          && antinode_col_1 >= 0 && antinode_col_1 < COLS) {
          if (antinode_map[antinode_row_1][antinode_col_1] == 0) {
            antinode_map[antinode_row_1][antinode_col_1] = 1; 
            signal_impact++;
          }
          antinode_row_1 -= row_diff;
          antinode_col_1 -= col_diff;
        }
        
        int antinode_row_2 = l2.row + row_diff;
        int antinode_col_2 = l2.col + col_diff;        
        
        while (antinode_row_2 >= 0 && antinode_row_2 < ROWS 
          && antinode_col_2 >= 0 && antinode_col_2 < COLS) {
          if (antinode_map[antinode_row_2][antinode_col_2] == 0) {
            antinode_map[antinode_row_2][antinode_col_2] = 1; 
            signal_impact++;
          }

          antinode_row_2 += row_diff;
          antinode_col_2 += col_diff;
        }
      }
    }
  }

  return signal_impact;
}

int main() {
  char map[ROWS][COLS+1];
  if (read_file("dayeight.txt", map) == EXIT_FAILURE)
    return EXIT_FAILURE;

  char *frequencies = collect_frequencies(map);
  if (!frequencies) {
    perror("No frequencies detected");
    return EXIT_FAILURE;
  }
  printf("%s\n", frequencies);

  int signal_impact = calculate_signal_impact(map, frequencies);
  if (signal_impact == -1)
    return EXIT_FAILURE;

  printf("%d\n", signal_impact);

  free(frequencies);

  return 0;
}