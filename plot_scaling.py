#!/usr/local/bin/python3

import sys
import math
import argparse
import matplotlib
import matplotlib.pyplot as plt
import numpy as np

# class Fib:
#   def __init__(self):
#     pass

#   def __iter__(self):
#     return self

#   def __next__(self):
    

def tree(base):
  val = 0
  for n in range(1, base+1):
    term = 1.0
    for i in range(1, n+1):
      term *= (base - i)
    term /= math.factorial(n - 1)
    val += term
  return val
    

if __name__ == "__main__":
  parser = argparse.ArgumentParser()
  # parser.add_argument('--out', help='Output filename base, no extension.', default='baseline_plot')
  # parser.add_argument('--dir', help='Baseline experiment dir.', default='.')
  args = parser.parse_args()

  xs = np.arange(2, 30)
  ys_naive = [math.factorial(x) for x in xs]
  ys_tree = [tree(x) for x in xs]

  for element in zip(xs, ys_tree):
    print(element)

  fig, ax = plt.subplots()
  ax.semilogy(xs, ys_naive, label='all permutations')
  ax.semilogy(xs, ys_tree, label='tree search')
  ax.grid()
  ax.set_ylim(bottom=0, top=1e11)
  ax.set_ylabel("Num tests to run")
  ax.set_xlabel("Base")
  ax.legend(loc='lower right')
  plt.show()
  
