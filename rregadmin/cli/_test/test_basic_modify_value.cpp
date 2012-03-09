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

#include <glib/gfileutils.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/cli/basic.h>
#include <rregadmin/cli/init.h>
#include <rregadmin/registry/registry.h>

#include "example_registry.h"
#include "good_stuff.h"
#include "helpers.h"

namespace
{
    const ef_line *val1 = RRegadmin::Test::get_good_value();
    const ef_line *val2 = RRegadmin::Test::get_good_value();

    class test_basic_modify_value : public Melunit::Test
    {
    private:

        bool test_fail1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            const char *args1[] = { val2->path, val2->name };

            assert_false_result(
                rra_cli_modify_value(hbs1, G_N_ELEMENTS(args1), args1));

            const char *args2[] = { "clear_bit",
                                    val2->path, val2->name,
                                    "0",
                                    "0x11" };

            assert_false_result(rra_cli_modify_value(hbs1, 0, args2));
            assert_false_result(rra_cli_modify_value(hbs1, 1, args2));
            assert_false_result(rra_cli_modify_value(hbs1, 2, args2));
            assert_false_result(rra_cli_modify_value(hbs1, 3, args2));

            const char *args3[] = { "set_bit",
                                    val2->path, val2->name,
                                    "0",
                                    "0x11" };

            assert_false_result(rra_cli_modify_value(hbs1, 0, args3));
            assert_false_result(rra_cli_modify_value(hbs1, 1, args3));
            assert_false_result(rra_cli_modify_value(hbs1, 2, args3));
            assert_false_result(rra_cli_modify_value(hbs1, 3, args3));

            const char *args4[] = { "foo_bit",
                                    val2->path, val2->name,
                                    "0",
                                    "0x11" };

            assert_false_result(rra_cli_modify_value(hbs1, 0, args4));
            assert_false_result(rra_cli_modify_value(hbs1, 1, args4));
            assert_false_result(rra_cli_modify_value(hbs1, 2, args4));
            assert_false_result(rra_cli_modify_value(hbs1, 3, args4));
            assert_false_result(rra_cli_modify_value(hbs1, 4, args4));

            const char *args5[] = { "foo_bit",
                                    val2->path, val2->name,
                                    "0",
                                    "0x1001" };

            assert_false_result(rra_cli_modify_value(hbs1, 0, args5));

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        GByteArray* get_ba(RRACliState *hbs1, const ef_line *in_line)
        {
            const char *args[2];
            args[0] = in_line->path;
            args[1] = in_line->name;

            RRACliResult *res1 = rra_cli_cat_value(hbs1, G_N_ELEMENTS(args),
                                                     args);
            assert_not_null(res1);
            assert_true(rra_cli_result_has_data(res1));

            GByteArray *da_bytes = g_byte_array_new();
            const GByteArray *tmp_ba = rra_cli_result_get_data(res1);

            da_bytes = g_byte_array_append(da_bytes, tmp_ba->data,
                                           tmp_ba->len);

            return da_bytes;
        }

        bool test_clear1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            GByteArray *pre_bytes = get_ba(hbs1, val2);

            const char *args2[] = { "clear_bit",
                                    val2->path, val2->name,
                                    "0",
                                    "0x11" };

            assert_true_result(
                rra_cli_modify_value(hbs1, G_N_ELEMENTS(args2), args2));

            GByteArray *post_bytes = get_ba(hbs1, val2);

            assert_equal(pre_bytes->len, post_bytes->len);
            assert_equal(pre_bytes->data[0] & ~0x11, post_bytes->data[0]);
            assert_equal(pre_bytes->data[1], post_bytes->data[1]);
            assert_equal(pre_bytes->data[2], post_bytes->data[2]);
            assert_equal(pre_bytes->data[3], post_bytes->data[3]);

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_set1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            RRACliState *hbs1 = rra_cli_state_new_from_win_dir(
                er1.get_dir().c_str());
            assert_not_null(hbs1);

            GByteArray *pre_bytes = get_ba(hbs1, val2);

            const char *args2[] = { "set_bit",
                                    val2->path, val2->name,
                                    "3",
                                    "0xfe" };

            assert_true_result(
                rra_cli_modify_value(hbs1, G_N_ELEMENTS(args2), args2));

            GByteArray *post_bytes = get_ba(hbs1, val2);

            assert_equal(pre_bytes->len, post_bytes->len);
            assert_equal(pre_bytes->data[0], post_bytes->data[0]);
            assert_equal(pre_bytes->data[1], post_bytes->data[1]);
            assert_equal(pre_bytes->data[2], post_bytes->data[2]);
            assert_equal(pre_bytes->data[3] | 0xfe, post_bytes->data[3]);

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_null(void)
        {
            return true;
        }

    public:

        test_basic_modify_value(): Melunit::Test("test_basic_modify_value")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_basic_modify_value:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_basic_modify_value:: name, this, ehindex))

//             for (int i = 0; i < ExampleHives::size(); i++)
//             {
//                 REG_BOOSTB(test_fail1, i);
//                 REG_BOOSTB(test_clear1, i);
//                 REG_BOOSTB(test_set1, i);
//             }
            REGISTER(test_null);

            Melunit::Suite::instance().register_test(this);

#undef REG_BOOSTB
#undef REGISTER
        }

    };

    test_basic_modify_value t1_;
}
