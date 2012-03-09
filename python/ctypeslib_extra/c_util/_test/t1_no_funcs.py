
from ctypes import *
STRING = c_char_p

class T1(Structure):
    pass
T1._fields_ = []

def foo_func(in_ob):
    pass
foo_func.restype = c_int
foo_func.argtypes = [ POINTER(T1) ]

def bar_func(in_ob):
    pass
