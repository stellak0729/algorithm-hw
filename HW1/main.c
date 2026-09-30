#include "bench.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
static void csvRow(const Measurement *m,void *user) {
    (void)user;
    printf("%d,%zu,%zu,%zu,%" PRIu32 ",%s,%d,%.9f,%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%d\n",
        m->spec.experiment,m->spec.baseN,m->spec.parameter,m->n,m->inputSeed,
        SORT_ALGORITHMS[m->algorithm].name,m->repetition,m->timeMs,
        m->stats.comparisons,m->stats.writes,m->stats.shifts,m->stable);
}
static int usage(const char *name) {
    fprintf(stderr,"Usage: %s [--csv] [--experiment 1|2|all] [--size 1000|10000|all] [--reps 1..1000]\n",name);
    return 1;
}
int main(int argc,char **argv) {
    int csv=0,experiment=0,reps=31; size_t size=0;
    for (int i=1;i<argc;i++) {
        if (!strcmp(argv[i],"--csv")) {csv=1;continue;}
        const char *option=argv[i]; if (++i>=argc) return usage(argv[0]);
        const char *v=argv[i];
        if (!strcmp(option,"--experiment")) {
            if (!strcmp(v,"all")) experiment=0;
            else if (!strcmp(v,"1")||!strcmp(v,"2")) experiment=atoi(v);
            else return usage(argv[0]);
        } else if (!strcmp(option,"--size")) {
            if (!strcmp(v,"all")) size=0;
            else if (!strcmp(v,"1000")||!strcmp(v,"10000")) size=(size_t)atoi(v);
            else return usage(argv[0]);
        } else if (!strcmp(option,"--reps")) {
            char *end; long x=strtol(v,&end,10);
            if (!*v||*end||x<1||x>1000) return usage(argv[0]);
            reps=(int)x;
        } else return usage(argv[0]);
    }
    if (csv) puts("experiment,base_n,condition,n,input_seed,algorithm,rep,time_ms,comparisons,array_writes,shifts,stable");
    else puts("exp base_n condition algorithm median_ms Q1_ms Q3_ms comparisons writes shifts");
    const size_t sizes[]={1000,10000},repeats[]={500,100,10,1};
    BenchResult *results=calloc(SORT_ALGORITHM_COUNT,sizeof *results);
    if (!results) return 1;
    for (int e=1;e<=2;e++) for (size_t si=0;si<2;si++) {
        size_t n=sizes[si]; if ((experiment&&e!=experiment)||(size&&n!=size)) continue;
        const size_t adds[]={0,n/1000,n/100,n/10};
        for (size_t c=0;c<4;c++) {
            ExperimentSpec spec={e,n,e==1?n/repeats[c]:adds[c]};
            benchRun(spec,reps,csv?csvRow:NULL,NULL,results);
            if (!csv) for (size_t a=0;a<SORT_ALGORITHM_COUNT;a++) {
                BenchResult r=results[a];
                printf("%d %zu %zu %-9s %.6f %.6f %.6f %.1f %.1f %.1f\n",e,n,spec.parameter,
                    SORT_ALGORITHMS[a].name,r.medianMs,r.q1Ms,r.q3Ms,r.comparisons,r.writes,r.shifts);
            }
        }
    }
    free(results); return 0;
}
