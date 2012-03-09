/*
 * Authors:	James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
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

#include <rregadmin/config.h>

#include <boost/bind.hpp>

#include <glib/gtypes.h>

#ifdef HAVE_GLIB_GSTDIO_H
#include <glib/gstdio.h>
#else
#include <stdio.h>
#define g_unlink unlink
#endif

#ifndef HAVE_G_FILE_SET_CONTENTS
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
static gboolean
rra_file_set_contents(const gchar *filename, const gchar *contents,
                     gssize length)
{
    gboolean ret_val = TRUE;
    int fd = open(filename, O_TRUNC | O_CREAT | O_WRONLY);
    if (write(fd, contents, length) != length)
    {
         ret_val = FALSE;
    }
    close(fd);
    return ret_val;
}
#else
#include <glib/gfileutils.h>
static gboolean
rra_file_set_contents(const gchar *filename, const gchar *contents,
                     gssize length)
{
    return g_file_set_contents(filename, contents, length, NULL);
}
#endif

#include <melunit/melunit-cxx.h>

#include <rregadmin/cli/basic.h>
#include <rregadmin/cli/init.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/util/options.h>

#include "example_registry.h"
#include "helpers.h"

namespace
{
    class test_basic_add_value : public Melunit::Test
    {
    private:

        bool test_add_empty_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                   "foobar" };

            assert_false_result(
                rra_cli_has_value(hbs1, G_N_ELEMENTS(args), args));
            assert_true_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args), args));
            assert_true_result(
                rra_cli_has_value(hbs1, G_N_ELEMENTS(args), args));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_add_simple_string_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                   "foobar",
                                   "this is the value" };

            assert_false_result(
                rra_cli_has_value(hbs1, 2, args));
            assert_true_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args), args));
            assert_true_result(
                rra_cli_has_value(hbs1, 2, args));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        void assure_string_equal(RRACliState *in_state, const char *args[])
        {
            RRACliResult *res1 = rra_cli_cat_value(in_state, 2, args);
            assert_true_result(res1);

            Cell *da_cell = rra_cli_result_get_cell(res1);
            assert_not_null(da_cell);
            ValueKeyCell *cell1 = value_key_cell_from_cell(da_cell);
            assert_not_null(cell1);

            Value *val1 = value_key_cell_get_val(cell1);
            assert_not_null(val1);
            ustring *val1_str = value_get_as_string(val1);
            assert_not_null(val1_str);
            assert_equal_msg(REG_SZ, value_get_type(val1),
                             ustr_as_utf8(val1_str));
            assert_equal(std::string(args[2]),
                         std::string(ustr_as_utf8(val1_str)));
            ustr_free(val1_str);
            assert_true(value_free(val1));
        }

        bool test_replace_simple_string_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                   "foobar",
                                   "this is the value" };

            assert_false_result(
                rra_cli_has_value(hbs1, 2, args));
            assert_true_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args), args));
            assert_true_result(
                rra_cli_has_value(hbs1, 2, args));

            assure_string_equal(hbs1, args);

            const char *args2[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                    "foobar",
                                    "this is the new value" };

            assert_true_result(
                rra_cli_has_value(hbs1, 2, args2));
            assert_true_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args2), args2));
            assert_true_result(
                rra_cli_has_value(hbs1, 2, args2));

            assure_string_equal(hbs1, args2);

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_replace_simple_string_value2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                   "foobar",
                                   "this is the value" };

            assert_false_result(
                rra_cli_has_value(hbs1, 2, args));
            assert_true_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args), args));
            assert_true_result(
                rra_cli_has_value(hbs1, 2, args));

            assure_string_equal(hbs1, args);

            assert_true(rra_cli_state_free(hbs1));
            hbs1 = rra_cli_state_new_from_win_dir(er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args2[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                    "foobar",
                                    "this is the new value" };

            assert_true_result(
                rra_cli_has_value(hbs1, 2, args2));
            assert_true_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args2), args2));
            assert_true_result(
                rra_cli_has_value(hbs1, 2, args2));

            assure_string_equal(hbs1, args2);

            assert_true(rra_cli_state_free(hbs1));
            hbs1 = rra_cli_state_new_from_win_dir(er1.get_dir().c_str());
            assert_not_null(hbs1);

            assure_string_equal(hbs1, args2);

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_add_typed_string_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                   "foobar",
                                   "SZ",
                                   "this is the value" };

            assert_false_result(
                rra_cli_has_value(hbs1, 2, args));
            assert_true_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args), args));
            assert_true_result(
                rra_cli_has_value(hbs1, 2, args));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_add_file_value1(int ehindex)
        {
            const char *test_file = "test-file";

            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);
            const char data1[] = { 0xff, 0xae, 0x99 };
            assert_true(rra_file_set_contents(test_file, data1, 3));

            const char *args1[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                    "foo",
                                    "BINARY",
                                    "-F",
                                    test_file };

            assert_true_result(rra_cli_add_value(hbs1, G_N_ELEMENTS(args1),
                                                  args1));

            g_unlink(test_file);
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_add_value_fail2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args1[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                    "foo",
                                    "foo", // <- not a type
                                    "some string"};

            assert_false_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args1), args1));

            const char *args2[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                    "foo"
                                    "SZ",
                                    "foo", // not -F for load from file
                                    "some string"};

            assert_false_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args2), args2));

            assert_false_result(rra_cli_add_value(hbs1, 0, NULL));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_add_hex_values1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args1[] = { "HKEY_LOCAL_MACHINE\\system\\WPA",
                                    "foo",
                                    "REG_DWORD", // <- not a type
                                    "0xffffffff"};

            assert_true_result(
                rra_cli_add_value(hbs1, G_N_ELEMENTS(args1), args1));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

    public:

        test_basic_add_value(): Melunit::Test("test_basic_add_value")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_basic_add_value:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_basic_add_value:: name, this, ehindex))

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_add_value_fail2, i);
                REG_BOOSTB(test_add_empty_value1, i);
                REG_BOOSTB(test_add_simple_string_value1, i);
                REG_BOOSTB(test_replace_simple_string_value1, i);
                REG_BOOSTB(test_replace_simple_string_value2, i);
                REG_BOOSTB(test_add_typed_string_value1, i);
                REG_BOOSTB(test_add_file_value1, i);
                REG_BOOSTB(test_add_hex_values1, i);
            }

            Melunit::Suite::instance().register_test(this);

#undef REG_BOOSTB
#undef REGISTER
        }

    };

    test_basic_add_value t1_;
}
