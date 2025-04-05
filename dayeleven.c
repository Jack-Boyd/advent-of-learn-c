#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DIGITS 20
#define MAX_STONES 1000000
#define ITERATIONS 25

long count_digits(long n) {
  if (n == 0)
    return 1;
  long digits = 0;
  while (n) {
    n /= 10;
    digits++;
  }
  return digits;
}

void split_number(long n, long *left, long *right) {
  char buffer[MAX_DIGITS];
  sprintf(buffer, "%ld", n);
  long len = strlen(buffer);
  long mid = len / 2;

  char left_str[MAX_DIGITS], right_str[MAX_DIGITS];
  strncpy(left_str, buffer, mid);
  left_str[mid] = '\0';
  strcpy(right_str, buffer + mid);

  *left = atoi(left_str);
  *right = atoi(right_str);
}

int main() {
  long initial[] = {28, 4, 3179, 96938, 0, 6617406, 490, 816207};
  long size = sizeof(initial) / sizeof(initial[0]);

  long *current = malloc(MAX_STONES * sizeof(long));
  long *next = malloc(MAX_STONES * sizeof(long));
  long current_size = size;

  for (long i = 0; i < size; i++) {
    current[i] = initial[i];
  }

  for (long iter = 0; iter < ITERATIONS; iter++) {
    long next_size = 0;

    for (long i = 0; i < current_size; i++) {
      long val = current[i];

      if (val == 0) {
        next[next_size++] = 1;
      }
      else {
        long digits = count_digits(val);
        if (digits % 2 == 0) {
          long left, right;
          split_number(val, &left, &right);
          next[next_size++] = left;
          next[next_size++] = right;
        }
        else {
          next[next_size++] = val * 2024;
        }
      }
    }

    long *temp = current;
    current = next;
    next = temp;
    
    current_size = next_size;
  }

  printf("Total stones after 25 blinks: %ld\n", current_size);

  free(current);
  free(next);

  return 0;
}
