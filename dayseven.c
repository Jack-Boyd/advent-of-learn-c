#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024

typedef struct {
  int* numbers;
  int count;
  long answer;
} Equation;

int read_file(const char* filename, Equation** equations, int* rows) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error reading file");
    return EXIT_FAILURE;
  }

  Equation *eqs = NULL;
  int count = 0;
  char line[MAX_LINE_LENGTH];

  while(fgets(line, sizeof(line), file)) {
    char *separator = strchr(line, ':');
    if (separator == NULL) {
      fprintf(stderr, "Invalid format: missing ':' in line\n");
      for (int i = 0; i < count; i++)
        free(eqs[i].numbers);
      free(eqs);
      fclose(file);
      return EXIT_FAILURE;
    } 
    
    long answer;
    if (sscanf(line, "%ld:", &answer) != 1) {
      fprintf(stderr, "Error reading answer from line\n");
      for (int i = 0; i < count; i++)
        free(eqs[i].numbers);
      free(eqs);
      fclose(file);
      return EXIT_FAILURE;
    }
    
    separator++;
    while (*separator && isspace((unsigned char)*separator))
      separator++;


    char *line_copy = strdup(separator);
    if (!line_copy) {
      perror("Memory allocation failed");
      for (int i = 0; i < count; i++)
        free(eqs[i].numbers);
      free(eqs);
      fclose(file);
      return EXIT_FAILURE;
    }
    
    int num_tokens = 0;
    char *token = strtok(line_copy, " \n");
    while (token) {
      num_tokens++;
      token = strtok(NULL, " \n");
    }
    free(line_copy);

    int *numbers = malloc(num_tokens * sizeof(int));
    if (!numbers) {
      perror("Memory allocation failed");
      for (int i = 0; i < count; i++)
        free(eqs[i].numbers);
      free(eqs);
      fclose(file);
      return EXIT_FAILURE;
    }

    line_copy = strdup(separator);
    if (!line_copy) {
      perror("Memory allocation failed");
      free(numbers);
      for (int i = 0; i < count; i++)
        free(eqs[i].numbers);
      free(eqs);
      fclose(file);
      return EXIT_FAILURE;
    }
    int idx = 0;
    token = strtok(line_copy, " \n");
    while (token) {
      numbers[idx++] = atoi(token);
      token = strtok(NULL, " \n");
    }
    free(line_copy);

    Equation eq;
    eq.answer = answer;
    eq.count = num_tokens;
    eq.numbers = numbers;

    Equation *tmp = realloc(eqs, (count + 1) * sizeof(Equation));
    if (!tmp) {
      perror("Memory allocation failed");
      free(numbers);
      for (int i = 0; i < count; i++) 
        free(eqs[i].numbers);
      free(eqs);
      fclose(file);
      return EXIT_FAILURE;
    }
    eqs = tmp;
    eqs[count++] = eq;
  }
  
  fclose(file);
  *equations = eqs;
  *rows = count;
  return EXIT_SUCCESS;
}

int num_digits(int n) {
  if (n == 0) return 1;
  int count = 0;
  while (n) {
    count++;
    n /= 10;
  }
  return count;
}

long concatenate(long a, int b) {
  int digits = num_digits(b);
  long multiplier = 1;
  while (digits--) {
    multiplier *= 10;
  }
  return a * multiplier + b;
}

long calculate_total_calibration(Equation *equations, int rows) {
  long total_calibration = 0;
  int *operations = NULL;
  for (int i = 0; i < rows; i++) {
    //0 is + and 1 is *
    int operation_count = equations[i].count - 1;
    bool match = false;

    int *operations = (operation_count > 0) ? malloc(operation_count * sizeof(int)) : NULL;
    if (operation_count > 0 && !operations) {
      perror("Memory allocation failed");
      exit(EXIT_FAILURE);
    }

    for (int mask = 0; (mask < (1 << operation_count)) && !match; mask++) {
      for (int j = operation_count - 1; j >= 0; j--) 
        operations[j] = (mask >> j) & 1;
      
      long current_calibration = equations[i].numbers[0];
      for (int j = 0; j < operation_count; j++) {
        if (operations[j] == 0)
          current_calibration += equations[i].numbers[j + 1];
        else 
          current_calibration *= equations[i].numbers[j + 1];
      }

      if (current_calibration == equations[i].answer) {
        total_calibration += equations[i].answer;
        match = true;
      }
    }
    free(operations);
  }
  return total_calibration;
}

long calculate_total_calibration_p2(Equation *equations, int rows) {
  long total_calibration = 0;
  for (int i = 0; i < rows; i++) {
    int operation_count = equations[i].count - 1;
    bool match = false;
    
    int total_combinations = 1;
    for (int j = 0; j < operation_count; j++)
      total_combinations *= 3;

    for (int j = 0; j < total_combinations && !match; j++) {
      int temp = j;
      
      int *operations = (operation_count > 0) ? malloc(operation_count * sizeof(int)) : NULL;
      if (operation_count > 0 && !operations) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
      }

      for (int k = 0; k < operation_count; k++) {
        operations[k] = temp % 3;
        temp /= 3;
      }

      long current = equations[i].numbers[0];
      for (int j = 0; j < operation_count; j++) {
        int next = equations[i].numbers[j+1];
        if (operations[j] == 0) {
          current += next;
        } else if (operations[j] == 1) {
          current *= next;
        } else if (operations[j] == 2) {
          current = concatenate(current, next);
        }
      }
   
      free(operations);
      
      if (current == equations[i].answer) {
        total_calibration += equations[i].answer;
        match = true;
      }
    }
  }
  return total_calibration;
}
int main() {
  Equation *equations = NULL;
  int rows = 0;

  if (read_file("dayseven.txt", &equations, &rows) == EXIT_FAILURE)
    return EXIT_FAILURE;

  long total_calibration = calculate_total_calibration(equations, rows);
  printf("%ld\n", total_calibration);

  long total_calibration_p2 = calculate_total_calibration_p2(equations, rows);
  printf("%ld\n", total_calibration_p2);

  for (int i = 0; i < rows; i++) 
    free(equations[i].numbers);
  free(equations);
  
  return 0;
}