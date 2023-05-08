"""
Example of analyzing dynesty result
===================================
"""

import sys
import pickle
import matplotlib.pyplot as plt

from dynesty import plotting as dyplot
import corner
import arviz as az


def analyze(pkl_name):

    with open(pkl_name, "rb") as f:
        res = pickle.load(f)

    dyplot.runplot(res) 
    plt.savefig("dynesty_runplot.pdf")

    dyplot.traceplot(res)
    plt.savefig("dynesty_traceplot.pdf")

    dyplot.cornerpoints(res)
    plt.savefig("dynesty_corner.pdf")


if __name__ == "__main__":
    analyze(sys.argv[1])
