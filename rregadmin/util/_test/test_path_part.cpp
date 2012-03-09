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


#include <melunit/melunit-cxx.h>

#include <rregadmin/util/path_part.h>
#include <rregadmin/util/path_info.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_path_part : public Melunit::Test
    {
    private:

        bool test_create_empty1(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_UNIX);
            assert_not_null(pi1);
            rra_path_part *pp1 = rra_path_part_new(pi1);
            assert_not_null(pp1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_get_token_unix1(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_UNIX);
            rra_path_part *pp1 = rra_path_part_new(pi1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            const char *data1 = "\\";
            const char *data1_ptr = data1;

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\0', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            const char *data2 = "/";
            const char *data2_ptr = data2;

            assert_true(rra_path_part_grab_token(pp1, &data2_ptr));
            assert_equal('\0', data2_ptr[0]);
            assert_equal(RRA_PATH_PART_DELIM, rra_path_part_get_type(pp1));
            assert_equal(std::string("/"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_clear_unix2(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_UNIX);
            rra_path_part *pp1 = rra_path_part_new(pi1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            const char *data1 = "\\";
            const char *data1_ptr = data1;

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\0', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_clear(pp1));
            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_get_token_unix3(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_UNIX);
            rra_path_part *pp1 = rra_path_part_new(pi1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            const char *data1 = "\\foo/bar";
            const char *data1_ptr = data1;

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('/', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\foo"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('b', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_DELIM, rra_path_part_get_type(pp1));
            assert_equal(std::string("/"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\0', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("bar"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_get_token_unix4(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_UNIX);
            rra_path_part *pp1 = rra_path_part_new_element(pi1, "foobar");
            assert_not_null(pp1);

            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_free(pp1));

            pp1 = rra_path_part_new_element(pi1, "foo/bar");
            assert_null(pp1);

            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_get_token_win1(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_WIN);
            rra_path_part *pp1 = rra_path_part_new(pi1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            const char *data1 = "/";
            const char *data1_ptr = data1;

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\0', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("/"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            const char *data2 = "\\";
            const char *data2_ptr = data2;

            assert_true(rra_path_part_grab_token(pp1, &data2_ptr));
            assert_equal('\0', data2_ptr[0]);
            assert_equal(RRA_PATH_PART_DELIM, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_clear_win2(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_WIN);
            rra_path_part *pp1 = rra_path_part_new(pi1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            const char *data1 = "/";
            const char *data1_ptr = data1;

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\0', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("/"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_clear(pp1));
            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_get_token_win3(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_WIN);
            rra_path_part *pp1 = rra_path_part_new(pi1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            const char *data1 = "/foo\\bar";
            const char *data1_ptr = data1;

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\\', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("/foo"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('b', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_DELIM, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\0', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("bar"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_get_token_win4(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_WIN);
            rra_path_part *pp1 = rra_path_part_new_element(pi1, "foobar");
            assert_not_null(pp1);

            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_free(pp1));

            pp1 = rra_path_part_new_element(pi1, "foo\\bar");
            assert_null(pp1);

            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_get_token_mix1(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_MIX);
            rra_path_part *pp1 = rra_path_part_new(pi1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            const char *data1 = "/";
            const char *data1_ptr = data1;

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\0', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_DELIM, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            const char *data2 = "\\";
            const char *data2_ptr = data2;

            assert_true(rra_path_part_grab_token(pp1, &data2_ptr));
            assert_equal('\0', data2_ptr[0]);
            assert_equal(RRA_PATH_PART_DELIM, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_clear_mix2(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_MIX);
            rra_path_part *pp1 = rra_path_part_new(pi1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            const char *data1 = "/";
            const char *data1_ptr = data1;

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\0', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_DELIM, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_clear(pp1));
            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_get_token_mix3(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_MIX);
            rra_path_part *pp1 = rra_path_part_new(pi1);

            assert_equal(RRA_PATH_PART_EMPTY, rra_path_part_get_type(pp1));
            assert_null(rra_path_part_get_value(pp1));

            const char *data1 = "/foo\\bar";
            const char *data1_ptr = data1;

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('f', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_DELIM, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\\', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('b', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_DELIM, rra_path_part_get_type(pp1));
            assert_equal(std::string("\\"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_grab_token(pp1, &data1_ptr));
            assert_equal('\0', data1_ptr[0]);
            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("bar"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_free(pp1));
            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_get_token_mix4(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_MIX);
            rra_path_part *pp1 = rra_path_part_new_element(pi1, "foobar");
            assert_not_null(pp1);

            assert_equal(RRA_PATH_PART_ELEMENT, rra_path_part_get_type(pp1));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(
                                         rra_path_part_get_value(pp1))));

            assert_true(rra_path_part_free(pp1));

            pp1 = rra_path_part_new_element(pi1, "foo\\bar");
            assert_null(pp1);

            pp1 = rra_path_part_new_element(pi1, "foo/bar");
            assert_null(pp1);

            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_compare1(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_MIX);
            rra_path_info *pi2 = rra_path_info_new(RRA_PATH_OPT_WIN);
            rra_path_info *pi3 = rra_path_info_new(RRA_PATH_OPT_UNIX);

            rra_path_part *pp1_1 = rra_path_part_new_element(pi1, "foobar");
            rra_path_part *pp2_1 = rra_path_part_new_element(pi1, "FooBar");

            rra_path_part *pp1_2 = rra_path_part_new_element(pi2, "foobar");
            rra_path_part *pp2_2 = rra_path_part_new_element(pi2, "FooBar");

            rra_path_part *pp1_3 = rra_path_part_new_element(pi3, "foobar");
            rra_path_part *pp2_3 = rra_path_part_new_element(pi3, "FooBar");

            assert_equal(0, rra_path_part_compare(NULL, NULL));
            assert_compare(std::greater, 0,
                           rra_path_part_compare(NULL, pp1_1));
            assert_compare(std::less, 0,
                           rra_path_part_compare(pp1_1, NULL));

            assert_equal(0, rra_path_part_compare(pp1_1, pp2_1));
            assert_equal(0, rra_path_part_compare(pp1_2, pp2_2));
            assert_compare(std::less, 0,
                           rra_path_part_compare(pp1_3, pp2_3));
            assert_compare(std::greater, 0,
                           rra_path_part_compare(pp2_3, pp1_3));

            assert_equal(0, rra_path_part_compare(pp1_1, pp1_2));
            assert_equal(0, rra_path_part_compare(pp1_1, pp2_2));
            assert_equal(0, rra_path_part_compare(pp1_1, pp1_3));
            assert_equal(0, rra_path_part_compare(pp1_1, pp2_3));

            assert_equal(0, rra_path_part_compare(pp1_2, pp1_3));
            assert_equal(0, rra_path_part_compare(pp1_2, pp2_3));

            assert_true(rra_path_part_free(pp1_1));
            assert_true(rra_path_part_free(pp2_1));
            assert_true(rra_path_part_free(pp1_2));
            assert_true(rra_path_part_free(pp2_2));
            assert_true(rra_path_part_free(pp1_3));
            assert_true(rra_path_part_free(pp2_3));

            assert_true(rra_path_info_free(pi1));
            assert_true(rra_path_info_free(pi2));
            assert_true(rra_path_info_free(pi3));

            return true;
        }

    public:

        test_path_part(): Melunit::Test("test_path_part")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_path_part:: name)

            REGISTER(test_create_empty1);

            REGISTER(test_get_token_unix1);
            REGISTER(test_clear_unix2);
            REGISTER(test_get_token_unix3);
            REGISTER(test_get_token_unix4);

            REGISTER(test_get_token_win1);
            REGISTER(test_clear_win2);
            REGISTER(test_get_token_win3);
            REGISTER(test_get_token_win4);

            REGISTER(test_get_token_mix1);
            REGISTER(test_clear_mix2);
            REGISTER(test_get_token_mix3);
            REGISTER(test_get_token_mix4);

            REGISTER(test_compare1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_path_part t1_;
}
