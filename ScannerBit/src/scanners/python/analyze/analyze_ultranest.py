"""
Example of analyzing ultranest result
=====================================
"""

import sys

import pickle
import matplotlib.pyplot as plt

import ultranest


def analyze(pkl_name):

    sampler = ultranest.ReactiveNestedSampler(["dummy"], lambda x: 1)

    with open(pkl_name, "rb") as f:
        sampler.results = pickle.load(f)

    sampler.plot_corner()
    plt.savefig("ultranest_corner.pdf")


if __name__ == "__main__":
    analyze(sys.argv[1])
