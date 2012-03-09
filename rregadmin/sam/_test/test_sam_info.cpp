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

#include <sstream>

#include <boost/bind.hpp>
#include <boost/assign/list_of.hpp>

#include <melunit/melunit-cxx.h>

#include <rregadmin/sam/sam_info.h>
#include <rregadmin/sam/user.h>
#include <rregadmin/registry/registry.h>
#include <rregadmin/cli/basic.h>
#include <rregadmin/cli/init.h>

#include "example_registry.h"
#include "vector_utils.h"

namespace
{
    using namespace boost::assign;

    class test_sam_info : public Melunit::Test
    {
    private:

        bool test_create_fail1(void)
        {
            SamInfo *info1 = sam_info_new(NULL);
            assert_null(info1);

            Registry *reg1 = registry_new();
            info1 = sam_info_new(reg1);
            assert_null(info1);

            return true;
        }

        template<class TypeT1, class TypeT2>
        void assert_in_internal(
            const TypeT1 c1, const std::vector<TypeT2> &tv,
            const char filename[] = "",
            int line_num = 0, const std::string &msg = "")
        {
            bool is_in = false;
            for (typename std::vector<TypeT2>::const_iterator i = tv.begin();
                 i != tv.end();
                 i++)
            {
                if (c1 == *i)
                {
                    is_in = true;
                    break;
                }
            }

            std::stringstream ss;
            if (is_in)
            {
                ss << c1 << " is in " << tv;
            }
            else
            {
                ss << c1 << " is not in " << tv;
            }
            std::string my_msg = ss.str();

            assert_true_internal(is_in, filename, line_num,
                                 msg + "\n" + my_msg);
        }
#define assert_in(o1, o2)                                       \
        assert_in_internal((o1), (o2), __FILE__, __LINE__,      \
                           "assert_in(" #o1 " in " #o2 ")")

        bool test_create1(int index)
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

            try
            {
                assert_true(sam_info_is_valid(info1));
                assert_compare(std::less, 0, sam_info_get_next_rid(info1));
                assert_compare(std::less, (guint32)0,
                               sam_info_get_auto_increment(info1));
                assert_true(
                    nttime_is_relative(sam_info_get_max_password_age(info1)));
                assert_true(
                    nttime_is_relative(sam_info_get_lockout_duration(info1)));
                assert_true(
                    nttime_is_relative(sam_info_get_lockout_reset(info1)));

                std::vector<guint32> allowed_flags = list_of(0)(1)(2);
                assert_in(sam_info_get_flags(info1), allowed_flags);

                std::vector<guint16> allowed_min_pass = list_of(0)(7)(8);
                assert_in(sam_info_get_min_pass(info1), allowed_min_pass);

                // assert_false(
                //     sam_info_should_enforce_password_history(info1));
                assert_equal(0, sam_info_get_lockout_threshold(info1));
                assert_not_null(sam_info_get_obfs_syskey(info1));
            }
            catch (const Melunit::assert_error &e)
            {
                sam_info_debug_print(info1);
                throw;
            }

            assert_true(sam_info_free(info1));
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_get_user_number1(int index)
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

            assert_compare(std::less, 0,
                           sam_info_get_number_of_users(info1));

            assert_true(sam_info_free(info1));
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_get_user_name_list1(int index)
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
            assert_equal(sam_info_get_number_of_users(info1),
                         ustrlist_size(arr));

            assert_true(sam_info_free(info1));
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_get_user_rid_list1(int index)
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
            assert_equal(sam_info_get_number_of_users(info1),
                         arr->len);

            assert_true(sam_info_free(info1));
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_get_user_by_name1(int index)
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
                SamUser *u1 = sam_info_get_user_by_name(info1, name);
                assert_not_null(u1);
                assert_equal(std::string(ustr_as_utf8(name)),
                             std::string(
                                 ustr_as_utf8(samuser_get_username(u1))));
                assert_true(samuser_free(u1));
            }

            assert_true(sam_info_free(info1));
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

        bool test_get_user_by_id1(int index)
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
                SamUser *u1 = sam_info_get_user_by_id(info1, id);
                assert_not_null(u1);
                assert_equal(id, samuser_get_rid(u1));
                assert_true(samuser_free(u1));
            }

            assert_true(sam_info_free(info1));
            assert_true(rra_cli_state_free(hbs1));

            return true;
        }

    public:

        test_sam_info(): Melunit::Test("test_sam_info")
        {
            rra_cli_init();
            rra_cli_init_from_env();

#define REGISTER(name) register_test(#name, &test_sam_info:: name)
#define REG_BOOSTB(name, index) \
            register_test(#name, \
                          boost::bind(&test_sam_info:: name, this, index))

            REGISTER(test_create_fail1);

            for (int exs = 0;
                 exs < RRegAdmin::Test::ExampleReg::size();
                 exs++)
            {
                REG_BOOSTB(test_create1, exs);
                REG_BOOSTB(test_get_user_number1, exs);
                REG_BOOSTB(test_get_user_name_list1, exs);
                REG_BOOSTB(test_get_user_rid_list1, exs);
                REG_BOOSTB(test_get_user_by_id1, exs);
                REG_BOOSTB(test_get_user_by_name1, exs);
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_sam_info t1_;
}
