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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include <fstream>
#include <string>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <glib/gstrfuncs.h>
#include <glib/gmem.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>

#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>

#include "example_registry.h"
#include "FileInfo.hpp"

namespace
{
    using namespace RRegAdmin::Test;

    namespace ba = boost::algorithm;

    static const std::string system_hive_name(
        "system32/config/system");

    std::string get_sys_hive_name(ExampleReg& in_er)
    {
        return in_er.get_dir() + "/" + system_hive_name;
    }

    class test_hive2 : public Melunit::Test
    {
    private:

        bool test_load_unload1(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                assert_false(hive_is_dirty(hv1));
                assert_true(hive_free(hv1));
            }

            return true;
        }

        bool test_key_dirty1(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                assert_false(hive_is_dirty(hv1));

                KeyCell *kc1 = hive_get_root_key(hv1);
                assert_not_null(kc1);
                ustring *name = ustr_create("foobar");
                KeyCell *kc2 = key_cell_add_subkey(kc1, name);
                assert_not_null(kc2);
                ustr_free(name);

                assert_true(hive_is_dirty(hv1));

                assert_true(hive_free(hv1));
            }

            return true;
        }

        bool test_value_dirty1(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                assert_false(hive_is_dirty(hv1));

                KeyCell *kc1 = hive_get_root_key(hv1);
                assert_not_null(kc1);
                Value *val1 = value_create_dword(10);
                ValueKeyCell *kc2 = key_cell_add_value(kc1, "foobar", val1);
                assert_not_null(kc2);
                value_free(val1);

                assert_true(hive_is_dirty(hv1));

                assert_true(hive_free(hv1));
            }

            return true;
        }

        bool test_write1(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            Hive *hv1 = hive_read(get_sys_hive_name(er1).c_str());
            do_if_fail(
                assert_not_null_msg(hv1,
                                    er1.get_name()
                                    + ", " + get_sys_hive_name(er1)),
                do
                {
                    std::string ls_cmd = "ls -lR ";
                    ls_cmd += er1.get_dir();
                    system(ls_cmd.c_str());
                } while(0));

            assert_false(hive_is_dirty(hv1));

            FileInfo before(get_sys_hive_name(er1));

            KeyCell *kc1 = hive_get_root_key(hv1);
            assert_not_null(kc1);
            Value *val1 = value_create_dword(10);
            ValueKeyCell *kc2 = key_cell_add_value(kc1, "foobar", val1);
            assert_not_null(kc2);
            value_free(val1);

            assert_true(hive_is_dirty(hv1));

            assert_true(hive_write(hv1, get_sys_hive_name(er1).c_str()));

            FileInfo after(get_sys_hive_name(er1));

            assert_compare(std::not_equal_to, after, before);

            return true;
        }

        bool test_write2(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            Hive *hv1 = hive_read(get_sys_hive_name(er1).c_str());
            assert_not_null(hv1);

            KeyCell *kc1 = hive_get_root_key(hv1);
            assert_not_null(kc1);
            Value *val1 = value_create_dword(10);
            ValueKeyCell *vkc1 = key_cell_add_value(kc1, "foobar", val1);
            assert_not_null(vkc1);
            value_free(val1);

            assert_true(hive_write(hv1, get_sys_hive_name(er1).c_str()));

            assert_true(hive_free(hv1));

            Hive *hv2 = hive_read(get_sys_hive_name(er1).c_str());
            assert_not_null(hv2);

            KeyCell *kc2 = hive_get_root_key(hv2);
            assert_not_null(kc2);
            ValueKeyCell *vkc2 = key_cell_get_value_str(kc2, "foobar");
            assert_not_null(vkc2);

            Value *val2 = value_key_cell_get_val(vkc2);
            assert_not_null(val2);
            assert_equal(REG_DWORD, value_get_type(val2));
            assert_equal(10, value_get_as_dword(val2));
            assert_true(value_free(val2));

            assert_true(hive_free(hv2));

            return true;
        }

    public:

        test_hive2(): Melunit::Test("test_hive2")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_hive2:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_hive2:: name, this, ehindex))

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_load_unload1, i);
                REG_BOOSTB(test_key_dirty1, i);
                REG_BOOSTB(test_value_dirty1, i);
                REG_BOOSTB(test_write1, i);
                REG_BOOSTB(test_write2, i);
            }

            Melunit::Suite::instance().register_test(this);

#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_hive2 t1_;
}
