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

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <functional>

#include <melunit/melunit-cxx.h>

#include <glib/gmem.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <rregadmin/util/value_type.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_value_type : public Melunit::Test
    {
    private:

        bool test_str_to_num1(void)
        {
            assert_equal(REG_NONE,
                         value_type_from_string("NONE"));
            assert_equal(REG_NONE,
                         value_type_from_string("REG_NONE"));

            assert_equal(REG_SZ,
                         value_type_from_string("SZ"));
            assert_equal(REG_SZ,
                         value_type_from_string("REG_SZ"));

            assert_equal(REG_EXPAND_SZ,
                         value_type_from_string("EXPAND_SZ"));
            assert_equal(REG_EXPAND_SZ,
                         value_type_from_string("REG_EXPAND_SZ"));

            assert_equal(REG_BINARY,
                         value_type_from_string("BINARY"));
            assert_equal(REG_BINARY,
                         value_type_from_string("REG_BINARY"));

            assert_equal(REG_DWORD,
                         value_type_from_string("DWORD"));
            assert_equal(REG_DWORD,
                         value_type_from_string("REG_DWORD"));

            assert_equal(REG_DWORD_BIG_ENDIAN,
                         value_type_from_string("DWORD_BIG_ENDIAN"));
            assert_equal(REG_DWORD_BIG_ENDIAN,
                         value_type_from_string("REG_DWORD_BIG_ENDIAN"));

            assert_equal(REG_LINK,
                         value_type_from_string("LINK"));
            assert_equal(REG_LINK,
                         value_type_from_string("REG_LINK"));

            assert_equal(REG_MULTI_SZ,
                         value_type_from_string("MULTI_SZ"));
            assert_equal(REG_MULTI_SZ,
                         value_type_from_string("REG_MULTI_SZ"));

            assert_equal(REG_RESOURCE_LIST,
                         value_type_from_string("RESOURCE_LIST"));
            assert_equal(REG_RESOURCE_LIST,
                         value_type_from_string("REG_RESOURCE_LIST"));

            assert_equal(REG_FULL_RESOURCE_DESCRIPTOR,
                         value_type_from_string("FULL_RESOURCE_DESCRIPTOR"));
            assert_equal(
                REG_FULL_RESOURCE_DESCRIPTOR,
                value_type_from_string("REG_FULL_RESOURCE_DESCRIPTOR"));

            assert_equal(REG_RESOURCE_REQUIREMENTS_LIST,
                         value_type_from_string("RESOURCE_REQUIREMENTS_LIST"));
            assert_equal(
                REG_RESOURCE_REQUIREMENTS_LIST,
                value_type_from_string("REG_RESOURCE_REQUIREMENTS_LIST"));

            assert_equal(REG_QWORD,
                         value_type_from_string("QWORD"));
            assert_equal(REG_QWORD,
                         value_type_from_string("REG_QWORD"));

            return true;
        }

        bool test_num_to_str1(void)
        {
            assert_equal(
                std::string("NONE"),
                std::string(value_type_get_string(REG_NONE)));

            assert_equal(
                std::string("SZ"),
                std::string(value_type_get_string(REG_SZ)));

            assert_equal(
                std::string("EXPAND_SZ"),
                std::string(value_type_get_string(REG_EXPAND_SZ)));

            assert_equal(
                std::string("BINARY"),
                std::string(value_type_get_string(REG_BINARY)));

            assert_equal(
                std::string("DWORD"),
                std::string(value_type_get_string(REG_DWORD)));

            assert_equal(
                std::string("DWORD_BIG_ENDIAN"),
                std::string(value_type_get_string(REG_DWORD_BIG_ENDIAN)));

            assert_equal(
                std::string("LINK"),
                std::string(value_type_get_string(REG_LINK)));

            assert_equal(
                std::string("MULTI_SZ"),
                std::string(value_type_get_string(REG_MULTI_SZ)));

            assert_equal(
                std::string("RESOURCE_LIST"),
                std::string(value_type_get_string(REG_RESOURCE_LIST)));

            assert_equal(
                std::string("FULL_RESOURCE_DESCRIPTOR"),
                std::string(value_type_get_string(
                                REG_FULL_RESOURCE_DESCRIPTOR)));

            assert_equal(
                std::string("RESOURCE_REQUIREMENTS_LIST"),
                std::string(value_type_get_string(
                                REG_RESOURCE_REQUIREMENTS_LIST)));

            assert_equal(
                std::string("QWORD"),
                std::string(value_type_get_string(REG_QWORD)));

            return true;
        }

        bool test_is_valid1(void)
        {
            assert_true(value_type_is_valid(REG_NONE));
            assert_true(value_type_is_valid(REG_SZ));
            assert_true(value_type_is_valid(REG_EXPAND_SZ));
            assert_true(value_type_is_valid(REG_BINARY));
            assert_true(value_type_is_valid(REG_DWORD));
            assert_true(value_type_is_valid(REG_DWORD_BIG_ENDIAN));
            assert_true(value_type_is_valid(REG_LINK));
            assert_true(value_type_is_valid(REG_MULTI_SZ));
            assert_true(value_type_is_valid(REG_RESOURCE_LIST));
            assert_true(value_type_is_valid(REG_FULL_RESOURCE_DESCRIPTOR));
            assert_true(value_type_is_valid(REG_RESOURCE_REQUIREMENTS_LIST));
            assert_true(value_type_is_valid(REG_QWORD));

            return true;
        }

        bool test_is_binary_type1(void)
        {
            assert_true(value_type_is_binary(REG_NONE));
            assert_false(value_type_is_binary(REG_SZ));
            assert_false(value_type_is_binary(REG_EXPAND_SZ));
            assert_true(value_type_is_binary(REG_BINARY));
            assert_false(value_type_is_binary(REG_DWORD));
            assert_false(value_type_is_binary(REG_DWORD_BIG_ENDIAN));
            assert_false(value_type_is_binary(REG_LINK));
            assert_false(value_type_is_binary(REG_MULTI_SZ));
            assert_true(value_type_is_binary(REG_RESOURCE_LIST));
            assert_true(value_type_is_binary(REG_FULL_RESOURCE_DESCRIPTOR));
            assert_true(value_type_is_binary(REG_RESOURCE_REQUIREMENTS_LIST));
            assert_false(value_type_is_binary(REG_QWORD));

            return true;
        }

        bool test_is_string_type1(void)
        {
            assert_false(value_type_is_string(REG_NONE));
            assert_true(value_type_is_string(REG_SZ));
            assert_true(value_type_is_string(REG_EXPAND_SZ));
            assert_false(value_type_is_string(REG_BINARY));
            assert_false(value_type_is_string(REG_DWORD));
            assert_false(value_type_is_string(REG_DWORD_BIG_ENDIAN));
            assert_true(value_type_is_string(REG_LINK));
            assert_true(value_type_is_string(REG_MULTI_SZ));
            assert_false(value_type_is_string(REG_RESOURCE_LIST));
            assert_false(value_type_is_string(REG_FULL_RESOURCE_DESCRIPTOR));
            assert_false(value_type_is_string(REG_RESOURCE_REQUIREMENTS_LIST));
            assert_false(value_type_is_string(REG_QWORD));

            return true;
        }

        bool test_is_integral_type1(void)
        {
            assert_false(value_type_is_integral(REG_NONE));
            assert_false(value_type_is_integral(REG_SZ));
            assert_false(value_type_is_integral(REG_EXPAND_SZ));
            assert_false(value_type_is_integral(REG_BINARY));
            assert_true(value_type_is_integral(REG_DWORD));
            assert_true(value_type_is_integral(REG_DWORD_BIG_ENDIAN));
            assert_false(value_type_is_integral(REG_LINK));
            assert_false(value_type_is_integral(REG_MULTI_SZ));
            assert_false(value_type_is_integral(REG_RESOURCE_LIST));
            assert_false(value_type_is_integral(REG_FULL_RESOURCE_DESCRIPTOR));
            assert_false(value_type_is_integral(REG_RESOURCE_REQUIREMENTS_LIST));
            assert_true(value_type_is_integral(REG_QWORD));

            return true;
        }

    public:

        test_value_type(): Melunit::Test("test_value_type")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_value_type:: name)

            REGISTER(test_str_to_num1);
            REGISTER(test_num_to_str1);
            REGISTER(test_is_valid1);
            REGISTER(test_is_binary_type1);
            REGISTER(test_is_string_type1);
            REGISTER(test_is_integral_type1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_value_type t1_;
}
