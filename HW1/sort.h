#ifndef SORT_H
#define SORT_H
#include <stddef.h>
#include <stdint.h>

/* key만 비교합니다. tag는 입력에서의 순서를 검증하기 위한 값입니다. */
typedef struct { int key, tag; } Record;
typedef struct { uint64_t comparisons, writes, shifts; } SortStats;
typedef void (*SortFunction)(Record *base, size_t n, SortStats *stats);
typedef struct {
    const char *name;
    const char *timeComplexity;
    const char *spaceComplexity;
    int stable;
    SortFunction sort;
} SortAlgorithm;

/* n > 0이면 base가 유효해야 하며 stats는 항상 유효한 포인터여야 합니다.
   호출자는 측정 시작 전에 *stats를 0으로 초기화합니다. */
void insertionSort(Record *, size_t, SortStats *);
void mergeSort(Record *, size_t, SortStats *);
void quickSort(Record *, size_t, SortStats *);
extern const SortAlgorithm SORT_ALGORITHMS[];
extern const size_t SORT_ALGORITHM_COUNT;
#endif
