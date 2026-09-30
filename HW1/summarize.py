"""Summarize raw measurements; uses only the Python standard library."""
import csv
import statistics as st
from collections import defaultdict
from pathlib import Path
root = Path(__file__).resolve().parents[1]
groups = defaultdict(list)
with (root / 'results/raw.csv').open() as f:
    for row in csv.DictReader(f):
        groups[tuple(row[k] for k in ('experiment', 'base_n', 'condition', 'algorithm'))].append(row)
with (root / 'results/summary.csv').open('w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['experiment','base_n','condition','algorithm','median_ms','q1_ms','q3_ms','observations','mean_comparisons','mean_array_writes','mean_shifts'])
    for key, rows in groups.items():
        times = [float(r['time_ms']) for r in rows]
        quartiles = st.quantiles(times, n=4, method='inclusive')
        writer.writerow([*key,st.median(times),quartiles[0],quartiles[2],len(times),*[st.mean(int(r[k]) for r in rows) for k in ('comparisons','array_writes','shifts')]])
print('Saved results/summary.csv')
