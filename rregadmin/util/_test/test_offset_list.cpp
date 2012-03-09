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

#include <glib/gmem.h>

#include <rregadmin/util/offsets.h>
#include <rregadmin/util/offset_list.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_offset_list : public Melunit::Test
    {
    private:

        offset_holder* make_offset_holder(void)
        {
            int size1 = 1000;
            guint8 *buf1 = g_new0(guint8, size1);
            for (int i = 0; i < size1; i++)
            {
                buf1[i] = i;
            }

            offset_holder *h1 = offset_holder_new(buf1, size1);
            assert_not_null(h1);

            return h1;
        }

        void delete_offset_holder(offset_holder *in_h1)
        {
            assert_true(offset_holder_free(in_h1));
        }

        bool test_simple_new1()
        {
            offset_holder *h1 = make_offset_holder();

            offset_list *ol1 = ofslist_new(h1);
            assert_not_null(ol1);

            assert_equal(0, ofslist_size(ol1));
            assert_equal(h1, ofslist_get_holder(ol1));

            assert_true(ofslist_free(ol1));
            delete_offset_holder(h1);

            return true;
        }

        bool test_add1()
        {
            offset_holder *h1 = make_offset_holder();

            offset_list *ol1 = ofslist_new(h1);
            assert_not_null(ol1);

            offset tofs = offset_holder_make_offset(h1, 10);
            assert_equal(0, ofslist_size(ol1));

            assert_false(ofslist_remove(ol1, tofs));

            assert_true(ofslist_add(ol1, tofs));
            assert_equal(1, ofslist_size(ol1));
            assert_true(ofslist_add(ol1, tofs));
            assert_equal(1, ofslist_size(ol1));

            assert_true(ofslist_contains(ol1, tofs));
            assert_false(ofslist_contains(ol1,
                                          offset_holder_make_offset(h1, 20)));

            assert_true(ofslist_remove(ol1, tofs));
            assert_false(ofslist_remove(ol1, tofs));

            assert_true(ofslist_free(ol1));
            delete_offset_holder(h1);

            return true;
        }

        bool test_add_fail1()
        {
            offset_holder *h1 = make_offset_holder();
            offset_holder *h2 = make_offset_holder();

            offset_list *ol1 = ofslist_new(h1);
            offset_list *ol2 = ofslist_new(h2);
            assert_not_null(ol1);
            assert_not_null(ol2);

            offset tofs_h1 = offset_holder_make_offset(h1, 10);
            assert_equal(0, ofslist_size(ol2));

            assert_false(ofslist_remove(ol2, tofs_h1));

            assert_false(ofslist_add(ol2, tofs_h1));
            assert_equal(0, ofslist_size(ol2));

            assert_false(ofslist_contains(ol2, tofs_h1));

            assert_true(ofslist_free(ol1));
            assert_true(ofslist_free(ol2));
            delete_offset_holder(h1);
            delete_offset_holder(h2);

            return true;
        }

        bool test_add_more1()
        {
            offset_holder *h1 = make_offset_holder();

            offset_list *ol1 = ofslist_new(h1);
            assert_not_null(ol1);

            offset tofs1 = offset_holder_make_offset(h1, 10);
            offset tofs2 = offset_holder_make_offset(h1, 100);
            offset tofs3 = offset_holder_make_offset(h1, 500);

            assert_equal(0, ofslist_size(ol1));

            assert_false(ofslist_remove(ol1, tofs1));
            assert_false(ofslist_remove(ol1, tofs2));
            assert_false(ofslist_remove(ol1, tofs3));

            assert_false(ofslist_contains(ol1, tofs1));
            assert_false(ofslist_contains(ol1, tofs2));
            assert_false(ofslist_contains(ol1, tofs3));

            assert_true(ofslist_add(ol1, tofs1));
            assert_equal(1, ofslist_size(ol1));
            assert_true(ofslist_add(ol1, tofs1));
            assert_equal(1, ofslist_size(ol1));
            assert_true(ofslist_contains(ol1, tofs1));
            assert_false(ofslist_contains(ol1, tofs2));
            assert_false(ofslist_contains(ol1, tofs3));

            assert_true(ofslist_add(ol1, tofs2));
            assert_equal(2, ofslist_size(ol1));
            assert_true(ofslist_add(ol1, tofs2));
            assert_equal(2, ofslist_size(ol1));
            assert_true(ofslist_contains(ol1, tofs1));
            assert_true(ofslist_contains(ol1, tofs2));
            assert_false(ofslist_contains(ol1, tofs3));

            assert_true(ofslist_add(ol1, tofs3));
            assert_equal(3, ofslist_size(ol1));
            assert_true(ofslist_add(ol1, tofs3));
            assert_equal(3, ofslist_size(ol1));
            assert_true(ofslist_contains(ol1, tofs1));
            assert_true(ofslist_contains(ol1, tofs2));
            assert_true(ofslist_contains(ol1, tofs3));

            assert_false(ofslist_contains(ol1,
                                          offset_holder_make_offset(h1, 20)));

            assert_true(ofslist_remove(ol1, tofs1));
            assert_false(ofslist_remove(ol1, tofs1));
            assert_false(ofslist_contains(ol1, tofs1));
            assert_true(ofslist_contains(ol1, tofs2));
            assert_true(ofslist_contains(ol1, tofs3));

            assert_true(ofslist_remove(ol1, tofs2));
            assert_false(ofslist_remove(ol1, tofs2));
            assert_false(ofslist_contains(ol1, tofs1));
            assert_false(ofslist_contains(ol1, tofs2));
            assert_true(ofslist_contains(ol1, tofs3));

            assert_true(ofslist_remove(ol1, tofs3));
            assert_false(ofslist_remove(ol1, tofs3));
            assert_false(ofslist_contains(ol1, tofs1));
            assert_false(ofslist_contains(ol1, tofs2));
            assert_false(ofslist_contains(ol1, tofs3));

            assert_true(ofslist_free(ol1));
            delete_offset_holder(h1);

            return true;
        }

    public:

        test_offset_list(): Melunit::Test("test_offset_list")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_offset_list:: name)

            REGISTER(test_simple_new1);
            REGISTER(test_add1);
            REGISTER(test_add_fail1);
            REGISTER(test_add_more1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_offset_list t1_;
}
