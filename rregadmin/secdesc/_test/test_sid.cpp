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


#include <melunit/melunit-cxx.h>

#include <vector>

#include <boost/assign/list_of.hpp>

#include <rregadmin/secdesc/sid.h>
#include <rregadmin/util/init.h>

namespace
{
    using namespace boost::assign;

    class test_sid : public Melunit::Test
    {
    private:

        bool test_new(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new();
            assert_not_null(s1);

            assert_equal(SID_DEFAULT_REV, sid_get_rev(s1));
            assert_equal(0, sid_get_subauth_count(s1));
            assert_equal(SID_DEFAULT_ID, sid_get_id(s1));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_set_id(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new();
            assert_not_null(s1);

            assert_equal(SID_DEFAULT_ID, sid_get_id(s1));

            assert_true(sid_set_id(s1, 10));
            assert_equal(10, sid_get_id(s1));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_new_id(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(20);
            assert_not_null(s1);

            assert_equal(SID_DEFAULT_REV, sid_get_rev(s1));
            assert_equal(0, sid_get_subauth_count(s1));
            assert_equal(20, sid_get_id(s1));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_push_subauth(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(10);
            assert_not_null(s1);

            assert_equal(0, sid_get_subauth_count(s1));
            assert_equal(SID_ERROR_ID, sid_get_subauth(s1, 0));
            assert_equal(SID_ERROR_ID, sid_get_subauth(s1, 1));

            assert_true(sid_push_subauth(s1, 30));
            assert_equal(1, sid_get_subauth_count(s1));

            assert_equal(30, sid_get_subauth(s1, 0));
            assert_equal(30, sid_get_last_subauth(s1));
            assert_equal(SID_ERROR_ID, sid_get_subauth(s1, 1));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_push_pop_subauth(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(10);
            assert_not_null(s1);

            assert_equal(0, sid_get_subauth_count(s1));

            assert_true(sid_push_subauth(s1, 30));
            assert_equal(1, sid_get_subauth_count(s1));
            assert_equal(30, sid_get_subauth(s1, 0));
            assert_equal(30, sid_get_last_subauth(s1));

            assert_true(sid_pop_subauth(s1));
            assert_equal(0, sid_get_subauth_count(s1));
            assert_equal(SID_ERROR_ID, sid_get_subauth(s1, 0));
            assert_equal(SID_ERROR_ID, sid_get_last_subauth(s1));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_copy(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(10);
            assert_not_null(s1);
            assert_true(sid_push_subauth(s1, 30));

            Sid *s2 = sid_copy(s1);

            assert_compare(std::not_equal_to, s1, s2);
            assert_equal(sid_get_id(s1), sid_get_id(s2));
            assert_equal(sid_get_subauth_count(s1),
                         sid_get_subauth_count(s2));
            for (int i = 0; i < sid_get_subauth_count(s2); i++)
            {
                assert_equal(sid_get_subauth(s1, i),
                             sid_get_subauth(s2, i));
            }

            assert_true(sid_free(s1));
            assert_true(sid_free(s2));

            return true;
        }

        bool test_push_all_subauth(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(10);
            assert_not_null(s1);

            assert_equal(0, sid_get_subauth_count(s1));

            for (int i = 0; i < 15; i++)
            {
                rid_type id1 = (i + 2) * (i + 2);
                assert_true(sid_push_subauth(s1, id1));
                assert_equal(i + 1, sid_get_subauth_count(s1));
                assert_equal(id1, sid_get_last_subauth(s1));
            }

            assert_false(sid_push_subauth(s1, 1234));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_copy2(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(10);
            assert_not_null(s1);
            for (int i = 0; i < 15; i++)
            {
                rid_type id1 = (i + 2) * (i + 2);
                assert_true(sid_push_subauth(s1, id1));
                assert_equal(i + 1, sid_get_subauth_count(s1));
                assert_equal(id1, sid_get_last_subauth(s1));
            }

            Sid *s2 = sid_copy(s1);

            assert_compare(std::not_equal_to, s1, s2);
            assert_equal(sid_get_id(s1), sid_get_id(s2));
            assert_equal(sid_get_subauth_count(s1),
                         sid_get_subauth_count(s2));
            for (int i = 0; i < sid_get_subauth_count(s2); i++)
            {
                assert_equal(sid_get_subauth(s1, i),
                             sid_get_subauth(s2, i));
            }

            assert_true(sid_free(s1));
            assert_true(sid_free(s2));

            return true;
        }

        bool test_compare1(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(10);
            Sid *s2 = sid_new_id(10);

            assert_equal(0, sid_compare(s1, s2));

            assert_true(sid_free(s1));
            assert_true(sid_free(s2));

            return true;
        }

        bool test_compare2(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(10);
            assert_true(sid_push_subauth(s1, 40));
            Sid *s2 = sid_new_id(10);
            assert_true(sid_push_subauth(s2, 40));

            assert_equal(0, sid_compare(s1, s2));

            assert_true(sid_free(s1));
            assert_true(sid_free(s2));

            return true;
        }

        bool test_compare3(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(10);
            assert_true(sid_push_subauth(s1, 40));
            Sid *s2 = sid_new_id(10);
            assert_true(sid_push_subauth(s2, 50));

            assert_compare(std::greater, 0, sid_compare(s1, s2));

            assert_true(sid_free(s1));
            assert_true(sid_free(s2));

            return true;
        }

        bool test_to_sddl1(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1 = sid_new_id(10);

            assert_equal(std::string("S-1-10"),
                         std::string(sid_to_sddl(s1)));

            assert_true(sid_push_subauth(s1, 30));
            assert_equal(std::string("S-1-10-30"),
                         std::string(sid_to_sddl(s1)));

            assert_true(sid_push_subauth(s1, 31));
            assert_equal(std::string("S-1-10-30-31"),
                         std::string(sid_to_sddl(s1)));

            assert_true(sid_pop_subauth(s1));
            assert_equal(std::string("S-1-10-30"),
                         std::string(sid_to_sddl(s1)));

            assert_true(sid_set_id(s1, 9));
            assert_equal(std::string("S-1-9-30"),
                         std::string(sid_to_sddl(s1)));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_parse_sddl1(void)
        {
            assert_equal(0, sid_allocation_count());
            Sid *s1;
            const char *sid_str;

            sid_str = "S-1-10";
            s1 = sid_new_parse_sddl(&sid_str);
            assert_equal('\0', sid_str[0]);

            assert_equal(1, sid_get_rev(s1));
            assert_equal(0, sid_get_subauth_count(s1));
            assert_equal(10, sid_get_id(s1));

            assert_true(sid_free(s1));

            sid_str = "S-1-10-5";
            s1 = sid_new_parse_sddl(&sid_str);
            assert_equal('\0', sid_str[0]);

            assert_equal(1, sid_get_rev(s1));
            assert_equal(1, sid_get_subauth_count(s1));
            assert_equal(10, sid_get_id(s1));
            assert_equal(5, sid_get_subauth(s1, 0));

            assert_true(sid_free(s1));

            sid_str = "S-1-10-5-30";
            s1 = sid_new_parse_sddl(&sid_str);
            assert_equal('\0', sid_str[0]);

            assert_equal(1, sid_get_rev(s1));
            assert_equal(2, sid_get_subauth_count(s1));
            assert_equal(10, sid_get_id(s1));
            assert_equal(5, sid_get_subauth(s1, 0));
            assert_equal(30, sid_get_subauth(s1, 1));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_parse_sddl_quick1(void)
        {
            assert_equal(0, sid_allocation_count());
            Sid *s1;

            s1 = sid_new_parse_sddl_quick("S-1-10");

            assert_equal(1, sid_get_rev(s1));
            assert_equal(0, sid_get_subauth_count(s1));
            assert_equal(10, sid_get_id(s1));

            assert_true(sid_free(s1));

            s1 = sid_new_parse_sddl_quick("S-1-10-5");

            assert_equal(1, sid_get_rev(s1));
            assert_equal(1, sid_get_subauth_count(s1));
            assert_equal(10, sid_get_id(s1));
            assert_equal(5, sid_get_subauth(s1, 0));

            assert_true(sid_free(s1));

            s1 = sid_new_parse_sddl_quick("S-1-10-5-30");

            assert_equal(1, sid_get_rev(s1));
            assert_equal(2, sid_get_subauth_count(s1));
            assert_equal(10, sid_get_id(s1));
            assert_equal(5, sid_get_subauth(s1, 0));
            assert_equal(30, sid_get_subauth(s1, 1));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_parse_sddl_null1(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1;
            const char *sddl_str;

            sddl_str = "";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_null(s1);

            sddl_str = "S";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_null(s1);

            sddl_str = "S-";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_null(s1);

            sddl_str = "S-1";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_null(s1);

            sddl_str = "S-1-";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_null(s1);

            sddl_str = "S-1-foo";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_null(s1);

            sddl_str = "s";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_null(s1);

            sddl_str = "A-10-1";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_null(s1);

            return true;
        }

        struct sid_data
        {
            const char *code;
            const char *sid_str;
            std::vector<rid_type> ids;
        };

        bool test_aliases(void)
        {
            assert_equal(0, sid_allocation_count());

            struct sid_data aliases[] = {
                {
                    "AO",
                    SID_BUILTIN_ACCOUNT_OPERATORS,
                    list_of(5)(32)(548)
                },
                {
                    "BA",
                    SID_BUILTIN_ADMINISTRATORS,
                    list_of(5)(32)(544)
                },
                {
                    "RU",
                    SID_BUILTIN_PREW2K,
                    list_of(5)(32)(554)
                },
                {
                    "PO",
                    SID_BUILTIN_PRINT_OPERATORS,
                    list_of(5)(32)(550)
                },
                {
                    "RS",
                    SID_BUILTIN_RAS_SERVERS,
                    list_of(5)(32)(553)
                },
                {
                    "AU",
                    SID_NT_AUTHENTICATED_USERS,
                    list_of(5)(11)
                },
                {
                    "SY",
                    SID_NT_SYSTEM,
                    list_of(5)(18)
                },
                {
                    "PS",
                    SID_NT_SELF,
                    list_of(5)(10)
                },
                {
                    "WD",
                    SID_WORLD,
                    list_of(1)(0)
                },
                {
                    "ED",
                    SID_NT_ENTERPRISE_DCS,
                    list_of(5)(9)
                },
                {
                    "CO",
                    SID_CREATOR_OWNER,
                    list_of(3)(0)
                },
                {
                    "CG",
                    SID_CREATOR_GROUP,
                    list_of(3)(1)
                },
            };

            for (int i = 0; i < G_N_ELEMENTS(aliases); i++)
            {
                const char *alias_str = aliases[i].code;
                Sid *sa = sid_new_parse_sddl(&alias_str);
                assert_not_null(sa);
                assert_equal('\0', alias_str[0]);
                const char *sddl_str = aliases[i].sid_str;
                Sid *ss = sid_new_parse_sddl(&sddl_str);
                assert_equal('\0', sddl_str[0]);
                assert_not_null(ss);
                Sid *sm = sid_new_id(aliases[i].ids[0]);
                assert_not_null(sm);

                int num_sauths = aliases[i].ids.size();
                for (int j = 1; j < num_sauths; j++)
                {
                    assert_true(sid_push_subauth(sm, aliases[i].ids[j]));
                }

                assert_equal(num_sauths - 1, sid_get_subauth_count(sa));
                assert_equal(num_sauths - 1, sid_get_subauth_count(ss));
                assert_equal(num_sauths - 1, sid_get_subauth_count(sm));

                assert_equal(aliases[i].ids[0], sid_get_id(sa));
                assert_equal(aliases[i].ids[0], sid_get_id(ss));
                assert_equal(aliases[i].ids[0], sid_get_id(sm));

                for (int j = 1; j < num_sauths; j++)
                {
                    assert_equal(aliases[i].ids[j],
                                 sid_get_subauth(sa, j - 1));
                    assert_equal(aliases[i].ids[j],
                                 sid_get_subauth(ss, j - 1));
                    assert_equal(aliases[i].ids[j],
                                 sid_get_subauth(sm, j - 1));
                }

                assert_equal(0, sid_compare(sa, ss));
                assert_equal(0, sid_compare(sa, sm));
                assert_equal(0, sid_compare(ss, sm));

                assert_equal(std::string(aliases[i].code),
                             std::string(sid_to_sddl(sa)));
                assert_equal(std::string(aliases[i].code),
                             std::string(sid_to_sddl(ss)));
                assert_equal(std::string(aliases[i].code),
                             std::string(sid_to_sddl(sm)));

                assert_true(sid_free(sa));
                assert_true(sid_free(ss));
                assert_true(sid_free(sm));
            }

            return true;
        }

        bool test_to_binary1(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1;
            GByteArray *arr1;
            int i;
            const char *sddl_str;

            sddl_str = "S-1-10";
            s1 = sid_new_parse_sddl(&sddl_str);
            arr1 = sid_to_binary(s1);
            assert_not_null(arr1);
            int expected1_size = 8;
            guint8 expected1[] = { 1, 0, 0, 0, 10, 0, 0, 0 };
            assert_equal(expected1_size, arr1->len);
            for (i = 0; i < expected1_size; i++)
            {
                assert_equal(expected1[i], arr1->data[i]);
            }

            assert_true(sid_free(s1));

            sddl_str = "S-1-10-5";
            s1 = sid_new_parse_sddl(&sddl_str);
            arr1 = sid_to_binary(s1);
            assert_not_null(arr1);
            int expected2_size = 12;
            guint8 expected2[] = { 1, 1, 0, 0, 10, 0, 0, 0, 5, 0, 0, 0 };
            assert_equal(expected2_size, arr1->len);
            for (i = 0; i < expected2_size; i++)
            {
                assert_equal(expected2[i], arr1->data[i]);
            }

            assert_true(sid_free(s1));

            sddl_str = "S-1-10-5-30";
            s1 = sid_new_parse_sddl(&sddl_str);
            arr1 = sid_to_binary(s1);
            assert_not_null(arr1);
            int expected3_size = 16;
            guint8 expected3[] = {
                1, 2, 0, 0, 10, 0, 0, 0, 5, 0, 0, 0, 30, 0, 0, 0 };
            assert_equal(expected3_size, arr1->len);
            for (i = 0; i < expected3_size; i++)
            {
                assert_equal(expected3[i], arr1->data[i]);
            }

            assert_true(sid_free(s1));

            return true;
        }

        bool test_write_binary1(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1;
            GByteArray *arr1;
            int i;
            const char *sddl_str;

            sddl_str = "S-1-10";
            s1 = sid_new_parse_sddl(&sddl_str);
            arr1 = g_byte_array_new();
            assert_true(sid_write_to_binary(s1, arr1));

            int expected1_size = 8;
            guint8 expected1[] = { 1, 0, 0, 0, 10, 0, 0, 0 };
            assert_equal(expected1_size, arr1->len);
            for (i = 0; i < expected1_size; i++)
            {
                assert_equal(expected1[i], arr1->data[i]);
            }

            assert_true(sid_free(s1));
            g_byte_array_free(arr1, TRUE);
            arr1 = g_byte_array_new();

            sddl_str = "S-1-10-5";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_true(sid_write_to_binary(s1, arr1));
            int expected2_size = 12;
            guint8 expected2[] = { 1, 1, 0, 0, 10, 0, 0, 0, 5, 0, 0, 0 };
            assert_equal(expected2_size, arr1->len);
            for (i = 0; i < expected2_size; i++)
            {
                assert_equal(expected2[i], arr1->data[i]);
            }

            assert_true(sid_free(s1));
            g_byte_array_free(arr1, TRUE);
            arr1 = g_byte_array_new();

            sddl_str = "S-1-10-5-30";
            s1 = sid_new_parse_sddl(&sddl_str);
            assert_true(sid_write_to_binary(s1, arr1));
            int expected3_size = 16;
            guint8 expected3[] = {
                1, 2, 0, 0, 10, 0, 0, 0, 5, 0, 0, 0, 30, 0, 0, 0 };
            assert_equal(expected3_size, arr1->len);
            for (i = 0; i < expected3_size; i++)
            {
                assert_equal(expected3[i], arr1->data[i]);
            }

            assert_true(sid_free(s1));
            g_byte_array_free(arr1, TRUE);

            return true;
        }

        struct bin_data
        {
            gssize size;
            gssize size_remaining;
            const char *sddl;
            guint8 data[32];
        };

        bool test_from_binary1(void)
        {
            assert_equal(0, sid_allocation_count());

            struct bin_data data[] = {
                {
                    8,
                    0,
                    "S-1-10",
                    { 1, 0, 0, 0, 10, 0, 0, 0 },
                },
                {
                    12,
                    0,
                    "S-1-10-5",
                    { 1, 1, 0, 0, 10, 0, 0, 0, 5, 0, 0, 0 },
                },
                {
                    16,
                    0,
                    "S-1-10-5-30",
                    { 1, 2, 0, 0, 10, 0, 0, 0, 5, 0, 0, 0, 30, 0, 0, 0 },
                },
                {
                    16,
                    4,
                    "S-1-10-5",
                    { 1, 1, 0, 0, 10, 0, 0, 0, 5, 0, 0, 0, 30, 0, 0, 0 },
                },
            };

            for (int i = 0; i < G_N_ELEMENTS(data); i++)
            {
                gssize expected_size = data[i].size;
                Sid *s1 = sid_new_parse_binary(data[i].data, &expected_size);
                assert_not_null(s1);
                assert_equal(data[i].size_remaining, expected_size);
                assert_equal(std::string(data[i].sddl),
                             std::string(sid_to_sddl(s1)));

                assert_true(sid_free(s1));
            }

            return true;
        }

        bool test_from_binary_null1(void)
        {
            assert_equal(0, sid_allocation_count());

            Sid *s1;

            gssize expected1_size = 6;
            guint8 expected1[] = { 1, 0, 0, 0, 10, 0, 0, 0 };
            s1 = sid_new_parse_binary(expected1, &expected1_size);
            assert_null(s1);

            gssize expected2_size = 8;
            guint8 expected2[] = { 1, 1, 0, 0, 10, 0, 0, 0, 5, 0, 0, 0 };
            s1 = sid_new_parse_binary(expected2, &expected2_size);
            assert_null(s1);

            gssize expected3_size = 12;
            guint8 expected3[] = {
                1, 2, 0, 0, 10, 0, 0, 0, 5, 0, 0, 0, 30, 0, 0, 0 };
            s1 = sid_new_parse_binary(expected3, &expected3_size);
            assert_null(s1);

            return true;
        }

    public:

        test_sid(): Melunit::Test("test_sid")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_sid:: name)

            REGISTER(test_new);
            REGISTER(test_set_id);
            REGISTER(test_new_id);
            REGISTER(test_push_subauth);
            REGISTER(test_push_pop_subauth);
            REGISTER(test_copy);
            REGISTER(test_push_all_subauth);
            REGISTER(test_copy2);
            REGISTER(test_compare1);
            REGISTER(test_compare2);
            REGISTER(test_compare3);
            REGISTER(test_to_sddl1);
            REGISTER(test_parse_sddl1);
            REGISTER(test_parse_sddl_quick1);
            REGISTER(test_parse_sddl_null1);
            REGISTER(test_aliases);
            REGISTER(test_to_binary1);
            REGISTER(test_write_binary1);
            REGISTER(test_from_binary1);
            REGISTER(test_from_binary_null1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_sid t1_;
}
