"""
Example of analyzing pocomc result
==================================
"""

import sys
import pickle
import matplotlib.pyplot as plt

import pocomc


def analyze(pkl_name):

    with open(pkl_name, "rb") as f:
        res = pickle.load(f)

    pocomc.plotting.corner(res)
    plt.savefig("pocomc_corner.pdf")


if __name__ == "__main__":
    analyze(sys.argv[1])
