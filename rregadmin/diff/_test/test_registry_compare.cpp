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

#include <rregadmin/diff/registry.h>

#include <rregadmin/registry/registry_key.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/registry/init.h>
#include <rregadmin/util/value.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/options.h>

#include "good_stuff.h"
#include "FileInfo.hpp"
#include "example_registry.h"

namespace
{

    using namespace RRegAdmin::Test;
    namespace ba = boost::algorithm;

    class test_registry_compare : public Melunit::Test
    {
    private:

        static void print_actions(const char *in_func_name,
                                  int in_index,
                                  const rra_diff_info *in_di,
                                  bool should_print_actions = false)
        {
            if (should_print_actions)
            {
                for (int i = 0; i < rra_di_action_count(in_di); i++)
                {
                    fprintf (stderr, "%s(%d): %s\n", in_func_name, in_index,
                             rra_di_action_to_string(
                                 rra_di_get_action(in_di, i)));
                }
            }
        }

        bool test_no_compare1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            assert_equal(0, rra_di_get_compare_value(di1));
            assert_equal(0, rra_di_action_count(di1));

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_add_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            assert_not_null(registry_path_add_key(
                                reg2, "\\HKEY_LOCAL_MACHINE\\software\\foo"));

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            print_actions(__func__, ehindex, di1);

            assert_compare(std::greater, 0, rra_di_get_compare_value(di1));
            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_ADD_KEY, act1->action);

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_add_key2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            assert_not_null(registry_path_add_key(
                                reg1, "\\HKEY_LOCAL_MACHINE\\software\\foo"));

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            print_actions(__func__, ehindex, di1);

            assert_compare(std::less, 0, rra_di_get_compare_value(di1));
            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_DELETE_KEY, act1->action);

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_delete_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            const ef_line *line = RRegadmin::Test::get_good_key();

            assert_true(registry_path_is_key(reg2, line->path));
            assert_true(registry_path_delete_key(reg2, line->path));
            assert_false(registry_path_is_key(reg2, line->path));

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            print_actions(__func__, ehindex, di1);

            assert_compare(std::less, 0, rra_di_get_compare_value(di1));
            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_DELETE_KEY, act1->action);

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_delete_key2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            const ef_line *line = RRegadmin::Test::get_good_key();

            assert_true(registry_path_is_key(reg1, line->path));
            assert_true(registry_path_delete_key(reg1, line->path));
            assert_false(registry_path_is_key(reg1, line->path));

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            print_actions(__func__, ehindex, di1);

            assert_compare(std::greater, 0, rra_di_get_compare_value(di1));
            assert_compare(std::less_equal, (guint32)1,
                           rra_di_action_count(di1));

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_add_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            const ef_line *line = RRegadmin::Test::get_good_key();
            const char *val_name = "foo";
            Value *val1 = value_create_dword(44);

            assert_false(registry_path_is_value(reg2, line->path, val_name));
            assert_not_null(registry_path_add_value(reg2, line->path, val_name,
                                                    val1));
            assert_true(registry_path_is_value(reg2, line->path, val_name));

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            print_actions(__func__, ehindex, di1);

            assert_compare(std::greater, 0, rra_di_get_compare_value(di1));
            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_ADD_VALUE, act1->action);

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_add_value2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            const ef_line *line = RRegadmin::Test::get_good_key();
            const char *val_name = "foo";
            Value *val1 = value_create_dword(44);

            assert_false(registry_path_is_value(reg1, line->path, val_name));
            assert_not_null(registry_path_add_value(reg1, line->path, val_name,
                                                    val1));
            assert_true(registry_path_is_value(reg1, line->path, val_name));

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            print_actions(__func__, ehindex, di1);

            assert_compare(std::less, 0, rra_di_get_compare_value(di1));
            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_DELETE_VALUE, act1->action);

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_delete_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            const ef_line *line = RRegadmin::Test::get_good_value();

            assert_true(registry_path_is_value(reg2, line->path, line->name));
            assert_not_null(registry_path_delete_value(reg2, line->path,
                                                       line->name));
            assert_false(registry_path_is_value(reg2, line->path, line->name));

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            print_actions(__func__, ehindex, di1);

            assert_compare(std::less, 0, rra_di_get_compare_value(di1));
            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_DELETE_VALUE, act1->action);

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_delete_value2(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            const ef_line *line = RRegadmin::Test::get_good_value();

            assert_true(registry_path_is_value(reg1, line->path, line->name));
            assert_not_null(registry_path_delete_value(reg1, line->path,
                                                       line->name));
            assert_false(registry_path_is_value(reg1, line->path, line->name));

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            print_actions(__func__, ehindex, di1);

            assert_compare(std::greater, 0, rra_di_get_compare_value(di1));
            try
            {
                assert_equal(1, rra_di_action_count(di1));
            }
            catch (const Melunit::assert_error& e)
            {
                print_actions(__func__, ehindex, di1, true);

                for (int i = 0; i < rra_di_action_count(di1); i++)
                {
                    const rra_diff_action *action =
                        rra_di_get_action(di1, i);

                    if (action->action == RRA_DI_CHANGE_VALUE)
                    {
                        rra_di_value_action *va =
                            (rra_di_value_action*)action->data;

                        ValueKeyCell *vkc1 =
                            registry_path_get_value(reg1,
                                                    ustr_as_utf8(va->path),
                                                    ustr_as_utf8(va->name));
                        value_key_cell_debug_print(vkc1);

                        ValueKeyCell *vkc2 =
                            registry_path_get_value(reg2,
                                                    ustr_as_utf8(va->path),
                                                    ustr_as_utf8(va->name));
                        value_key_cell_debug_print(vkc2);
                    }
                }

                throw e;
            }

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_ADD_VALUE, act1->action);

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_change_value1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            const ef_line *line = RRegadmin::Test::get_good_value();
            Value *val1 = value_create_dword(44);

            assert_true(registry_path_is_value(reg2, line->path,
                                               line->name));
            assert_not_null(registry_path_delete_value(reg2, line->path,
                                                       line->name));
            assert_not_null(registry_path_add_value(reg2, line->path,
                                                    line->name, val1));
            assert_true(registry_path_is_value(reg2, line->path, line->name));

            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            print_actions(__func__, ehindex, di1);

            assert_compare(std::not_equal_to,
                           0, rra_di_get_compare_value(di1));
            assert_equal(1, rra_di_action_count(di1));

            const rra_diff_action *act1 = rra_di_get_action(di1, 0);
            assert_not_null(act1);
            assert_equal(RRA_DI_CHANGE_VALUE, act1->action);

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

        bool test_change_case_key1(int ehindex)
        {
            RRegAdmin::Test::ExampleReg er1(ehindex, true);
            Registry *reg1 = registry_new_win_dir(er1.get_dir().c_str());
            RRegAdmin::Test::ExampleReg er2(ehindex, true);
            Registry *reg2 = registry_new_win_dir(er1.get_dir().c_str());

            const ef_line *line = RRegadmin::Test::get_good_key();

            assert_true(registry_path_is_key(reg2, line->path));


            rra_diff_info *di1 = registry_diff(reg1, reg2);
            assert_not_null(di1);
            assert_equal(0, rra_di_get_compare_value(di1));
            assert_equal(0, rra_di_action_count(di1));

            assert_true(registry_free(reg1));
            assert_true(registry_free(reg2));

            return true;
        }

    public:

        test_registry_compare(): Melunit::Test("test_registry_compare")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_registry_compare:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name, \
                          boost::bind(&test_registry_compare:: name, \
                                      this, ehindex))

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_no_compare1, i);
                REG_BOOSTB(test_add_key1, i);
                REG_BOOSTB(test_add_key2, i);
                REG_BOOSTB(test_delete_key1, i);
                REG_BOOSTB(test_delete_key2, i);
                REG_BOOSTB(test_add_value1, i);
                REG_BOOSTB(test_add_value2, i);
                REG_BOOSTB(test_delete_value1, i);
                REG_BOOSTB(test_delete_value2, i);
                REG_BOOSTB(test_change_value1, i);
            }

            Melunit::Suite::instance().register_test(this);

#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_registry_compare t1_;
}
