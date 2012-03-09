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

#include <boost/bind.hpp>

#include <melunit/melunit-cxx.h>

#include <rregadmin/sam/uservinfo.h>
#include <rregadmin/sam/sam_info.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/cli/basic.h>
#include <rregadmin/cli/init.h>
#include <rregadmin/hive/value_key_cell.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/util/ustring.h>

#include "example_registry.h"

#include "userxinfo.h"

namespace
{
    class test_uservinfo2 : public Melunit::Test
    {
    private:

        bool test_create_null(void)
        {
            UserVInfo *ufi1 = uservinfo_new(NULL);
            assert_null(ufi1);

            return true;
        }

        ValueCell* get_v_value(Registry *in_reg, ustring *in_path)
        {
            ValueKeyCell *vkc =
                registry_path_get_value(in_reg, ustr_as_utf8(in_path), "V");
            assert_not_null(vkc);
            ValueCell *ret_val = value_key_cell_get_value_cell(vkc);
            assert_not_null(ret_val);
            return ret_val;
        }

        typedef void (test_uservinfo2::*test_func)(UserVInfo*);

        void run_for_each_user(RRACliState *in_bs,
                               test_func func)
        {
            int i;
            Registry *reg = rra_cli_state_get_registry(in_bs);

            KeyCell *kc = registry_path_get_key(reg, SAM_TEST_USER_PATH);
            if (kc == NULL)
                return;

            for (i = 0; i < key_cell_get_number_of_subkeys(kc); i++)
            {
                KeyCell *subk = key_cell_get_subkey(kc, i);
                ustring *u1 = key_cell_get_name(subk);
                // fprintf (stderr, "Key: %s\n", ustr_as_utf8(u1));
                if (ustr_as_utf8(u1)[0] == '0')
                {
                    ustr_strnprepend(u1, "\\", 1);
                    ustr_strnprepend(u1, SAM_TEST_USER_PATH,
                                     strlen(SAM_TEST_USER_PATH));

                    ValueCell *vc = get_v_value(reg, u1);
                    assert_not_null(vc);
                    UserVInfo *uvi1 = uservinfo_new(vc);
                    assert_not_null(uvi1);

                    (this->*func)(uvi1);

                    assert_true(uservinfo_free(uvi1));
                }
            }
        }

        void create1_each_helper(UserVInfo *in_uvi)
        {
            assert_true(uservinfo_is_valid(in_uvi));

            // uservinfo_debug_print(in_uvi);
        }

        bool test_create1(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            RRACliState *hbs1 =
                rra_cli_state_new_from_win_dir(er1.get_dir().c_str());
            assert_not_null(hbs1);

            run_for_each_user(hbs1, &test_uservinfo2::create1_each_helper);

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        void strings1_each_helper(UserVInfo *in_uvi)
        {
            assert_not_null(in_uvi);

            assert_not_null(uservinfo_get_username(in_uvi));
            assert_not_null(uservinfo_get_fullname(in_uvi));
            assert_not_null(uservinfo_get_comment(in_uvi));
            assert_not_null(uservinfo_get_user_comment(in_uvi));
            assert_not_null(uservinfo_get_homedir(in_uvi));
            assert_not_null(uservinfo_get_scriptpath(in_uvi));
            assert_not_null(uservinfo_get_profilepath(in_uvi));
            assert_not_null(uservinfo_get_secdesc(in_uvi));
        }

        bool test_strings1(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            RRACliState *hbs1 =
                rra_cli_state_new_from_win_dir(er1.get_dir().c_str());
            assert_not_null(hbs1);

            run_for_each_user(hbs1, &test_uservinfo2::strings1_each_helper);

            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

    public:

        test_uservinfo2(): Melunit::Test("test_uservinfo2")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_uservinfo2:: name)
#define REG_BOOSTB(name, index) \
            register_test(#name, \
                          boost::bind(&test_uservinfo2:: name, this, index))

            REGISTER(test_create_null);

            for (int exs = 0;
                 exs < RRegAdmin::Test::ExampleReg::size();
                 exs++)
            {
                REG_BOOSTB(test_create1, exs);
                REG_BOOSTB(test_strings1, exs);
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_uservinfo2 t1_;
}
