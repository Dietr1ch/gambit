from .copydoc import copydoc
from .version import *
from .mpi import *
import os

def get_filename(filename, path, default_output_path="./", **kwargs):
    defpath = default_output_path + "/" + path + "/"
    if not os.path.exists(defpath):
        os.makedirs(defpath)
        
    return defpath + filename
