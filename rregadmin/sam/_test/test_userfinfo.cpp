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

#include <rregadmin/sam/userfinfo.h>
#include <rregadmin/sam/sam_info.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/cli/basic.h>
#include <rregadmin/cli/init.h>
#include <rregadmin/hive/value_key_cell.h>

#include "example_registry.h"

#include "userxinfo.h"

namespace
{
    class test_userfinfo : public Melunit::Test
    {
    private:

        bool test_create_null(void)
        {
            UserFInfo *ufi1 = userfinfo_new(NULL);
            assert_null(ufi1);

            return true;
        }

        ValueCell* get_f_value(RRACliState *in_bs)
        {
            Registry *reg = rra_cli_state_get_registry(in_bs);
            ValueKeyCell *vkc =
                registry_path_get_value(reg, SAM_TEST_ADMIN_PATH, "F");
            if (vkc == NULL)
                return NULL;

            ValueCell *ret_val = value_key_cell_get_value_cell(vkc);
            assert_not_null(ret_val);
            return ret_val;
        }

        bool test_create1(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);

            RRACliState *hbs1 =
                rra_cli_state_new_from_win_dir(er1.get_dir().c_str());
            assert_not_null(hbs1);

            ValueCell *vkc = get_f_value(hbs1);
            if (vkc == NULL)
            {
                return true;
            }

            UserFInfo *ufi1 = userfinfo_new(vkc);
            assert_not_null(ufi1);

            assert_false(nttime_is_relative(
                             userfinfo_get_last_login(ufi1)));
            assert_false(nttime_is_relative(
                             userfinfo_get_last_password_change(ufi1)));
            assert_false(nttime_is_relative(
                             userfinfo_get_account_expires(ufi1)));
            // not always true
            // assert_equal(nttime_get_largest_date(),
            //              userfinfo_get_account_expires(ufi1));
            assert_false(nttime_is_relative(
                             userfinfo_get_last_invalid_password(ufi1)));
            assert_equal(500, userfinfo_get_rid(ufi1));
            assert_equal(ACB_NORMAL | ACB_PWNOEXP,
                         userfinfo_get_acb(ufi1));
            assert_compare(std::less_equal, 0,
                           userfinfo_get_bad_login_count(ufi1));
            assert_compare(std::less_equal, 0,
                           userfinfo_get_number_of_logins(ufi1));
            assert_equal(0, userfinfo_get_country_code(ufi1));
            assert_equal(std::string("password-no-expire,normal-account"),
                         std::string(ustr_as_utf8(
                                         userfinfo_get_acb_string(ufi1))));

            assert_true(userfinfo_free(ufi1));
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

    public:

        test_userfinfo(): Melunit::Test("test_userfinfo")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_userfinfo:: name)
#define REG_BOOSTB(name, index) \
            register_test(#name, \
                          boost::bind(&test_userfinfo:: name, this, index))

            REGISTER(test_create_null);

            for (int exs = 0;
                 exs < RRegAdmin::Test::ExampleReg::size();
                 exs++)
            {
                REG_BOOSTB(test_create1, exs);
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_userfinfo t1_;
}
