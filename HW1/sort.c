#include "sort.h"

/* 정렬을 부르는 쪽은 이 표를 순회합니다. */
const SortAlgorithm SORT_ALGORITHMS[] = {
    {"insertion", "best O(n); average/worst O(n^2)", "O(1)", 1, insertionSort},
    {"merge", "O(n log n)", "O(n) + O(log n) stack", 1, mergeSort},
    {"quick", "expected O(n log n) for distinct keys; worst O(n^2)", "O(log n) call stack", 0, quickSort}
};
const size_t SORT_ALGORITHM_COUNT = sizeof SORT_ALGORITHMS / sizeof SORT_ALGORITHMS[0];
