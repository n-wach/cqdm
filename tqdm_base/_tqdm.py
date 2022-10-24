from warnings import warn

from .std import *  # NOQA
from .std import __all__  # NOQA
from .std import TqdmDeprecationWarning

warn("This function will be removed in tqdm_base==5.0.0\n"
     "Please use `tqdm_base.std.*` instead of `tqdm_base._tqdm.*`",
     TqdmDeprecationWarning, stacklevel=2)
