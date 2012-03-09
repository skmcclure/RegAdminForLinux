/*
 * Authors:     James LewisMoss <jlm@racemi.com>
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
#include "good_stuff.h"
#include "helpers.h"

namespace
{
    class test_basic_delete_key : public Melunit::Test
    {
    private:

        bool test_delete_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const ef_line *line = RRegadmin::Test::get_good_key();
            const char *args[] = { line->path };

            assert_true_result(
                (rra_cli_has_key(hbs1, G_N_ELEMENTS(args), args)));
            assert_true_result(
                (rra_cli_delete_key(hbs1, G_N_ELEMENTS(args), args)));
            assert_false_result(
                (rra_cli_has_key(hbs1, G_N_ELEMENTS(args), args)));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_delete_key2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const ef_line *line = RRegadmin::Test::get_good_key();
            const char *args[] = { line->path, "foo" };

            assert_false_result(
                rra_cli_delete_key(hbs1, G_N_ELEMENTS(args), args));
            assert_false_result(rra_cli_delete_key(hbs1, 0, NULL));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_delete_key3(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const ef_line *line = RRegadmin::Test::get_good_key();
            const char *args1[] = { line->path };

            assert_true_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args1), args1));
            assert_true_result(
                rra_cli_delete_key(hbs1, G_N_ELEMENTS(args1), args1));
            assert_false_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args1), args1));

            line = RRegadmin::Test::get_good_key();
            const char *args2[] = { line->path };

            assert_true_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args2), args2));
            assert_true_result(
                rra_cli_delete_key(hbs1, G_N_ELEMENTS(args2), args2));
            assert_false_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args2), args2));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_apply_delete_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const ef_line *line = RRegadmin::Test::get_good_key();
            const char *args[] = { "delete_key", line->path };

            assert_true_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args) - 1, args + 1));
            assert_true_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args), args));
            assert_false_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args) - 1, args + 1));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_apply_delete_key2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const ef_line *line = RRegadmin::Test::get_good_key();
            const char *args[] = { "delete_key", line->path, "foo" };

            assert_false_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args), args));
            assert_false_result(
                rra_cli_state_apply(hbs1, 0, NULL));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_apply_delete_key3(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const ef_line *line = RRegadmin::Test::get_good_key();
            const char *args1[] = { "delete_key", line->path };

            assert_true_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args1) - 1, args1 + 1));
            assert_true_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args1), args1));
            assert_false_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args1) - 1, args1 + 1));

            line = RRegadmin::Test::get_good_key();
            const char *args2[] = { "delete_key", line->path };

            assert_true_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args2) - 1, args2 + 1));
            assert_true_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args2), args2));
            assert_false_result(
                rra_cli_has_key(hbs1, G_N_ELEMENTS(args2) - 1, args2 + 1));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_apply_delete_key4(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const ef_line *line = RRegadmin::Test::get_good_key();
            const char *args[] = { line->path };

            assert_true_result(rra_cli_cd(hbs1, G_N_ELEMENTS(args), args));

            const char *args1[] = { "delete_key" };

            assert_false_result(
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args1), args1));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

    public:

        test_basic_delete_key(): Melunit::Test("test_basic_delete_key")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_basic_delete_key:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_basic_delete_key:: name, this, ehindex))

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_delete_key1, i);
                REG_BOOSTB(test_delete_key2, i);
                REG_BOOSTB(test_delete_key3, i);
                REG_BOOSTB(test_apply_delete_key1, i);
                REG_BOOSTB(test_apply_delete_key2, i);
                REG_BOOSTB(test_apply_delete_key3, i);
                REG_BOOSTB(test_apply_delete_key4, i);
            }

            Melunit::Suite::instance().register_test(this);

#undef REG_BOOSTB
#undef REGISTER
        }

    };

    test_basic_delete_key t1_;
}
