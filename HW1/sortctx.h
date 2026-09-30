#ifndef SORTCTX_H
#define SORTCTX_H
#include "sort.h"

/* 정렬 구현끼리만 공유하는 비교·기록·교환 도구. */
static inline int greater(Record a, Record b, SortStats *s) {
    s->comparisons++;
    return a.key > b.key;
}
static inline void put(Record *dst, Record value, SortStats *s) {
    *dst = value;
    s->writes++; /* 배열(보조 배열 포함)의 원소 기록 1회 */
}
static inline void swap(Record *a, Record *b, SortStats *s) {
    Record t = *a;
    put(a, *b, s);
    put(b, t, s);
}


static inline uint32_t next_random(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13; x ^= x >> 17; x ^= x << 5;
    return *state = x;
}


#endif
