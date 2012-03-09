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

#include <boost/bind.hpp>

#include <melunit/melunit-cxx.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/hive_check.h>

#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;

    class test_hive_check : public Melunit::Test
    {
    private:

        bool test_basic1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename, false);
            Hive* hv1 = eh1.get_hive();

            RRACheckData data = {
                RRA_CHECK_VERBOSITY_MESSAGE,
                0,
                0};

            assert_true(hive_check(hv1, &data));

            return true;
        }

    public:

        test_hive_check(): Melunit::Test("test_hive_check")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_hive_check:: name)
#define REG_BOOSTB(name, hive_name) \
            register_test(#name, \
                          boost::bind(&test_hive_check:: name, this, \
                                      hive_name))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_basic1, *i);
                }
            }

            Melunit::Suite::instance().register_test(this);

#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_hive_check t1_;
}
