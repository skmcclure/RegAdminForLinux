
from ctypes import *
STRING = c_char_p

T1 = None

def t1_new():
    return T1()
t1_new.restype = POINTER(T1)
t1_new.argtypes = []

def t1_free(in_ob):
    pass
t1_free.restype = c_int
t1_free.argtypes = [ POINTER(T1) ]

def t1_func1(in_ob, in_num):
    pass
t1_func1.restype = c_int
t1_func1.argtypes = [ POINTER(T1), c_int ]

def t1_func2(in_ob, in_string):
    pass
t1_func2.restype = c_int
t1_func2.argtypes = [ POINTER(T1), STRING ]

def foo_func(in_ob):
    pass
foo_func.restype = c_int
foo_func.argtypes = [ POINTER(T1) ]

def bar_func(in_ob):
    pass
