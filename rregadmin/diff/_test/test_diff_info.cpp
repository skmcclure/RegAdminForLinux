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

#include <rregadmin/diff/diff_info.h>
#include <rregadmin/registry/init.h>

namespace
{
    class test_diff_info : public Melunit::Test
    {
    private:

        bool test_null1(void)
        {
            rra_diff_info *i1 = NULL;
            assert_false(rra_diff_info_free(i1));
            ustring *p1 = ustr_create("foo");
            ustring *n1 = ustr_create("bar");
            Value *v1 = value_create_string("bar", 3, USTR_TYPE_UTF8);
            assert_false(rra_di_add_key(NULL, p1));
            assert_false(rra_di_delete_key(NULL, p1));
            assert_false(rra_di_add_value(NULL, p1, n1, v1));
            assert_false(rra_di_delete_value(NULL, p1, n1, v1));

            assert_true(ustr_free(p1));
            assert_true(ustr_free(n1));
            assert_true(value_free(v1));

            return true;
        }

        bool test_new1(void)
        {
            rra_diff_info *i1 = rra_diff_info_new();
            assert_not_null(i1);

            assert_equal(0, rra_di_action_count(i1));
            assert_null(rra_di_get_action(i1, 0));
            assert_null(rra_di_get_action(i1, 1));

            assert_true(rra_diff_info_free(i1));

            return true;
        }

        bool test_null2(void)
        {
            rra_diff_info *i1 = rra_diff_info_new();
            assert_not_null(i1);

            ustring *u1 = ustr_create("foo");
            ustring *u2 = ustr_new();
            ustring *u3 = ustr_create("bar");
            Value *v1 = value_create_string("bar", 3, USTR_TYPE_UTF8);

            /* null names */
            assert_false(rra_di_add_key(i1, NULL));
            assert_false(rra_di_delete_key(i1, NULL));
            assert_false(rra_di_add_value(i1, NULL, u3, v1));
            assert_false(rra_di_add_value(i1, u1, NULL, v1));
            assert_false(rra_di_add_value(i1, u1, u3, NULL));
            assert_false(rra_di_add_value(i1, u1, NULL, NULL));
            assert_false(rra_di_add_value(i1, NULL, u3, NULL));
            assert_false(rra_di_add_value(i1, NULL, NULL, v1));
            assert_false(rra_di_delete_value(i1, NULL, u3, v1));
            assert_false(rra_di_delete_value(i1, u1, NULL, v1));
            assert_false(rra_di_delete_value(i1, u1, u3, NULL));
            assert_false(rra_di_delete_value(i1, NULL, NULL, v1));
            assert_false(rra_di_delete_value(i1, NULL, u3, NULL));
            assert_false(rra_di_delete_value(i1, u1, NULL, NULL));
            assert_equal(0, rra_di_action_count(i1));

            /* empty names */
            assert_false(rra_di_add_key(i1, u2));
            assert_false(rra_di_delete_key(i1, u2));
            assert_false(rra_di_delete_value(i1, u2, u3, v1));
            assert_equal(0, rra_di_action_count(i1));

            assert_true(ustr_free(u1));
            assert_true(value_free(v1));
            assert_true(rra_diff_info_free(i1));

            return true;
        }

        bool test_add_key1(void)
        {
            rra_diff_info *i1 = rra_diff_info_new();
            assert_not_null(i1);
            assert_equal(0, rra_di_action_count(i1));

            ustring *u1 = ustr_create("foo");
            assert_true(rra_di_add_key(i1, u1));
            assert_equal(1, rra_di_action_count(i1));

            const rra_diff_action *a1 = rra_di_get_action(i1, 0);
            assert_not_null(a1);
            assert_equal(RRA_DI_ADD_KEY, a1->action);
            assert_equal(u1, a1->data);

            assert_true(rra_diff_info_free(i1));

            return true;
        }

        bool test_delete_key1(void)
        {
            rra_diff_info *i1 = rra_diff_info_new();
            assert_not_null(i1);
            assert_equal(0, rra_di_action_count(i1));

            ustring *u1 = ustr_create("foo");
            assert_true(rra_di_delete_key(i1, u1));
            assert_equal(1, rra_di_action_count(i1));

            const rra_diff_action *a1 = rra_di_get_action(i1, 0);
            assert_not_null(a1);
            assert_equal(RRA_DI_DELETE_KEY, a1->action);
            assert_equal(u1, a1->data);

            assert_true(rra_diff_info_free(i1));

            return true;
        }

        bool test_add_value1(void)
        {
            rra_diff_info *i1 = rra_diff_info_new();
            assert_not_null(i1);
            assert_equal(0, rra_di_action_count(i1));

            ustring *u1 = ustr_create("foo");
            ustring *u2 = ustr_create("bar");
            Value *v1 = value_create_string("bar", 3, USTR_TYPE_UTF8);
            assert_true(rra_di_add_value(i1, u1, u2, v1));
            assert_equal(1, rra_di_action_count(i1));

            const rra_diff_action *a1 = rra_di_get_action(i1, 0);
            assert_not_null(a1);
            assert_equal(RRA_DI_ADD_VALUE, a1->action);
            rra_di_value_action *av = (rra_di_value_action*)a1->data;
            assert_not_null(av);

            assert_equal(u1, av->path);
            assert_equal(u2, av->name);
            assert_equal(v1, av->val_new);

            assert_true(rra_diff_info_free(i1));

            return true;
        }

        bool test_delete_value1(void)
        {
            rra_diff_info *i1 = rra_diff_info_new();
            assert_not_null(i1);
            assert_equal(0, rra_di_action_count(i1));

            ustring *u1 = ustr_create("foo");
            ustring *u2 = ustr_create("bar");
            Value *v1 = value_create_string("bar", 3, USTR_TYPE_UTF8);
            assert_true(rra_di_delete_value(i1, u1, u2, v1));
            assert_equal(1, rra_di_action_count(i1));

            const rra_diff_action *a1 = rra_di_get_action(i1, 0);
            assert_not_null(a1);
            assert_equal(RRA_DI_DELETE_VALUE, a1->action);
            const rra_di_value_action *va =
                (const rra_di_value_action*)a1->data;
            assert_equal(u1, va->path);
            assert_equal(u2, va->name);

            assert_true(rra_diff_info_free(i1));

            return true;
        }

        bool test_compare_value1(void)
        {
            rra_diff_info *i1 = rra_diff_info_new();
            assert_not_null(i1);

            assert_equal(0, rra_di_get_compare_value(i1));

            assert_true(rra_di_set_compare_value(i1, -1));
            assert_equal(-1, rra_di_get_compare_value(i1));
            assert_false(rra_di_set_compare_value(i1, 0));

            assert_true(rra_di_clear_compare_value(i1));
            assert_equal(0, rra_di_get_compare_value(i1));

            assert_true(rra_di_set_compare_value(i1, 1));
            assert_equal(1, rra_di_get_compare_value(i1));
            assert_false(rra_di_set_compare_value(i1, -1));

            assert_true(rra_diff_info_free(i1));

            return true;
        }

    public:

        test_diff_info(): Melunit::Test("test_diff_info")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_diff_info:: name)

            REGISTER(test_null1);
            REGISTER(test_new1);
            REGISTER(test_null2);
            REGISTER(test_add_key1);
            REGISTER(test_delete_key1);
            REGISTER(test_add_value1);
            REGISTER(test_delete_value1);
            REGISTER(test_compare_value1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_diff_info t1_;
}
