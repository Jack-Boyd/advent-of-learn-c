#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int read_file(const char *filename, int ***array, int **col_counts, int *rows) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Unable to open file");
    return EXIT_FAILURE;
  }

  *rows = 0;
  *col_counts = NULL;
  char line[MAX_LINE_LENGTH];

  while (fgets(line, sizeof(line), file) != NULL) {
    int cols = 0;
    char *line_copy = strdup(line);
    if (!line_copy) {
      perror("Memory allocation failed");
      fclose(file);
      return EXIT_FAILURE;
    }

    char *token = strtok(line_copy, " ");
    while (token) {
      cols++;
      token = strtok(NULL, " ");
    }
    free(line_copy);

    int *new_col_counts = realloc(*col_counts, (*rows + 1) * sizeof(int));
    if (!new_col_counts) {
      perror("Memory allocation failed");
      free(*col_counts);
      fclose(file);
      return EXIT_FAILURE;
    }

    *col_counts = new_col_counts;
    (*col_counts)[*rows] = cols;
    (*rows)++;
  }

  *array = malloc(*rows * sizeof(int *));
  if (!*array) {
    perror("Memory allocation failed");
    free(*col_counts);
    fclose(file);
    return EXIT_FAILURE;
  }

  for (int i = 0; i < *rows; i++) {
    (*array)[i] = malloc((*col_counts)[i] * sizeof(int));
    if (!(*array)[i]) {
      perror("Memory allocation failed");
      for (int j = 0; j < i; j++) free((*array)[j]);
      free(*array);
      free(*col_counts);
      fclose(file);
      return EXIT_FAILURE;
    }
  }

  rewind(file);
  for (int i = 0; i < *rows; i++) {
    for (int j = 0; j < (*col_counts)[i]; j++) {
      if (fscanf(file, "%d", &(*array)[i][j]) != 1) {
        fprintf(stderr, "Error reading number from file\n");
        fclose(file);
        return EXIT_FAILURE;
      }
    }
  }
  fclose(file);
  return EXIT_SUCCESS;
}

int is_safe(int *levels, int count) {
  if (count < 2) return 0;
  int increasing = (levels[1] > levels[0]);
  for (int i = 1; i < count; i++) {
    int diff = levels[i] - levels[i - 1];
    if (increasing) {
      if (diff <= 0 || diff > 3) return 0;
    } else {
      if (diff >= 0 || diff < -3) return 0;
    }
  }
  return 1;
}

int count_valid_reports(int **array, int *col_counts, int rows) {
  int valid_reports = 0;
  for (int i = 0; i < rows; i++) {
    int count = col_counts[i];
    if (count < 2) continue;
    
    if (is_safe(array[i], count)) {
      valid_reports++;
      continue;
    }
    
    int safe_with_removal = 0;
    for (int remove = 0; remove < count; remove++) {
      int valid = 1;
      int first_valid = 1;
      int determined = 0;
      int increasing = 0;
      int prev;
      
      for (int j = 0; j < count; j++) {
        if (j == remove)
          continue;
        int current = array[i][j];
        if (first_valid) {
          prev = current;
          first_valid = 0;
          continue;
        }
        if (!determined) {
          // Determine the report direction using the first two valid entries.
          increasing = (current > prev);
          determined = 1;
        }
        int diff = current - prev;
        if (increasing) {
          if (diff <= 0 || diff > 3) { valid = 0; break; }
        } else {
          if (diff >= 0 || diff < -3) { valid = 0; break; }
        }
        prev = current;
      }
      
      if (valid) {
        safe_with_removal = 1;
        break;
      }
    }
    
    if (safe_with_removal) valid_reports++;
  }
  return valid_reports;
}

int main() {
  int **array, *col_counts, rows;

  if (read_file("daytwo.txt", &array, &col_counts, &rows) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }
  
  // Part Two: Count reports that are safe either outright or by removing one bad level.
  int valid_reports = count_valid_reports(array, col_counts, rows);

  for (int i = 0; i < rows; i++) free(array[i]);
  free(array);
  free(col_counts);

  printf("%d\n", valid_reports);
  return EXIT_SUCCESS;
}
