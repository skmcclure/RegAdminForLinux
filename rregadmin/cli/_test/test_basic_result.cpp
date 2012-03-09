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

#include <glib/garray.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/cli/basic_result.h>
#include <rregadmin/cli/init.h>

namespace
{
    class test_basic_result : public Melunit::Test
    {
    private:

        bool test_null1(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            assert_false(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));
            assert_false(rra_cli_result_has_data(res1));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

        bool test_error1(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            assert_true(rra_cli_result_add_error(res1, "Error message1"));

            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));
            assert_true(rra_cli_result_has_content(res1));
            assert_false(rra_cli_result_has_data(res1));

            assert_equal(std::string("Error message1"),
                         std::string(
                             rra_cli_result_get_content(res1)));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

        bool test_error2(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            assert_true(rra_cli_result_add_error(res1, "Error message1: %d",
                                                  10));

            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));
            assert_true(rra_cli_result_has_content(res1));
            assert_false(rra_cli_result_has_data(res1));

            assert_equal(std::string("Error message1: 10"),
                         std::string(
                             rra_cli_result_get_content(res1)));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

        bool test_message1(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            assert_true(rra_cli_result_add_message(res1, "Success message1"));

            assert_false(rra_cli_result_is_error(res1));
            assert_true(rra_cli_result_is_success(res1));
            assert_true(rra_cli_result_has_content(res1));
            assert_false(rra_cli_result_has_data(res1));

            assert_equal(std::string("Success message1"),
                         std::string(
                             rra_cli_result_get_content(res1)));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

        bool test_set1(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            assert_true(rra_cli_result_set_error(res1));
            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));

            assert_false(rra_cli_result_set_success(res1));
            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

        bool test_set2(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            assert_true(rra_cli_result_set_success(res1));
            assert_false(rra_cli_result_is_error(res1));
            assert_true(rra_cli_result_is_success(res1));

            assert_true(rra_cli_result_set_error(res1));
            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));

            assert_false(rra_cli_result_set_success(res1));
            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

        bool test_set3(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            assert_true(
                rra_cli_result_add_message(res1, "Success message4"));

            assert_false(rra_cli_result_is_error(res1));
            assert_true(rra_cli_result_is_success(res1));
            assert_true(rra_cli_result_has_content(res1));

            assert_true(rra_cli_result_set_error(res1));
            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

        bool test_set4(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            assert_true(rra_cli_result_set_error(res1));
            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));

            assert_false(
                rra_cli_result_add_message(res1, "Success message5"));

            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

        bool test_data1(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            guint8 bytes1[] = { 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
                                0x11, 0x00 };

            GByteArray *gba1 = g_byte_array_new();
            g_byte_array_append(gba1, bytes1, G_N_ELEMENTS(bytes1));

            assert_true(rra_cli_result_set_data(res1, gba1));

            assert_false(rra_cli_result_is_error(res1));
            assert_true(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));
            assert_true(rra_cli_result_has_data(res1));

            const GByteArray *gba2 = rra_cli_result_get_data(res1);
            assert_equal(gba1, gba2);

            assert_true(rra_cli_result_free(res1));

            return true;
        }


        bool test_data2(void)
        {
            RRACliResult *res1 = rra_cli_result_new();


            guint8 bytes1[] = { 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
                                0x11, 0x00 };

            GByteArray *gba1 = g_byte_array_new();
            g_byte_array_append(gba1, bytes1, G_N_ELEMENTS(bytes1));

            assert_true(rra_cli_result_set_data(res1, gba1));

            assert_false(rra_cli_result_is_error(res1));
            assert_true(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));
            assert_true(rra_cli_result_has_data(res1));

            assert_true(rra_cli_result_add_message(res1, "Success message1"));

            assert_false(rra_cli_result_is_error(res1));
            assert_true(rra_cli_result_is_success(res1));
            assert_true(rra_cli_result_has_content(res1));
            assert_true(rra_cli_result_has_data(res1));

            assert_equal(std::string("Success message1"),
                         std::string(
                             rra_cli_result_get_content(res1)));

            assert_true(rra_cli_result_free(res1));

            return true;
        }


        bool test_data3(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            assert_true(rra_cli_result_set_error(res1));
            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));
            assert_false(rra_cli_result_has_data(res1));

            guint8 bytes1[] = { 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
                                0x11, 0x00 };

            GByteArray *gba1 = g_byte_array_new();
            g_byte_array_append(gba1, bytes1, G_N_ELEMENTS(bytes1));

            assert_false(rra_cli_result_set_data(res1, gba1));

            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));
            assert_false(rra_cli_result_has_data(res1));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

        bool test_data4(void)
        {
            RRACliResult *res1 = rra_cli_result_new();

            guint8 bytes1[] = { 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22,
                                0x11, 0x00 };

            GByteArray *gba1 = g_byte_array_new();
            g_byte_array_append(gba1, bytes1, G_N_ELEMENTS(bytes1));

            assert_true(rra_cli_result_set_data(res1, gba1));

            assert_false(rra_cli_result_is_error(res1));
            assert_true(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));
            assert_true(rra_cli_result_has_data(res1));

            assert_true(rra_cli_result_set_error(res1));
            assert_true(rra_cli_result_is_error(res1));
            assert_false(rra_cli_result_is_success(res1));
            assert_false(rra_cli_result_has_content(res1));
            assert_false(rra_cli_result_has_data(res1));

            assert_true(rra_cli_result_free(res1));

            return true;
        }

public:

        test_basic_result(): Melunit::Test("test_basic_result")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_basic_result:: name)

            REGISTER(test_null1);
            REGISTER(test_error1);
            REGISTER(test_error2);
            REGISTER(test_message1);
            REGISTER(test_set1);
            REGISTER(test_set2);
            REGISTER(test_set3);
            REGISTER(test_set4);
            REGISTER(test_data1);
            REGISTER(test_data2);
            REGISTER(test_data3);
            REGISTER(test_data4);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_basic_result t1_;
}
