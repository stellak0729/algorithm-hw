#include "sortctx.h"

/* 같은 값은 오른쪽에 두는 2방향 분할입니다. */
/* Random pivot + strict-less Lomuto partition. Equal keys go to the right.
   Recurse on the smaller side; loop on the larger side to bound call stack. */
static void quick_range(Record *a, size_t lo, size_t hi, SortStats *s, uint32_t *rng) {
    while (hi - lo > 1) {
        size_t chosen = lo + next_random(rng) % (hi - lo);
        if (chosen != hi - 1) swap(&a[chosen], &a[hi - 1], s);
        Record pivot = a[hi - 1];
        size_t boundary = lo;
        for (size_t j = lo; j < hi - 1; j++) {
            if (greater(pivot, a[j], s)) {
                if (boundary != j) swap(&a[boundary], &a[j], s);
                boundary++;
            }
        }
        if (boundary != hi - 1) swap(&a[boundary], &a[hi - 1], s);
        if (boundary - lo < hi - boundary - 1) {
            quick_range(a, lo, boundary, s, rng);
            lo = boundary + 1;
        } else {
            quick_range(a, boundary + 1, hi, s, rng);
            hi = boundary;
        }
    }
}
void quickSort(Record *a, size_t n, SortStats *s) {
    uint32_t rng = 0xA531C98Bu; /* identical pivot stream for repeated runs */
    quick_range(a, 0, n, s, &rng);
}
