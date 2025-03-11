#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h> 

#define LENGTH 19999

int read_file(const char* filename, int diskmap[LENGTH]) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return EXIT_FAILURE;
  }

  int i = 0;
  int c;
  while (i < LENGTH && (c = fgetc(file)) != EOF)
    if (isdigit(c)) 
      diskmap[i++] = c - '0';

  fclose(file);
  return EXIT_SUCCESS;
}

int* create_blocks(int diskmap[LENGTH], int full_size) {
  int* file_rep = malloc(full_size * sizeof(int));
  if (!file_rep) {
      perror("malloc failed");
      exit(EXIT_FAILURE);
  }
  
  int diskmap_idx = 0;
  int disk_val = diskmap[diskmap_idx];
  int file_id = 0;
  bool in_free_segment = false;  // false means we're in a file segment
  int pos = 0;
  
  while (pos < full_size) {
      if (disk_val == 0) {
          disk_val = diskmap[++diskmap_idx];
          // When leaving a file segment, increment the file id.
          if (!in_free_segment)
              file_id++;
          in_free_segment = !in_free_segment;
          continue;
      }
      
      if (!in_free_segment)
          file_rep[pos] = file_id;
      else
          file_rep[pos] = -1;
      
      disk_val--;
      pos++;
  }
  
  return file_rep;
}

long calculate_checksum(int diskmap[LENGTH]) {
  long checksum = 0;
  int full_size = 0;
  
  for (int i = 0; i < LENGTH; i++)
    full_size += diskmap[i];
  
  int* file_rep = create_blocks(diskmap, full_size);
  
  while (true) {
    int left = -1;
    for (int j = 0; j < full_size; j++) {
      if (file_rep[j] == -1) {
        left = j;
        break;
      }
    }
    if (left == -1)
      break;
    
    int right = -1;
    for (int j = full_size - 1; j >= 0; j--) {
      if (file_rep[j] != -1) {
        right = j;
        break;
      }
    }
    if (right <= left)
      break;
    
    file_rep[left] = file_rep[right];
    file_rep[right] = -1;
  }

  for (int i = 0; i < full_size; i++) {
    if (file_rep[i] == -1)
      break;
    checksum += (long)i * file_rep[i];
  }
  
  free(file_rep);
  return checksum;
}

long calculate_new_checksum(int diskmap[LENGTH]) {
  long checksum = 0;
  int full_size = 0;
  int n_files = 0;
  
  for (int i = 0; i < LENGTH; i++) {
    full_size += diskmap[i];
    if (i % 2 == 0)
      n_files++;
  }

  int* file_rep = create_blocks(diskmap, full_size);

  for (int file_id = n_files - 1; file_id >= 0; file_id--) {
    int seg_start = -1, seg_end = -1;
    for (int j = 0; j < full_size; j++) {
      if (file_rep[j] == file_id) {
        seg_start = j;
        int k = j;
        while (k < full_size && file_rep[k] == file_id)
          k++;
        seg_end = k - 1;
        break;
      }
    }
    if (seg_start == -1)
      continue;
      
    int seg_len = seg_end - seg_start + 1;
    
    int best_start = -1;
    int current_start = -1;
    int current_len = 0;
    for (int j = 0; j < seg_start; j++) {
      if (file_rep[j] == -1) {
        if (current_start == -1)
          current_start = j;
        current_len++;
      } else {
        if (current_len >= seg_len) {
          best_start = current_start;
          break;
        }
        current_start = -1;
        current_len = 0;
      }
    }
    if (best_start == -1 && current_len >= seg_len)
      best_start = current_start;
    
    if (best_start != -1) {
      for (int j = 0; j < seg_len; j++) {
        file_rep[best_start + j] = file_id;
        file_rep[seg_start + j] = -1;
      }
    }
  }

  for (int i = 0; i < full_size; i++) {
    if (file_rep[i] == -1)
      continue;
    checksum += (long)i * file_rep[i];
  }

  free(file_rep);
  return checksum;
}


int main() {
  int diskmap[LENGTH];

  if (read_file("daynine.txt", diskmap) == EXIT_FAILURE)
    return EXIT_FAILURE;

  long checksum = calculate_checksum(diskmap);
  long new_checksum = calculate_new_checksum(diskmap);

  printf("%ld\n", checksum);
  printf("%ld\n", new_checksum);
  return 0;
}
  