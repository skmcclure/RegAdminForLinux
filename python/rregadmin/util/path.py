
import ctypes
from ctypeslib_extra.c_util import c_obj
import rregadmin.util.path_wrapper
import rregadmin.util.path_info_wrapper

pathw = rregadmin.util.path_wrapper
pathi = rregadmin.util.path_info_wrapper

class Path (c_obj.CObj):

    # _DEBUG_LEVEL = 3

    class Mode:
        IN_WIN_DELIMITER = pathi.RRA_PATH_OPT_IN_WIN_DELIMITER
        CASE_INSENSITIVE = pathi.RRA_PATH_OPT_CASE_INSENSITIVE
        WIN = pathi.RRA_PATH_OPT_WIN
        DEFAULT = pathi.RRA_PATH_OPT_MIX

    def _convert(self, arg, typ):
        if (typ == ctypes.POINTER(pathw.rra_path)
            and type(arg) == Path):
            return arg.c_ob()
        else:
            return arg

    def _rev_convert(self, arg, typ):
        if typ == ctypes.POINTER(pathw.rra_path):
            return Path(arg)
        else:
            return arg

    def _ccu_init(self, *args):
        if len(args) == 0:
            # This should cause an exception to be raised
            self._set_c_ob(self._ccu_constructor_new())

        elif len(args) == 1:
            if type(args[0]) == ctypes.POINTER(pathw.rra_path):
                self._set_c_ob(args[0])
            else:
                self._set_c_ob(self._ccu_constructor_new(args[0]))

        elif len(args) == 2:
            self._set_c_ob(self._ccu_constructor_new_full(*args))

        else:
            # this should cause an exception to be raised
            self._set_c_ob(self._ccu_constructor_new_full(*args))

        return

Path.initialize_from_module(
    module_name="rregadmin.util.path_wrapper",
    object_name="rra_path",
    include_patterns=["rra_path_(.+)"])
