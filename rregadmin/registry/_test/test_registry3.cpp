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

#include <glib/gstrfuncs.h>
#include <glib/gfileutils.h>
#include <glib/gmem.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/registry/registry.h>
#include <rregadmin/registry/registry_key.h>
#include <rregadmin/registry/init.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/util/value.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/log.h>

#include "FileInfo.hpp"
#include "example_registry.h"

namespace
{
    using namespace RRegAdmin::Test;
    namespace ba = boost::algorithm;

    class test_registry3 : public Melunit::Test
    {
    private:

        // These were some problematic keys/values that I wanted to test
        // explicitly.  If more test data is added this may fail with the
        // new test data.
        bool test_good_keys1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            const char *good_keys[] = {
                "\\HKEY_LOCAL_MACHINE\\software\\Microsoft\\Internet Explorer\\ActiveX Compatibility\\{00000566-0000-0010-8000-00AA006D2EA4}",
                "\\HKEY_LOCAL_MACHINE\\software\\Microsoft\\Windows\\CurrentVersion\\Applets\\DeluxeCD\\Providers\\Provider0001",
                "\\HKEY_LOCAL_MACHINE\\software\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\Central European Standard Time"
            };

            guint i;

            for (i = 0; i < G_N_ELEMENTS(good_keys); i++)
            {
                assert_true_msg(registry_path_is_key(reg1, good_keys[i]),
                                good_keys[i]);
            }

            assert_true(registry_free(reg1));

            return true;
        }

        bool test_good_values1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());

            const char *good_values[][2] = {
                {
                    "\\HKEY_LOCAL_MACHINE\\software\\Microsoft\\Internet Explorer\\ActiveX Compatibility\\{00000566-0000-0010-8000-00AA006D2EA4}",
                    "Compatibility Flags"
                },
                {
                    "\\HKEY_LOCAL_MACHINE\\system\\ControlSet001\\Services\\sym_hi\\Parameters\\PnpInterface",
                    "5"
                },
                {
                    "\\HKEY_LOCAL_MACHINE\\software\\Microsoft\\Windows\\CurrentVersion\\Controls Folder\\Device\\shellex\\PropertySheetHandlers\\ICM Monitor Management\\",
                    ""
                },
                {
                    "\\HKEY_LOCAL_MACHINE\\software\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\Eastern Standard Time",
                    "Dlt"
                },
                {
                    "\\HKEY_LOCAL_MACHINE\\software\\Microsoft\\NetDDE\\DDE Shares\\Chat$",
                    "ShareName"
                }
            };

            guint i;

            for (i = 0; i < G_N_ELEMENTS(good_values); i++)
            {
                assert_true_msg(registry_path_is_value(reg1,
                                                       good_values[i][0],
                                                       good_values[i][1]),
                                good_values[i][0]);
            }

            assert_true(registry_free(reg1));

            return true;
        }

    public:

        test_registry3(): Melunit::Test("test_registry3")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_registry3:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_registry3:: name, this, ehindex))

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_good_keys1, i);
                REG_BOOSTB(test_good_values1, i);
            }

            Melunit::Suite::instance().register_test(this);

#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_registry3 t1_;
}
