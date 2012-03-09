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


#include <melunit/melunit-cxx.h>

#include <rregadmin/registry/hive_info.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/registry/init.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>

#include "example_hive.h"

namespace
{
    class test_regkey : public Melunit::Test
    {
    private:

        bool test_simple1(void)
        {
            RegKey *key1 = regkey_new("foo");
            assert_not_null(key1);

            assert_null(regkey_get_parent(key1));
            assert_equal(0, regkey_get_number_of_children(key1));
            assert_equal(std::string("foo"),
                         std::string(regkey_get_name(key1)));

            assert_true(regkey_free(key1));

            return true;
        }

    public:

        test_regkey(): Melunit::Test("test_regkey")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_regkey:: name)

            REGISTER(test_fail_base1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_regkey t1_;
}
