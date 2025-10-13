// poor_locality.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

static inline double now_s(void) {
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char **argv) {
    const size_t N = (argc > 1) ? strtoull(argv[1], NULL, 10) : 4096; // default 4096
    int *a = NULL;
#if defined(_POSIX_C_SOURCE)
    if (posix_memalign((void**)&a, 64, N * N * sizeof(int)) != 0) return 1;
#else
    a = (int*)aligned_alloc(64, N * N * sizeof(int));
    if (!a) return 1;
#endif

    // initialize
    for (size_t i = 0; i < N * N; ++i) a[i] = (int)(i & 0xFF);

    volatile uint64_t sink = 0; // prevent dead-code elimination

    double t0 = now_s();
    // column-major traversal: stride of N → poor spatial locality
    for (size_t j = 0; j < N; ++j) {
        for (size_t i = 0; i < N; ++i) {
            sink += (uint64_t)a[i * N + j];
        }
    }
    double t1 = now_s();

    printf("N=%zu | sum=%llu | time=%.3f s (column-major, poor locality)\n",
           N, (unsigned long long)sink, t1 - t0);

    free((void*)a);
    return 0;
}
