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

#include <rregadmin/util/path_info.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_path_info : public Melunit::Test
    {
    private:

        bool test_create1(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_WIN);
            assert_not_null(pi1);

            assert_equal(RRA_PATH_OPT_WIN, rra_path_info_get_mode(pi1));
            assert_equal(std::string("\\"), rra_path_info_get_delim(pi1));
            const char *delim1 = "/";
            assert_false(rra_path_info_is_delim(pi1, delim1));
            const char *delim2 = "\\";
            assert_true(rra_path_info_is_delim(pi1, delim2));

            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_delim1(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_WIN);
            assert_not_null(pi1);
            const char *delim_unix = "/";
            const char *delim_win  = "\\";

            assert_equal(std::string("\\"), rra_path_info_get_delim(pi1));
            assert_false(rra_path_info_is_delim(pi1, delim_unix));
            assert_true(rra_path_info_is_delim(pi1, delim_win));

            assert_true(rra_path_info_unset_option(
                            pi1, RRA_PATH_OPT_OUT_WIN_DELIMITER));
            assert_equal(std::string("/"), rra_path_info_get_delim(pi1));
            assert_false(rra_path_info_is_delim(pi1, delim_unix));
            assert_true(rra_path_info_is_delim(pi1, delim_win));

            assert_true(rra_path_info_set_option(
                            pi1, RRA_PATH_OPT_IN_UNIX_DELIMITER));
            assert_equal(std::string("/"), rra_path_info_get_delim(pi1));
            assert_true(rra_path_info_is_delim(pi1, delim_unix));
            assert_true(rra_path_info_is_delim(pi1, delim_win));

            assert_true(rra_path_info_set_option(
                            pi1, RRA_PATH_OPT_OUT_WIN_DELIMITER));
            assert_equal(std::string("\\"), rra_path_info_get_delim(pi1));
            assert_true(rra_path_info_is_delim(pi1, delim_unix));
            assert_true(rra_path_info_is_delim(pi1, delim_win));

            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_options1(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_WIN);
            assert_not_null(pi1);

            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_OUT_WIN_DELIMITER));
            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_CASE_INSENSITIVE));
            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_IN_WIN_DELIMITER));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_IN_XML_ESCAPES));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_OUT_MINIMAL_XML_ESCAPES));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_IN_UNIX_DELIMITER));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_OUT_XML_ESCAPES));

            assert_true(rra_path_info_set_mode(pi1, RRA_PATH_OPT_UNIX));
            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_IN_XML_ESCAPES));
            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_OUT_MINIMAL_XML_ESCAPES));
            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_IN_UNIX_DELIMITER));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_OUT_WIN_DELIMITER));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_CASE_INSENSITIVE));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_IN_WIN_DELIMITER));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_OUT_XML_ESCAPES));

            assert_true(rra_path_info_set_option(
                            pi1, RRA_PATH_OPT_OUT_XML_ESCAPES));
            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_IN_XML_ESCAPES));
            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_OUT_MINIMAL_XML_ESCAPES));
            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_IN_UNIX_DELIMITER));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_OUT_WIN_DELIMITER));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_CASE_INSENSITIVE));
            assert_false(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_IN_WIN_DELIMITER));
            assert_true(rra_path_info_has_option(
                            pi1, RRA_PATH_OPT_OUT_XML_ESCAPES));

            assert_true(rra_path_info_free(pi1));

            return true;
        }

        bool test_contains_delim1(void)
        {
            rra_path_info *pi1 = rra_path_info_new(RRA_PATH_OPT_WIN);

            const char *delim_unix = "foo/bar";
            const char *delim_win = "foo\\bar";
            const char *delim_both = "foo/bar\\baz";

            assert_false(rra_path_info_contains_delim(pi1, delim_unix));
            assert_true(rra_path_info_contains_delim(pi1, delim_win));
            assert_true(rra_path_info_contains_delim(pi1, delim_both));

            assert_true(rra_path_info_set_mode(pi1, RRA_PATH_OPT_UNIX));

            assert_true(rra_path_info_contains_delim(pi1, delim_unix));
            assert_false(rra_path_info_contains_delim(pi1, delim_win));
            assert_true(rra_path_info_contains_delim(pi1, delim_both));

            assert_true(rra_path_info_set_option(
                            pi1, RRA_PATH_OPT_IN_WIN_DELIMITER));

            assert_true(rra_path_info_contains_delim(pi1, delim_unix));
            assert_true(rra_path_info_contains_delim(pi1, delim_win));
            assert_true(rra_path_info_contains_delim(pi1, delim_both));

            assert_true(rra_path_info_free(pi1));

            return true;
        }

    public:

        test_path_info(): Melunit::Test("test_path_info")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_path_info:: name)

            REGISTER(test_create1);
            REGISTER(test_delim1);
            REGISTER(test_options1);
            REGISTER(test_contains_delim1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_path_info t1_;
}
