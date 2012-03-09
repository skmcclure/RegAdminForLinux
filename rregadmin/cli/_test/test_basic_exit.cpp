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
    class test_basic_exit : public Melunit::Test
    {
    private:

        bool test_exit1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { };

            RRACliResult *res = rra_cli_exit(hbs1, 0, args);
            assert_not_null(res);

            assert_true(rra_cli_result_is_exit(res));
            assert_false(rra_cli_result_is_error(res));
            assert_false(rra_cli_result_is_success(res));
            assert_true(rra_cli_result_free(res));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_apply_exit1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args[] = { "exit" };

            RRACliResult *res =
                rra_cli_state_apply(hbs1, G_N_ELEMENTS(args), args);
            assert_not_null(res);

            assert_true(rra_cli_result_is_exit(res));
            assert_false(rra_cli_result_is_error(res));
            assert_false(rra_cli_result_is_success(res));
            assert_true(rra_cli_result_free(res));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

    public:

        test_basic_exit(): Melunit::Test("test_basic_exit")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_basic_exit:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_basic_exit:: name, this, ehindex))

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_exit1, i);
                REG_BOOSTB(test_apply_exit1, i);
            }

            Melunit::Suite::instance().register_test(this);

#undef REG_BOOSTB
#undef REGISTER
        }

    };

    test_basic_exit t1_;
}
