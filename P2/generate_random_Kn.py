#!/usr/bin/env python3
import argparse
import random

parser = argparse.ArgumentParser(
    prog='Kn generator',
    description='Generates complete graphs with random weights in DIMACS format',
    epilog='The edge weights are integers with at most specified absolute value. Among those integers, they are equally distributed.',
    formatter_class=argparse.ArgumentDefaultsHelpFormatter,
)
parser.add_argument('-o', '--output-file', help='File name to write the graph to', required=True)
parser.add_argument('-n', '--vertex-count', help='Number of vertices in the complete graph', type=int, required=True)
parser.add_argument('-w', '--max-weight', help='Maximum absolute weight of an edge', type=int, default=1000)
args = parser.parse_args()

num_vertices = args.vertex_count
if num_vertices < 3:
    print('Number of vertices must be at least 3')
    exit(1)

max_weight = args.max_weight
if max_weight < 0:
    print('Maximum edge weight cannot be negative')
    exit(1)
min_weight = -args.max_weight

with open(args.output_file, 'w') as f:
    print(f'p edge {num_vertices} {int(num_vertices * (num_vertices - 1) / 2)}', file=f)
    for lower_node in range(1, num_vertices):
        for upper_node in range(lower_node + 1, num_vertices + 1):
            weight = random.randrange(min_weight, max_weight + 1)
            print(f'e {lower_node} {upper_node} {weight}', file=f)
