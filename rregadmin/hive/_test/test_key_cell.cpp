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

#include <algorithm>
#include <functional>
#include <map>
#include <string>

#include <melunit/melunit-cxx.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <glib/gtree.h>

#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/security_descriptor_cell.h>
#include <rregadmin/hive/hive_check.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>

#include "example_hive.h"
#include "log_level_setter.hpp"

namespace
{
    using namespace RRegAdmin::Test;

    class test_key_cell : public Melunit::Test
    {
    private:

        bool test_root_keys(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            KeyCell* root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_true(key_cell_is_valid(root1));
            assert_equal(0, key_cell_get_number_of_values(root1));
            assert_equal((KeyCell*)0, key_cell_get_parent(root1));

            ustring *name = key_cell_get_name(root1);
            assert_not_null(name);
            // name is different for each hive type
            // assert_equal(std::string("$$$PROTO.HIV"),
            //              std::string(ustr_as_utf8(name)));
            ustr_free(name);
            assert_equal(std::string("root"),
                         std::string(key_cell_get_type_str(root1)));

            return true;
        }

        bool test_top_keys(const std::string &in_hivename)
        {
            bool do_debug = false;
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            ustring *hname = hive_get_name(hv1);
            if (do_debug)
            {
                fprintf (stderr, "For hive %s with name %s\n",
                         in_hivename.c_str(), ustr_as_utf8(hname));
            }
            ustr_free(hname);

            KeyCell* root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_true(key_cell_is_valid(root1));
            assert_equal(0, key_cell_get_number_of_values(root1));

            ustring *name = key_cell_get_name(root1);
            assert_not_null(name);
            if (do_debug)
            {
                fprintf (stderr, "  the root key has name %s\n",
                         ustr_as_utf8(name));
            }
            ustr_free(name);
            assert_equal(std::string("root"),
                         std::string(key_cell_get_type_str(root1)));

            if (do_debug)
            {
                for (int j = 0;
                     j < key_cell_get_number_of_subkeys(root1);
                     j++)
                {
                    KeyCell *sub1 = key_cell_get_subkey(root1, j);
                    assert_not_null(sub1);
                    ustring *subname1 = key_cell_get_name(sub1);
                    assert_not_null(subname1);
                    fprintf (stderr, "    subkey %d has name %s\n",
                             j, ustr_as_utf8(subname1));
                    ustr_free(subname1);
                }
            }

            return true;
        }

        void walk_tree_helper(Hive *hv1, KeyCell *key1)
        {
            do_if_fail((assert_true(key_cell_is_valid(key1))),
                       (key_cell_debug_print(key1)));

            if (key_cell_get_type_id(key1) == KEY_LINK)
            {
                key_cell_debug_print(key1);
            }

            ustring *pstr = ustr_new();
            assert_true(key_cell_get_parseable_output(key1, "", pstr));
            assert_compare(std::greater, ustr_strlen(pstr), 0);
            ustr_free(pstr);

            pstr = ustr_new();
            assert_true(key_cell_get_pretty_output(key1, "", pstr));
            assert_compare(std::greater, ustr_strlen(pstr), 0);
            ustr_free(pstr);

            pstr = ustr_new();
            assert_true(key_cell_get_xml_output(key1, pstr));
            assert_compare(std::greater, ustr_strlen(pstr), 0);
            ustr_free(pstr);

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

        bool test_removed_key_list_cell1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            KeyCell *root1 = hive_get_root_key(hv1);

            // Find a key with subkeys down a couple of layers.
            if (key_cell_get_number_of_subkeys(root1) == 0)
            {
                return true;
            }

            KeyCell *test_key = 0;

            for (int i = 0; i < key_cell_get_number_of_subkeys(root1); i++)
            {
                KeyCell *layer1 = key_cell_get_subkey(root1, i);

                if (key_cell_get_number_of_subkeys(layer1) != 0)
                {
                    for (int j = 0;
                         j < key_cell_get_number_of_subkeys(layer1);
                         j++)
                    {
                        KeyCell *layer2 = key_cell_get_subkey(layer1, j);
                        if (key_cell_get_number_of_subkeys(layer2) != 0)
                        {
                            test_key = layer2;
                            break;
                        }
                    }
                }
                if (test_key != 0)
                {
                    break;
                }
            }

            if (test_key == 0)
            {
                // Didn't find a key cell with subkeys 2 levels down.
                // Just return true.
                return true;
            }

            // LogLevelSetter lls1(RRA_LOG_LEVEL_INFO);
            while (key_cell_get_number_of_subkeys(test_key) > 0)
            {
                assert_true(key_cell_delete_subkey(test_key, 0));
            }

            assert_equal(0, key_cell_get_number_of_subkeys(test_key));

            assert_null(key_cell_get_key_list_cell(test_key));

            RRACheckData cd1 = {
                RRA_CHECK_VERBOSITY_WARNING,
                0,
                0};

            assert_true(hive_check(hv1, &cd1));

            return true;
        }

        bool test_add_key1(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            assert_not_null(hv1);

            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);

            SecurityDescriptorCell *root_sdc1 =
                key_cell_get_security_descriptor_cell(root1);
            assert_not_null(root_sdc1);
            const SecurityDescriptor *root_sd =
                key_cell_get_secdesc(root1);
            assert_not_null(root_sd);

            assert_equal(0, key_cell_get_number_of_subkeys(root1));
            assert_null(key_cell_get_subkey(root1, 1));

            ustring *key_name1 = ustr_create("key1");
            KeyCell *kc1 = key_cell_add_subkey(root1, key_name1);
            ustr_free(key_name1);
            assert_not_null(kc1);
            ustring *kn1 = key_cell_get_name(kc1);
            assert_not_null(kn1);
            assert_equal(std::string("key1"),
                         std::string(ustr_as_utf8(kn1)));
            assert_equal(1, key_cell_get_number_of_subkeys(root1));
            assert_equal(0, key_cell_get_number_of_subkeys(kc1));

            hive_free(hv1);
            return true;
        }

        bool test_add_key2(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            assert_not_null(hv1);

            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_equal(0, key_cell_get_number_of_subkeys(root1));
            assert_null(key_cell_get_subkey(root1, 1));

            for (int i = 0; i < 10; i++)
            {
                ustring *key_name1 = ustr_new();
                ustr_printf(key_name1, "key%d", i);
                KeyCell *kc1 = key_cell_add_subkey(root1, key_name1);
                assert_not_null(kc1);
                assert_equal(i + 1,
                             key_cell_get_number_of_subkeys(root1));

                for (int j = 0; j < 10; j++)
                {
                    ustring *key_name2 = ustr_new();
                    ustr_printf(key_name2, "key%d_%d", i, j);
                    KeyCell *kc2 = key_cell_add_subkey(kc1, key_name2);
                    ustr_free(key_name2);
                    assert_not_null(kc2);
                    assert_equal(j + 1,
                                 key_cell_get_number_of_subkeys(kc1));
                }
                ustr_free(key_name1);
            }

            hive_free(hv1);
            return true;
        }

        typedef std::map<std::string, int> string_map;

        int count_keys(KeyCell *kc1, string_map &key_cache)
        {
            if (kc1 == NULL)
            {
                fprintf (stderr, "Got a null key.  This shouldn't happen\n");
                return 0;
            }

            // one for me.
            int ret_val = 1;

            ustring *kc1_name = key_cell_get_name(kc1);
            int key_count =
                key_cache.count(std::string(ustr_as_utf8(kc1_name)));
            assert_equal(1, key_count);
            key_cache[std::string(ustr_as_utf8(kc1_name))]++;
            ustr_free(kc1_name);

            for (int i = 0; i < key_cell_get_number_of_subkeys(kc1); i++)
            {
                ret_val += count_keys(key_cell_get_subkey(kc1, i), key_cache);
            }

            return ret_val;
        }

        bool print_tree(KeyCell *kc1, ustring *prefix)
        {
            if (kc1 == NULL)
            {
                fprintf (stderr, "Got a null key.  This shouldn't happen\n");
                return false;
            }

            ustring *key_name = key_cell_get_name(kc1);
            fprintf (stderr, "%s=> %s\n", ustr_as_utf8(prefix),
                     ustr_as_utf8(key_name));

            ustring *new_prefix = ustr_create(ustr_as_utf8(prefix));
            ustr_strcat(new_prefix, "    ");
            for (int i = 0; i < key_cell_get_number_of_subkeys(kc1); i++)
            {
                if (!print_tree(key_cell_get_subkey(kc1, i), new_prefix))
                {
                    key_cell_debug_print(kc1);
                }
            }
            ustr_free(new_prefix);

            ustr_free(key_name);

            return true;
        }

        int add_key_helper1(KeyCell *kc1, const ustring *key_name, int count,
                            string_map &key_cache)
        {
            ustring *kc1_name = key_cell_get_name(kc1);
            key_cache.insert(string_map::value_type(
                                 std::string(ustr_as_utf8(kc1_name)), 0));
            ustr_free(kc1_name);
            if (count <= 0)
            {
                return 1;
            }

            // one for me
            int my_key_total = 1;

            int my_count = (rand() % count) + 2;

            for (int i = 0; i < my_count; i++)
            {
                ustring *da_name = ustr_create(ustr_as_utf8(key_name));
                ustr_printfa(da_name, "_%d", i);

                KeyCell *kc2 = key_cell_add_subkey(kc1, da_name);
                assert_not_null(kc2);
                assert_equal(i + 1,
                             key_cell_get_number_of_subkeys(kc1));

                my_key_total += add_key_helper1(kc2, da_name, count - 5,
                                                key_cache);

                ustr_free(da_name);
            }

            return my_key_total;
        }

        void check_key_use(string_map::value_type &val)
        {
            assert_equal(1, val.second);
        }

        int add_key_counter(Hive *hv1, int num)
        {
            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_equal(0, key_cell_get_number_of_subkeys(root1));
            assert_null(key_cell_get_subkey(root1, 1));

            srand(time(0));

            ustring *key_name1 = ustr_create("key");
            string_map key_cache;

            int total_keys = add_key_helper1(root1, key_name1, num, key_cache);
            ustr_free(key_name1);

            ustring *prefix = ustr_create("");
            //print_tree(root1, prefix);
            ustr_free(prefix);

            int key_count = count_keys(root1, key_cache);

            std::for_each(key_cache.begin(), key_cache.end(),
                          boost::bind(&test_key_cell::check_key_use,
                                      this, _1));

            assert_equal(key_cache.size(), key_count);
            assert_equal(total_keys, key_count);

            return total_keys;
        }

        void delete_random_keys_helper(KeyCell *kc1)
        {
            int num_keys = key_cell_get_number_of_subkeys(kc1);

            for (int i = 0; i < num_keys; i++)
            {
                if ((rand() % 4) == 0)
                {
                    key_cell_delete_subkey(kc1, i);
                    i--;
                    num_keys--;
                    assert_equal(num_keys,
                                 key_cell_get_number_of_subkeys(kc1));
                }
                else
                {
                    delete_random_keys_helper(key_cell_get_subkey(kc1, i));
                }
            }
        }

        void delete_random_keys(Hive *hv1)
        {
            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);

            delete_random_keys_helper(root1);
        }

        bool test_add_key3(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            ustr_free(name1);
            assert_not_null(hv1);
            add_key_counter(hv1, 10);
            hive_free(hv1);
            return true;
        }

        bool test_add_key4(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            ustr_free(name1);
            assert_not_null(hv1);
            add_key_counter(hv1, 20);
            hive_free(hv1);
            return true;
        }

        bool test_delete_key1(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            ustr_free(name1);
            assert_not_null(hv1);
            int num_keys = add_key_counter(hv1, 20);
            delete_random_keys(hv1);
            hive_free(hv1);
            return true;
        }

        bool test_add_key_secdesc1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            KeyCell* root1 = hive_get_root_key(hv1);
            assert_not_null(root1);

            SecurityDescriptorCell *root_sdc1 =
                key_cell_get_security_descriptor_cell(root1);
            assert_not_null(root_sdc1);
            int orig_secdesc_count =
                security_descriptor_cell_get_use_count(root_sdc1);

            ustring *key_name1 = ustr_create("key1");
            KeyCell *kc1 = key_cell_add_subkey(root1, key_name1);
            ustr_free(key_name1);
            assert_not_null(kc1);

            assert_equal(orig_secdesc_count + 1,
                         security_descriptor_cell_get_use_count(root_sdc1));

            return true;
        }

    public:

        test_key_cell(): Melunit::Test("test_key_cell")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_key_cell:: name)

#define REG_BOOSTB(name, hive_name) \
            register_test(#name, \
                          boost::bind(&test_key_cell:: name, this, \
                                      hive_name))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_root_keys, *i);
                    REG_BOOSTB(test_top_keys, *i);
                    REG_BOOSTB(test_walk_tree, *i);
                    REG_BOOSTB(test_removed_key_list_cell1, *i);
                    REG_BOOSTB(test_add_key_secdesc1, *i);
                }
            }

            REGISTER(test_add_key1);
            REGISTER(test_add_key2);
            REGISTER(test_add_key3);
            REGISTER(test_add_key4);
            REGISTER(test_delete_key1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_key_cell t1_;
}
