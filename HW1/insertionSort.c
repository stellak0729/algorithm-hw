#include "sortctx.h"

/* 정렬된 앞부분에 다음 원소를 삽입합니다. 같은 값은 밀지 않습니다. */
void insertionSort(Record *a, size_t n, SortStats *s) {
    for (size_t i = 1; i < n; i++) {
        Record value = a[i];
        size_t j = i;
        while (j && greater(a[j - 1], value, s)) {
            put(&a[j], a[j - 1], s);
            s->shifts++;
            j--;
        }
        if (j != i) put(&a[j], value, s);
    }
}

