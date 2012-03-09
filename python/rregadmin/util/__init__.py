from pkgutil import extend_path
__path__ = extend_path(__path__, __name__)

import rregadmin.util.init_wrapper

init_wrapper.rra_util_init()
