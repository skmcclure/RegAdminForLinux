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

#include <rregadmin/hive/foreach.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/hive/util.h>
#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;

    class test_foreach : public Melunit::Test
    {
    private:

        static int count;

        static gboolean
        foreach_key_count_helper(KeyCell *in_kc,
                                 const rra_path *in_key_location,
                                 gpointer in_data)
        {
            count++;
            return TRUE;
        }

        int
        manual_key_counter(KeyCell *in_kc)
        {
            int ret_val = 1;

            for (int i = 0; i < key_cell_get_number_of_subkeys(in_kc); i++)
            {
                ret_val += manual_key_counter(key_cell_get_subkey(in_kc, i));
            }

            return ret_val;
        }

        bool test_count_keys1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            count = 0;

            int manual_count = manual_key_counter(hive_get_root_key(hv1));

            assert_true(hive_foreach_key(hv1, NULL, foreach_key_count_helper,
                                         NULL));

            assert_equal(manual_count, count);

            return true;
        }

        static gboolean
        foreach_val_count_helper(KeyCell *in_kc, ValueKeyCell *in_vc,
                                 const rra_path *in_key_location,
                                 gpointer in_data)
        {
            count++;
            return TRUE;
        }

        int
        manual_val_counter(KeyCell *in_kc)
        {
            int ret_val = key_cell_get_number_of_values(in_kc);

            for (int i = 0; i < key_cell_get_number_of_subkeys(in_kc); i++)
            {
                ret_val += manual_val_counter(key_cell_get_subkey(in_kc, i));
            }

            return ret_val;
        }

        bool test_count_vals1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            count = 0;

            int manual_count = manual_val_counter(hive_get_root_key(hv1));

            assert_true(
                hive_foreach_value(hv1, NULL, foreach_val_count_helper, NULL));

            assert_equal(manual_count, count);

            return true;
        }

        static gboolean
        foreach_key_path_checker(KeyCell *in_kc,
                                 const rra_path *in_key_location,
                                 gpointer in_data)
        {
            rra_path *calc_path =
                generate_key_cell_path(key_cell_get_parent(in_kc));

            gboolean ret_val = rra_path_equal(calc_path, in_key_location);

            rra_path_free(calc_path);

            return ret_val;
        }

        bool test_correct_key_path(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            assert_true(hive_foreach_key(hv1, NULL, foreach_key_path_checker,
                                         NULL));

            return true;
        }

        static gboolean
        foreach_value_key_path_checker(KeyCell *in_kc,
                                       ValueKeyCell *in_vkc,
                                       const rra_path *in_key_location,
                                       gpointer in_data)
        {
            rra_path *calc_path =
                generate_key_cell_path(in_kc);

            gboolean ret_val = rra_path_equal(calc_path, in_key_location);

            rra_path_free(calc_path);

            return ret_val;
        }

        bool test_correct_value_key_path(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            assert_true(hive_foreach_value(hv1, NULL,
                                           foreach_value_key_path_checker,
                                           NULL));

            return true;
        }


    public:

        test_foreach(): Melunit::Test("test_foreach")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_foreach:: name)
#define REG_BOOSTB(name, hive_name) \
            register_test(#name, \
                          boost::bind(&test_foreach:: name, this, \
                                      hive_name))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_count_keys1, *i);
                    REG_BOOSTB(test_count_vals1, *i);
                    REG_BOOSTB(test_correct_key_path, *i);
                    REG_BOOSTB(test_correct_value_key_path, *i);
                }
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    int test_foreach::count;

    test_foreach t1_;
}
