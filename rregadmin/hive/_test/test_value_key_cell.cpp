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

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/util/value_type.h>
#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;

    class test_value_key_cell : public Melunit::Test
    {
    private:

        void check_values_helper(Hive *hv1, KeyCell *key1)
        {
            for (int i = 0; i < key_cell_get_number_of_values(key1); i++)
            {
                ValueKeyCell *vkc1 = key_cell_get_value(key1, i);

                assert_not_null(vkc1);

                // value_key_cell_debug_print(vkc1);

                ustring *name1 = value_key_cell_get_name(vkc1);
                assert_not_null(name1);
                ustr_free(name1);

                Value *val1 = value_key_cell_get_val(vkc1);
                do_if_fail((assert_not_null(val1)),
                           (value_key_cell_debug_print(vkc1)));

                do_if_fail((assert_equal(value_get_type(val1),
                                         value_key_cell_get_type(vkc1))),
                           (value_key_cell_debug_print(vkc1)));

                value_free(val1);

                do_if_fail(
                    (assert_true(value_key_cell_is_valid(vkc1))),
                    (value_key_cell_debug_print(vkc1)));
            }
        }

        void walk_tree_helper(Hive *hv1, KeyCell *key1)
        {
            // key_cell_debug_print(hv1, key1);
            check_values_helper(hv1, key1);

            for (int i = 0; i < key_cell_get_number_of_subkeys(key1); i++)
            {
                KeyCell *subkey1 = key_cell_get_subkey(key1, i);

                assert_true(cell_equal(key_cell_to_cell(key1),
                                       key_cell_to_cell(
                                           key_cell_get_parent(subkey1))));

                walk_tree_helper(hv1, subkey1);
            }
        }

        bool test_walk_tree(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            KeyCell* root1 = hive_get_root_key(hv1);
            assert_not_null(root1);

            walk_tree_helper(hv1, root1);

            return true;
        }

        bool test_add_values1(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            assert_not_null(hv1);

            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_equal(0, key_cell_get_number_of_subkeys(root1));
            assert_null(key_cell_get_subkey(root1, 1));

            ustring *key_name1 = ustr_create("key1");
            KeyCell *kc1 = key_cell_add_subkey(root1, key_name1);
            ustr_free(key_name1);
            assert_not_null(kc1);

            assert_equal(0, key_cell_get_number_of_values(kc1));

            Value *val1 = value_create_dword(30);
            ValueKeyCell *val_key1 = key_cell_add_value(kc1, "foo", val1);
            assert_not_null(val_key1);
            assert_equal(1, key_cell_get_number_of_values(kc1));

            assert_equal(value_get_type(val1),
                         value_key_cell_get_type(val_key1));

            ValueKeyCell *val_key2 = key_cell_get_value_str(kc1, "foo");
            assert_not_null(val_key2);
            Value *val2 = value_key_cell_get_val(val_key2);
            assert_not_null(val2);

            assert_equal(value_get_type(val1), value_get_type(val2));
            assert_equal(value_get_as_dword(val1), value_get_as_dword(val2));

            hive_free(hv1);
            return true;
        }

        bool test_set_values1(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            assert_not_null(hv1);

            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_equal(0, key_cell_get_number_of_subkeys(root1));
            assert_null(key_cell_get_subkey(root1, 1));

            ustring *key_name1 = ustr_create("key1");
            KeyCell *kc1 = key_cell_add_subkey(root1, key_name1);
            ustr_free(key_name1);
            assert_not_null(kc1);

            Value *val1 = value_create_dword(30);
            ValueKeyCell *val_key1 = key_cell_add_value(kc1, "foo", val1);
            assert_not_null(val_key1);
            Value *val2 = value_key_cell_get_val(val_key1);
            assert_not_null(val2);
            assert_equal(value_get_type(val1), value_get_type(val2));
            assert_equal(value_get_as_dword(val1), value_get_as_dword(val2));
            value_free(val1);
            value_free(val2);

            Value *val3 = value_create_string("foo", 3, USTR_TYPE_UTF8);
            assert_true(value_key_cell_set_val(val_key1, val3));

            Value *val4 = value_key_cell_get_val(val_key1);
            assert_equal(REG_SZ, value_get_type(val4));
            ustring *val4_str = value_get_as_string(val4);
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(val4_str)));
            ustr_free(val4_str);
            value_free(val3);
            value_free(val4);

            hive_free(hv1);
            return true;
        }

        bool test_delete_values1(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            assert_not_null(hv1);

            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_equal(0, key_cell_get_number_of_subkeys(root1));
            assert_null(key_cell_get_subkey(root1, 1));

            ustring *key_name1 = ustr_create("key1");
            KeyCell *kc1 = key_cell_add_subkey(root1, key_name1);
            ustr_free(key_name1);
            assert_not_null(kc1);

            assert_equal(0, key_cell_get_number_of_values(kc1));

            Value *val1 = value_create_dword(30);
            ValueKeyCell *val_key1 = key_cell_add_value(kc1, "foo", val1);
            assert_equal(1, key_cell_get_number_of_values(kc1));

            value_free(val1);

            assert_true(key_cell_delete_value_str(kc1, "foo"));
            assert_equal(0, key_cell_get_number_of_values(kc1));

            hive_free(hv1);
            return true;
        }

        bool value_read_write_helper(Value *in_val)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            assert_not_null(hv1);

            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_equal(0, key_cell_get_number_of_subkeys(root1));
            assert_null(key_cell_get_subkey(root1, 1));

            ustring *key_name1 = ustr_create("key1");
            KeyCell *kc1 = key_cell_add_subkey(root1, key_name1);
            ustr_free(key_name1);
            assert_not_null(kc1);

            assert_equal(0, key_cell_get_number_of_values(kc1));

            ValueKeyCell *val_key1 = key_cell_add_value(kc1, "foo", in_val);
            assert_not_null(val_key1);
            assert_equal(1, key_cell_get_number_of_values(kc1));

            assert_equal(value_get_type(in_val),
                         value_key_cell_get_type(val_key1));

            ValueKeyCell *val_key2 = key_cell_get_value_str(kc1, "foo");
            assert_not_null(val_key2);
            Value *val2 = value_key_cell_get_val(val_key2);
            assert_not_null(val2);
            ustring *valstr2 = value_get_as_string(val2);
            assert_true_msg(value_equal(in_val, val2),
                            ustr_as_utf8(valstr2));
            ustr_free(valstr2);

            assert_true(hive_write(hv1, "tmp.hive"));
            hive_free(hv1);

            // Read it back in
            hv1 = hive_read("tmp.hive");
            assert_not_null(hv1);

            root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_equal(1, key_cell_get_number_of_subkeys(root1));

            kc1 = key_cell_get_subkey_str(root1, "key1");
            assert_not_null(kc1);

            assert_equal(1, key_cell_get_number_of_values(kc1));
            val_key2 = key_cell_get_value_str(kc1, "foo");
            assert_not_null(val_key2);
            val2 = value_key_cell_get_val(val_key2);
            assert_not_null(val2);
            assert_true(value_equal(in_val, val2));

            hive_free(hv1);

            assert_equal(0, unlink("tmp.hive"));

            value_free(in_val);

            return true;
        }

        bool test_add_dword1(void)
        {
            assert_true(value_read_write_helper(value_create_dword(30)));

            return true;
        }

        bool test_add_binary(void)
        {
            guint8 data1[] = { 0xff, 0x00, 0xab, 0x99, 0x55 };
            int data1_len = G_N_ELEMENTS(data1);

            assert_true(value_read_write_helper(
                            value_create_binary(data1, data1_len)));

            return true;
        }

        bool test_add_string(void)
        {
            char data1[] = "Whoops I did it again";
            int data1_len = strlen(data1);

            assert_true(value_read_write_helper(
                            value_create_string(data1, data1_len,
                                                USTR_TYPE_UTF8)));

            return true;
        }

        static Value* modder1 (Value *in_val, gpointer in_data)
        {
            value_clear_bits(in_val, 0, 0x88);
            return in_val;
        }

        bool test_modify_value1(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            assert_not_null(hv1);

            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);

            ustring *key_name1 = ustr_create("key1");
            KeyCell *kc1 = key_cell_add_subkey(root1, key_name1);
            ustr_free(key_name1);
            assert_not_null(kc1);

            Value *val1 = value_create_dword(0xffee0099);
            ValueKeyCell *val_key1 = key_cell_add_value(kc1, "foo", val1);
            assert_not_null(val_key1);
            value_free(val1);

            assert_true(value_key_cell_modify_val(val_key1, modder1, NULL));

            val1 = value_key_cell_get_val(val_key1);
            assert_equal(0xffee0011,
                         value_get_as_dword(val1));

            hive_free(hv1);

            return true;
        }

    public:

        test_value_key_cell(): Melunit::Test("test_value_key_cell")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_value_key_cell:: name)

#define REG_BOOSTB(name, hive_name) \
            register_test(#name, \
                          boost::bind(&test_value_key_cell:: name, this, \
                                      hive_name))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_walk_tree, *i);
                }
            }

            REGISTER(test_add_values1);
            REGISTER(test_set_values1);
            REGISTER(test_delete_values1);
            REGISTER(test_add_dword1);
            REGISTER(test_add_binary);
            REGISTER(test_add_string);
            REGISTER(test_modify_value1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_value_key_cell t1_;
}
