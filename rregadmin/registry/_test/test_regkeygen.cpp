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

#include <rregadmin/registry/hive_info.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/registry/regkeygen.h>
#include <rregadmin/registry/init.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;

    class test_regkeygen : public Melunit::Test
    {
    private:

        typedef std::pair<Registry*, hive_id> reg_pair;

        reg_pair
        get_test_reg(void)
        {
            Registry *reg1 = registry_new();
            std::vector<std::string> hlist = ExampleHive::get_available();

            hive_id id = registry_load_hive(reg1, hlist[0].c_str());

            return std::make_pair(reg1, id);
        }

        bool test_null1(void)
        {
            assert_null(reg_key_gen(NULL, -1));

            reg_pair rp1 = get_test_reg();
            assert_null(reg_key_gen(rp1.first, -1));

            assert_null(reg_key_gen(NULL, 1));

            return true;
        }

        bool test_first1(void)
        {
            reg_pair rp1 = get_test_reg();

            GPtrArray *keys1 = reg_key_gen(rp1.first, rp1.second);
            assert_not_null(keys1);
            assert_compare(std::less, (guint)0, keys1->len);

            RegistryKey *key1 = (RegistryKey*)g_ptr_array_index(keys1, 0);
            assert_equal(REG_KEY_TYPE_BASE, registry_key_get_type(key1));

            return true;
        }

        bool test_each1(void)
        {
            Registry *reg1 = registry_new();
            std::vector<std::string> hlist = ExampleHive::get_available();

            for (int i = 0; i < hlist.size(); i++)
            {
                hive_id id = registry_load_hive(reg1, hlist[i].c_str());

                if (id == -1)
                    continue;

                GPtrArray *keys1 = reg_key_gen(reg1, id);

                const hive_info *info = registry_get_hive_info(reg1, id);
                HiveType type = hive_info_get_type(info);

                if (type == HTYPE_SYSTEM)
                {
                    assert_equal(2, keys1->len);
                }
                else
                {
                    assert_equal(1, keys1->len);
                }

                g_ptr_array_free(keys1, TRUE);

                registry_unload(reg1, id);
            }

            return true;
        }


    public:

        test_regkeygen(): Melunit::Test("test_regkeygen")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_regkeygen:: name)

            REGISTER(test_null1);
            REGISTER(test_first1);
            REGISTER(test_each1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_regkeygen t1_;
}
