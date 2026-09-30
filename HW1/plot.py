"""Per-algorithm panels emphasize changes within each sorting algorithm."""
import csv
from pathlib import Path
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
root = Path(__file__).resolve().parents[1]
with (root/'results/summary.csv').open() as f:
    rows = list(csv.DictReader(f))
for experiment in (1,2):
    for field, label in [('median_ms','Median time (ms)'),('mean_comparisons','Mean key comparisons'),('mean_array_writes','Mean array writes')]:
        fig, axes = plt.subplots(2,3,figsize=(13,7),layout='constrained')
        for ri,n in enumerate((1000,10000)):
            for ci,algo in enumerate(('insertion','merge','quick')):
                values = [r for r in rows if int(r['experiment'])==experiment and int(r['base_n'])==n and r['algorithm']==algo]
                values.sort(key=lambda r: -int(r['condition']) if experiment==1 else int(r['condition']))
                xlabels = [str(n//int(r['condition'])) for r in values] if experiment==1 else [f"{100*int(r['condition'])/n:g}%" for r in values]
                y = [float(r[field]) for r in values]
                ax = axes[ri,ci]
                ax.plot(range(4),y,'o-',color=('#2274a5','#348a5a','#ba4b44')[ci])
                if field=='median_ms':
                    ax.fill_between(range(4),[float(r['q1_ms']) for r in values],[float(r['q3_ms']) for r in values],alpha=.15)
                ax.set(xticks=range(4),xticklabels=xlabels,title=f'{algo} | base n = {n:,}',ylabel=label,xlabel='Occurrences per distinct key' if experiment==1 else 'Appended / base size')
                ax.set_ylim(bottom=0)
                ax.grid(alpha=.2)
        fig.suptitle(f'Experiment {experiment} — independent y-axis per panel'+(' | shaded: Q1–Q3' if field=='median_ms' else ''))
        for ext in ('svg','png'):
            fig.savefig(root/f'results/experiment{experiment}_{field}.{ext}',dpi=160)
        plt.close(fig)
print('Saved 6 charts in SVG and PNG formats')
