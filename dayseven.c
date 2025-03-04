#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024

int read_file(const char* filename, int*** equations, int** equation_col_counts, long** answers, int* rows) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error reading file");
    return EXIT_FAILURE;
  }

  *rows = 0;
  *equation_col_counts = NULL;
  char line[MAX_LINE_LENGTH];

  while(fgets(line, sizeof(line), file)) {
    int cols = 0;
    char* data = strchr(line, ':');
    if (data == NULL) {
      perror("Memory allocation failed");
      fclose(file);
      return EXIT_FAILURE;
    } 
    data++; 
    while (*data && isspace((unsigned char)*data)) {
      data++;
    }
    char *line_copy = strdup(data);
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

    int *new_col_counts = realloc(*equation_col_counts, (*rows + 1) * sizeof(int));
    if (!new_col_counts) {
      perror("Memory allocation failed");
      free(*equation_col_counts);
      fclose(file);
      return EXIT_FAILURE;
    }

    *equation_col_counts = new_col_counts;
    (*equation_col_counts)[*rows] = cols;
    (*rows)++;

  }

  *answers = malloc(*rows * sizeof(long));
  if (!*answers) {
    perror("Memory allocation failure");
    free(*equation_col_counts);
    fclose(file);
    return EXIT_FAILURE;
  }

  *equations = malloc(*rows * sizeof(int *));
  if (!*equations) {
    perror("Memory allocation failure");
    free(*answers);
    free(*equation_col_counts);
    fclose(file);
    return EXIT_FAILURE;
  }

  for (int i = 0; i < *rows; i++) {
    (*equations)[i] = malloc((*equation_col_counts)[i] * sizeof(int));
    if (!(*equations)[i]) {
      perror("Memory allocation failed");
      for (int j = 0; j < i; j++) 
        free((*equations)[j]);
      free(*equations);
      free(*answers);
      free(*equation_col_counts);
      fclose(file);
      return EXIT_FAILURE;
    }
  }

  rewind(file);
  for (int i = 0; i < *rows; i++) {
    if (fscanf(file, "%ld: ", &(*answers)[i]) != 1) {
      fprintf(stderr, "Error reading header integer in line %d\n", i);
      goto cleanup;
    }
    for (int j = 0; j < (*equation_col_counts)[i]; j++) {
      if (fscanf(file, "%d", &(*equations)[i][j]) != 1) {
        fprintf(stderr, "Error reading number from file\n");
        goto cleanup;
      }
    }
  }
  
  fclose(file);
  return EXIT_SUCCESS;

cleanup:
  for (int i = 0; i < (*equation_col_counts)[i]; i++) 
    free((*equations)[i]); 
  free(*equations);
  free(*answers);
  free(*equation_col_counts);
  fclose(file);
  return EXIT_FAILURE;
}

int calculate_total_calibration(int*** equations, int** equation_col_counts, long** answers, int rows) {
   //TODO
}

int main() {
  int **equations;
  int *equation_col_counts;
  long *answers;
  int rows;

  if (read_file("dayseven.txt", &equations, &equation_col_counts, &answers, &rows) == EXIT_FAILURE)
    return EXIT_FAILURE;

  int total_calibration = calculate_total_calibration(&equations, &equation_col_counts, &answers, rows);
  return 0;
}