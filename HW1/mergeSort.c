#include "sortctx.h"
#include <stdio.h>
#include <stdlib.h>

/* 보조 배열을 한 번 할당하고 모든 재귀 구간에서 공유합니다. */
static void merge_range(Record *a, Record *tmp, size_t lo, size_t hi, SortStats *s) {
    if (hi - lo < 2) return;
    size_t mid = lo + (hi - lo) / 2;
    merge_range(a, tmp, lo, mid, s);
    merge_range(a, tmp, mid, hi, s);
    size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        /* 같을 때 왼쪽을 먼저 기록해야 안정 정렬이다. */
        if (greater(a[i], a[j], s)) put(&tmp[k++], a[j++], s);
        else put(&tmp[k++], a[i++], s);
    }
    while (i < mid) put(&tmp[k++], a[i++], s);
    while (j < hi) put(&tmp[k++], a[j++], s);
    for (k = lo; k < hi; k++) put(&a[k], tmp[k], s);
}
void mergeSort(Record *a, size_t n, SortStats *s) {
    if (n < 2) return;
    Record *tmp = malloc(n * sizeof *tmp);
    if (!tmp) { perror("malloc"); exit(EXIT_FAILURE); }
    merge_range(a, tmp, 0, n, s);
    free(tmp);
}

