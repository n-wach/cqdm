from warnings import warn

from .gui import *  # NOQA
from .gui import __all__  # NOQA
from .std import TqdmDeprecationWarning

warn("This function will be removed in tqdm_base==5.0.0\n"
     "Please use `tqdm_base.gui.*` instead of `tqdm_base._tqdm_gui.*`",
     TqdmDeprecationWarning, stacklevel=2)
