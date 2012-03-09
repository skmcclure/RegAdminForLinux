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

#include <boost/bind.hpp>

#include <melunit/melunit-cxx.h>

#include <rregadmin/cli/basic.h>
#include <rregadmin/cli/init.h>
#include <rregadmin/registry/registry.h>

#include "example_registry.h"
#include "helpers.h"

namespace
{
    class test_basic_has_key : public Melunit::Test
    {
    private:

        bool test_has_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "HKEY_LOCAL_MACHINE\\system" };

            assert_true_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args), args));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_has_key2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "HKEY_LOCAL_MACHINE\\system", "foo" };

            assert_false_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args), args));
            assert_false_result(rra_cli_has_key(hbs1, 0, NULL));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_has_key3(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args1[] = { "HKEY_LOCAL_MACHINE\\system" };

            assert_true_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args1), args1));

            const char *args2[] = { "\\HKEY_LOCAL_MACHINE\\software" };

            assert_true_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args2), args2));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_apply_has_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "has_key", "HKEY_LOCAL_MACHINE\\system" };

            assert_true_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args), args));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_apply_has_key2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "has_key", "HKEY_LOCAL_MACHINE\\system",
                                   "foo" };

            assert_false_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args), args));
            assert_false_result(
                rra_cli_state_apply(hbs1, 0, NULL));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_apply_has_key3(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args1[] = { "has_key", "HKEY_LOCAL_MACHINE\\system" };

            assert_true_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args1), args1));

            const char *args2[] = { "has_key",
                                    "\\HKEY_LOCAL_MACHINE\\software" };

            assert_true_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args2), args2));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_apply_has_key4(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = {
                "HKEY_LOCAL_MACHINE\\system\\ControlSet001" };

            assert_true_result(rra_cli_cd(hbs1, G_N_ELEMENTS(args), args));

            const char *args1[] = { "has_key" };

            assert_false_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args1), args1));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

    public:

        test_basic_has_key(): Melunit::Test("test_basic_has_key")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_basic_has_key:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_basic_has_key:: name, this, ehindex))

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_has_key1, i);
                REG_BOOSTB(test_has_key2, i);
                REG_BOOSTB(test_has_key3, i);
                REG_BOOSTB(test_apply_has_key1, i);
                REG_BOOSTB(test_apply_has_key2, i);
                REG_BOOSTB(test_apply_has_key3, i);
                REG_BOOSTB(test_apply_has_key4, i);
            }

            Melunit::Suite::instance().register_test(this);

#undef REG_BOOSTB
#undef REGISTER
        }

    };

    test_basic_has_key t1_;
}
