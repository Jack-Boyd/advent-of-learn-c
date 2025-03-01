#include <stdio.h>
#include <stdlib.h>

#define MAX_LINES 1001

int keys[MAX_LINES];
int values[MAX_LINES];

int compare(const void* a, const void* b) {
  return *(int*)a - *(int*)b;
}

int calc_total_distance(int* list1, int* list2, int size) {
  int total_distance = 0;
  for (int i = 0; i < size; i++) {
    total_distance += abs(list1[i] - list2[i]);
  }
  return total_distance;
}

int read_file(const char* filename, int* list1, int* list2) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Unable to open file");
    return -1;
  }

  int lines = 0;
  while (fscanf(file, "%d %d", &list1[lines], &list2[lines]) == 2) {
    lines++;
    if (lines == MAX_LINES) break;
  }

  fclose(file);
  return lines;
}

int get_index_of_key(int key, int size) { 
  for (int i = 0; i < size; i++) { 
    if (keys[i] == key) { 
      return i; 
    } 
  } 
  return -1;
} 

int calc_similarity_score(int* list1, int* list2, int size) {
  for (int i = 0; i < size; i++) {
    keys[i] = list1[i];
    values[i] = 0;
  }

  for (int i = 0; i < size; i++) {
    int index = get_index_of_key(list2[i], size);
    if (index != -1) {
      values[index]++;
    }
  }

  int similarity_score = 0;
  for (int i = 0; i < size; i++) {
    similarity_score += keys[i] * values[i];
  }

  return similarity_score;
}

int main() {
  int list1[MAX_LINES], list2[MAX_LINES];
  int size = read_file("dayone.txt", list1, list2);
  if (size == -1) return EXIT_FAILURE;

  qsort(list1, size, sizeof(list1[0]), compare);
  qsort(list2, size, sizeof(list2[0]), compare);

  // int total_distance = calc_total_distance(list1, list2, size);
  // printf("%d\n", total_distance);

  int similarity_score = calc_similarity_score(list1, list2, size);
  printf("%d\n", similarity_score);
  
  return 0;
}