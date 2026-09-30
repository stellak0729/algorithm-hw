#define _POSIX_C_SOURCE 200809L
#include "bench.h"
#include "sortctx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void *allocate(size_t bytes) {
    void *p = calloc(bytes ? bytes : 1, 1);
    if (!p) { perror("calloc"); exit(EXIT_FAILURE); }
    return p;
}
size_t inputSize(ExperimentSpec s) {
    return s.baseN + (s.experiment == 2 ? s.parameter : 0);
}
void makeInput(Record *a, ExperimentSpec s, uint32_t seed) {
    size_t n = inputSize(s);
    if (s.experiment == 1) {
        for (size_t i=0; i<n; i++) a[i]=(Record){(int)(i/(n/s.parameter)),0};
        for (size_t i=n; i>1; i--) {
            size_t j=next_random(&seed)%i;
            Record t=a[i-1]; a[i-1]=a[j]; a[j]=t;
        }
    } else {
        for (size_t i=0; i<s.baseN; i++) a[i]=(Record){(int)i,0};
        for (size_t i=s.baseN; i<n; i++)
            a[i]=(Record){(int)(((uint64_t)next_random(&seed)*s.baseN)>>32),0};
    }
    for (size_t i=0; i<n; i++) a[i].tag=(int)i;
}
/* tag is the original input position, so this also detects lost/duplicated records. */
int checkResult(const Record *input, const Record *a, size_t n, int *stable) {
    unsigned char *seen=allocate(n);
    int ok=1; *stable=1;
    for (size_t i=0; i<n; i++) {
        int tag=a[i].tag;
        if (tag<0 || (size_t)tag>=n || seen[tag] || a[i].key!=input[tag].key) {
            ok=0; break;
        }
        seen[tag]=1;
        if (i && a[i-1].key>a[i].key) ok=0;
        if (i && a[i-1].key==a[i].key && a[i-1].tag>tag) *stable=0;
    }
    free(seen); return ok;
}
static double nowMs(void) {
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC,&t)) {perror("clock_gettime"); exit(1);}
    return (double)t.tv_sec*1000+t.tv_nsec/1000000.0;
}
static int compareDouble(const void *a,const void *b) {
    double x=*(const double *)a,y=*(const double *)b;
    return (x>y)-(x<y);
}
static double percentile(const double *v,size_t n,double p) {
    double pos=(n-1)*p; size_t lo=(size_t)pos;
    return lo+1<n ? v[lo]+(v[lo+1]-v[lo])*(pos-lo) : v[lo];
}
void benchRun(ExperimentSpec spec,int reps,RowSink sink,void *user,BenchResult *results) {
    const uint32_t seeds[]={0x12345678u,0x9e3779b9u,123456789u,987654321u,20260930u};
    size_t n=inputSize(spec),count=5*(size_t)reps,ac=SORT_ALGORITHM_COUNT;
    Record *input=allocate(n*sizeof *input),*out=allocate(n*sizeof *out);
    double *times=allocate(ac*count*sizeof *times);
    memset(results,0,ac*sizeof *results);
    for (size_t si=0;si<5;si++) {
        makeInput(input,spec,seeds[si]);
        for (size_t a=0;a<ac;a++) {
            memcpy(out,input,n*sizeof *out); SortStats stats={0};
            SORT_ALGORITHMS[a].sort(out,n,&stats);
        }
        for (int rep=0;rep<reps;rep++) for (size_t turn=0;turn<ac;turn++) {
            size_t a=(turn+si+(size_t)rep)%ac;
            memcpy(out,input,n*sizeof *out);
            Measurement m={.spec=spec,.n=n,.algorithm=a,.inputSeed=seeds[si],.repetition=rep};
            double start=nowMs();
            SORT_ALGORITHMS[a].sort(out,n,&m.stats);
            m.timeMs=nowMs()-start;
            if (!checkResult(input,out,n,&m.stable) || (SORT_ALGORITHMS[a].stable&&!m.stable)) {
                fprintf(stderr,"Invalid sort: %s\n",SORT_ALGORITHMS[a].name); exit(2);
            }
            BenchResult *r=&results[a];
            times[a*count+r->observations++]=m.timeMs;
            r->stableObservations+=(size_t)m.stable;
            r->comparisons+=(double)m.stats.comparisons;
            r->writes+=(double)m.stats.writes; r->shifts+=(double)m.stats.shifts;
            if (sink) sink(&m,user);
        }
    }
    for (size_t a=0;a<ac;a++) {
        double *v=times+a*count; BenchResult *r=&results[a];
        qsort(v,count,sizeof *v,compareDouble);
        r->medianMs=percentile(v,count,.5);r->q1Ms=percentile(v,count,.25);r->q3Ms=percentile(v,count,.75);
        r->comparisons/=count;r->writes/=count;r->shifts/=count;
    }
    free(input);free(out);free(times);
}
