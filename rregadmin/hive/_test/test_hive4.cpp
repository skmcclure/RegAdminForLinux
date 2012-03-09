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
#include <libgen.h>

#include <functional>

#include <melunit/melunit-cxx.h>

#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>
#include <rregadmin/util/malloc.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;

    class test_hive4 : public Melunit::Test
    {
    private:

        std::string randomize_name(const std::string &in_name)
        {
            std::string ret_val;
            for(int i = 0; i < in_name.size(); i++)
            {
                if (rand() % 2 == 0)
                {
                    ret_val += toupper(in_name[i]);
                }
                else
                {
                    ret_val += tolower(in_name[i]);
                }
            }
            return ret_val;
        }

        std::string copy_random(const std::string &in_filename,
                                const char *in_basename)
        {
            std::string base_fn = std::string(in_basename);
            std::string new_file_name = randomize_name(base_fn);

            std::string cp_cmd =
                (boost::format("cp %1% %2%")
                 % in_filename
                 % new_file_name).str();
            assert_true(system(cp_cmd.c_str()) != -1);

            return new_file_name;
        }

        void remove_random(const std::string &in_filename)
        {
            unlink(in_filename.c_str());
        }

        bool test_casei(const std::string &in_filename)
        {
            char *bn_cs = g_strdup(in_filename.c_str());
            char *bn = basename(bn_cs);

            std::string fn = copy_random(in_filename, bn);

            Hive* hv1 = hive_read(bn);

            assert_not_null(hv1);

            hive_free(hv1);
            remove_random(fn);

            return true;
        }

    public:

        test_hive4(): Melunit::Test("test_hive4")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_hive4:: name)
#define REG_BOOSTB(name, in_arg) \
            register_test(#name, \
                          boost::bind(&test_hive4:: name, this, in_arg))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_casei, *i);
                }
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_hive4 t1_;
}
