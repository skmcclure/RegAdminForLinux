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

#include <rregadmin/secdesc/aclist.h>
#include <rregadmin/secdesc/acentry.h>
#include <rregadmin/secdesc/sid.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_aclist : public Melunit::Test
    {
    private:

        bool test_new(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());

            ACList *a1 = aclist_new();
            assert_not_null(a1);
            assert_equal(1, aclist_allocation_count());

            assert_equal(ACLIST_DEFAULT_REV, aclist_get_rev(a1));
            assert_equal(0, aclist_get_acentry_count(a1));

            assert_true(aclist_free(a1));

            assert_equal(ACLIST_ERROR_REV, aclist_get_rev(NULL));
            assert_equal(ACLIST_ERROR_ACENTRY_COUNT,
                         aclist_get_acentry_count(NULL));

            assert_false(aclist_free(NULL));

            return true;
        }

        bool test_rev(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());

            ACList *a1 = aclist_new();
            assert_not_null(a1);
            assert_equal(1, aclist_allocation_count());

            assert_equal(ACLIST_DEFAULT_REV, aclist_get_rev(a1));

            assert_true(aclist_set_rev(a1, 10));
            assert_equal(10, aclist_get_rev(a1));

            assert_true(aclist_free(a1));

            return true;
        }

        bool test_entries(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());

            ACList *a1 = aclist_new();
            assert_not_null(a1);
            assert_equal(1, aclist_allocation_count());

            assert_equal(0, aclist_get_acentry_count(a1));

            const char *sddl_str;

            sddl_str = "A;;FA;;;S-1-0-5-10";
            assert_true(aclist_push_acentry(
                            a1, acentry_new_parse_sddl(&sddl_str)));
            assert_equal(1, aclist_get_acentry_count(a1));
            assert_equal(1, sid_allocation_count());
            assert_equal(1, acentry_allocation_count());
            assert_equal(1, aclist_allocation_count());

            sddl_str = "A;;FA;;;S-1-0-5-10";
            ACEntry *e1 = acentry_new_parse_sddl(&sddl_str);

            assert_null(aclist_get_acentry(a1, 1));

            assert_not_null(aclist_get_acentry(a1, 0));
            assert_equal(0, acentry_compare(e1, aclist_get_acentry(a1, 0)));

            assert_true(aclist_pop_acentry(a1));
            assert_null(aclist_get_acentry(a1, 1));
            assert_null(aclist_get_acentry(a1, 0));
            assert_equal(0, aclist_get_acentry_count(a1));

            assert_false(aclist_pop_acentry(a1));

            assert_true(aclist_free(a1));
            assert_true(acentry_free(e1));

            assert_equal(0, aclist_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, sid_allocation_count());

            return true;
        }

        bool test_copy(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            const char *sddl_str;

            ACList *a1 = aclist_new();
            sddl_str = "A;;FA;;;S-1-0-5-10";
            assert_true(aclist_push_acentry(
                            a1, acentry_new_parse_sddl(&sddl_str)));

            assert_equal(1, sid_allocation_count());
            assert_equal(1, acentry_allocation_count());
            assert_equal(1, aclist_allocation_count());

            ACList *a2 = aclist_copy(a1);
            assert_not_null(a2);

            assert_equal(2, sid_allocation_count());
            assert_equal(2, acentry_allocation_count());
            assert_equal(2, aclist_allocation_count());

            assert_true(aclist_free(a1));
            assert_true(aclist_free(a2));

            assert_equal(0, aclist_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, sid_allocation_count());

            return true;
        }

        bool test_compare(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            const char *sddl_str;

            ACList *a1 = aclist_new();
            sddl_str = "A;;FA;;;S-1-0-5-10";
            assert_true(aclist_push_acentry(
                            a1, acentry_new_parse_sddl(&sddl_str)));
            ACList *a2 = aclist_new();
            sddl_str = "A;;FA;;;S-1-0-5-10";
            assert_true(aclist_push_acentry(
                            a2, acentry_new_parse_sddl(&sddl_str)));
            ACList *a3 = aclist_new();
            sddl_str = "A;;FA;;;S-1-0-5-9";
            assert_true(aclist_push_acentry(
                            a3, acentry_new_parse_sddl(&sddl_str)));
            ACList *a_null = NULL;

            assert_equal(0, aclist_compare(a1, a2));
            assert_equal(0, aclist_compare(a_null, a_null));
            assert_compare(std::less, 0, aclist_compare(a1, a3));
            assert_compare(std::less, 0, aclist_compare(a1, a_null));
            assert_compare(std::greater, 0, aclist_compare(a3, a1));
            assert_compare(std::greater, 0, aclist_compare(a_null, a1));

            assert_true(aclist_free(a1));
            assert_true(aclist_free(a2));
            assert_true(aclist_free(a3));

            return true;
        }

        bool test_parse_sddl(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());

            ACList *a1;
            const char *tmp_str;

            const char *sddl_str1 = "(A;;FA;;;S-1-0-5-9)";
            tmp_str = sddl_str1;
            a1 = aclist_new_parse_sddl(&tmp_str);
            assert_not_null(a1);

            assert_equal(ACLIST_DEFAULT_REV, aclist_get_rev(a1));
            assert_equal(1, aclist_get_acentry_count(a1));

            assert_equal(std::string(sddl_str1),
                         std::string(aclist_to_sddl(a1)));

            assert_true(aclist_free(a1));

            const char *sddl_str2 = "(A;;FA;;;S-1-0-5-9)(A;;FA;;;S-1-0-10-20)";
            tmp_str = sddl_str2;
            a1 = aclist_new_parse_sddl(&tmp_str);
            assert_not_null(a1);

            assert_equal(ACLIST_DEFAULT_REV, aclist_get_rev(a1));
            assert_equal(2, aclist_get_acentry_count(a1));

            assert_equal(std::string(sddl_str2),
                         std::string(aclist_to_sddl(a1)));

            assert_true(aclist_free(a1));

            return true;
        }

        bool test_parse_sddl_quick(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());

            ACList *a1;
            const char *tmp_str;

            const char *sddl_str1 = "(A;;FA;;;S-1-0-5-9)";
            a1 = aclist_new_parse_sddl_quick(sddl_str1);
            assert_not_null(a1);

            assert_equal(ACLIST_DEFAULT_REV, aclist_get_rev(a1));
            assert_equal(1, aclist_get_acentry_count(a1));

            assert_equal(std::string(sddl_str1),
                         std::string(aclist_to_sddl(a1)));

            assert_true(aclist_free(a1));

            const char *sddl_str2 = "(A;;FA;;;S-1-0-5-9)(A;;FA;;;S-1-0-10-20)";
            a1 = aclist_new_parse_sddl_quick(sddl_str2);
            assert_not_null(a1);

            assert_equal(ACLIST_DEFAULT_REV, aclist_get_rev(a1));
            assert_equal(2, aclist_get_acentry_count(a1));

            assert_equal(std::string(sddl_str2),
                         std::string(aclist_to_sddl(a1)));

            assert_true(aclist_free(a1));

            return true;
        }

        void assert_bin_data_equal(GByteArray *arr,
                                   const guint8 *data,
                                   guint32 data_len)
        {
            assert_not_null(arr);

            assert_equal(arr->len, data_len);

            for (int i = 0; i < data_len; i++)
            {
                // fprintf (stderr, "Comparing byte %d: %#x, %#x\n",
                //          i, arr->data[i], data[i]);
                assert_equal(arr->data[i], data[i]);
            }

            g_byte_array_free(arr, TRUE);
        }

        struct bin_data
        {
            aclist_rev rev;
            aclist_acentry_count count;
            const char *sddl;

            gssize size;
            gssize size_remaining;
            gssize block_size;
            guint8 data[1024];
        };

        bool test_parse_bin(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());

            struct bin_data data[] = {
                {
                    2,
                    2,
                    "(A;;FA;;;S-1-83886080-32-544)"
                    "(A;;FA;;;S-1-83886080-21-507921405-651377827-725345543-1004)",
                    256,
                    0,
                    256,
                    {
                        0x02, 0x00,  // revision
                        0x00, 0x01,  // size
                        0x02, 0x00, 0x00, 0x00,  //num aces

                        // Ace
                        0x00,  // type
                        0x00,  // flags
                        0x18, 0x00,  // size
                        0xff, 0x01, 0x1f, 0x00,  // access mask 0x001f01ff
                        // Sid
                        0x01,
                        0x02,
                        0x00, 0x00,
                        0x00, 0x00, 0x00, 0x05,
                        0x20, 0x00, 0x00, 0x00,
                        0x20, 0x02, 0x00, 0x00,
                        // Sid end
                        // Ace end

                        // Ace
                        0x00,
                        0x00,
                        0x24, 0x00,
                        0xff, 0x01, 0x1f, 0x00,
                        // Sid
                        0x01,
                        0x05,
                        0x00, 0x00,
                        0x00, 0x00, 0x00, 0x05,
                        0x15, 0x00, 0x00, 0x00,
                        0xfd, 0x43, 0x46, 0x1e,
                        0xa3, 0x3c, 0xd3, 0x26,
                        0x07, 0xe5, 0x3b, 0x2b,
                        0xec, 0x03, 0x00, 0x00,
                        // Sid end
                        // Ace end

                        0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,


                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    }
                },
            };

            for (int i = 0; i < G_N_ELEMENTS(data); i++)
            {
                gssize sz = data[i].size;
                ACList *a1 = aclist_new_parse_binary(data[i].data, &sz);
                assert_not_null(a1);

                assert_equal(data[i].size_remaining, sz);

                assert_equal(data[i].rev, aclist_get_rev(a1));
                assert_equal(data[i].count, aclist_get_acentry_count(a1));

                assert_equal(std::string(data[i].sddl),
                             std::string(aclist_to_sddl(a1)));

                assert_bin_data_equal(
                    aclist_to_binary(a1, data[i].block_size),
                    data[i].data, data[i].size - data[i].size_remaining);
                assert_true(aclist_free(a1));
            }

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());

            return true;
        }

    public:

        test_aclist(): Melunit::Test("test_aclist")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_aclist:: name)

            REGISTER(test_new);
            REGISTER(test_rev);
            REGISTER(test_entries);
            REGISTER(test_copy);
            REGISTER(test_compare);
            REGISTER(test_parse_sddl);
            REGISTER(test_parse_sddl_quick);
            REGISTER(test_parse_bin);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_aclist t1_;
}
