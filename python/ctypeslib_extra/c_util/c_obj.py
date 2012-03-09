
import ctypes
import re
import exceptions
import inspect

class CObjException(exceptions.StandardError):

    def __init__(self, msg, *args, **kwds):
        exceptions.Exception(self, msg, *args, **kwds)
        self.msg = msg
        return

    def __str__(self):
        return "%s: %s" % (self, self.msg)

class CObj(object):
    """Helper wrapper to objectify C objects from ctypes.

    """

    _DEBUG_LEVEL = 0

    def __init__(self, *args, **kwds):
        super(CObj, self).__init__(args, kwds)
        if hasattr(self, "_ccu_init"):
            self._ccu_init(*args)
        return

    def _convert(self, arg, typ):
        return arg

    def _rev_convert(self, arg, typ):
        return arg

    def _check_arg_types(self, in_args, arg_types):
        in_len = len(in_args)
        in_typ = len(arg_types)

        if in_len > in_typ:
            raise CObjException("Too many arguments: expected %s got %s" % (
                in_typ, in_len))
        elif in_len < in_typ:
            raise CObjException("Too few arguments: expected %s got %s" % (
                in_typ, in_len))

        da_args = []
        for i in xrange(0, in_len):
            da_args.append(self._convert(in_args[i], arg_types[i]))

        return da_args

    def c_ob(self):
        if type(self._ccu_ob) == type(()):
            return self._ccu_ob[0]
        else:
            return self._ccu_ob

    def _set_c_ob(self, ob):
        self._ccu_ob = ob

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
    def __get_func_info(cls, sym):
        # Get function from module
        func = getattr(cls._ccu_module, sym)
        if func is None:
            return None

        # Function must have this attribute even though it may be None
        if not hasattr(func, "restype"):
            return None
        res = getattr(func, "restype")

        # Function must have this attribute with at least one argument
        if not hasattr(func, "argtypes"):
            return None
        f_args = getattr(func, "argtypes")
        if (len(f_args) < 1
            and type(res) is not type(ctypes.POINTER(cls._ccu_ob_class))):
            return None

        return (func, res, f_args)

    _SIMPLE_FUNC = 1
    _CONSTRUCTOR_FUNC = 2
    _DESTRUCTOR_FUNC = 3

    @classmethod
    def __add_symbol(cls, match_ob, f_type=_SIMPLE_FUNC):

        if cls._DEBUG_LEVEL > 2:
            print "Adding"

        finfo = cls.__get_func_info(match_ob.group())
        if finfo is None:
            if cls._DEBUG_LEVEL > 1:
                print "finfo is None"
            return False

        if f_type == cls._CONSTRUCTOR_FUNC:
            def _tmp_func_(self, *args):
                da_args = self._check_arg_types(args, finfo[2])

                return finfo[0](*da_args),
        else:
            if finfo[2][0] != ctypes.POINTER(cls._ccu_ob_class):
                if cls._DEBUG_LEVEL > 0:
                    print "First argument required to be pointer to class"
                return False

            def _tmp_func_(self, *args):
                da_args = self._check_arg_types(args, finfo[2][1:])

                return self._rev_convert(finfo[0](self.c_ob(), *da_args),
                                         finfo[1])

        if f_type == cls._SIMPLE_FUNC:
            func_name = match_ob.groups()[0]
        elif f_type == cls._CONSTRUCTOR_FUNC:
            func_name = '_ccu_constructor_' + match_ob.groups()[0]
        elif f_type == cls._DESTRUCTOR_FUNC:
            func_name = '_ccu_destructor_' + match_ob.groups()[0]

        if cls._DEBUG_LEVEL > 0:
            print "Setting func %s in %s to %s" % (
                func_name, cls, _tmp_func_)

        setattr(cls, func_name, _tmp_func_)

        return True

    @classmethod
    def __add_constructor(cls, c_syms):

        if hasattr(cls, "_ccu_init"):
            if cls._DEBUG_LEVEL > 1:
                print "Class already has constructor. Skipping"
            return False

        if len(c_syms) == 0:
            if cls._DEBUG_LEVEL > 1:
                print "No constructor funcs found"
            return False
        elif len(c_syms) == 1:
            if cls._DEBUG_LEVEL > 0:
                print "Constructor func: %s" % c_syms[0]

            finfo = cls.__get_func_info(c_syms[0])
            if finfo is None:
                return False

            def _tmp_ccu_init(self, *args):
                da_args = self._check_arg_types(args, finfo[2])
                self._set_c_ob(finfo[0](*da_args))
                return

            setattr(cls, "_ccu_init", _tmp_ccu_init)

            return True
        else:
            if cls._DEBUG_LEVEL > 2:
                print "Multiple constructor funcs: %s" % c_syms
            raise CObjException(
                "No _ccu_init defined and multiple constructor candidates: %s"
                % c_syms)

        return False

    @classmethod
    def __add_destructor(cls, d_syms):

        if hasattr(cls, "__del__"):
            if cls._DEBUG_LEVEL > 1:
                print "Class already has destructor. Skipping"
            return False

        if len(d_syms) == 0:
            if cls._DEBUG_LEVEL > 1:
                print "No destructor funcs found"
            return False
        elif len(d_syms) == 1:
            if cls._DEBUG_LEVEL > 0:
                print "Destructor func: %s" % d_syms[0]

            finfo = cls.__get_func_info(d_syms[0])
            if finfo is None:
                return False

            if len(finfo[2]) > 1:
                raise CObjException(
                    "Destructor func needs arguments.  Can't wrap.")

            if finfo[2][0] != ctypes.POINTER(cls._ccu_ob_class):
                raise CObjException(
                    "Destructor's first arg needs to be a pointer to class")
            def _tmp_del(self):
                if hasattr(self, '_ccu_ob'):
                    if cls._DEBUG_LEVEL > 1:
                        print "Deleted object type: %s" % type(self.c_ob())
                    finfo[0](self.c_ob())
                    self._set_c_ob(None)
                else:
                    print "No _ccu_ob in self"
                return

            setattr(cls, "__del__", _tmp_del)
            return True
        else:
            if cls._DEBUG_LEVEL > 1:
                print "Multiple destructor funcs: %s" % d_syms
            raise CObjException(
                "No __del__ defined and multiple destructor candidates: %s"
                % d_syms)

        return False

    @classmethod
    def __find_struct(cls, mod, object_name):

        # module must contain a class with name object_name
        if not hasattr(mod, object_name):
            raise CObjException(
                "No %s contained in %s" % (object_name, mod))
        # and it can't be None
        ob_class = getattr(mod, object_name)
        if ob_class is None:
            raise CObjException(
                "%s in %s is None"  % (object_name, mod))

        # and it must be a class derived from ctypes.Structure
        if not inspect.isclass(ob_class):
            raise CObjException(
                "%s from %s is not a class" % (object_name, mod))
        if not issubclass(ob_class, ctypes.Structure):
            raise CObjException(
                "%s from %s is not derived from ctypes.Structure" % (
                object_name, mod))

        return ob_class

    @classmethod
    def __find_module(cls, module_name, object_name):
        try:
            mod = __import__(module_name, globals(), locals(), [object_name])
        except StandardError, e:
            raise CObjException(e)

        if mod is None:
            raise CObjException("No module %s found" % module_name)

        return mod

    @classmethod
    def initialize_from_module(
        cls,
        module_name=None,
        object_name=None,
        include_patterns=[],
        exclude_patterns=[],
        constructor_patterns=[ "new.*", "create.*", "alloc.*" ],
        destructor_patterns=[ "destroy.*", "free.*",
                              "dealloc.*", "unalloc.*" ]):

        if cls.__name__ == "CObj":
            raise CObjException("Cannot initialize base CObj class")

        cls._ccu_module = cls.__find_module(module_name, object_name)
        cls._ccu_ob_class = cls.__find_struct(cls._ccu_module, object_name)
        cls._ccu_found_sym = False

        syms = dir(cls._ccu_module)

        cls._ccu_e_regs = cls.__make_regexp_list(exclude_patterns)
        cls._ccu_i_regs = cls.__make_regexp_list(include_patterns)
        cls._ccu_c_regs = cls.__make_regexp_list(constructor_patterns)
        cls._ccu_d_regs = cls.__make_regexp_list(destructor_patterns)

        c_syms = []
        d_syms = []
        for sym in syms:
            if cls._DEBUG_LEVEL > 2:
                print "------------------\n%s" % sym
            if not cls.__in_reg_list(sym, cls._ccu_e_regs):
                for pat in cls._ccu_i_regs:
                    match_ob = pat.match(sym)
                    if match_ob is not None:
                        if cls.__in_reg_list(match_ob.groups()[0],
                                             cls._ccu_c_regs):
                            c_syms.append(sym)
                            if cls.__add_symbol(match_ob,
                                                cls._CONSTRUCTOR_FUNC):
                                cls._ccu_found_sym = True

                        elif cls.__in_reg_list(match_ob.groups()[0],
                                               cls._ccu_d_regs):
                            d_syms.append(sym)
                            if cls.__add_symbol(match_ob,
                                                cls._DESTRUCTOR_FUNC):
                                cls._ccu_found_sym = True

                        elif cls.__add_symbol(match_ob):
                            cls._ccu_found_sym = True

        if cls.__add_constructor(c_syms):
            cls._ccu_found_sym = True
        if cls.__add_destructor(d_syms):
            cls._ccu_found_sym = True

        if not cls._ccu_found_sym:
            cls._ccu_module = None
            raise CObjException(
                "Could not find any functions in %s" % module_name)

        return True
