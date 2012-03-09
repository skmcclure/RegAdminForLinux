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

#include <stdlib.h>
#include <string.h>

#include <boost/assign/list_of.hpp>
#include <boost/format.hpp>

#include <melunit/melunit-cxx.h>

#include <glib/gmacros.h>

#include "ustring_to_string.hpp"

#include <rregadmin/util/ustring.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/init.h>

namespace
{
    using namespace boost::assign;

    class test_path : public Melunit::Test
    {
    private:

        bool test_new_empty()
        {
            rra_path *path1;

            assert_equal(0, rra_path_allocated_count());

            path1 = rra_path_new_unix("");
            assert_equal(1, rra_path_allocated_count());

            assert_not_null(path1);
            assert_equal(0, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_relative(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("."),
                         std::string(rra_path_dirname(path1)));
            assert_equal(std::string(""),
                         std::string(rra_path_basename(path1)));
            assert_equal(std::string(""),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_free(path1));
            assert_equal(0, rra_path_allocated_count());

            return true;
        }

        bool test_new_absolute()
        {
            rra_path *path1;
            assert_equal(0, rra_path_allocated_count());

            path1 = rra_path_new_unix("/");
            assert_equal(1, rra_path_allocated_count());

            assert_not_null(path1);
            assert_equal(0, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_relative(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/"),
                         std::string(rra_path_dirname(path1)));
            assert_equal(std::string(""),
                         std::string(rra_path_basename(path1)));
            assert_equal(std::string("/"),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_free(path1));
            assert_equal(0, rra_path_allocated_count());

            return true;
        }

        bool test_new_simple1()
        {
            rra_path *path1;
            assert_equal(0, rra_path_allocated_count());

            path1 = rra_path_new_unix("foo");
            assert_equal(1, rra_path_allocated_count());
            assert_not_null(path1);
            assert_equal(1, rra_path_number_of_parts(path1));
            assert_equal(std::string("foo"),
                         to_string(rra_path_get_part(path1, 0)));

            assert_true(rra_path_is_relative(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("."),
                         std::string(rra_path_dirname(path1)));
            assert_equal(std::string("foo"),
                         std::string(rra_path_basename(path1)));
            assert_equal(std::string("foo"),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_free(path1));
            assert_equal(0, rra_path_allocated_count());

            return true;
        }

        bool test_new_simple2()
        {
            rra_path *path1;

            path1 = rra_path_new_unix("foo/foo");
            assert_not_null(path1);
            assert_equal(2, rra_path_number_of_parts(path1));
            assert_equal(std::string("foo"),
                         to_string(rra_path_get_part(path1, 0)));
            assert_equal(std::string("foo"),
                         to_string(rra_path_get_part(path1, 1)));

            assert_true(rra_path_is_relative(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("foo"),
                         std::string(rra_path_dirname(path1)));
            assert_equal(std::string("foo"),
                         std::string(rra_path_basename(path1)));
            assert_equal(std::string("foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_free(path1));

            return true;
        }

        typedef std::vector<std::string> str_vec;

        struct parse_data
        {
            std::string input;
            bool absolute;
            str_vec parts;

            parse_data(std::string in_input,
                       bool in_absolute,
                       str_vec in_parts)
                : input(in_input),
                  absolute(in_absolute),
                  parts(in_parts)
            {
            }
        };

        bool test_parse1()
        {
            parse_data data1[] = {
                parse_data("./foo/.", false, list_of(".")("foo")(".")),
                parse_data("  foo / bar \\ bbbb", false,
                           list_of("  foo ")(" bar ")(" bbbb")),
                parse_data("\\HKEY_LOCAL_MACHINE\\software\\Microsoft\\Internet Explorer\\ActiveX Compatibility\\{00000566-0000-0010-8000-00AA006D2EA4}\\Compatibility Flags",
                           true,
                           list_of
                           ("HKEY_LOCAL_MACHINE")
                           ("software")
                           ("Microsoft")
                           ("Internet Explorer")
                           ("ActiveX Compatibility")
                           ("{00000566-0000-0010-8000-00AA006D2EA4}")
                           ("Compatibility Flags")),

                parse_data("/foo", true, list_of("foo")),
                parse_data("foo", false, list_of("foo"))
            };

            rra_path *path1 =
                rra_path_new_full("", (RRA_PATH_OPT_UNIX
                                       | RRA_PATH_OPT_IN_WIN_DELIMITER));

            for (int i = 0; i < G_N_ELEMENTS(data1); i++)
            {
                assert_true(rra_path_parse(path1, data1[i].input.c_str()));
                if (data1[i].absolute)
                {
                    assert_true(rra_path_is_absolute(path1));
                    assert_false(rra_path_is_relative(path1));
                }
                else
                {
                    assert_false(rra_path_is_absolute(path1));
                    assert_true(rra_path_is_relative(path1));
                }

                assert_equal_msg(data1[i].parts.size(),
                                 rra_path_number_of_parts(path1),
                                 data1[i].input);

                for (int j = 0; j < data1[i].parts.size(); j++)
                {
                    assert_equal(data1[i].parts[j],
                                 to_string(rra_path_get_part(path1, j)));
                }
            }

            assert_true(rra_path_free(path1));
            assert_equal(0, rra_path_allocated_count());

            return true;
        }

        bool test_parse_winmode1()
        {
            parse_data data1[] = {
                parse_data(".\\foo\\.", false, list_of(".")("foo")(".")),
                parse_data("  foo / bar \\ bbbb", false,
                           list_of("  foo / bar ")(" bbbb")),
                parse_data("\\HKEY_LOCAL_MACHINE\\software\\Microsoft\\Internet Explorer\\ActiveX Compatibility\\{00000566-0000-0010-8000-00AA006D2EA4}\\Compatibility Flags",
                           true,
                           list_of
                           ("HKEY_LOCAL_MACHINE")
                           ("software")
                           ("Microsoft")
                           ("Internet Explorer")
                           ("ActiveX Compatibility")
                           ("{00000566-0000-0010-8000-00AA006D2EA4}")
                           ("Compatibility Flags")),


                parse_data("\\foo", true, list_of("foo")),
                parse_data("foo", false, list_of("foo"))
            };

            rra_path *path1 = rra_path_new_win("");

            for (int i = 0; i < G_N_ELEMENTS(data1); i++)
            {
                assert_true(rra_path_parse(path1, data1[i].input.c_str()));
                if (data1[i].absolute)
                {
                    assert_true(rra_path_is_absolute(path1));
                    assert_false(rra_path_is_relative(path1));
                }
                else
                {
                    assert_false(rra_path_is_absolute(path1));
                    assert_true(rra_path_is_relative(path1));
                }

                assert_equal(data1[i].parts.size(),
                             rra_path_number_of_parts(path1));

                for (int j = 0; j < data1[i].parts.size(); j++)
                {
                    assert_equal(data1[i].parts[j],
                                 to_string(rra_path_get_part(path1, j)));
                }
            }

            assert_true(rra_path_free(path1));

            return true;
        }

        bool test_new_full_win_delim1()
        {
            rra_path *path1;

            path1 = rra_path_new_full("foo/bar",
                                      RRA_PATH_OPT_IN_UNIX_DELIMITER
                                      | RRA_PATH_OPT_IN_WIN_DELIMITER);
            assert_not_null(path1);
            assert_equal(2, rra_path_number_of_parts(path1));
            assert_equal(std::string("foo"),
                         to_string(rra_path_get_part(path1, 0)));
            assert_equal(std::string("bar"),
                         to_string(rra_path_get_part(path1, 1)));

            assert_equal("\\", rra_path_get_delimiter(path1));

            assert_true(rra_path_is_relative(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("foo"),
                         std::string(rra_path_dirname(path1)));
            assert_equal(std::string("bar"),
                         std::string(rra_path_basename(path1)));
            assert_equal(std::string("foo\\bar"),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_free(path1));

            return true;
        }

        bool test_clear1()
        {
            rra_path *path1 = rra_path_new_unix("foo/bar/baz");
            assert_not_null(path1);
            assert_true(rra_path_is_relative(path1));
            assert_equal(3, rra_path_number_of_parts(path1));

            assert_true(rra_path_clear(path1));
            assert_true(rra_path_is_relative(path1));
            assert_equal(0, rra_path_number_of_parts(path1));

            rra_path *path2 = rra_path_new_unix("/baz/bar");
            assert_not_null(path2);
            assert_true(rra_path_is_absolute(path2));
            assert_equal(2, rra_path_number_of_parts(path2));

            assert_true(rra_path_clear(path2));
            assert_true(rra_path_is_relative(path1));
            assert_equal(0, rra_path_number_of_parts(path1));

            rra_path_free(path1);
            rra_path_free(path2);
            assert_equal(0, rra_path_allocated_count());

            return true;
        }

        bool test_set1()
        {
            rra_path *path1 = rra_path_new_unix("foo/bar/baz");
            assert_not_null(path1);
            assert_true(rra_path_is_relative(path1));
            assert_equal(3, rra_path_number_of_parts(path1));

            rra_path *path2 = rra_path_new_unix("/baz/bar");
            assert_not_null(path2);
            assert_true(rra_path_is_absolute(path2));
            assert_equal(2, rra_path_number_of_parts(path2));

            assert_true(rra_path_set(path1, path2));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(2, rra_path_number_of_parts(path1));

            assert_true(rra_path_free(path1));
            assert_true(rra_path_free(path2));

            return true;
        }

        bool test_normalize1()
        {
            rra_path *path1;

            path1 = rra_path_new_unix("foo/nfoo");
            assert_not_null(path1);
            assert_equal(2, rra_path_number_of_parts(path1));
            assert_equal(std::string("foo/nfoo"),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_is_relative(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("foo"),
                         to_string(rra_path_get_part(path1, 0)));
            assert_equal(std::string("nfoo"),
                         to_string(rra_path_get_part(path1, 1)));

            assert_true(rra_path_normalize(path1));

            assert_equal(2, rra_path_number_of_parts(path1));

            assert_true(rra_path_is_relative(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("foo"),
                         to_string(rra_path_get_part(path1, 0)));
            assert_equal(std::string("nfoo"),
                         to_string(rra_path_get_part(path1, 1)));

            assert_true(rra_path_free(path1));
            return true;
        }

        struct norm_data
        {
            std::string input;
            bool absolute;
            str_vec parts_pre;
            str_vec parts_post;
            std::string post_str;

            norm_data(std::string in_input,
                      bool in_absolute,
                      str_vec in_parts_pre,
                      str_vec in_parts_post,
                      std::string in_post_str)
                : input(in_input),
                  absolute(in_absolute),
                  parts_pre(in_parts_pre),
                  parts_post(in_parts_post),
                  post_str(in_post_str)
            {
            }
        };

        bool test_normalize2()
        {
            norm_data data1[] = {
                norm_data("./foo/.", false, list_of(".")("foo")("."),
                          list_of("foo"), "foo"),
                norm_data("/./foo/.", true, list_of(".")("foo")("."),
                          list_of("foo"), "/foo"),
                norm_data("/./././././././.", true,
                          list_of(".")(".")(".")(".")(".")(".")(".")("."),
                          str_vec(), "/"),
                norm_data("././././././././.", false,
                          list_of(".")(".")(".")(".")(".")(".")(".")(".")("."),
                          str_vec(), ""),
                norm_data("././././.bar/.foo/baz././.", false,
                          list_of(".")(".")(".")(".")(".bar")(".foo")("baz.")(".")("."),
                          list_of(".bar")(".foo")("baz."),
                          ".bar/.foo/baz."),

                norm_data("../foo", false,
                          list_of("..")("foo"),
                          list_of("..")("foo"),
                          "../foo"),
                norm_data("../foo/..", false,
                          list_of("..")("foo")(".."),
                          list_of(".."),
                          ".."),
                norm_data("/../foo/..", true,
                          list_of("..")("foo")(".."),
                          str_vec(),
                          "/"),

                norm_data("foo", false, list_of("foo"), list_of("foo"),
                          "foo")
            };
            int data1_len = G_N_ELEMENTS(data1);

            for (int i = 0; i < data1_len; i++)
            {
                rra_path *path1 = rra_path_new_unix(data1[i].input.c_str());
                assert_not_null(path1);

                if (data1[i].absolute)
                {
                    assert_false(rra_path_is_relative(path1));
                    assert_true(rra_path_is_absolute(path1));
                }
                else
                {
                    assert_true(rra_path_is_relative(path1));
                    assert_false(rra_path_is_absolute(path1));
                }

                int pre_num = data1[i].parts_pre.size();
                assert_equal_msg(pre_num, rra_path_number_of_parts(path1),
                                 data1[i].input);
                for (int j = 0; j < pre_num; j++)
                {
                    assert_equal(data1[i].parts_pre[j],
                                 to_string(rra_path_get_part(path1, j)));
                }
                assert_equal(data1[i].input,
                             std::string(rra_path_as_str(path1)));

                assert_true(rra_path_normalize(path1));

                if (data1[i].absolute)
                {
                    assert_false(rra_path_is_relative(path1));
                    assert_true(rra_path_is_absolute(path1));
                }
                else
                {
                    assert_true(rra_path_is_relative(path1));
                    assert_false(rra_path_is_absolute(path1));
                }

                int post_num = data1[i].parts_post.size();
                assert_equal_msg(post_num, rra_path_number_of_parts(path1),
                                 data1[i].input);
                for (int j = 0; j < post_num; j++)
                {
                    assert_equal(data1[i].parts_post[j],
                                 to_string(rra_path_get_part(path1, j)));
                }

                assert_equal(data1[i].post_str,
                             std::string(rra_path_as_str(path1)));

                assert_true(rra_path_free(path1));
            }

            return true;
        }

        bool test_add1()
        {
            rra_path *path1 = rra_path_new_unix("foo/foo");
            assert_not_null(path1);
            rra_path *path2 = rra_path_new_unix("bar");

            assert_true(rra_path_add(path1, path2));

            assert_equal(3, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_relative(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("foo/foo"),
                         std::string(rra_path_dirname(path1)));
            assert_equal(std::string("bar"),
                         std::string(rra_path_basename(path1)));
            assert_equal(std::string("foo/foo/bar"),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_free(path1));
            assert_true(rra_path_free(path2));

            return true;
        }

        bool test_add2()
        {
            rra_path *path1 = rra_path_new_unix("foo/foo");
            assert_not_null(path1);
            rra_path *path2 = rra_path_new_unix("/bar");

            assert_false(rra_path_add(path1, path2));

            assert_true(rra_path_free(path1));
            assert_true(rra_path_free(path2));

            return true;
        }

        bool test_add3()
        {
            rra_path *path1 = rra_path_new_unix("/foo/foo");
            assert_not_null(path1);
            rra_path *path2 = rra_path_new_unix("bar");

            assert_true(rra_path_add(path1, path2));

            assert_equal(3, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_relative(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/foo/foo"),
                         std::string(rra_path_dirname(path1)));
            assert_equal(std::string("bar"),
                         std::string(rra_path_basename(path1)));
            assert_equal(std::string("/foo/foo/bar"),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_free(path1));
            assert_true(rra_path_free(path2));

            return true;
        }

        struct compare_data
        {
            std::string path1;
            std::string path2;
            gint ret;

            compare_data(const std::string &in_path1,
                         const std::string &in_path2,
                         gint in_ret)
                : path1(in_path1),
                  path2(in_path2),
                  ret(in_ret)
            {
            }
        };

        bool test_compare1()
        {
            compare_data data1[] = {
                compare_data("/./foo", "/foo", 0),
                compare_data("/foo", "foo", 1),
                compare_data("/foo/bar/..", "/foo", 0),
                compare_data("/foo", "/foo/bar", -1),

                compare_data("foo", "foo", 0)
            };

            int data1_len = G_N_ELEMENTS(data1);

            for (int i = 0; i < data1_len; i++)
            {
                rra_path *p1 = rra_path_new_unix(data1[i].path1.c_str());
                rra_path *p2 = rra_path_new_unix(data1[i].path2.c_str());

                if (data1[i].ret == 0)
                {
                    assert_equal_msg(0, rra_path_compare(p1, p2),
                                     data1[i].path1);
                }
                else if (data1[i].ret > 0)
                {
                    assert_compare(std::less, 0, rra_path_compare(p1, p2));
                }
                else if (data1[i].ret < 0)
                {
                    assert_compare(std::greater, 0, rra_path_compare(p1, p2));
                }

                if (data1[i].ret == 0)
                {
                    assert_true(rra_path_equal(p1, p2));
                }
                else if (data1[i].ret > 0)
                {
                    assert_true(rra_path_less(p2, p1));
                }
                else
                {
                    assert_true(rra_path_less(p1, p2));
                }

                assert_true(rra_path_free(p2));
                assert_true(rra_path_free(p1));
            }

            return true;
        }

        struct subpath_data
        {
            std::string path1;
            std::string path2;

            gboolean is_subpath;
            std::string subpath;

            subpath_data(const std::string &in_path1,
                         const std::string &in_path2,
                         gint in_is_subpath,
                         const std::string &in_subpath = "")
                : path1(in_path1),
                  path2(in_path2),
                  is_subpath(in_is_subpath),
                  subpath(in_subpath)
            {
            }
        };

        bool test_subpath1()
        {
            subpath_data data1[] = {
                subpath_data("/./foo", "/foo", TRUE, ""),
                subpath_data("/foo", "foo", FALSE),
                subpath_data("/foo/bar/..", "/foo", TRUE, ""),
                subpath_data("/foo", "/foo/bar", TRUE, "bar"),

                subpath_data("foo", "foo", TRUE, "")
            };

            int data1_len = G_N_ELEMENTS(data1);

            for (int i = 0; i < data1_len; i++)
            {
                rra_path *p1 = rra_path_new_unix(data1[i].path1.c_str());
                rra_path *p2 = rra_path_new_unix(data1[i].path2.c_str());

                assert_equal(data1[i].is_subpath,
                             rra_path_is_subpath(p1, p2));

                if (data1[i].is_subpath)
                {
                    rra_path *sp1 =
                        rra_path_new_unix(data1[i].subpath.c_str());
                    assert_not_null(sp1);
                    rra_path *sp2 = rra_path_make_subpath(p1, p2);
                    assert_not_null(sp2);

                    assert_true(rra_path_equal(sp1, sp2));

                    rra_path_free(sp2);
                    rra_path_free(sp1);
                }

                rra_path_free(p2);
                rra_path_free(p1);
            }

            return true;
        }

        bool test_subpath2()
        {
            subpath_data data1[] = {
                subpath_data("\\.\\foo", "\\foo", TRUE, ""),
                subpath_data("\\foo", "foo", FALSE),
                subpath_data("\\foo\\bar\\..", "\\foo", TRUE, ""),
                subpath_data("\\foo", "\\foo\\bar", TRUE, "bar"),

                subpath_data("foo", "foo", TRUE, "")
            };

            int data1_len = G_N_ELEMENTS(data1);

            for (int i = 0; i < data1_len; i++)
            {
                rra_path *p1 = rra_path_new_win(data1[i].path1.c_str());
                rra_path *p2 = rra_path_new_win(data1[i].path2.c_str());

                assert_equal(data1[i].is_subpath,
                             rra_path_is_subpath(p1, p2));

                if (data1[i].is_subpath)
                {
                    rra_path *sp1 = rra_path_new_win(data1[i].subpath.c_str());

                    assert_not_null(sp1);
                    rra_path *sp2 = rra_path_make_subpath(p1, p2);
                    assert_not_null(sp2);

                    assert_true(rra_path_equal(sp1, sp2));

                    rra_path_free(sp2);
                    rra_path_free(sp1);
                }

                rra_path_free(p2);
                rra_path_free(p1);
            }

            return true;
        }

        bool test_subpath_case_insensitive1()
        {
            subpath_data data1[] = {
                subpath_data("/./foo", "/Foo", TRUE, ""),
                subpath_data("/foo", "foo", FALSE),
                subpath_data("/fOo/bar/..", "/FoO", TRUE, ""),
                subpath_data("/foO", "/foo/bar", TRUE, "bar"),

                subpath_data("fOO", "foo", TRUE, "")
            };

            int data1_len = G_N_ELEMENTS(data1);

            for (int i = 0; i < data1_len; i++)
            {
                rra_path *p1 =
                    rra_path_new_full(data1[i].path1.c_str(),
                                      RRA_PATH_OPT_MIX);
                rra_path *p2 = rra_path_new_unix(data1[i].path2.c_str());

                assert_equal(data1[i].is_subpath,
                             rra_path_is_subpath(p1, p2));

                if (data1[i].is_subpath)
                {
                    rra_path *sp1 =
                        rra_path_new_unix(data1[i].subpath.c_str());
                    assert_not_null(sp1);
                    rra_path *sp2 = rra_path_make_subpath(p1, p2);
                    assert_not_null(sp2);

                    assert_true(rra_path_equal(sp1, sp2));

                    rra_path_free(sp2);
                    rra_path_free(sp1);
                }

                rra_path_free(p2);
                rra_path_free(p1);
            }

            return true;
        }

        bool test_subpath_case_insensitive2()
        {
            subpath_data data1[] = {
                subpath_data("\\.\\foo", "\\Foo", TRUE, ""),
                subpath_data("\\foo", "foo", FALSE),
                subpath_data("\\fOo", "\\FoO\\bar", TRUE, "bar"),
                subpath_data("\\foO\\bar", "\\foo\\bar\\baz", TRUE, "baz"),

                subpath_data("fOO", "foo", TRUE, "")
            };

            int data1_len = G_N_ELEMENTS(data1);

            for (int i = 0; i < data1_len; i++)
            {
                rra_path *p1 = rra_path_new_win(data1[i].path1.c_str());
                rra_path *p2 = rra_path_new_win(data1[i].path2.c_str());

                assert_equal_msg(data1[i].is_subpath,
                                 rra_path_is_subpath(p1, p2),
                                 data1[i].path1);

                if (data1[i].is_subpath)
                {
                    rra_path *sp1 = rra_path_new_win(data1[i].subpath.c_str());

                    assert_not_null(sp1);
                    rra_path *sp2 = rra_path_make_subpath(p1, p2);
                    assert_not_null(sp2);

                    assert_true(rra_path_equal(sp1, sp2));

                    rra_path_free(sp2);
                    rra_path_free(sp1);
                }

                rra_path_free(p2);
                rra_path_free(p1);
            }

            return true;
        }

        struct trim_data
        {
            std::string before;
            std::string after;
            gboolean trim_succeed;
            int trim_amount;

            trim_data(const std::string &in_before,
                      const std::string &in_after,
                      gboolean in_trim_succeed,
                      int in_trim_amount)
                : before(in_before),
                  after(in_after),
                  trim_succeed(in_trim_succeed),
                  trim_amount(in_trim_amount)
            {
            }

            std::string as_str(void)
            {
                return (boost::format(
                            "trim_data(before(%1%), after(%2%), succ(%3%), "
                            "amount(%4%))")
                        % before % after % trim_succeed % trim_amount).str();
            }
        };

        bool test_trim_front1()
        {
            trim_data data1[] = {
                trim_data("/foo/bar/baz", "foo/bar/baz", TRUE, 1),

                trim_data("", "", FALSE, 1),
                trim_data("/", "", TRUE, 1),
                trim_data("/foo/bar", "/foo/bar", FALSE, 4),
                trim_data("/foo/bar", "/foo/bar", FALSE, 10),
                trim_data("/foo/bar", "/foo/bar", FALSE, 20),
                trim_data("/foo/bar", "/foo/bar", FALSE, 100),
                trim_data("/foo/bar", "", TRUE, 3),

                trim_data("bar/baz/foo", "bar/baz/foo", TRUE, 0),
                trim_data("bar/baz/foo", "baz/foo", TRUE, 1),
                trim_data("bar/baz/foo", "foo", TRUE, 2),
                trim_data("bar/baz/foo", "", TRUE, 3),
                trim_data("bar/baz/foo", "bar/baz/foo", FALSE, 4),

                trim_data("foo", "", TRUE, 1)
            };
            int data1_len = G_N_ELEMENTS(data1);

            for (int i = 0; i < data1_len; i++)
            {
                rra_path *p1 = rra_path_new_unix(data1[i].before.c_str());
                assert_not_null(p1);

                rra_path *p2 = rra_path_copy(p1);
                if (data1[i].trim_succeed)
                {
                    assert_true(rra_path_trim_front(p2, data1[i].trim_amount));

                    if (rra_path_is_absolute(p1))
                    {
                        assert_equal_msg(
                            rra_path_number_of_parts(p1)
                            - data1[i].trim_amount
                            + 1,
                            rra_path_number_of_parts(p2),
                            data1[i].as_str());
                        assert_true(rra_path_is_relative(p2));
                        assert_false(rra_path_is_absolute(p2));
                    }
                    else
                    {
                        assert_equal_msg(
                            rra_path_number_of_parts(p1) - data1[i].trim_amount,
                            rra_path_number_of_parts(p2),
                            data1[i].as_str());
                    }
                    assert_equal(data1[i].after,
                                 std::string(rra_path_as_str(p2)));
                }
                else
                {
                    assert_false(rra_path_trim_front(p2, data1[i].trim_amount));
                }
                assert_true(rra_path_free(p1));
                assert_true(rra_path_free(p2));
            }

            return true;
        }

        bool test_trim_back1()
        {
            trim_data data1[] = {
                trim_data("/foo/bar/baz", "/foo/bar", TRUE, 1),

                trim_data("", "", FALSE, 1),
                trim_data("/", "", TRUE, 1),
                trim_data("/foo/bar", "/foo/bar", FALSE, 4),
                trim_data("/foo/bar", "/foo/bar", FALSE, 10),
                trim_data("/foo/bar", "/foo/bar", FALSE, 20),
                trim_data("/foo/bar", "/foo/bar", FALSE, 100),
                trim_data("/foo/bar", "", TRUE, 3),
                trim_data("/foo/bar", "/", TRUE, 2),
                trim_data("/foo/bar", "/foo", TRUE, 1),

                trim_data("bar/baz/foo", "bar/baz/foo", TRUE, 0),
                trim_data("bar/baz/foo", "bar/baz", TRUE, 1),
                trim_data("bar/baz/foo", "bar", TRUE, 2),
                trim_data("bar/baz/foo", "", TRUE, 3),
                trim_data("bar/baz/foo", "bar/baz/foo", FALSE, 4),

                trim_data("foo", "", TRUE, 1)
            };
            int data1_len = G_N_ELEMENTS(data1);

            for (int i = 0; i < data1_len; i++)
            {
                rra_path *p1 = rra_path_new_unix(data1[i].before.c_str());
                assert_not_null(p1);

                rra_path *p2 = rra_path_copy(p1);
                if (data1[i].trim_succeed)
                {
                    assert_true(rra_path_trim_back(p2, data1[i].trim_amount));

                    if (rra_path_is_absolute(p1))
                    {
                        if (data1[i].trim_amount > rra_path_number_of_parts(p1))
                        {
                            assert_equal_msg(0, rra_path_number_of_parts(p2),
                                             data1[i].as_str());
                            assert_true(rra_path_is_relative(p2));
                            assert_false(rra_path_is_absolute(p2));
                        }
                        else
                        {
                            assert_equal_msg(
                                rra_path_number_of_parts(p1)
                                - data1[i].trim_amount,
                                rra_path_number_of_parts(p2),
                                data1[i].as_str());
                        }
                    }
                    else
                    {
                        assert_equal_msg(
                            rra_path_number_of_parts(p1) - data1[i].trim_amount,
                            rra_path_number_of_parts(p2),
                            data1[i].as_str());
                    }
                    assert_equal(data1[i].after,
                                 std::string(rra_path_as_str(p2)));
                }
                else
                {
                    assert_false(rra_path_trim_back(p2, data1[i].trim_amount));
                }

                rra_path_free(p1);
                rra_path_free(p2);
            }

            return true;
        }

        bool test_append1(void)
        {
            rra_path *path1 = rra_path_new_unix("/foo/foo");
            assert_not_null(path1);

            assert_equal(2, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));

            assert_true(rra_path_append_part(path1, "baz"));
            assert_equal(3, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/foo/foo/baz"),
                         std::string(rra_path_as_str(path1)));

            rra_path_free(path1);
            return true;
        }

        bool test_append2(void)
        {
            rra_path *path1 = rra_path_new_unix("/foo/foo");
            assert_not_null(path1);

            assert_equal(2, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));

            assert_false(rra_path_append_part(path1, ""));
            assert_equal(2, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_false(rra_path_append_part(path1, "bla/boo"));
            assert_equal(2, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_false(rra_path_append_part(path1, NULL));
            assert_equal(2, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_false(rra_path_append_part(NULL, "foo"));

            rra_path_free(path1);
            return true;
        }

        bool test_prepend1(void)
        {
            rra_path *path1 = rra_path_new_unix("foo/foo");
            assert_not_null(path1);

            assert_equal(2, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_absolute(path1));

            assert_true(rra_path_prepend_part(path1, "baz"));
            assert_equal(3, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("baz/foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_prepend_part(path1, "/"));
            assert_equal(3, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/baz/foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_false(rra_path_prepend_part(path1, "bee"));
            assert_equal(3, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/baz/foo/foo"),
                         std::string(rra_path_as_str(path1)));

            rra_path_free(path1);
            return true;
        }

        bool test_prepend2(void)
        {
            rra_path *path1 = rra_path_new_unix("foo/foo");
            assert_not_null(path1);

            assert_equal(2, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_absolute(path1));

            assert_true(rra_path_prepend_part(path1, "baz"));
            assert_equal(3, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("baz/foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_true(rra_path_prepend_part(path1, "/"));
            assert_equal(3, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/baz/foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_false(rra_path_prepend_part(path1, "bee"));
            assert_equal(3, rra_path_number_of_parts(path1));
            assert_true(rra_path_is_absolute(path1));
            assert_equal(std::string("/baz/foo/foo"),
                         std::string(rra_path_as_str(path1)));

            rra_path_free(path1);
            return true;
        }

        bool test_prepend3(void)
        {
            rra_path *path1 = rra_path_new_unix("foo/foo");
            assert_not_null(path1);

            assert_equal(2, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_absolute(path1));

            assert_false(rra_path_prepend_part(path1, ""));
            assert_equal(2, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_false(rra_path_prepend_part(path1, "bla/boo"));
            assert_equal(2, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_false(rra_path_prepend_part(path1, NULL));
            assert_equal(2, rra_path_number_of_parts(path1));
            assert_false(rra_path_is_absolute(path1));
            assert_equal(std::string("foo/foo"),
                         std::string(rra_path_as_str(path1)));

            assert_false(rra_path_prepend_part(NULL, "foo"));

            rra_path_free(path1);
            return true;
        }

        bool test_default1()
        {
            rra_path *path1 = rra_path_new_win("\\HKEY_LOCAL_MACHINE\\system");
            rra_path *path2 = rra_path_new_win("\\HKEY_LOCAL_MACHINE\\System");

            assert_true(rra_path_equal(path1, path2));
            assert_true(rra_path_free(path1));
            assert_true(rra_path_free(path2));

            return true;
        }

        bool test_alloc_count1(void)
        {
            const int count1 = 10;
            rra_path *ps1[count1];
            int i;

            assert_equal(0, rra_path_allocated_count());

            for (i = 0; i < count1; i++)
            {
                ps1[i] = rra_path_new_unix("\foo");
                assert_equal(i + 1, rra_path_allocated_count());
            }

            assert_equal(count1, rra_path_allocated_count());

            for (i = 0; i < count1; i++)
            {
                rra_path_free(ps1[i]);
                assert_equal(count1 - 1 - i, rra_path_allocated_count());
            }

            assert_equal(0, rra_path_allocated_count());

            return true;
        }

    public:

        test_path(): Melunit::Test("test_path")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_path:: name)

            REGISTER(test_new_empty);
            REGISTER(test_new_absolute);
            REGISTER(test_new_simple1);
            REGISTER(test_new_simple2);
            REGISTER(test_parse1);
            REGISTER(test_parse_winmode1);
            REGISTER(test_set1);
            REGISTER(test_clear1);
            REGISTER(test_normalize1);
            REGISTER(test_normalize2);
            REGISTER(test_add1);
            REGISTER(test_add2);
            REGISTER(test_add3);
            REGISTER(test_compare1);
            REGISTER(test_subpath1);
            REGISTER(test_subpath_case_insensitive1);
            REGISTER(test_subpath2);
            REGISTER(test_subpath_case_insensitive2);
            REGISTER(test_trim_front1);
            REGISTER(test_trim_back1);
            REGISTER(test_append1);
            REGISTER(test_append2);
            REGISTER(test_prepend1);
            REGISTER(test_prepend2);
            REGISTER(test_prepend3);
            REGISTER(test_default1);
            REGISTER(test_alloc_count1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_path t1_;
}
