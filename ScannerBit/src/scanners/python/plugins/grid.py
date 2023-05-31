"""
Grid scannner written in Python
===============================
"""

import numpy as np

from .scanner import Scanner


class Grid(Scanner):
    """
    Simple grid scanner. Evaluates points on a user-defined grid.
    """

    name = "python_grid"
    version = "gambit internal"

    def __init__(self, grid_points=10):
        """
        Parameters:

        :param: grid_points (10)
        """
        if isinstance(grid_points, int):
            grid_points = [grid_points] * self.dim

        if len(grid_points) != self.dim:
            raise RuntimeError(
                f"grid dimension {len(grid_points)} does not match dim {self.dim}")

        # make grid
        self.grid = [np.linspace(0., 1., g) for g in grid_points]
        self.grid_size = np.prod(grid_points)

    def run(self):
        # scan the grid of points, divided evenly across the numper of MPI
        # processes
        for p in np.vstack(np.meshgrid(*
                                       self.grid)).reshape(self.dim, -
                                                           1).T[self.mpi_rank:self.grid_size:self.mpi_size]:
            self.loglike_hypercube(p)
