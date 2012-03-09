# Copyright 2006 Racemi
# All rights reserved.

try:
    import xml.etree.ElementTree as ET
except ImportError:
    import elementtree.ElementTree as ET

import sys
import os.path
import random
import subprocess
from itertools import ifilter

PERC_TO_RUN = 0.1

def find_file_up(in_filename):
    path = ""
    try_path = in_filename
    while True:
        if os.path.exists(try_path):
            return try_path
        if os.path.realpath(path) == "/":
            return None
        path = os.path.join(path, "..")
        try_path = os.path.join(path, in_filename)

if os.environ.has_key('REGISTRY_LIST_PROG'):
    REGISTRY_LIST_PROG = os.environ['REGISTRY_LIST_PROG']
else:
    REGISTRY_LIST_PROG = find_file_up("test/bin/ls_example_registries")


ef_fd = subprocess.Popen([REGISTRY_LIST_PROG], stdout=subprocess.PIPE).stdout

EXAMPLES_LIST = filter(lambda x: x != "",
                       map(lambda x: x.rstrip(), ef_fd.readlines()))
ef_fd.close()
ef_fd = None

if os.environ.has_key('EF_VALUES_FILE'):
    EF_VALUES_FILE = os.environ['EF_VALUES_FILE']
else:
    EF_VALUES_FILE = "../../test/data/shared-values"

if os.environ.has_key('EF_KEYS_FILE'):
    EF_KEYS_FILE = os.environ['EF_KEYS_FILE']
else:
    EF_KEYS_FILE = "../../test/data/shared-keys"

if os.environ.has_key('SCRIPT_LIB_DIR'):
    SCRIPT_LIB_DIR = os.environ['SCRIPT_LIB_DIR']
else:
    SCRIPT_LIB_DIR = "."

if os.environ.has_key('CL_PROG'):
    CL_PROG = os.environ['CL_PROG']
else:
    CL_PROG = "../rra_basic_cl"

if os.environ.has_key('DIFF_PROG'):
    DIFF_PROG = os.environ['DIFF_PROG']
else:
    DIFF_PROG = "../rra_registry_diff"

if os.environ.has_key('BUILD_TYPE'):
    BUILD_TYPE = os.environ['BUILD_TYPE']
else:
    BUILD_TYPE = "none"

if os.environ.has_key('VALGRIND_CHECK'):
    VALGRIND_CHECK = True
else:
    VALGRIND_CHECK = False

DEBUG_TEST = True

def ef_debug_out(*args):
    if DEBUG_TEST:
        for i in args:
            print i,
        print

class EfFile(object):

    def __init__(self, filename):
        self.et = ET.parse(filename)
        return

    class my_iter(object):

        def __init__(self, ef_file):
            self.ef = ef_file
            self.my_iter = iter(ef_file.et.getiterator("value"))
            return

        def __iter__(self):
            return self

        def next(self):
            da_next = self.my_iter.next()
            return (da_next.findtext("path"),
                    da_next.findtext("name"))

    def __iter__(self):
        return self.my_iter(self)


def perc_check(perc):
    return (random.random() * 100) < perc

def create_perc_reader(perc, filename):
    return ifilter(lambda x: perc_check(perc), iter(EfFile(filename)))

import uuid
def move_to_unique_file(in_filename, in_pattern):
    new_name = in_pattern % str(uuid.uuid4())
    p = subprocess.Popen(["mv", in_filename, new_name])
    p.wait()
    return

def add_test_for_each(test_func, class_ob, extra_test_name_part,
                      ef_file, example_dirs):
    for d in example_dirs:
        end_part = os.path.basename(d)
        def inner_test_func(self):
            test_dir = "test-data"
            if os.path.isdir(d):
                p = subprocess.Popen([SCRIPT_LIB_DIR + "/copy_example_hives",
                                      d, test_dir])
                p.wait()
                p = None

                for i in create_perc_reader(PERC_TO_RUN, ef_file):
                    test_func(self, i, test_dir, orig_dir=d)
                    if BUILD_TYPE == "profile" and os.path.exists("gmon.out"):
                        move_to_unique_file("gmon.out", "gmon_%s.out")

                sys.stdout.flush()
                sys.stderr.flush()

                p = subprocess.Popen(["rm", "-rf", test_dir])
                p.wait()
                p = None

            return

        class_ob.__class__.__setattr__(class_ob,
                                       'test_' + extra_test_name_part
                                       + '_' + end_part,
                                       inner_test_func)
    return

def add_key_example_tests(test_func, class_ob):
    add_test_for_each(test_func, class_ob, "key",
                      EF_KEYS_FILE, EXAMPLES_LIST)

def add_value_example_tests(test_func, class_ob):
    add_test_for_each(test_func, class_ob, "value",
                      EF_VALUES_FILE, EXAMPLES_LIST)

def list_or_tuple(x):
    return isinstance(x, (list, tuple))

def flatten(l, to_expand=list_or_tuple):
    for item in l:
        if to_expand(item):
            for subitem in flatten(item, to_expand):
                yield subitem
        else:
            yield item

import unittest
class EfTest(unittest.TestCase):

    def _get_prog_data(self, *args):
        args1 = []
        if VALGRIND_CHECK:
            args1.extend(["libtool", "--mode=execute", "valgrind", "-q", "--tool=callgrind"])
        args1.extend([ x for x in flatten(args) ])
        p = subprocess.Popen(args1, stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT)
        lines = p.stdout.readlines()
        p.wait()
        return (p.returncode, lines)

    def get_prog_out(self, *args):
        pd = self._get_prog_data(*args)
        return "".join(pd[1])

    def is_good_prog(self, *args):
        pd = self._get_prog_data(*args)
        return pd[0] == 0

    def assertGoodProg(self, *args):
        pd = self._get_prog_data(*args)
        self.assertEqual(0, pd[0],
                         msg = str(args) + ' Failed:\n' + "".join(pd[1]))

    def assertBadProg(self, *args):
        pd = self._get_prog_data(*args)
        self.assertNotEqual(0, pd[0],
                            msg = str(args) + ' Failed:\n' + "".join(pd[1]))

### Self test after here
class __MyTest(EfTest):
    pass

def __tMyself():

    def foo(self, i, test_dir):
        print self, test_dir, i

    add_key_example_tests(foo, __MyTest)
    add_value_example_tests(foo, __MyTest)

    unittest.main()

if __name__ == '__main__':
    __tMyself()
