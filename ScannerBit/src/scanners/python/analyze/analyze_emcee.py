"""
Example of analyzing emcee result
=================================
"""

import sys

import matplotlib.pyplot as plt

import emcee
import corner
import arviz as az


def analyze(filename):

    reader = emcee.backends.HDFBackend(filename)    
    samples = reader.get_chain(flat=True)

    corner.corner(samples)
    plt.savefig("emcee_corner.pdf")

    data = az.from_emcee(reader)
    print(az.summary(data))


if __name__ == "__main__":
    analyze(sys.argv[1])
