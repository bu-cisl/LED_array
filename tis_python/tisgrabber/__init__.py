from .tisgrabber import *
from .loader import loadLib
from enum import IntEnum
SinkFormats = IntEnum('SinkFormats', [(e.name, e.value) for e in SinkFormats])
FRAMEFILTER_PARAM_TYPE = IntEnum('FRAMEFILTER_PARAM_TYPE', [(e.name, e.value) for e in FRAMEFILTER_PARAM_TYPE])
