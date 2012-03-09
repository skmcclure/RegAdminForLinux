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

#include <rregadmin/diff/value_key_cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/util/options.h>
#include <rregadmin/registry/init.h>

namespace
{
    class test_value_key_cell_diff : public Melunit::Test
    {
    private:

        bool test_null1(void)
        {
            rra_path *path = rra_path_new_win("\\key\\null1");
            assert_equal(0, value_key_cell_diff(path, NULL, NULL, NULL));

            return true;
        }

        bool test_equal1(void)
        {
            rra_path *path = rra_path_new_win("\\key\\equal1");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);
            const char *name = "foo";
            Value *val = value_create_dword(10);
            ValueKeyCell *from_vkc = key_cell_add_value(root1, name, val);
            ValueKeyCell *to_vkc = key_cell_add_value(root1, name, val);

            assert_equal(0, value_key_cell_diff(path, from_vkc, to_vkc, di1));

            assert_equal(0, rra_di_action_count(di1));

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_null2(void)
        {
            rra_path *path = rra_path_new_win("\\key\\null2");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            const char *name = "foo";
            Value *val = value_create_dword(10);
            ValueKeyCell *from_vkc = key_cell_add_value(root1, name, val);

            assert_compare(std::less, 0,
                           value_key_cell_diff(path, from_vkc, NULL, di1));

            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *rda = rra_di_get_action(di1, 0);
            assert_not_null(rda);
            assert_equal(RRA_DI_DELETE_VALUE, rda->action);
            const rra_di_value_action *va =
                (const rra_di_value_action*)rda->data;
            assert_equal(std::string(name),
                         std::string(ustr_as_utf8(va->name)));

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_null3(void)
        {
            rra_path *path = rra_path_new_win("\\key\\null3");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);
            const char *name = "foo";
            Value *val = value_create_dword(10);
            ValueKeyCell *to_vkc = key_cell_add_value(root1, name, val);

            assert_compare(std::greater, 0,
                           value_key_cell_diff(path, NULL, to_vkc, di1));

            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *rda = rra_di_get_action(di1, 0);
            assert_not_null(rda);
            assert_equal(RRA_DI_ADD_VALUE, rda->action);
            const rra_di_value_action *va =
                (const rra_di_value_action*)rda->data;
            assert_equal(std::string(name),
                         std::string(ustr_as_utf8(va->name)));

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_greater1(void)
        {
            rra_path *path = rra_path_new_win("\\key\\greater1");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            const char *to_name = "foo";
            const char *from_name = "goo";
            Value *val = value_create_dword(10);
            ValueKeyCell *to_vkc = key_cell_add_value(root1, to_name, val);
            ValueKeyCell *from_vkc = key_cell_add_value(root1, from_name, val);

            assert_compare(std::less, 0,
                           value_key_cell_diff(path, from_vkc, to_vkc, di1));

            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *rda = rra_di_get_action(di1, 0);
            assert_not_null(rda);
            assert_equal(RRA_DI_ADD_VALUE, rda->action);
            const rra_di_value_action *va =
                (const rra_di_value_action*)rda->data;
            assert_equal(std::string(to_name),
                         std::string(ustr_as_utf8(va->name)));
            assert_true(value_equal(val, va->val_new));

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

    public:

        test_value_key_cell_diff(): Melunit::Test("test_value_key_cell_diff")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_value_key_cell_diff:: name)

            REGISTER(test_null1);
            REGISTER(test_equal1);
            REGISTER(test_null2);
            REGISTER(test_null3);
            REGISTER(test_greater1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_value_key_cell_diff t1_;
}
