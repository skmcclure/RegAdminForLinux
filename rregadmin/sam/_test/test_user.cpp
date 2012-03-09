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

#include <rregadmin/sam/sam_info.h>
#include <rregadmin/sam/user.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/cli/basic.h>
#include <rregadmin/cli/init.h>

#include "example_registry.h"

namespace
{
    class test_user : public Melunit::Test
    {
    private:

        bool test_create_fail1(void)
        {
            SamUser *su1 = samuser_new_by_id(NULL, 500);
            assert_null(su1);

            return true;
        }

        bool test_new_by_name1(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            RRACliState *hbs1 =
                rra_cli_state_new_from_win_dir(er1.get_dir().c_str());
            assert_not_null(hbs1);

            SamInfo *info1 =
                sam_info_new(rra_cli_state_get_registry(hbs1));
            if (info1 == NULL)
            {
                return true;
            }

            const ustring_list *arr = sam_info_get_user_name_list(info1);
            assert_not_null(arr);

            for (int i = 0; i < ustrlist_size(arr); i++)
            {
                const ustring* name = ustrlist_get(arr, i);
                SamUser *u1 = samuser_new_by_name(info1, name);
                assert_not_null(u1);
                assert_equal(std::string(ustr_as_utf8(name)),
                             std::string(
                                 ustr_as_utf8(samuser_get_username(u1))));
                assert_not_null(samuser_get_username(u1));
                assert_not_null(samuser_get_fullname(u1));
                assert_not_null(samuser_get_comment(u1));
                assert_not_null(samuser_get_user_comment(u1));
                assert_not_null(samuser_get_homedir(u1));
                assert_not_null(samuser_get_scriptpath(u1));
                assert_not_null(samuser_get_profilepath(u1));
                assert_not_null(samuser_get_secdesc(u1));
                assert_false(nttime_is_relative(
                                 samuser_get_last_login(u1)));
                assert_false(nttime_is_relative(
                                 samuser_get_last_password_change(u1)));
                assert_false(nttime_is_relative(
                                 samuser_get_account_expires(u1)));
                // assert_equal(nttime_get_largest_date(),
                //              samuser_get_account_expires(u1));
                assert_false(nttime_is_relative(
                                 samuser_get_last_invalid_password(u1)));
                assert_compare(std::less_equal, (rid_type)500,
                               samuser_get_rid(u1));
                // assert_equal(ACB_NORMAL | ACB_PWNOEXP,
                //              samuser_get_acb(u1));
                assert_compare(std::less_equal, 0,
                               samuser_get_bad_login_count(u1));
                assert_compare(std::less_equal, 0,
                               samuser_get_number_of_logins(u1));
                assert_equal(0, samuser_get_country_code(u1));

                assert_true(samuser_free(u1));
            }

            assert_true(sam_info_free(info1));
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_new_by_id1(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            RRACliState *hbs1 =
                rra_cli_state_new_from_win_dir(er1.get_dir().c_str());
            assert_not_null(hbs1);

            SamInfo *info1 =
                sam_info_new(rra_cli_state_get_registry(hbs1));
            if (info1 == NULL)
            {
                return true;
            }

            const GArray *arr = sam_info_get_user_rid_list(info1);
            assert_not_null(arr);

            for (int i = 0; i < arr->len; i++)
            {
                rid_type id = g_array_index(arr, rid_type, i);
                SamUser *u1 = samuser_new_by_id(info1, id);
                assert_not_null(u1);
                assert_equal(id, samuser_get_rid(u1));
                assert_not_null(samuser_get_username(u1));
                assert_not_null(samuser_get_fullname(u1));
                assert_not_null(samuser_get_comment(u1));
                assert_not_null(samuser_get_user_comment(u1));
                assert_not_null(samuser_get_homedir(u1));
                assert_not_null(samuser_get_scriptpath(u1));
                assert_not_null(samuser_get_profilepath(u1));
                assert_not_null(samuser_get_secdesc(u1));
                assert_false(nttime_is_relative(
                                 samuser_get_last_login(u1)));
                assert_false(nttime_is_relative(
                                 samuser_get_last_password_change(u1)));
                assert_false(nttime_is_relative(
                                 samuser_get_account_expires(u1)));
                // assert_equal(nttime_get_largest_date(),
                //              samuser_get_account_expires(u1));
                assert_false(nttime_is_relative(
                                 samuser_get_last_invalid_password(u1)));
                assert_compare(std::less_equal, (rid_type)500,
                               samuser_get_rid(u1));
                // assert_equal(ACB_NORMAL | ACB_PWNOEXP,
                //              samuser_get_acb(u1));
                assert_compare(std::less_equal, 0,
                               samuser_get_bad_login_count(u1));
                assert_compare(std::less_equal, 0,
                               samuser_get_number_of_logins(u1));
                assert_equal(0, samuser_get_country_code(u1));

                samuser_debug_print(u1);
                assert_true(samuser_free(u1));
            }

            assert_true(sam_info_free(info1));
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

    public:

        test_user(): Melunit::Test("test_user")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_user:: name)
#define REG_BOOSTB(name, index) \
            register_test(#name, \
                          boost::bind(&test_user:: name, this, index))

            REGISTER(test_create_fail1);

            for (int exs = 0;
                 exs < RRegAdmin::Test::ExampleReg::size();
                 exs++)
            {
                REG_BOOSTB(test_new_by_id1, exs);
                REG_BOOSTB(test_new_by_name1, exs);
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_user t1_;
}
