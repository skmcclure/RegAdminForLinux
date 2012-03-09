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

#include <boost/format.hpp>
#include <boost/bind.hpp>

#include <melunit/melunit-cxx.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;

    class test_keycmp : public Melunit::Test
    {
    private:

        void
        do_subk_order(KeyCell *in_cell)
        {
            guint32 i;

            if (key_cell_get_number_of_subkeys(in_cell) > 1)
            {
                KeyCell *last = key_cell_get_subkey(in_cell, 0);
                ustring *last_name = key_cell_get_name(last);

                for (i = 1; i < key_cell_get_number_of_subkeys(in_cell); i++)
                {
                    KeyCell *next = key_cell_get_subkey(in_cell, i);
                    ustring *next_name = key_cell_get_name(next);

                    int result1 = ustr_regcmp(last_name, next_name);
                    if (result1 > 0)
                    {
                        key_cell_debug_print(last);
                        key_cell_debug_print(next);
                        assert_fail_msg(
                            (boost::format("%1%: %2% vs %3% => %4%")
                             % hive_get_filename(
                                 cell_get_hive(key_cell_to_cell(in_cell)))
                             % ustr_as_utf8(last_name)
                             % ustr_as_utf8(next_name)
                             % result1).str());
                    }

                    ustr_free(last_name);
                    last = next;
                    last_name = next_name;
                }

                ustr_free(last_name);
            }

            for (i = 0; i < key_cell_get_number_of_subkeys(in_cell); i++)
            {
                KeyCell *subk = key_cell_get_subkey(in_cell, i);
                do_subk_order(subk);
            }
        }

        bool test_vs_hives1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            KeyCell *kc1 = hive_get_root_key(hv1);

            do_subk_order(kc1);

            return true;
        }

    public:

        test_keycmp(): Melunit::Test("test_keycmp")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_keycmp:: name)
#define REG_BOOSTB(name, in_hname) \
            register_test(#name, \
                          boost::bind(&test_keycmp:: name, this, in_hname))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_vs_hives1, *i);
                }
            }

            Melunit::Suite::instance().register_test(this);
#undef REG_BOOSTB
#undef REGISTER
        }

    };

    test_keycmp t1_;
}
