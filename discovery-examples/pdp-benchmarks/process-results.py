#!/usr/bin/python

import argparse
import os
from sets import Set
import csv
import sys

map_key = "maps"
reduction_key = "reductions"

parser = argparse.ArgumentParser(description='Aggregate pattern finder results '
                                 + 'into a CSV table.')
parser.add_argument('EXPERT_PATTERNS')
parser.add_argument('INPUT_DIR')
args = parser.parse_args()

# Map from benchmarks to loop entries.
data = {}
# Populate the map loading each solution file.
for filename in os.listdir(args.INPUT_DIR):
    if os.path.isfile(os.path.join(args.INPUT_DIR, filename)) and \
       filename.endswith(".szn"):
        [benchmark, loop_name, pattern, _] = filename.split(".")
        if not benchmark in data:
            data[benchmark] = {}
        loop = int(loop_name.split("loop")[1])
        if not loop in data[benchmark]:
            data[benchmark][loop] = (Set(), Set())
        # Assume the pattern is found unless the solution is unsatisfiable (we
        # assume no timeouts).
        matches = True
        with open(filename, "r") as szn:
            for line in szn:
                if "UNSATISFIABLE" in line:
                    matches = False
        if matches:
            data[benchmark][loop][0].add(pattern)

# Complete the map with expert results from the expert pattern data file.
r = csv.reader(open(args.EXPERT_PATTERNS), delimiter=",")
legend = r.next()
for [benchmark, loop_name, map_str, reduction_str] in r:
    loop = int(loop_name)
    if map_str == "1":
        data[benchmark][loop][1].add(map_key)
    if reduction_str == "1":
        data[benchmark][loop][1].add(reduction_key)

# Print the CSV table.
csvwriter = csv.writer(sys.stdout, delimiter=",", quoting=csv.QUOTE_MINIMAL)
csvwriter.writerow(["benchmark", "loop",
                    "map-tool", "map-expert",
                    "reduction-tool", "reduction-expert"])
for (benchmark, loop_data) in sorted(data.iteritems()):
    for (loop, pattern_data) in sorted(loop_data.iteritems()):
        csvwriter.writerow([benchmark, loop,
                            int(map_key in pattern_data[0]),
                            int(map_key in pattern_data[1]),
                            int(reduction_key in pattern_data[0]),
                            int(reduction_key in pattern_data[1])])
