from warnings import warn

from .cli import *  # NOQA
from .cli import __all__  # NOQA
from .std import TqdmDeprecationWarning

warn("This function will be removed in tqdm_base==5.0.0\n"
     "Please use `tqdm_base.cli.*` instead of `tqdm_base._main.*`",
     TqdmDeprecationWarning, stacklevel=2)
