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
#include <math.h>

#include <functional>

#include <melunit/melunit-cxx.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <rregadmin/diff/hive.h>
#include <rregadmin/registry/init.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/util/options.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;

    class test_hive_compare : public Melunit::Test
    {
    private:

        bool test_compare_versions1()
        {
            HiveVersion v1 = { 0, 0, 0, 0 };
            HiveVersion v2 = { 0, 0, 0, 0 };

            assert_equal(0, hive_version_compare(&v1, &v2));

            v1.major++;
            assert_compare(std::less, 0,
                           hive_version_compare(&v1, &v2));

            v2.major++;
            assert_equal(0, hive_version_compare(&v1, &v2));

            v2.major++;
            assert_compare(std::greater, 0,
                           hive_version_compare(&v1, &v2));

            v1.major++;
            assert_equal(0, hive_version_compare(&v1, &v2));

            ///////////////////
            v1.minor++;
            assert_compare(std::less, 0,
                           hive_version_compare(&v1, &v2));

            v2.minor++;
            assert_equal(0, hive_version_compare(&v1, &v2));

            v2.minor++;
            assert_compare(std::greater, 0,
                           hive_version_compare(&v1, &v2));

            v1.minor++;
            assert_equal(0, hive_version_compare(&v1, &v2));

            ///////////////////
            v1.release++;
            assert_compare(std::less, 0,
                           hive_version_compare(&v1, &v2));

            v2.release++;
            assert_equal(0, hive_version_compare(&v1, &v2));

            v2.release++;
            assert_compare(std::greater, 0,
                           hive_version_compare(&v1, &v2));

            v1.release++;
            assert_equal(0, hive_version_compare(&v1, &v2));

            ///////////////////
            v1.build++;
            assert_compare(std::less, 0,
                           hive_version_compare(&v1, &v2));

            v2.build++;
            assert_equal(0, hive_version_compare(&v1, &v2));

            v2.build++;
            assert_compare(std::greater, 0,
                           hive_version_compare(&v1, &v2));

            v1.build++;
            assert_equal(0, hive_version_compare(&v1, &v2));

            return true;
        }

        bool test_compare_same1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive *hv1 = eh1.get_hive();

            rra_path *path1 = rra_path_new_win("\\");

            assert_equal(0, hive_compare(path1, hv1, hv1));

            return true;
        }

        bool test_compare_similar1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = hive_read(eh1.get_dest().c_str());
            Hive* hv2 = hive_read(eh1.get_dest().c_str());
            assert_not_null(hv2);

            rra_path *path1 = rra_path_new_win("\\");
            assert_equal(0, hive_compare(path1, hv1, hv2));

            hive_free(hv1);
            hive_free(hv2);

            return true;
        }

        bool test_compare_similar2(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive *hv1 = eh1.get_hive();
            ExampleHive eh2(in_hivename);
            Hive *hv2 = eh2.get_hive();

            rra_path *path1 = rra_path_new_win("\\");
            assert_equal(0, hive_compare(path1, hv1, hv2));

            return true;
        }


        bool test_compare_extra_key1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive *hv1 = eh1.get_hive();
            ExampleHive eh2(in_hivename);
            Hive *hv2 = eh2.get_hive();

            rra_path *path1 = rra_path_new_win("\\");
            assert_equal(0, hive_compare(path1, hv1, hv2));

            KeyCell *root1 = hive_get_root_key(hv1);

            ustring *sk_name = ustr_create("test_subkey1");
            assert_not_null(key_cell_add_subkey(root1, sk_name));
            ustr_free(sk_name);

            assert_compare(std::less, 0, hive_compare(path1, hv1, hv2));

            return true;
        }

        bool test_compare_extra_key2(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive *hv1 = eh1.get_hive();
            ExampleHive eh2(in_hivename);
            Hive *hv2 = eh2.get_hive();

            rra_path *path1 = rra_path_new_win("\\");
            assert_equal(0, hive_compare(path1, hv1, hv2));

            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            KeyCell *key1 = key_cell_get_subkey(root1, 0);
            if (key1 != NULL)
            {
                ustring *sk_name = ustr_create("test_subkey1");
                assert_not_null(key_cell_add_subkey(key1, sk_name));
                ustr_free(sk_name);

                assert_compare(std::less, 0, hive_compare(path1, hv1, hv2));
            }

            return true;
        }

    public:

        test_hive_compare(): Melunit::Test("test_hive_compare")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_hive_compare:: name)
#define REG_BOOSTB(name, hive_name) \
            register_test(#name, \
                          boost::bind(&test_hive_compare:: name, this, \
                                      hive_name))

            REGISTER(test_compare_versions1);

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_compare_same1, *i);
                    REG_BOOSTB(test_compare_similar1, *i);
                    REG_BOOSTB(test_compare_similar2, *i);
                    REG_BOOSTB(test_compare_extra_key1, *i);
                    REG_BOOSTB(test_compare_extra_key2, *i);
                }
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_hive_compare t1_;
}
