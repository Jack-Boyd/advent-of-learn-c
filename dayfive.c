#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

int order_keys[1176], order_values[1176];

int read_file(const char *filename, int ***array, int **col_counts, int *rows) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("File load error");
    return EXIT_FAILURE;
  }
  
  char line[MAX_LINE];
  int i = 0;

  while (fgets(line, sizeof(line), file)) {
    if (!strchr(line, '|') || line[0] == '\n')
      break;
    if (sscanf(line, "%d|%d", &order_keys[i], &order_values[i]) != 2)
      fprintf(stderr, "Failed to parse line: %s", line);
    i++;
  }
  
  *rows = 0;
  *col_counts = NULL;
  
  long matrix_start = ftell(file);
  
  do {
    if (line[0] == '\n' || strlen(line) <= 1)
      continue;
    
    int cols = 0;
    char *line_copy = strdup(line);
    if (!line_copy) {
      perror("Memory allocation failed");
      fclose(file);
      free(*col_counts);
      return EXIT_FAILURE;
    }
    char *token = strtok(line_copy, ",");
    while (token) {
      cols++;
      token = strtok(NULL, ",");
    }
    free(line_copy);
    
    int *temp = realloc(*col_counts, ((*rows) + 1) * sizeof(int));
    if (!temp) {
      perror("Memory allocation failed");
      free(*col_counts);
      fclose(file);
      return EXIT_FAILURE;
    }
    *col_counts = temp;
    (*col_counts)[*rows] = cols;
    (*rows)++;
  } while (fgets(line, sizeof(line), file));
  
  *array = malloc((*rows) * sizeof(int *));
  if (!*array) {
    perror("Memory allocation failed");
    free(*col_counts);
    fclose(file);
    return EXIT_FAILURE;
  }
  for (i = 0; i < *rows; i++) {
    (*array)[i] = malloc((*col_counts)[i] * sizeof(int));
    if (!(*array)[i]) {
      perror("Memory allocation failed");
      for (int j = 0; j < i; j++)
        free((*array)[j]);
      free(*array);
      free(*col_counts);
      fclose(file);
      return EXIT_FAILURE;
    }
  }
  
  fseek(file, matrix_start, SEEK_SET);
  while (ftell(file) >= 0 && fgets(line, sizeof(line), file)) {
    if (strchr(line, ',') != NULL) {
      fseek(file, -strlen(line), SEEK_CUR);
      break;
    }
  }
  
  for (i = 0; i < *rows; i++) {
    for (int j = 0; j < (*col_counts)[i]; j++) {
      if (j < (*col_counts)[i] - 1) {
        if (fscanf(file, " %d,", &(*array)[i][j]) != 1) {
          fprintf(stderr, "Error reading number at row %d, col %d\n", i, j);
          goto cleanup;
        }
      } else {
        if (fscanf(file, " %d", &(*array)[i][j]) != 1) {
          fprintf(stderr, "Error reading number at row %d, col %d\n", i, j);
          goto cleanup;
        }
      }
    }
  }
    
  fclose(file);
  return EXIT_SUCCESS;
    
  cleanup:
    for (int k = 0; k < *rows; k++)
        free((*array)[k]);
    free(*array);
    free(*col_counts);
    fclose(file);
    return EXIT_FAILURE;
}

int is_correct_order(int first, int second) {
  for (int i = 0; i < 1176; i++) {
    if (order_keys[i] == second && order_values[i] == first)
      return 0;
  }
  return 1;
}

int is_update_correct(int *update, int col_size) {
  for (int i = 0; i < col_size; i++) {
    for (int j = i + 1; j < col_size; j++) {
      if (!is_correct_order(update[i], update[j]))
        return 0;
    }
  }
  return 1;
}

int find_correct_updates(int **array, int *col_counts, int rows, int **correct_rows) {
  *correct_rows = malloc(rows * sizeof(int));
  if (!*correct_rows) {
    perror("Memory allocation failed");
    return EXIT_FAILURE;
  }
  for (int i = 0; i < rows; i++)
    (*correct_rows)[i] = is_update_correct(array[i], col_counts[i]);
  
  return EXIT_SUCCESS;
}


void fix_update(int *update, int size) {
  int **graph = malloc(size * sizeof(int*));
  if (!graph) {
    perror("Memory allocation failed");
    return;
  }

  for (int i = 0; i < size; i++) {
    graph[i] = calloc(size, sizeof(int));
    if (!graph[i]) {
      perror("Memory allocation failed");
      for (int k = 0; k < i; k++)
        free(graph[k]);
      free(graph);
      return;
    }
  }

  int *in_degree = calloc(size, sizeof(int));
  if (!in_degree) {
    perror("Memory allocation failed");
    for (int i = 0; i < size; i++)
      free(graph[i]);
    free(graph);
    return;
  }

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (i == j)
        continue;
      for (int r = 0; r < 1176; r++) {
        if (order_keys[r] == update[i] && order_values[r] == update[j]) {
          if (!graph[i][j]) {
            graph[i][j] = 1;
            in_degree[j]++;
          }
          break;
        }
      }
    }
  }

  int *result = malloc(size * sizeof(int));
  if (!result) {
    perror("Memory allocation failed");
    free(in_degree);
    for (int i = 0; i < size; i++)
      free(graph[i]);
    free(graph);
    return;
  }

  int result_index = 0;

  int *queue = malloc(size * sizeof(int));
  if (!queue) {
    perror("Memory allocation failed");
    free(result);
    free(in_degree);
    for (int i = 0; i < size; i++)
      free(graph[i]);
    free(graph);
    return;
  }

  int q_start = 0, q_end = 0;

  for (int i = 0; i < size; i++) 
    if (in_degree[i] == 0)
      queue[q_end++] = i;
  
  while (q_start < q_end) {
    int v = queue[q_start++];
    result[result_index++] = update[v];
    for (int u = 0; u < size; u++) {
      if (graph[v][u]) {
        in_degree[u]--;
        if (in_degree[u] == 0) 
          queue[q_end++] = u;
      }
    }
  }

  if (result_index == size) {
    for (int i = 0; i < size; i++)
      update[i] = result[i];
  }
  
  free(result);
  free(queue);
  free(in_degree);
  for (int i = 0; i < size; i++)
    free(graph[i]);
  free(graph);
}

int main() {
  int **array = NULL, *col_counts = NULL, rows = 0;
    
  if (read_file("dayfive.txt", &array, &col_counts, &rows) == EXIT_FAILURE)
    return EXIT_FAILURE;

  int *correct_rows = NULL;
  if (find_correct_updates(array, col_counts, rows, &correct_rows) == EXIT_FAILURE) {
    for (int k = 0; k < rows; k++) 
      free(array[k]);
    free(array);
    free(col_counts);
    return EXIT_FAILURE;
  }
  
  int middle_sum = 0;
  int fixed_sum = 0;
  for (int i = 0; i < rows; i++) {
    if (correct_rows[i] == 1)
      middle_sum += array[i][(col_counts[i] - 1) / 2];
    else {
      fix_update(array[i], col_counts[i]);
      fixed_sum += array[i][(col_counts[i] - 1) / 2];
    }
  }

  printf("%d\n", middle_sum);
  printf("%d\n", fixed_sum);
  
  for (int k = 0; k < rows; k++)
    free(array[k]);
  free(array);
  free(col_counts);
  free(correct_rows);
  
  return EXIT_SUCCESS;
}
