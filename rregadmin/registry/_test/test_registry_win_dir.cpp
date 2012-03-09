/*
 * Authors:     James LewisMoss <jlm@racemi.com>
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

#include <ctype.h>

#include <fstream>
#include <string>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <glib.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/registry/registry.h>
#include <rregadmin/registry/init.h>

#include "example_registry.h"

namespace
{
    namespace ba = boost::algorithm;

    class test_registry_win_dir : public Melunit::Test
    {
    private:

        bool test_registry_load_win_dir1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            Registry *reg1 = registry_new();

            assert_true(registry_load_win_dir(reg1, er1.get_dir().c_str()));

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_registry_new_win_dir1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);

            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            assert_not_null(reg1);

            assert_true(registry_free(reg1));

            return true;
        }

    public:

        test_registry_win_dir(): Melunit::Test("test_registry_win_dir")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_registry_win_dir:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_registry_win_dir:: name, \
                                      this, ehindex))

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_registry_load_win_dir1, i);
                REG_BOOSTB(test_registry_new_win_dir1, i);
            }

            Melunit::Suite::instance().register_test(this);

#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_registry_win_dir t1_;
}
