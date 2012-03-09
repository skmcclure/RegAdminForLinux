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

#include <rregadmin/util/ustring_list.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_ustring_list : public Melunit::Test
    {
    private:

        bool test_create(void)
        {
            ustring_list *ul1 = ustrlist_new();
            assert_not_null(ul1);
            assert_equal(1, ustrlist_allocated_count());

            assert_true(ustrlist_is_valid(ul1));
            assert_false(ustrlist_clear(ul1));

            assert_equal(0, ustrlist_size(ul1));
            assert_null(ustrlist_get(ul1, 0));
            assert_null(ustrlist_get(ul1, 1));

            assert_true(ustrlist_free(ul1));
            assert_equal(0, ustrlist_allocated_count());

            return true;
        }

        bool test_create_list(void)
        {
            ustring_list *ul1 =
                ustrlist_new_list(ustr_create("foo"),
                                  ustr_create("bar"),
                                  NULL);
            assert_not_null(ul1);
            assert_equal(1, ustrlist_allocated_count());
            assert_equal(2, ustr_allocated_count());

            assert_true(ustrlist_is_valid(ul1));

            assert_equal(2, ustrlist_size(ul1));
            assert_equal(std::string("foo"),
                         ustr_as_utf8(ustrlist_get(ul1, 0)));
            assert_equal(std::string("bar"),
                         ustr_as_utf8(ustrlist_get(ul1, 1)));

            assert_true(ustrlist_clear(ul1));
            assert_equal(0, ustr_allocated_count());
            assert_equal(0, ustrlist_size(ul1));
            assert_null(ustrlist_get(ul1, 0));
            assert_null(ustrlist_get(ul1, 1));

            assert_true(ustrlist_free(ul1));
            assert_equal(0, ustrlist_allocated_count());
            assert_equal(0, ustr_allocated_count());

            return true;
        }

        bool test_copy(void)
        {
            ustring_list *ul1 =
                ustrlist_new_list(ustr_create("foo"),
                                  ustr_create("bar"),
                                  NULL);
            assert_not_null(ul1);
            assert_equal(1, ustrlist_allocated_count());
            assert_equal(2, ustr_allocated_count());

            ustring_list *ul2 = ustrlist_copy(ul1);
            assert_not_null(ul2);
            assert_equal(2, ustrlist_allocated_count());
            assert_equal(4, ustr_allocated_count());

            assert_equal(2, ustrlist_size(ul2));
            assert_equal(std::string("foo"),
                         ustr_as_utf8(ustrlist_get(ul2, 0)));
            assert_equal(std::string("bar"),
                         ustr_as_utf8(ustrlist_get(ul2, 1)));

            assert_true(ustrlist_free(ul1));
            assert_equal(1, ustrlist_allocated_count());
            assert_equal(2, ustr_allocated_count());

            assert_true(ustrlist_free(ul2));
            assert_equal(0, ustrlist_allocated_count());
            assert_equal(0, ustr_allocated_count());

            return true;
        }

        bool test_compare(void)
        {
            ustring_list *ul1 =
                ustrlist_new_list(ustr_create("foo"),
                                  ustr_create("bar"),
                                  NULL);
            ustring_list *ul2 =
                ustrlist_new_list(ustr_create("foo"),
                                  ustr_create("bar"),
                                  NULL);
            ustring_list *ul3 =
                ustrlist_new_list(ustr_create("foa"),
                                  ustr_create("bar"),
                                  NULL);
            ustring_list *ul4 =
                ustrlist_new_list(ustr_create("foo"),
                                  ustr_create("bab"),
                                  NULL);
            ustring_list *ul5 =
                ustrlist_new_list(ustr_create("foo"),
                                  NULL);

            assert_equal(0, ustrlist_compare(NULL, NULL));
            assert_equal(0, ustrlist_compare(ul1, ul2));
            assert_compare(std::less, 0, ustrlist_compare(ul1, NULL));
            assert_compare(std::greater, 0, ustrlist_compare(NULL, ul2));

            assert_compare(std::less, 0, ustrlist_compare(ul1, ul3));
            assert_compare(std::greater, 0, ustrlist_compare(ul3, ul1));

            assert_compare(std::less, 0, ustrlist_compare(ul1, ul4));
            assert_compare(std::greater, 0, ustrlist_compare(ul4, ul1));

            assert_compare(std::less, 0, ustrlist_compare(ul4, ul3));
            assert_compare(std::greater, 0, ustrlist_compare(ul3, ul4));

            assert_compare(std::greater, 0, ustrlist_compare(ul5, ul4));
            assert_compare(std::less, 0, ustrlist_compare(ul4, ul5));

            assert_true(ustrlist_free(ul1));
            assert_true(ustrlist_free(ul2));
            assert_true(ustrlist_free(ul3));
            assert_true(ustrlist_free(ul4));
            assert_true(ustrlist_free(ul5));

            assert_equal(0, ustrlist_allocated_count());
            assert_equal(0, ustr_allocated_count());

            return true;
        }

    public:

        test_ustring_list(): Melunit::Test("test_ustring_list")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_ustring_list:: name)

            REGISTER(test_create);
            REGISTER(test_create_list);
            REGISTER(test_copy);
            REGISTER(test_compare);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_ustring_list t1_;
}
