# Copyright 2006 Racemi
# All rights reserved.

import sys
import os
import os.path
import random
import subprocess
from itertools import ifilter

sys.path = sys.path + [ os.getcwd(), os.path.join(os.getcwd(), "TEST", "lib") ]

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
    EF_VALUES_FILE = find_file_up("test/data/shared-values")

if os.environ.has_key('EF_KEYS_FILE'):
    EF_KEYS_FILE = os.environ['EF_KEYS_FILE']
else:
    EF_KEYS_FILE = find_file_up("test/data/shared-keys")

if os.environ.has_key('SCRIPT_DIR'):
    SCRIPT_DIR = os.environ['SCRIPT_DIR']
else:
    SCRIPT_DIR = "."


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
    return random.randrange(0, 100) < perc

def create_perc_reader(perc, filename):
    return ifilter(lambda x: perc_check(perc), iter(EfFile(filename)))

def add_test_for_each(test_func, class_ob, extra_test_name_part,
                      ef_file, example_dirs):
    for d in example_dirs:
        end_part = os.path.basename(d)
        def inner_test_func(self):
            test_dir = "test-data"
            p = subprocess.Popen([SCRIPT_DIR + "/copy_example_hives",
                                  d, test_dir])
            p.wait()
            p = None

            for i in create_perc_reader(1, ef_file):
                test_func(self, i, test_dir)

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

import unittest
class RraTestCase(unittest.TestCase):

    assertTrue = unittest.TestCase.assert_
    assertFalse = unittest.TestCase.failIf

    def assertTrueNum(self, val, msg=""):
        self.assert_(val > 0, msg)

    def assertFalseNum(self, val, msg=""):
        self.assertEqual(0, val, msg)


##############################################################################
# Facilities for running tests from the command line
##############################################################################

class RraTestProgram:
    """A command-line program that runs a set of tests; this is primarily
       for making test modules conveniently executable.
    """
    USAGE = """\
Usage: %(progName)s [options] [test] [...]

Options:
  -h, --help       Show this message
  -v, --verbose    Verbose output
  -q, --quiet      Minimal output

Examples:
  %(progName)s                               - run default set of tests
  %(progName)s MyTestSuite                   - run suite 'MyTestSuite'
  %(progName)s MyTestCase.testSomething      - run MyTestCase.testSomething
  %(progName)s MyTestCase                    - run all 'test*' test methods
                                               in MyTestCase
"""
    def __init__(self,
                 module='__main__',
                 defaultTest=None,
                 argv=None,
                 testRunner=None,
                 testLoader=unittest.defaultTestLoader):
        if type(module) == type(''):
            self.module = __import__(module)
            for part in module.split('.')[1:]:
                self.module = getattr(self.module, part)
        else:
            self.module = module

        if argv is None:
            argv = []
            argv = argv + sys.argv
            if os.environ.has_key('RRA_TEST_PROGRAM_OPTS'):
                env_opts = os.environ['RRA_TEST_PROGRAM_OPTS']
                argv = argv + env_opts.split(" ")

        self.verbosity = 1
        self.defaultTest = defaultTest
        self.testRunner = testRunner
        self.testLoader = testLoader
        self.progName = os.path.basename(argv[0])
        self.parseArgs(argv)
        self.runTests()

    def usageExit(self, msg=None):
        if msg: print msg
        print self.USAGE % self.__dict__
        sys.exit(2)

    def parseArgs(self, argv):
        import getopt
        try:
            options, args = getopt.getopt(argv[1:],
                                          'hHvq',
                                          ['help','verbose','quiet'])
            for opt, value in options:
                if opt in ('-h','-H','--help'):
                    self.usageExit()
                if opt in ('-q','--quiet'):
                    self.verbosity = 0
                if opt in ('-v','--verbose'):
                    self.verbosity = 2
            if len(args) == 0 and self.defaultTest is None:
                self.test = self.testLoader.loadTestsFromModule(self.module)
                return
            if len(args) > 0:
                self.testNames = args
            else:
                self.testNames = (self.defaultTest,)
            self.createTests()
        except getopt.error, msg:
            self.usageExit(msg)

    def createTests(self):
        self.test = self.testLoader.loadTestsFromNames(self.testNames,
                                                       self.module)

    def runTests(self):
        if self.testRunner is None:
            self.testRunner = unittest.TextTestRunner(verbosity=self.verbosity)
        result = self.testRunner.run(self.test)
        sys.exit(not result.wasSuccessful())

main = RraTestProgram

### Self test after here
class __MyTest(RraTestCase):
    pass

def __tMyself():

    def foo(self, i, test_dir):
        print self, test_dir, i

    add_key_example_tests(foo, __MyTest)
    add_value_example_tests(foo, __MyTest)

    main()

if __name__ == '__main__':
    __tMyself()
