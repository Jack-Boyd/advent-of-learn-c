#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 1000003
#define ITERATIONS 75

typedef struct Node {
  long key;
  long count;
  struct Node *next;
} Node;

Node** hash_table;

unsigned long hash(long key) {
  return (unsigned long)(key % TABLE_SIZE);
}

void insert(long key, long count) {
  unsigned long index = hash(key);
  Node* node = hash_table[index];

  while (node) {
    if (node->key == key) {
      node->count += count;
      return;
    }
    node = node->next;
  }

  Node* new_node = malloc(sizeof(Node));
  new_node->key = key;
  new_node->count = count;
  new_node->next = hash_table[index];
  hash_table[index] = new_node;
}

long get_total() {
  long total = 0;
  for (int i = 0; i < TABLE_SIZE; i++) {
    Node* node = hash_table[i];
    while (node) {
      total += node->count;
      node = node->next;
    }
  }
  return total;
}

void clear_table(Node** table) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    Node* node = table[i];
    while (node) {
      Node* next = node->next;
      free(node);
      node = next;
    }
    table[i] = NULL;
  }
}

int count_digits(long val) {
  if (val == 0) return 1;
  int digits = 0;
  while (val) {
    val /= 10;
    digits++;
  }
  return digits;
}

void split_number(long n, long* left, long* right) {
  char str[50];
  sprintf(str, "%ld", n);
  int len = strlen(str);
  int mid = len / 2;

  char left_str[25], right_str[25];
  strncpy(left_str, str, mid);
  left_str[mid] = '\0';
  strcpy(right_str, str + mid);

  *left = atol(left_str);
  *right = atol(right_str);
}

void process(Node** from_table, Node** to_table) {
  hash_table = to_table;  

  for (int i = 0; i < TABLE_SIZE; i++) {
    Node* node = from_table[i];
    while (node) {
      long val = node->key;
      long count = node->count;

      if (val == 0) {
        insert(1, count);
      } else {
        int digits = count_digits(val);
        if (digits % 2 == 0) {
          long l, r;
          split_number(val, &l, &r);
          insert(l, count);
          insert(r, count);
        } else {
          insert(val * 2024, count);
        }
      }

      node = node->next;
    }
  }
}

int main() {
  long initial[] = {28, 4, 3179, 96938, 0, 6617406, 490, 816207};
  int size = sizeof(initial) / sizeof(initial[0]);

  Node** table_a = calloc(TABLE_SIZE, sizeof(Node*));
  Node** table_b = calloc(TABLE_SIZE, sizeof(Node*));

  Node** current_table = table_a;
  Node** next_table = table_b;

  hash_table = current_table;
  for (int i = 0; i < size; i++) {
    insert(initial[i], 1);
  }

  for (int iter = 0; iter < ITERATIONS; iter++) {
    printf("Iteration %d\n", iter + 1);
    clear_table(next_table);
    process(current_table, next_table);

    Node** temp = current_table;
    current_table = next_table;
    next_table = temp;
  }

  hash_table = current_table;
  printf("Total stones after %d blinks: %ld\n", ITERATIONS, get_total());

  clear_table(current_table);
  clear_table(next_table);
  free(table_a);
  free(table_b);

  return 0;
}
