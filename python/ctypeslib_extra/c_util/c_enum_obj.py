
import ctypes
import re
import exceptions
import inspect

class CEnumObjException(exceptions.StandardError):

    def __init__(self, msg, *args, **kwds):
        exceptions.Exception(self, msg, *args, **kwds)
        self.msg = msg
        return

    def __str__(self):
        return "%s: %s" % (self, self,msg)

class CEnumObj(object):
    """Helper wrapper to objectify C objects from ctypes.

    """

    _DEBUG_LEVEL = 0

    @classmethod
    def __make_regexp_list(cls, in_list):
        ret_val = []

        for pat in in_list:
            ret_val.append(re.compile(pat))

        return ret_val

    @classmethod
    def __in_reg_list(cls, sym, re_list):

        for pat in re_list:
            if pat.match(sym) is not None:
                return True
        return False

    @classmethod
    def __get_ob_info(cls, sym):
        # Get function from module
        return getattr(cls._ccu_module, sym)

    @classmethod
    def __add_symbol(cls, match_ob):

        ob_name = match_ob.groups()[0]
        setattr(cls, ob_name, cls.__get_ob_info(match_ob.group()))

        return True

    @classmethod
    def __find_type(cls, mod, object_name):

        # module must contain a class with name object_name
        if not hasattr(mod, object_name):
            raise CEnumObjException(
                "No %s contained in %s" % (object_name, mod))
        # and it can't be None
        ob_class = getattr(mod, object_name)
        if ob_class is None:
            raise CEnumObjException(
                "%s in %s is None"  % (object_name, mod))

        return ob_class

    @classmethod
    def __find_module(cls, module_name, object_name):
        try:
            mod = __import__(module_name, globals(), locals(), [object_name])
        except StandardError, e:
            raise CEnumObjException(e)

        if mod is None:
            raise CEnumObjException("No module %s found" % module_name)

        return mod

    @classmethod
    def initialize_from_module(
        cls,
        module_name=None,
        type_name=None,
        include_patterns=[],
        exclude_patterns=[]):

        if cls.__name__ == "CEnumObj":
            raise CEnumObjException("Cannot initialize base CEnumObj class")

        cls._ccu_module = cls.__find_module(module_name, type_name)
        cls.type = cls.__find_type(cls._ccu_module, type_name)
        cls._ccu_found_sym = False

        syms = dir(cls._ccu_module)

        cls._ccu_e_regs = cls.__make_regexp_list(exclude_patterns)
        cls._ccu_i_regs = cls.__make_regexp_list(include_patterns)

        c_syms = []
        d_syms = []
        for sym in syms:
            if cls._DEBUG_LEVEL > 2:
                print "------------------\n%s" % sym
            if not cls.__in_reg_list(sym, cls._ccu_e_regs):
                for pat in cls._ccu_i_regs:
                    match_ob = pat.match(sym)
                    if match_ob is not None:
                        if cls.__add_symbol(match_ob):
                            cls._ccu_found_sym = True

        if not cls._ccu_found_sym:
            cls._ccu_module = None
            raise CEnumObjException(
                "Could not find any functions in %s" % module_name)

        return True
