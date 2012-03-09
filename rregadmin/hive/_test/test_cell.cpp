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

extern "C"
{
    static int silly_count = 0;

    struct silly_data
    {
        int foo;
    };
    typedef struct silly_data silly_data;

    silly_data* silly_data_new(int in_foo)
    {
        silly_data *ret_val = rra_new_type(silly_data);
        ret_val->foo = in_foo;
        silly_count++;
        return ret_val;
    }

    void silly_data_delete (void *in_data)
    {
        rra_free_type(silly_data, in_data);
        silly_count--;
    }
}

namespace
{
    using namespace RRegAdmin::Test;

    class test_cell : public Melunit::Test
    {
    private:

        bool test_find_all(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            Bin *b1;
            for (b1 = hive_get_first_bin(hv1);
                 b1 != NULL;
                 b1 = bin_get_next_bin(b1))
            {
                Cell *c1;
                for (c1 = bin_first_cell(b1);
                     c1 != NULL;
                     c1 = cell_get_next(c1))
                {
                    assert_true(cell_is_valid(c1));
                    assert_compare(std::less_equal,
                                   (guint32)2, cell_get_data_length(c1));
                    assert_compare(std::less_equal,
                                   (guint32)8, cell_size(c1));
                    assert_true(cell_check_mark(c1, CELL_FLAG_VALID));
                }
            }

            return true;
        }

        bool test_cell_data1 (const std::string &in_hive)
        {
            assert_equal(0, silly_count);

            Hive *hv1 = hive_read(in_hive.c_str());

            Bin *b1 = hive_get_first_bin(hv1);
            Cell *c1 = bin_first_cell(b1);

            assert_true(cell_is_valid(c1));

            c1->data = silly_data_new(10);
            c1->deleter = silly_data_delete;

            assert_equal(1, silly_count);

            hive_free(hv1);

            assert_equal(0, silly_count);

            return true;
        }

        bool test_cell_data2 (const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            Bin *b1 = hive_get_first_bin(hv1);
            Cell *c1 = bin_first_cell(b1);

            assert_true(cell_is_valid(c1));

            c1->data = g_new0(char, 10);

            return true;
        }

    public:

        test_cell(): Melunit::Test("test_cell")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_cell:: name)
#define REG_BOOSTB(name, in_arg) \
            register_test(#name, \
                          boost::bind(&test_cell:: name, this, in_arg))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_find_all, *i);
                    REG_BOOSTB(test_cell_data1, *i);
                    REG_BOOSTB(test_cell_data2, *i);
                }
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_cell t1_;
}
