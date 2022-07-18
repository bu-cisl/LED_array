import ctypes
import importlib
from .tisgrabber import declareFunctions

def loadLib():
    lib_path = importlib.resources.files('tisgrabber') / 'tisgrabber_x64.dll'
    ic = ctypes.cdll.LoadLibrary(str(lib_path))
    declareFunctions(ic)
    ic.IC_InitLibrary(0)
    return ic
