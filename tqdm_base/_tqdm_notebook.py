from warnings import warn

from .notebook import *  # NOQA
from .notebook import __all__  # NOQA
from .std import TqdmDeprecationWarning

warn("This function will be removed in tqdm_base==5.0.0\n"
     "Please use `tqdm_base.notebook.*` instead of `tqdm_base._tqdm_notebook.*`",
     TqdmDeprecationWarning, stacklevel=2)
