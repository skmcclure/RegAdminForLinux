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

namespace
{
    using namespace RRegAdmin::Test;

    class test_hive3 : public Melunit::Test
    {
    private:

        bool test_get_offset_list(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            offset_list *ofs_list = hive_get_cell_offset_list(hv1);
            assert_not_null(ofs_list);

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
                    offset cofs = cell_get_offset(c1);
                    assert_true(ofslist_remove(ofs_list, cofs));
                }
            }

            assert_equal(0, ofslist_size(ofs_list));

            ofslist_free(ofs_list);

            return true;
        }

    public:

        test_hive3(): Melunit::Test("test_hive3")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_hive3:: name)
#define REG_BOOSTB(name, in_arg) \
            register_test(#name, \
                          boost::bind(&test_hive3:: name, this, in_arg))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_get_offset_list, *i);
                }
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_hive3 t1_;
}
