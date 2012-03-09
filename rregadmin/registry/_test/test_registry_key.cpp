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

#include <boost/algorithm/string.hpp>

#include <rregadmin/registry/hive_info.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/registry/init.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>

#include "example_hive.h"

namespace
{
    namespace ba = boost::algorithm;

    using namespace RRegAdmin::Test;

    class test_registry_key : public Melunit::Test
    {
    private:

        bool test_fail_base1(void)
        {
            RegistryKey *key1 = registry_key_new_base(NULL, 0, NULL);
            assert_null(key1);

            Registry *reg1 = registry_new();
            key1 = registry_key_new_base(reg1, 0, NULL);
            assert_null(key1);

            key1 = registry_key_new_base(reg1, -1, "\\some\\path");
            assert_null(key1);

            key1 = registry_key_new_base(reg1, -1, "a\\non\\absolute");
            assert_null(key1);

            registry_free(reg1);

            return true;
        }

        bool test_fail_meta1(void)
        {
            RegistryKey *key1 = registry_key_new_meta(NULL, NULL, 0);
            assert_null(key1);

            Registry *reg1 = registry_new();
            key1 = registry_key_new_meta(reg1, NULL, 0);
            assert_null(key1);

            key1 = registry_key_new_meta(reg1, "\\some\\path",
                                         registry_key_id_start() + 10);
            assert_null(key1);

            key1 = registry_key_new_meta(reg1, "a\\non\\absolute", 0);
            assert_null(key1);

            return true;
        }

        bool test_fail_link1(void)
        {
            RegistryKey *key1 = registry_key_new_link(NULL, 0, NULL, NULL);
            assert_null(key1);

            Registry *reg1 = registry_new();
            key1 = registry_key_new_link(reg1, 0, "\\some\\path", NULL);
            assert_null(key1);

            key1 = registry_key_new_link(reg1, -1, "\\some\\path",
                                         (KeyCell*)0xff);
            assert_null(key1);

            key1 = registry_key_new_link(reg1, 0, "a\\non\\absolute",
                                         (KeyCell*)0xff);
            assert_null(key1);

            return true;
        }

        typedef std::pair<Registry*, hive_id> reg_pair;

        reg_pair
        get_test_reg(void)
        {
            Registry *reg1 = registry_new();
            std::vector<std::string> hlist = ExampleHive::get_available();

            for (int i = 0; i < hlist.size(); i++)
            {
                if (ba::iends_with(hlist[i], "system"))
                {
                    hive_id id = registry_load_hive(reg1, hlist[i].c_str());
                    return std::make_pair(reg1, id);
                }
            }

            return std::make_pair(reg1, -1);
        }

        bool test_base1(void)
        {
            reg_pair rp1 = get_test_reg();

            RegistryKey *key1 = registry_key_new_base(rp1.first, rp1.second,
                                                      "\\A\\location");
            assert_not_null(key1);

            assert_equal(REG_KEY_TYPE_BASE,
                         registry_key_get_type(key1));
            assert_equal(std::string("\\A\\location"),
                         std::string(rra_path_as_str(
                                         registry_key_get_path(key1))));
            assert_not_null(registry_key_get_hive(key1));
            assert_equal(HTYPE_SYSTEM,
                         registry_key_get_hive_type(key1));
            assert_not_null(registry_key_get_key_cell(key1));
            assert_compare(std::greater_equal,
                           registry_key_get_id(key1),
                           registry_key_id_start());
            assert_false(registry_key_set_key_cell(key1, (KeyCell*)0xff));

            RegistryKey *key2 = registry_key_copy(key1);
            assert_not_null(key2);
            assert_equal(REG_KEY_TYPE_BASE,
                         registry_key_get_type(key2));
            assert_equal(std::string("\\A\\location"),
                         std::string(rra_path_as_str(
                                         registry_key_get_path(key2))));
            assert_not_null(registry_key_get_hive(key2));
            assert_equal(HTYPE_SYSTEM,
                         registry_key_get_hive_type(key2));
            assert_not_null(registry_key_get_key_cell(key2));
            assert_compare(std::greater_equal,
                           registry_key_get_id(key2),
                           registry_key_id_start());
            assert_true(registry_key_set_key_cell(key2, (KeyCell*)0xff));

            assert_true(registry_key_free(key2));
            assert_false(registry_key_free(key1));
            assert_true(registry_key_free_force(key1));

            return true;
        }

        bool test_meta1(void)
        {
            reg_pair rp1 = get_test_reg();

            RegistryKey *key1 = registry_key_new_meta(rp1.first,
                                                      "\\A\\location",
                                                      0);
            assert_not_null(key1);

            assert_equal(REG_KEY_TYPE_META,
                         registry_key_get_type(key1));
            assert_equal(std::string("\\A\\location"),
                         std::string(rra_path_as_str(
                                         registry_key_get_path(key1))));
            assert_null(registry_key_get_hive(key1));
            assert_equal(HTYPE_UNKNOWN,
                         registry_key_get_hive_type(key1));
            assert_null(registry_key_get_key_cell(key1));
            assert_compare(std::less_equal,
                           registry_key_get_id(key1),
                           registry_key_id_start());
            assert_compare(std::greater_equal,
                           registry_key_get_id(key1),
                           0);
            assert_false(registry_key_set_key_cell(key1, (KeyCell*)0xff));

            RegistryKey *key2 = registry_key_copy(key1);
            assert_not_null(key2);
            assert_equal(REG_KEY_TYPE_META,
                         registry_key_get_type(key2));
            assert_equal(std::string("\\A\\location"),
                         std::string(rra_path_as_str(
                                         registry_key_get_path(key2))));
            assert_null(registry_key_get_hive(key2));
            assert_equal(HTYPE_UNKNOWN,
                         registry_key_get_hive_type(key2));
            assert_null(registry_key_get_key_cell(key2));
            assert_compare(std::less_equal,
                           registry_key_get_id(key2),
                           registry_key_id_start());
            assert_compare(std::greater_equal,
                           registry_key_get_id(key2),
                           0);
            assert_true(registry_key_set_key_cell(key2, (KeyCell*)0xff));

            assert_true(registry_key_free(key2));
            assert_false(registry_key_free(key1));
            assert_true(registry_key_free_force(key1));

            return true;
        }

        bool test_link1(void)
        {
            reg_pair rp1 = get_test_reg();
            Hive *hv1 = registry_id_get_hive(rp1.first, rp1.second);
            assert_not_null(hv1);
            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            KeyCell *kc1 = key_cell_get_subkey(root1, 0);
            assert_not_null(kc1);

            RegistryKey *key1 = registry_key_new_link(rp1.first,
                                                      rp1.second,
                                                      "\\A\\location",
                                                      kc1);
            assert_not_null(key1);

            assert_equal(REG_KEY_TYPE_LINK,
                         registry_key_get_type(key1));
            assert_equal(std::string("\\A\\location"),
                         std::string(rra_path_as_str(
                                         registry_key_get_path(key1))));
            assert_not_null(registry_key_get_hive(key1));
            assert_equal(HTYPE_SYSTEM,
                         registry_key_get_hive_type(key1));
            assert_not_null(registry_key_get_key_cell(key1));
            assert_compare(std::not_equal_to,
                           root1,
                           registry_key_get_key_cell(key1));
            assert_equal(kc1, registry_key_get_key_cell(key1));
            assert_compare(std::greater_equal,
                           registry_key_get_id(key1),
                           registry_key_id_start());
            assert_false(registry_key_set_key_cell(key1, (KeyCell*)0xff));

            RegistryKey *key2 = registry_key_copy(key1);
            assert_not_null(key2);
            assert_equal(REG_KEY_TYPE_LINK,
                         registry_key_get_type(key2));
            assert_equal(std::string("\\A\\location"),
                         std::string(rra_path_as_str(
                                         registry_key_get_path(key2))));
            assert_not_null(registry_key_get_hive(key2));
            assert_equal(HTYPE_SYSTEM,
                         registry_key_get_hive_type(key2));
            assert_not_null(registry_key_get_key_cell(key2));
            assert_compare(std::not_equal_to,
                           root1,
                           registry_key_get_key_cell(key1));
            assert_equal(kc1, registry_key_get_key_cell(key1));
            assert_compare(std::greater_equal,
                           registry_key_get_id(key2),
                           registry_key_id_start());
            assert_true(registry_key_set_key_cell(key2, (KeyCell*)0xff));

            assert_true(registry_key_free(key2));
            assert_false(registry_key_free(key1));
            assert_true(registry_key_free_force(key1));

            return true;
        }

    public:

        test_registry_key(): Melunit::Test("test_registry_key")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_registry_key:: name)

            REGISTER(test_fail_base1);
            REGISTER(test_fail_meta1);
            REGISTER(test_fail_link1);
            REGISTER(test_base1);
            REGISTER(test_meta1);
            REGISTER(test_link1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_registry_key t1_;
}
