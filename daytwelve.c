#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define ROWS 140
#define COLS 140

typedef struct {
  int r, c;
} Point;

typedef struct {
  Point *points;
  int size;
  int capacity;
} Region;

typedef struct {
  Region *regions;
  int size;
  int capacity;
} RegionArray;

char **read_input(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    perror("fopen");
    return NULL;
  }
  char **grid = NULL;
  int capacity = 0;
  int count = 0;
  char *line = NULL;
  size_t len = 0;

  while(getline(&line, &len, fp) != -1) {
    size_t l = strlen(line);
    if(l > 0 && line[l-1] == '\n')
      line[l-1] = '\0';
    if (count >= capacity) {
      capacity = (capacity == 0) ? 8 : capacity * 2;
      grid = realloc(grid, capacity * sizeof(char *));
    }
    grid[count++] = strdup(line);
  }

  free(line);
  fclose(fp);

  return grid;
}

void free_input(char **grid) {
  for (int i = 0; i < ROWS; i++) {
    free(grid[i]);
  }
  free(grid);
}

void add_point(Region *region, Point p) {
  if (region->size >= region->capacity) {
    region->capacity = (region->capacity == 0) ? 16 : region->capacity * 2;
    region->points = realloc(region->points, region->capacity * sizeof(Point));
  }
  region->points[region->size++] = p;
}

void push_point(Point **stack, int *stackSize, int *stackCapacity, Point p) {
  if (*stackSize >= *stackCapacity) {
    *stackCapacity = (*stackCapacity == 0) ? 16 : (*stackCapacity * 2);
    *stack = realloc(*stack, (*stackCapacity) * sizeof(Point));
  }
  (*stack)[(*stackSize)++] = p;
}

void add_region(RegionArray *rarr, Region reg) {
  if (rarr->size >= rarr->capacity) {
    rarr->capacity = (rarr->capacity == 0) ? 16 : rarr->capacity * 2;
    rarr->regions = realloc(rarr->regions, rarr->capacity * sizeof(Region));
  }
  rarr->regions[rarr->size++] = reg;
}

bool in_bounds(int r, int c) {
  return r >= 0 && r < ROWS && c >= 0 && c < COLS;
}

Region flood_fill(char **grid, bool *visited, int startR, int startC) {
  Region region;
  region.size = 0;
  region.capacity = 16;
  region.points = malloc(region.capacity * sizeof(Point));
  char cell_value = grid[startR][startC];

  Point *stack = NULL;
  int stackSize = 0, stackCapacity = 0;
  push_point(&stack, &stackSize, &stackCapacity, (Point){startR, startC});

  while (stackSize > 0) {
    Point cur = stack[--stackSize];
    int idx = cur.r * COLS + cur.c;
    if (visited[idx])
      continue;
    visited[idx] = true;
    add_point(&region, cur);

    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};
    for (int i = 0; i < 4; i++) {
      int nr = cur.r + dr[i];
      int nc = cur.c + dc[i];
      if (in_bounds(nr, nc)) {
        if (!visited[nr * COLS + nc] && grid[nr][nc] == cell_value) {
          push_point(&stack, &stackSize, &stackCapacity, (Point){nr, nc});
        }
      }
    }
  }
  free(stack);
  return region;
}


void compute_region_costs(char **grid, Region reg, long long *perimeter_price, long long *side_price) {
  int region_size = reg.size;
  int perimeter = 0;
  int num_corners = 0;

  bool *mask = calloc(ROWS * COLS, sizeof(bool));
  for (int i = 0; i < reg.size; i++) {
    int r = reg.points[i].r, c = reg.points[i].c;
    mask[r * COLS + c] = true;
  }

  for (int i = 0; i < reg.size; i++) {
    int r = reg.points[i].r, c = reg.points[i].c;
    char cell_value = grid[r][c];

    int count = 0;
    int dr[4] = {-1, 1, 0, 0};
    int dc[4] = {0, 0, -1, 1};
    for (int j = 0; j < 4; j++) {
      int nr = r + dr[j], nc = c + dc[j];
      if (in_bounds(nr, nc) && grid[nr][nc] == cell_value)
        count++;
    }
    perimeter += (4 - count);

    int diag_offsets[2] = {1, -1};
    for (int di = 0; di < 2; di++) {
      for (int dj = 0; dj < 2; dj++) {
        int drd = diag_offsets[di], dcd = diag_offsets[dj];
        int rn_r = r + drd, rn_c = c;
        int cn_r = r, cn_c = c + dcd;
        int diag_r = r + drd, diag_c = c + dcd;

        bool rn_in = in_bounds(rn_r, rn_c) && mask[rn_r * COLS + rn_c];
        bool cn_in = in_bounds(cn_r, cn_c) && mask[cn_r * COLS + cn_c];
        bool diag_in = in_bounds(diag_r, diag_c) && mask[diag_r * COLS + diag_c];

        if (!rn_in && !cn_in)
          num_corners++;
        if (rn_in && cn_in && !diag_in)
          num_corners++;
      }
    }
  }
  *perimeter_price += (long long)perimeter * region_size;
  *side_price += (long long)num_corners * region_size;
  free(mask);
}

int main(void) {
  char **grid = read_input("daytwelve.txt");
  if (!grid) {
    fprintf(stderr, "Error reading input.\n");
    return 1;
  }

  bool *visited = calloc(ROWS * COLS, sizeof(bool));
  if (!visited) {
    perror("calloc");
    free_input(grid);
    return 1;
  }

  RegionArray rarr;
  rarr.size = 0;
  rarr.capacity = 16;
  rarr.regions = malloc(rarr.capacity * sizeof(Region));

  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      if (visited[r * COLS + c])
        continue;
      Region reg = flood_fill(grid, visited, r, c);
      add_region(&rarr, reg);
    }
  }
  free(visited);

  long long perimeter_price = 0;
  long long side_price = 0;
  for (int i = 0; i < rarr.size; i++) {
    compute_region_costs(grid, rarr.regions[i], &perimeter_price, &side_price);
    free(rarr.regions[i].points);
  }
  free(rarr.regions);
  free_input(grid);

  printf("answer for part 1: %lld\n", perimeter_price);
  printf("answer for part 2: %lld\n", side_price);

  return 0;
}