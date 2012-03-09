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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <string>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <glib/gstrfuncs.h>
#include <glib/gmem.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/registry/registry.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/registry/hive_info.h>
#include <rregadmin/registry/init.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;
    namespace ba = boost::algorithm;

    class test_registry : public Melunit::Test
    {
    private:

        bool test_new(void)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));
            assert_null(registry_get_registry_key(reg1, -1));
            assert_false(registry_is_dirty(reg1));

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_default_meta_keys(void)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_not_null(
                registry_get_registry_key(reg1, ID_HKEY_CLASSES_ROOT));
            assert_not_null(
                registry_get_registry_key(reg1, ID_HKEY_CURRENT_USER));
            assert_not_null(
                registry_get_registry_key(reg1, ID_HKEY_LOCAL_MACHINE));
            assert_not_null(
                registry_get_registry_key(reg1, ID_HKEY_USERS));
            assert_not_null(
                registry_get_registry_key(reg1, ID_HKEY_PERFORMANCE_DATA));
            assert_not_null(
                registry_get_registry_key(reg1, ID_HKEY_CURRENT_CONFIG));
            assert_not_null(
                registry_get_registry_key(reg1, ID_HKEY_DYN_DATA));

            assert_true(registry_free(reg1));

            return true;
        }

        void key_test_helper(Registry *reg1, const RegistryKey *key1,
                             std::string filename1, reg_id id1,
                             std::string mount_point
                             = "\\HKEY_LOCAL_MACHINE\\system")
        {
            assert_not_null(key1);

            assert_equal(filename1,
                         std::string(registry_key_get_hive_filename(key1)));
            assert_equal(filename1,
                         std::string(
                             registry_id_get_hive_filename(reg1, id1)));

            assert_not_null(registry_key_get_path(key1));

            rra_path *p1 = rra_path_new_win(mount_point.c_str());

            assert_equal(std::string(rra_path_as_str(p1)),
                         std::string(rra_path_as_str(
                                         registry_key_get_path(key1))));
            assert_not_null(registry_key_get_hive(key1));
            assert_not_null(registry_id_get_hive(reg1, id1));
            assert_equal(registry_key_get_hive(key1),
                         registry_id_get_hive(reg1, id1));

            assert_equal(registry_id_get_hive_type(reg1, id1),
                         registry_key_get_hive_type(key1));

            assert_not_null(registry_key_get_key_cell(key1));
        }

        bool test_load_hive1(void)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            std::vector<std::string> hlist = ExampleHive::get_available();

            hive_id id1 = registry_load_hive(reg1, hlist[0].c_str());
            assert_compare(std::not_equal_to, -1, id1);

            assert_equal(1, registry_number_of_hives(reg1));


            assert_false(registry_is_dirty(reg1));

            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_get_registry_key1(void)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            std::vector<std::string> hlist = ExampleHive::get_available();

            hive_id id1 = -1;
            int hlist_index;
            for (hlist_index = 0;
                 hlist_index < hlist.size();
                 hlist_index++)
            {
                if (ba::iends_with(hlist[hlist_index], "system"))
                {
                    id1 = registry_load_hive(reg1, hlist[hlist_index].c_str());
                    break;
                }
            }

            assert_compare(std::not_equal_to, -1, id1);

            assert_equal(1, registry_number_of_hives(reg1));

            const RegistryKey *key1 =
                registry_get_base_registry_key(reg1, id1);
            key_test_helper(reg1, key1, hlist[hlist_index], id1);

            RegistryKey *key2 = registry_key_copy(key1);
            key_test_helper(reg1, key2, hlist[hlist_index], id1);
            assert_true(registry_key_free(key2));

            const RegistryKey *key3 =
                registry_find_registry_key(
                    reg1, "\\HKEY_LOCAL_MACHINE\\system");
            assert_not_null(key3);

            const RegistryKey *key4 =
                registry_find_registry_key(
                    reg1, "\\HKEY_LOCAL_MACHINE\\system\\CurrentControlSet");
            assert_not_null(key4);

            assert_compare(std::not_equal_to, key3, key4);

            assert_equal(key1, key3);

            assert_false(registry_is_dirty(reg1));

            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_load_unload(void)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            for (int i = 0; i < 1000; i++)
            {
                assert_false(registry_unload(reg1, i));
            }

            assert_equal(0, registry_number_of_hives(reg1));

            std::vector<std::string> hlist = ExampleHive::get_available();

            for (int i = 0; i < hlist.size(); i++)
            {
                assert_equal(0, registry_number_of_hives(reg1));
                reg_id id1 = registry_load_hive(reg1, hlist[i].c_str());
                if (id1 == -1)
                    continue;

                assert_equal(1, registry_number_of_hives(reg1));

                assert_true(registry_unload(reg1, id1));
                assert_equal(0, registry_number_of_hives(reg1));
            }

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_load_hive_full1(void)
        {
            Registry *reg1 = registry_new();
            assert_not_null(reg1);

            assert_equal(0, registry_number_of_hives(reg1));

            std::vector<std::string> hlist = ExampleHive::get_available();
            int hive_count_offset = 0;

            for (int i = 0; i < hlist.size(); i++)
            {
                std::string mount_point = "\\mount_point\\";

                reg_id id1 = registry_load_hive_full(
                    reg1, hlist[i].c_str(),
                    (mount_point + hlist[i]).c_str());
                if (id1 == -1)
                {
                    hive_count_offset++;
                    continue;
                }

                assert_equal(i + 1 - hive_count_offset,
                             registry_number_of_hives(reg1));

                const RegistryKey *key1 =
                    registry_get_base_registry_key(reg1, id1);
                key_test_helper(reg1, key1, hlist[i], id1,
                                mount_point + hlist[i]);

                RegistryKey *key2 = registry_key_copy(key1);
                key_test_helper(reg1, key2, hlist[i], id1,
                                mount_point + hlist[i]);

                assert_false(registry_is_dirty(reg1));
            }

            assert_true(registry_unload_all(reg1));
            assert_equal(0, registry_number_of_hives(reg1));

            assert_true(registry_free(reg1));

            return true;
        }

//         void find_all_paths_helper(Registry *reg1,
//                                    const std::string &key_filename,
//                                    const char *hive_path_prefix)
//         {
//             std::ifstream key_file(key_filename.c_str());
//             assert_true(key_file.is_open());

//             while(key_file)
//             {
//                 std::string line1;
//                 std::getline(key_file, line1);

//                 if (line1 == "")
//                     continue;

//                 gchar *path1 = g_ascii_strdown(line1.c_str(), line1.size());
//                 gchar *path2 = g_ascii_strdown(hive_path_prefix,
//                                                strlen(hive_path_prefix));

//                 if (g_str_has_prefix(path1, path2))
//                 {
//                     if (registry_path_is_key(reg1, line1.c_str()))
//                     {
//                         KeyCell *kc = registry_path_get_key(reg1,
//                                                             line1.c_str());
//                         assert_not_null(kc);
//                     }
//                     else if (registry_path_is_value(reg1, line1.c_str()))
//                     {
//                         ValueKeyCell *vkc =
//                             registry_path_get_value(reg1, line1.c_str());
//                         assert_not_null(vkc);
//                     }
//                 }

//                 g_free(path1);
//                 g_free(path2);
//             }
//         }

//         bool file_exists(const std::string &filename)
//         {
//             struct stat stat_data;
//             return ((stat (filename.c_str(), &stat_data) == 0)
//                     ? true
//                     : false);
//         }

//         bool test_find_key1(void)
//         {
//             Registry *reg1 = registry_new();
//             assert_not_null(reg1);

//             assert_equal(0, registry_number_of_hives(reg1));

//             std::vector<std::string> hlist = ExampleHive::get_available();

//             for (int i = 0; i < hlist.size(); i++)
//             {
//                 if (file_exists(hlist[i] + ".paths"))
//                 {
//                     hive_id id1 = registry_load_hive(reg1, hlist[i].c_str());
//                     assert_compare(std::not_equal_to, -1, id1);
//                     const RegistryKey *key1 =
//                         registry_get_base_registry_key(reg1, id1);
//                     find_all_paths_helper(
//                         reg1,
//                         hlist[i] + ".paths",
//                         rra_path_as_str(registry_key_get_path(key1)));

//                     assert_true(registry_unload(reg1, id1));
//                 }
//             }

//             assert_true(registry_free(reg1));

//             return true;
//         }


    public:

        test_registry(): Melunit::Test("test_registry")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_registry:: name)

            REGISTER(test_new);
            REGISTER(test_default_meta_keys);
            REGISTER(test_load_hive1);
            REGISTER(test_get_registry_key1);
            REGISTER(test_load_unload);
            REGISTER(test_load_hive_full1);
//             REGISTER(test_find_key1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_registry t1_;
}
