/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2007 Racemi Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <vector>
#include <iostream>

#include <melunit/melunit-cxx.h>

#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>

#include <rregadmin/util/fs.h>
#include <rregadmin/util/init.h>

bool DEBUG_TEST = false;

namespace
{
    using namespace boost::assign;

    class test_fs : public Melunit::Test
    {
    private:

        typedef std::vector<std::string> str_vec;
        struct ffci_test
        {
            std::string test_path;
            std::string expected;
            bool good;

            ffci_test(std::string in_test_path,
                      std::string in_expected,
                      bool in_good)
                : test_path(in_test_path),
                  expected(in_expected),
                  good(in_good)
            {
            }
        };


        typedef std::vector<ffci_test> ffci_test_vec;

        struct ffci_data
        {
            ffci_test_vec tests; // [0] = search, [1] = expected
            str_vec path_commands;

            ffci_data(
                str_vec in_path_commands=str_vec(),
                ffci_test_vec in_tests=ffci_test_vec())
                : tests(in_tests),
                  path_commands(in_path_commands)
            {
            }

            bool run_commands()
            {
                for (str_vec::iterator i = path_commands.begin();
                     i != path_commands.end();
                     i++)
                {
                    if (DEBUG_TEST)
                    {
                        std::cout << "Running " << *i << std::endl;
                    }

                    system(i->c_str());
                }

                if (DEBUG_TEST)
                {
                    system("ls -lR");
                }

                return true;
            }

        };

        void check_one_path(const char *base_func_name,
                            bool is_good,
                            ustring *test_path,
                            ustring *expected)
        {
            if (DEBUG_TEST)
            {
                std::cout << base_func_name
                          << ": Checking " << ustr_as_utf8(test_path)
                          << " vs " << ustr_as_utf8(expected) << std::endl;
            }

            assert_not_null(test_path);
            assert_not_null(expected);

            std::string errmsg =
                (boost::format(
                     "%1%/ffci_test(%2%, %3%, %4%)")
                 % base_func_name
                 % ustr_as_utf8(test_path)
                 % ustr_as_utf8(expected)
                 % is_good).str();

            if (is_good)
            {
                assert_equal_msg(TRUE,
                                 find_file_casei_bang(test_path),
                                 errmsg);
                assert_equal_msg(TRUE,
                                 ustr_equal(expected, test_path),
                                 errmsg);
            }
            else
            {
                assert_equal_msg(FALSE,
                                 find_file_casei_bang(test_path),
                                 errmsg);
            }

        }

        bool run_ffci_data_tests(const char *base_func_name,
                                 ffci_data data1[], int count)
        {
            for (int i = 0; i < count; i++)
            {
                ffci_data &da_data = data1[i];

                gchar *tmp_dir_name = rra_mkdtemp("test_fs_dir");
                if (tmp_dir_name == NULL)
                {
                    perror("rra_mkdtemp failed in test_ffc_simple1:");
                    return false;
                }
                chdir(tmp_dir_name);

                da_data.run_commands();

                for (ffci_test_vec::iterator j = da_data.tests.begin();
                     j != da_data.tests.end();
                     j++)
                {
                    ustring *test_path = ustr_create(j->test_path.c_str());
                    ustring *expected = ustr_create(j->expected.c_str());

                    // relative paths
                    check_one_path(base_func_name,
                                   j->good, test_path, expected);

                    gchar *cwd = g_get_current_dir();

                    ustr_strset(test_path, cwd);
                    ustr_strcat(test_path, "/");
                    ustr_strcat(test_path, j->test_path.c_str());

                    ustr_strset(expected, cwd);
                    ustr_strcat(expected, "/");
                    ustr_strcat(expected, j->expected.c_str());

                    // absolute paths
                    check_one_path(base_func_name,
                                   j->good, test_path, expected);

                    ustr_free(expected);
                    ustr_free(test_path);
                }

                chdir("..");
                char *rm_cmd = g_strdup_printf("rm -rf %s", tmp_dir_name);
                system(rm_cmd);

                g_free(tmp_dir_name);
                g_free(rm_cmd);
            }

            return true;
        }

        bool test_ffc_simple1(void)
        {
            ffci_data data1[] = {
                /* Simple lower case */
                ffci_data(
                    list_of("touch foo"),
                    list_of
                    (ffci_test("foo", "foo", true))
                    (ffci_test("bar", "", false))
                    (ffci_test("fOo", "foo", true))
                    (ffci_test("FOO", "foo", true))
                    (ffci_test("Foo", "foo", true))
                    ),
                /* Simple upper case */
                ffci_data(
                    list_of("touch FOO"),
                    list_of
                    (ffci_test("foo", "FOO", true))
                    (ffci_test("bar", "", false))
                    (ffci_test("fOo", "FOO", true))
                    (ffci_test("FOO", "FOO", true))
                    (ffci_test("Foo", "FOO", true))
                    ),
                /* Simple random case */
                ffci_data(
                    list_of("touch foO"),
                    list_of
                    (ffci_test("foo", "foO", true))
                    (ffci_test("bar", "", false))
                    (ffci_test("fOo", "foO", true))
                    (ffci_test("FOO", "foO", true))
                    (ffci_test("Foo", "foO", true))
                    ),
                /* Two items random case */
                ffci_data(
                    list_of
                    ("touch foO")
                    ("touch BAr"),
                    list_of
                    (ffci_test("foo", "foO", true))
                    (ffci_test("bar", "BAr", true))
                    (ffci_test("BAR", "BAr", true))
                    (ffci_test("Bar", "BAr", true))
                    (ffci_test("baz", "BAr", false))
                    (ffci_test("BAZ", "BAr", false))
                    (ffci_test("Baz", "BAr", false))
                    (ffci_test("fOo", "foO", true))
                    (ffci_test("FOO", "foO", true))
                    (ffci_test("Foo", "foO", true))
                    ),
                ffci_data()
            };

            return run_ffci_data_tests(__func__, data1, G_N_ELEMENTS(data1));
        }

        bool test_ffc_simple_conflict1(void)
        {
            ffci_data data1[] = {
                ffci_data(
                    list_of
                    ("touch foo")
                    ("touch FOO"),
                    list_of
                    (ffci_test("foo", "foo", true))
                    (ffci_test("FOO", "FOO", true))
                    (ffci_test("fOo", "", false))
                    (ffci_test("Foo", "", false))
                    ),
                ffci_data(
                    list_of
                    ("touch FOO")
                    ("touch fOO"),
                    list_of
                    (ffci_test("foo", "", false))
                    (ffci_test("fOO", "fOO", true))
                    (ffci_test("fOo", "", false))
                    (ffci_test("FOO", "FOO", true))
                    (ffci_test("Foo", "", false))
                    ),
                ffci_data(
                    list_of
                    ("touch foo")
                    ("touch fOO"),
                    list_of
                    (ffci_test("foo", "foo", true))
                    (ffci_test("fOO", "fOO", true))
                    (ffci_test("fOo", "", false))
                    (ffci_test("FOO", "", false))
                    (ffci_test("Foo", "", false))
                    ),
                ffci_data(
                    list_of
                    ("touch foO")
                    ("touch fOO"),
                    list_of
                    (ffci_test("foo", "", false))
                    (ffci_test("fOo", "", false))
                    (ffci_test("fOO", "fOO", true))
                    (ffci_test("FOO", "", false))
                    (ffci_test("Foo", "", false))
                    ),
                ffci_data()
            };

            return run_ffci_data_tests(__func__, data1, G_N_ELEMENTS(data1));
        }

        bool test_ffc_one_level1(void)
        {
            ffci_data data1[] = {
                /* Simple lower case */
                ffci_data(
                    list_of
                    ("mkdir bar")
                    ("touch bar/foo"),
                    list_of
                    (ffci_test("bar/foo", "bar/foo", true))
                    (ffci_test("foo/bar", "", false))
                    (ffci_test("bAR/fOo", "bar/foo", true))
                    (ffci_test("BAR/FOO", "bar/foo", true))
                    (ffci_test("Bar/Foo", "bar/foo", true))
                    ),
                /* Simple upper case */
                ffci_data(
                    list_of
                    ("mkdir BAR")
                    ("touch BAR/FOO"),
                    list_of
                    (ffci_test("bar/foo", "BAR/FOO", true))
                    (ffci_test("foo/bar", "", false))
                    (ffci_test("bAr/fOo", "BAR/FOO", true))
                    (ffci_test("BAR/FOO", "BAR/FOO", true))
                    (ffci_test("Bar/Foo", "BAR/FOO", true))
                    ),
                /* Simple random case */
                ffci_data(
                    list_of
                    ("mkdir bAR")
                    ("touch bAR/foO"),
                    list_of
                    (ffci_test("bar/foo", "bAR/foO", true))
                    (ffci_test("foo/bar", "", false))
                    (ffci_test("bAr/fOo", "bAR/foO", true))
                    (ffci_test("BAR/FOO", "bAR/foO", true))
                    (ffci_test("Bar/Foo", "bAR/foO", true))
                    ),
                /* Two items random case */
                ffci_data(
                    list_of
                    ("mkdir baz")
                    ("touch baz/foO")
                    ("touch baz/BAr"),
                    list_of
                    (ffci_test("baz/foo", "baz/foO", true))
                    (ffci_test("baz/bar", "baz/BAr", true))
                    (ffci_test("BAZ/BAR", "baz/BAr", true))
                    (ffci_test("Baz/Bar", "baz/BAr", true))
                    (ffci_test("foo/baz", "", false))
                    (ffci_test("foo/BAZ", "", false))
                    (ffci_test("foo/Baz", "", false))
                    (ffci_test("bAz/fOo", "baz/foO", true))
                    (ffci_test("BAZ/FOO", "baz/foO", true))
                    (ffci_test("Baz/Foo", "baz/foO", true))
                    ),
                ffci_data()
            };

            return run_ffci_data_tests(__func__, data1, G_N_ELEMENTS(data1));
        }

        bool test_ffc_conflict_level_one1(void)
        {
            ffci_data data1[] = {
                ffci_data(
                    list_of
                    ("mkdir bar")
                    ("mkdir BAR")
                    ("touch BAR/foo")
                    ("touch bar/foo"),
                    list_of
                    (ffci_test("bar/foo", "bar/foo", true))
                    (ffci_test("bar/FOO", "bar/foo", true))
                    (ffci_test("bAR/fOo", "", false))
                    (ffci_test("BAR/FOO", "BAR/foo", true))
                    (ffci_test("Bar/Foo", "", false))
                    ),
                ffci_data(
                    list_of
                    ("mkdir BAR")
                    ("mkdir BaR")
                    ("touch BaR/foo")
                    ("touch BAR/foo"),
                    list_of
                    (ffci_test("bar/foo", "", false))
                    (ffci_test("BaR/fOo", "BaR/foo", true))
                    (ffci_test("BAR/FOO", "BAR/foo", true))
                    (ffci_test("Bar/Foo", "", false))
                    ),
                ffci_data(
                    list_of
                    ("mkdir bar")
                    ("mkdir BaR")
                    ("touch BaR/foo")
                    ("touch bar/foo"),
                    list_of
                    (ffci_test("bar/foo", "bar/foo", true))
                    (ffci_test("BaR/fOo", "BaR/foo", true))
                    (ffci_test("BAR/FOO", "", false))
                    (ffci_test("Bar/Foo", "", false))
                    ),
                ffci_data()
            };

            return run_ffci_data_tests(__func__, data1, G_N_ELEMENTS(data1));
        }

        bool test_ffc_conflict_level_two1(void)
        {
            ffci_data data1[] = {
                ffci_data(
                    list_of
                    ("mkdir bar")
                    ("touch bar/FOO")
                    ("touch bar/foo"),
                    list_of
                    (ffci_test("bar/foo", "bar/foo", true))
                    (ffci_test("bar/FOO", "bar/FOO", true))
                    (ffci_test("bAR/fOo", "", false))
                    (ffci_test("BAR/FOO", "bar/FOO", true))
                    (ffci_test("Bar/Foo", "", false))
                    ),
                ffci_data(
                    list_of
                    ("mkdir BAR")
                    ("touch BAR/FoO")
                    ("touch BAR/foo"),
                    list_of
                    (ffci_test("bar/foo", "BAR/foo", true))
                    (ffci_test("bar/fOo", "", false))
                    (ffci_test("BAR/FOO", "", false))
                    (ffci_test("BAR/FoO", "BAR/FoO", true))
                    (ffci_test("Bar/Foo", "", false))
                    ),
                ffci_data(
                    list_of
                    ("mkdir BaR")
                    ("touch BaR/FOO")
                    ("touch BaR/fOO"),
                    list_of
                    (ffci_test("bar/foo", "", false))
                    (ffci_test("BaR/fOo", "", false))
                    (ffci_test("BAR/FOO", "BaR/FOO", true))
                    (ffci_test("Bar/fOO", "BaR/fOO", true))
                    ),
                ffci_data()
            };

            return run_ffci_data_tests(__func__, data1, G_N_ELEMENTS(data1));
        }

        bool test_ffc_conflict_level_one_and_two1(void)
        {
            ffci_data data1[] = {
                ffci_data(
                    list_of
                    ("mkdir bar")
                    ("mkdir BAR")
                    ("touch bar/FOO")
                    ("touch bar/foo")
                    ("touch BAR/FOO")
                    ("touch BAR/foo"),
                    list_of
                    (ffci_test("bar/foo", "bar/foo", true))
                    (ffci_test("bar/FOO", "bar/FOO", true))
                    (ffci_test("bAR/fOo", "", false))
                    (ffci_test("BAR/FOO", "BAR/FOO", true))
                    (ffci_test("Bar/Foo", "", false))
                    ),
                ffci_data(
                    list_of
                    ("mkdir BAR")
                    ("mkdir bar")
                    ("touch BAR/FoO")
                    ("touch BAR/foo")
                    ("touch BAR/baz")
                    ("touch bar/fOo")
                    ("touch bar/foo"),
                    list_of
                    (ffci_test("bar/foo", "bar/foo", true))
                    (ffci_test("bar/fOo", "bar/fOo", true))
                    (ffci_test("bar/FoO", "", false))
                    (ffci_test("bar/baz", "", false))
                    (ffci_test("BAR/FOO", "", false))
                    (ffci_test("BAR/FoO", "BAR/FoO", true))
                    (ffci_test("BAR/fOo", "", false))
                    (ffci_test("Bar/Foo", "", false))
                    ),
                ffci_data(
                    list_of
                    ("mkdir bAR")
                    ("mkdir bar")
                    ("touch bAR/FoO")
                    ("touch bAR/foo")
                    ("touch bAR/baz")
                    ("touch bar/fOo")
                    ("touch bar/foo"),
                    list_of
                    (ffci_test("bar/foo", "bar/foo", true))
                    (ffci_test("bar/fOo", "bar/fOo", true))
                    (ffci_test("bar/FoO", "", false))
                    (ffci_test("bar/baz", "", false))
                    (ffci_test("BAR/FOO", "", false))
                    (ffci_test("BAR/FoO", "", false))
                    (ffci_test("BAR/fOo", "", false))
                    (ffci_test("Bar/Foo", "", false))
                    (ffci_test("bAR/Foo", "", false))
                    (ffci_test("bAR/FoO", "bAR/FoO", true))
                    ),
                ffci_data()
            };

            return run_ffci_data_tests(__func__, data1, G_N_ELEMENTS(data1));
        }

    public:

        test_fs(): Melunit::Test("test_fs")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_fs:: name)

            REGISTER(test_ffc_simple1);
            REGISTER(test_ffc_simple_conflict1);
            REGISTER(test_ffc_one_level1);
            REGISTER(test_ffc_conflict_level_one1);
            REGISTER(test_ffc_conflict_level_two1);
            REGISTER(test_ffc_conflict_level_one_and_two1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_fs t1_;

    class test_mkdtemp : public Melunit::Test
    {
    private:

        bool test_simple1()
        {
            gchar *tempd1 = rra_mkdtemp("foo");
            assert_not_null(tempd1);
            assert_equal(10, strlen(tempd1));
            assert_true(g_file_test(tempd1, G_FILE_TEST_IS_DIR));
            assert_equal('f', tempd1[0]);
            assert_equal('o', tempd1[1]);
            assert_equal('o', tempd1[2]);
            assert_equal('-', tempd1[3]);

            assert_equal(0, rmdir(tempd1));

            g_free(tempd1);

            return true;
        }

    public:

        test_mkdtemp(): Melunit::Test("test_mkdtemp")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_mkdtemp:: name)

            REGISTER(test_simple1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_mkdtemp t2_;
}
