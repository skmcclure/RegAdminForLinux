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

#include <rregadmin/diff/key_cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/util/options.h>
#include <rregadmin/registry/init.h>

namespace
{
    class test_key_cell_diff : public Melunit::Test
    {
    private:

        static void print_actions(const char *in_func_name,
                                  int in_index,
                                  const rra_diff_info *in_di)
        {
            static bool should_print_actions = false;
            if (should_print_actions)
            {
                for (int i = 0; i < rra_di_action_count(in_di); i++)
                {
                    fprintf (stderr, "%s(%d): %s\n", in_func_name, in_index,
                             rra_di_action_to_string(
                                 rra_di_get_action(in_di, i)));
                }
            }
        }

        bool test_null1(void)
        {
            rra_path *path = rra_path_new_win("\\key\\null1");
            assert_equal(0, key_cell_diff(path, NULL, NULL, NULL));

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
            ustring *name1 = ustr_create("foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, name1);
            assert_true(ustr_free(name1));
            ustring *name2 = ustr_create("bar");
            KeyCell *to_kc = key_cell_add_subkey(root1, name2);
            assert_true(ustr_free(name2));

            assert_equal(0, key_cell_diff(path, from_kc, to_kc, di1));

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
            ustring *name1 = ustr_create("foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, name1);

            assert_compare(std::less, 0,
                           key_cell_diff(path, from_kc, NULL, di1));

            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *rda = rra_di_get_action(di1, 0);
            assert_not_null(rda);
            assert_equal(RRA_DI_DELETE_KEY, rda->action);
            assert_equal(std::string("\\key\\null2"),
                         std::string(ustr_as_utf8((ustring*)rda->data)));

            assert_true(ustr_free(name1));
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
            ustring *name2 = ustr_create("bar");
            KeyCell *to_kc = key_cell_add_subkey(root1, name2);

            assert_compare(std::greater, 0,
                           key_cell_diff(path, NULL, to_kc, di1));

            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *rda = rra_di_get_action(di1, 0);
            assert_not_null(rda);
            assert_equal(RRA_DI_ADD_KEY, rda->action);
            assert_equal(std::string("\\key\\null3"),
                         std::string(ustr_as_utf8((ustring*)rda->data)));

            assert_true(ustr_free(name2));
            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_key_greater1(void)
        {
            rra_path *path = rra_path_new_win("\\key\\greater1");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("bar");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            ustring *from2_name = ustr_create("baz");
            KeyCell *from2_kc = key_cell_add_subkey(from_kc, from2_name);
            assert_true(ustr_free(from2_name));

            assert_compare(std::less, 0,
                           key_cell_diff(path, from_kc, to_kc, di1));

            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *rda = rra_di_get_action(di1, 0);
            assert_not_null(rda);
            assert_equal(RRA_DI_DELETE_KEY, rda->action);
            ustring *exp_path = ustr_create("\\key\\greater1\\baz");
            assert_true(ustr_equal((ustring*)rda->data, exp_path));

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_key_lesser1(void)
        {
            rra_path *path = rra_path_new_win("\\key\\lesser1");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("bar");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            ustring *to2_name = ustr_create("baz");
            KeyCell *to2_kc = key_cell_add_subkey(to_kc, to2_name);
            assert_true(ustr_free(to2_name));

            assert_compare(std::greater, 0,
                           key_cell_diff(path, from_kc, to_kc, di1));

            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *rda = rra_di_get_action(di1, 0);
            assert_not_null(rda);
            assert_equal(RRA_DI_ADD_KEY, rda->action);
            ustring *exp_path = ustr_create("\\key\\lesser1\\baz");
            assert_true(ustr_equal((ustring*)rda->data, exp_path));
            ustr_free(exp_path);

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_key_greater2(void)
        {
            rra_path *path = rra_path_new_win("\\key\\greater2");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("bar");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            ustring *from2_name = ustr_create("baz");
            KeyCell *from2_kc = key_cell_add_subkey(from_kc, from2_name);
            assert_true(ustr_free(from2_name));

            ustring *from3_name = ustr_create("beee");
            KeyCell *from3_kc = key_cell_add_subkey(from_kc, from3_name);
            assert_true(ustr_free(from3_name));

            assert_compare(std::less, 0,
                           key_cell_diff(path, from_kc, to_kc, di1));

            assert_equal(2, rra_di_action_count(di1));

            const rra_diff_action *rda0 = rra_di_get_action(di1, 0);
            assert_not_null(rda0);
            assert_equal(RRA_DI_DELETE_KEY, rda0->action);
            ustring *exp_path = ustr_create("\\key\\greater2\\baz");
            assert_true(ustr_equal((ustring*)rda0->data, exp_path));
            ustr_free(exp_path);

            const rra_diff_action *rda1 = rra_di_get_action(di1, 1);
            assert_not_null(rda1);
            assert_equal(RRA_DI_DELETE_KEY, rda1->action);
            exp_path = ustr_create("\\key\\greater2\\beee");
            assert_true(ustr_equal((ustring*)rda1->data, exp_path));
            ustr_free(exp_path);

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_key_lesser2(void)
        {
            rra_path *path = rra_path_new_win("\\key\\lesser2");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("bar");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            ustring *to2_name = ustr_create("baz");
            KeyCell *to2_kc = key_cell_add_subkey(to_kc, to2_name);
            assert_true(ustr_free(to2_name));

            ustring *to3_name = ustr_create("bee");
            KeyCell *to3_kc = key_cell_add_subkey(to_kc, to3_name);
            assert_true(ustr_free(to3_name));

            assert_compare(std::greater, 0,
                           key_cell_diff(path, from_kc, to_kc, di1));

            assert_equal(2, rra_di_action_count(di1));

            const rra_diff_action *rda0 = rra_di_get_action(di1, 0);
            assert_not_null(rda0);
            assert_equal(RRA_DI_ADD_KEY, rda0->action);
            ustring *exp_path = ustr_create("\\key\\lesser2\\baz");
            assert_true(ustr_equal((ustring*)rda0->data, exp_path));
            ustr_free(exp_path);

            const rra_diff_action *rda1 = rra_di_get_action(di1, 1);
            assert_not_null(rda1);
            assert_equal(RRA_DI_ADD_KEY, rda1->action);
            exp_path = ustr_create("\\key\\lesser2\\bee");
            assert_true(ustr_equal((ustring*)rda1->data, exp_path));
            ustr_free(exp_path);

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_key_greater3(void)
        {
            rra_path *path = rra_path_new_win("\\key\\greater3");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("bar");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            ustring *from2_name = ustr_create("baz");
            KeyCell *from2_kc = key_cell_add_subkey(from_kc, from2_name);
            assert_true(ustr_free(from2_name));

            ustring *to3_name = ustr_create("beee");
            KeyCell *to3_kc = key_cell_add_subkey(to_kc, to3_name);
            assert_true(ustr_free(to3_name));

            assert_compare(std::greater, 0,
                           key_cell_diff(path, from_kc, to_kc, di1));

            assert_equal(2, rra_di_action_count(di1));

            ustring *exp_path;

            const rra_diff_action *rda0 = rra_di_get_action(di1, 0);
            assert_not_null(rda0);
            const rra_diff_action *rda1 = rra_di_get_action(di1, 1);
            assert_not_null(rda1);

            assert_equal(RRA_DI_ADD_KEY, rda1->action);
            exp_path = ustr_create("\\key\\greater3\\beee");
            assert_true(ustr_equal((ustring*)rda1->data, exp_path));
            ustr_free(exp_path);

            assert_equal(RRA_DI_DELETE_KEY, rda0->action);
            exp_path = ustr_create("\\key\\greater3\\baz");
            assert_true(ustr_equal((ustring*)rda0->data, exp_path));
            ustr_free(exp_path);

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_key_lesser3(void)
        {
            rra_path *path = rra_path_new_win("\\key\\lesser2");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("bar");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            ustring *to2_name = ustr_create("baz");
            KeyCell *to2_kc = key_cell_add_subkey(to_kc, to2_name);
            assert_true(ustr_free(to2_name));

            ustring *from3_name = ustr_create("bee");
            KeyCell *from3_kc = key_cell_add_subkey(from_kc, from3_name);
            assert_true(ustr_free(from3_name));

            assert_compare(std::less, 0,
                           key_cell_diff(path, from_kc, to_kc, di1));

            assert_equal(2, rra_di_action_count(di1));

            ustring *exp_path;

            const rra_diff_action *rda0 = rra_di_get_action(di1, 0);
            assert_not_null(rda0);
            const rra_diff_action *rda1 = rra_di_get_action(di1, 1);
            assert_not_null(rda1);

            assert_equal(RRA_DI_DELETE_KEY, rda1->action);
            exp_path = ustr_create("\\key\\lesser2\\bee");
            assert_true(ustr_equal((ustring*)rda1->data, exp_path));
            ustr_free(exp_path);

            assert_equal(RRA_DI_ADD_KEY, rda0->action);
            exp_path = ustr_create("\\key\\lesser2\\baz");
            assert_true(ustr_equal((ustring*)rda0->data, exp_path));
            ustr_free(exp_path);

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        ustring*
        make_random_name(void)
        {
            static const char *used_chars =
                " abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "1234567890!@#$%^&*()[]{}/=?+-_',\"<";
            static int used_chars_len = strlen(used_chars);

            ustring *ret_val = ustr_new();

            int count = rand() % 30;
            int i;

            for (i = 0; i < count; i++)
            {
                assert_true(
                    ustr_charcat(ret_val,
                                 used_chars[rand() % used_chars_len]));
            }

            return ret_val;
        }

        void
        add_same_keys(KeyCell *in_kc1, KeyCell *in_kc2, int in_level)
        {
            int i;
            for (i = 0; i < in_level; i++)
            {
                ustring *name = make_random_name();
                KeyCell *sub_kc1 = key_cell_add_subkey(in_kc1, name);
                KeyCell *sub_kc2 = key_cell_add_subkey(in_kc2, name);
                ustr_free(name);
                add_same_keys(sub_kc1, sub_kc2, in_level - 1);
            }
        }

        bool test_equal2(void)
        {
            rra_path *path = rra_path_new_win("\\key\\equal2");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("bar");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            add_same_keys(to_kc, from_kc, 2);

            assert_equal(0, key_cell_diff(path, from_kc, to_kc, di1));

            assert_equal(0, rra_di_action_count(di1));

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_add_value_equal1(void)
        {
            rra_path *path = rra_path_new_win("\\key\\value_equal1");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("foo1");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("foo2");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            const char *vname = "value1";
            Value *val = value_create_dword(1001);

            assert_not_null(key_cell_add_value(from_kc, vname, val));
            assert_not_null(key_cell_add_value(to_kc, vname, val));

            assert_equal(0, key_cell_diff(path, from_kc, to_kc, di1));

            assert_equal(0, rra_di_action_count(di1));

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_add_value_greater1(void)
        {
            rra_path *path = rra_path_new_win("\\key\\value_equal1");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("to_foo");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("from_foo");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            const char *vname = "value1";
            Value *val = value_create_dword(1001);

            assert_not_null(key_cell_add_value(to_kc, vname, val));

            assert_compare(std::greater, 0,
                           key_cell_diff(path, from_kc, to_kc, di1));

            print_actions(__func__, 0, di1);

            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_ADD_VALUE, act1->action);

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

        bool test_add_value_less1(void)
        {
            rra_path *path = rra_path_new_win("\\key\\value_equal1");
            rra_diff_info *di1 = rra_diff_info_new();

            ustring *hive_name = ustr_create("foobar");
            Hive *hv1 = hive_new(hive_name);
            assert_true(ustr_free(hive_name));
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *to_name = ustr_create("foo1");
            KeyCell *to_kc = key_cell_add_subkey(root1, to_name);
            assert_true(ustr_free(to_name));

            ustring *from_name = ustr_create("foo2");
            KeyCell *from_kc = key_cell_add_subkey(root1, from_name);
            assert_true(ustr_free(from_name));

            const char *vname = "value1";
            Value *val = value_create_dword(1001);

            assert_not_null(key_cell_add_value(from_kc, vname, val));

            assert_compare(std::less, 0,
                           key_cell_diff(path, from_kc, to_kc, di1));

            print_actions(__func__, 0, di1);

            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_DELETE_VALUE, act1->action);

            assert_true(hive_free(hv1));
            assert_true(rra_diff_info_free(di1));
            assert_true(rra_path_free(path));

            return true;
        }

    public:

        test_key_cell_diff(): Melunit::Test("test_key_cell_diff")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_key_cell_diff:: name)

            REGISTER(test_null1);
            REGISTER(test_equal1);
            REGISTER(test_null2);
            REGISTER(test_null3);
            REGISTER(test_key_greater1);
            REGISTER(test_key_lesser1);
            REGISTER(test_key_greater2);
            REGISTER(test_key_lesser2);
            REGISTER(test_key_greater3);
            REGISTER(test_key_lesser3);
            REGISTER(test_equal2);
            REGISTER(test_add_value_equal1);
            REGISTER(test_add_value_greater1);
            REGISTER(test_add_value_less1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_key_cell_diff t1_;
}
