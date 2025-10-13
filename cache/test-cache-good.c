#include <stdio.h>

#define M 4
#define N 5

int main() {
  int a[M][N];

  // Initialize each element with a sample value, e.g. row index + column index
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      a[i][j] = i + j;
    }
  }

  // Optional: print array to check initialization
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < N; j++) {
      printf("%d ", a[i][j]);
    }
    printf("\n");
  }

  return 0;
}
