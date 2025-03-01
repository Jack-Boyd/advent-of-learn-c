#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_INPUT 18000

int read_file(const char* filename, char* input) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
      perror("Unable to open file");
      return -1;
  }
  
  size_t bytesRead = fread(input, 1, MAX_INPUT - 1, file);
  input[bytesRead] = '\0';
  fclose(file);
  return 0;
}


int main() {
  char input[MAX_INPUT] = {0};
  int sum_of_mul = 0;
  
  if (read_file("daythree.txt", input) == 0) {
    char *pos = input;

    char *do_pos = strstr(pos, "do()");
    char *dont_pos = strstr(pos, "don't()");
    bool perform = true;

    ptrdiff_t do_diff;
    ptrdiff_t dont_diff;

    while ((pos = strstr(pos, "mul(")) != NULL) {
      do_diff = (do_pos == NULL) ? -1 : pos - do_pos; 
      dont_diff = (dont_pos == NULL) ? -1 : pos - dont_pos;
      if (do_diff > 0 || dont_diff > 0) {
        if (do_diff > 0 && dont_diff > 0)
          perform = (do_diff > dont_diff);
        else if (do_diff > 0)
          perform = true;
        else if (dont_diff > 0)
          perform = false;
        
        if (do_diff > 0)
          do_pos = strstr(pos, "do()");
        if (dont_diff > 0)
          dont_pos = strstr(pos, "don't()");
      }
      
      char *p = pos + 4;
      
      if (!perform) {
        pos = p + 1;
        continue;
      }
      
      char num1[4] = {0};
      char num2[4] = {0};
      int i = 0;
            
      while (i < 3 && isdigit(*p)) {
        num1[i++] = *p++;
      }
      if (i < 1 || isdigit(*p) || *p != ',') {
        pos += 4;
        continue;
      }
      p++; 
      
      int j = 0;
      while (j < 3 && isdigit(*p)) {
        num2[j++] = *p++;
      }
      if (j < 1 || isdigit(*p) || *p != ')') {
        pos += 4;
        continue;
      }
      
      int a = atoi(num1);
      int b = atoi(num2);
      sum_of_mul += (a * b);
      pos = p + 1;
    }
  }
  printf("%d\n", sum_of_mul);
  return 0;
}
