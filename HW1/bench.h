#ifndef BENCH_H
#define BENCH_H
#include "sort.h"

typedef struct { int experiment; size_t baseN, parameter; } ExperimentSpec;
typedef struct {
    ExperimentSpec spec;
    size_t n, algorithm;
    uint32_t inputSeed;
    int repetition, stable;
    double timeMs;
    SortStats stats;
} Measurement;
typedef void (*RowSink)(const Measurement *, void *);
typedef struct {
    double medianMs, q1Ms, q3Ms, comparisons, writes, shifts;
    size_t observations, stableObservations;
} BenchResult;

void makeInput(Record *, ExperimentSpec, uint32_t seed);
size_t inputSize(ExperimentSpec);
int checkResult(const Record *input, const Record *output, size_t n, int *stable);
void benchRun(ExperimentSpec, int repetitions, RowSink, void *user, BenchResult *results);
#endif
