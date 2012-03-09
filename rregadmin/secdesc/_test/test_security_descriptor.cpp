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

#include <boost/format.hpp>

#include <rregadmin/secdesc/security_descriptor.h>
#include <rregadmin/secdesc/sid.h>
#include <rregadmin/secdesc/aclist.h>
#include <rregadmin/secdesc/acentry.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_security_descriptor : public Melunit::Test
    {
    private:

        bool test_null(void)
        {
            assert_equal(SECDESC_ERROR_REV, secdesc_get_rev(NULL));
            assert_equal(SECDESC_ERROR_TYPE, secdesc_get_type(NULL));
            assert_null(secdesc_get_owner(NULL));
            assert_null(secdesc_get_group(NULL));
            assert_null(secdesc_get_sacl(NULL));
            assert_null(secdesc_get_dacl(NULL));

            assert_false(secdesc_set_rev(NULL, 0));
            assert_false(secdesc_set_type(NULL, 0));

            Sid *s1 = sid_new_id(10);
            assert_false(secdesc_set_owner(NULL, s1));
            assert_false(secdesc_set_group(NULL, s1));

            assert_true(sid_free(s1));

            return true;
        }

        bool test_new(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            SecurityDescriptor *sd1 = secdesc_new();
            assert_not_null(sd1);

            assert_equal(SECDESC_DEFAULT_REV, secdesc_get_rev(sd1));
            assert_equal(SECDESC_DEFAULT_TYPE, secdesc_get_type(sd1));
            assert_null(secdesc_get_owner(sd1));
            assert_null(secdesc_get_group(sd1));
            assert_not_null(secdesc_get_sacl(sd1));
            assert_not_null(secdesc_get_dacl(sd1));

            assert_true(secdesc_free(sd1));

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            return true;
        }

        bool test_rev(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            SecurityDescriptor *sd1 = secdesc_new();
            assert_not_null(sd1);
            assert_equal(SECDESC_DEFAULT_REV, secdesc_get_rev(sd1));

            assert_true(secdesc_set_rev(sd1, 35));
            assert_equal(35, secdesc_get_rev(sd1));

            assert_true(secdesc_free(sd1));

            return true;
        }

        struct type_code_info
        {
            secdesc_type_code code;
            gboolean def;
            gboolean allow_to_set;
        };

        bool test_type(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            SecurityDescriptor *sd1 = secdesc_new();
            assert_not_null(sd1);
            assert_equal(SECDESC_DEFAULT_TYPE, secdesc_get_type(sd1));

            struct type_code_info cinfo[] = {
                { SEC_DESC_OWNER_DEFAULTED, FALSE, TRUE },
                { SEC_DESC_GROUP_DEFAULTED, FALSE, TRUE },

                { SEC_DESC_SERVER_SECURITY, FALSE, TRUE },

                { SEC_DESC_DACL_PRESENT, FALSE, FALSE },
                { SEC_DESC_DACL_TRUSTED, FALSE, TRUE },
                { SEC_DESC_DACL_DEFAULTED, FALSE, TRUE },
                { SEC_DESC_DACL_AUTO_INHERIT_REQ, FALSE, TRUE },
                { SEC_DESC_DACL_AUTO_INHERITED, FALSE, TRUE },
                { SEC_DESC_DACL_PROTECTED, FALSE, TRUE },

                { SEC_DESC_SACL_PRESENT, FALSE, FALSE },
                { SEC_DESC_SACL_DEFAULTED, FALSE, TRUE },
                { SEC_DESC_SACL_AUTO_INHERIT_REQ, FALSE, TRUE },
                { SEC_DESC_SACL_AUTO_INHERITED, FALSE, TRUE },
                { SEC_DESC_SACL_PROTECTED, FALSE, TRUE },

                { SEC_DESC_RM_CONTROL_VALID, FALSE, TRUE },
                { SEC_DESC_SELF_RELATIVE, TRUE, FALSE },
            };

            for (int i = 0; i < G_N_ELEMENTS(cinfo); i++)
            {
                assert_equal(cinfo[i].def,
                             secdesc_has_type_code(sd1, cinfo[i].code));
                if (cinfo[i].allow_to_set)
                {
                    assert_true(secdesc_set_type_code(sd1, cinfo[i].code));
                    assert_true(secdesc_has_type_code(sd1, cinfo[i].code));
                    assert_true(secdesc_unset_type_code(sd1, cinfo[i].code));
                    assert_false(secdesc_unset_type_code(sd1, cinfo[i].code));
                }
                else
                {
                    assert_false(secdesc_set_type_code(sd1, cinfo[i].code));
                    assert_equal(cinfo[i].def,
                                 secdesc_has_type_code(sd1, cinfo[i].code));
                }
            }

            assert_true(secdesc_free(sd1));

            return true;
        }

        bool test_owner(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            SecurityDescriptor *sd1 = secdesc_new();
            assert_not_null(sd1);
            assert_null(secdesc_get_owner(sd1));

            Sid *o1 = sid_new_id(10);

            assert_true(secdesc_set_owner(sd1, o1));

            assert_not_null(secdesc_get_owner(sd1));

            assert_true(secdesc_free(sd1));

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            return true;
        }

        bool test_group(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            SecurityDescriptor *sd1 = secdesc_new();
            assert_not_null(sd1);
            assert_null(secdesc_get_group(sd1));

            Sid *o1 = sid_new_id(10);

            assert_true(secdesc_set_group(sd1, o1));

            assert_not_null(secdesc_get_group(sd1));

            assert_true(secdesc_free(sd1));

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            return true;
        }

        bool test_dacl(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            SecurityDescriptor *sd1 = secdesc_new();
            assert_not_null(sd1);
            assert_false(secdesc_has_type_code(sd1, SEC_DESC_DACL_PRESENT));
            assert_false(secdesc_has_type_code(sd1, SEC_DESC_SACL_PRESENT));

            const char *sddl_str = "A;;KA;;;S-1-0-5-10";
            ACEntry *a1 = acentry_new_parse_sddl_quick(sddl_str);

            assert_true(aclist_push_acentry(secdesc_get_dacl(sd1), a1));
            assert_true(secdesc_has_type_code(sd1, SEC_DESC_DACL_PRESENT));
            assert_false(secdesc_has_type_code(sd1, SEC_DESC_SACL_PRESENT));

            assert_true(secdesc_free(sd1));

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            return true;
        }

        bool test_sacl(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            SecurityDescriptor *sd1 = secdesc_new();
            assert_not_null(sd1);
            assert_false(secdesc_has_type_code(sd1, SEC_DESC_DACL_PRESENT));
            assert_false(secdesc_has_type_code(sd1, SEC_DESC_SACL_PRESENT));

            const char *sddl_str = "A;;KA;;;S-1-0-5-10";
            ACEntry *a1 = acentry_new_parse_sddl_quick(sddl_str);

            assert_true(aclist_push_acentry(secdesc_get_sacl(sd1), a1));
            assert_false(secdesc_has_type_code(sd1, SEC_DESC_DACL_PRESENT));
            assert_true(secdesc_has_type_code(sd1, SEC_DESC_SACL_PRESENT));

            assert_true(secdesc_free(sd1));

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            return true;
        }

        bool test_copy(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            Sid *o1 = sid_new_id(10);
            Sid *g1 = sid_new_id(30);

            SecurityDescriptor *sd1 = secdesc_new_full(o1, g1);
            assert_not_null(sd1);
            assert_not_null(secdesc_get_owner(sd1));
            assert_not_null(secdesc_get_group(sd1));
            assert_true(secdesc_set_type_code(sd1, SEC_DESC_DACL_TRUSTED));
            const char *sddl_str = "A;;KA;;;S-1-0-5-10";
            ACEntry *a1 = acentry_new_parse_sddl_quick(sddl_str);

            assert_true(aclist_push_acentry(secdesc_get_sacl(sd1), a1));
            assert_true(secdesc_has_type_code(sd1, SEC_DESC_SACL_PRESENT));

            SecurityDescriptor *sd2 = secdesc_copy(sd1);
            assert_not_null(sd2);
            assert_not_null(secdesc_get_owner(sd2));
            assert_not_null(secdesc_get_group(sd2));
            assert_true(secdesc_has_type_code(sd2, SEC_DESC_DACL_TRUSTED));
            assert_true(secdesc_has_type_code(sd2, SEC_DESC_SACL_PRESENT));
            assert_equal(1, aclist_get_acentry_count(secdesc_get_sacl(sd2)));

            assert_compare(std::not_equal_to,
                           secdesc_get_owner(sd1),
                           secdesc_get_owner(sd2));
            assert_compare(std::not_equal_to,
                           secdesc_get_group(sd1),
                           secdesc_get_group(sd2));

            assert_equal(0, secdesc_compare(sd1, sd2));
            assert_true(secdesc_equal(sd1, sd2));

            assert_true(secdesc_free(sd1));
            assert_true(secdesc_free(sd2));

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            return true;
        }

        bool test_compare(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            const char *sddl_str = "A;;KA;;;S-1-0-5-10";

            SecurityDescriptor *sd1 = secdesc_new_full(sid_new_id(10),
                                                       sid_new_id(30));
            assert_true(secdesc_set_type_code(sd1, SEC_DESC_DACL_TRUSTED));
            assert_true(aclist_push_acentry(
                            secdesc_get_sacl(sd1),
                            acentry_new_parse_sddl_quick(sddl_str)));

            SecurityDescriptor *sd2 = secdesc_new_full(sid_new_id(10),
                                                       sid_new_id(30));
            assert_true(secdesc_set_type_code(sd2, SEC_DESC_DACL_TRUSTED));
            assert_true(aclist_push_acentry(
                            secdesc_get_sacl(sd2),
                            acentry_new_parse_sddl_quick(sddl_str)));

            SecurityDescriptor *sd3 = secdesc_new_full(sid_new_id(9),
                                                       sid_new_id(30));
            assert_true(secdesc_set_type_code(sd3, SEC_DESC_DACL_TRUSTED));
            assert_true(aclist_push_acentry(
                            secdesc_get_sacl(sd3),
                            acentry_new_parse_sddl_quick(sddl_str)));

            SecurityDescriptor *sd4 = secdesc_new_full(sid_new_id(10),
                                                       sid_new_id(30));
            assert_true(aclist_push_acentry(
                            secdesc_get_sacl(sd4),
                            acentry_new_parse_sddl_quick(sddl_str)));

            SecurityDescriptor *sd5 = secdesc_new_full(sid_new_id(10),
                                                       sid_new_id(30));

            assert_equal(0, secdesc_compare(NULL, NULL));
            assert_equal(0, secdesc_compare(sd1, sd2));
            assert_compare(std::less, 0, secdesc_compare(sd1, NULL));
            assert_compare(std::greater, 0, secdesc_compare(NULL, sd1));
            assert_compare(std::less, 0, secdesc_compare(sd1, sd3));
            assert_compare(std::greater, 0, secdesc_compare(sd3, sd1));
            assert_compare(std::less, 0, secdesc_compare(sd1, sd4));
            assert_compare(std::greater, 0, secdesc_compare(sd4, sd1));
            assert_compare(std::less, 0, secdesc_compare(sd1, sd5));
            assert_compare(std::greater, 0, secdesc_compare(sd5, sd1));

            assert_true(secdesc_free(sd1));
            assert_true(secdesc_free(sd2));
            assert_true(secdesc_free(sd3));
            assert_true(secdesc_free(sd4));
            assert_true(secdesc_free(sd5));

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            return true;
        }

        bool test_sddl(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            const char *data[] = {
                "O:AOG:DAD:(A;;RPWPCCDCLCSWRCWDWOGA;;;S-1-0-0)",
                "O:BAG:DUD:ARAI(A;;FA;;;BA)(A;;FA;;;SY)(A;;0x1200a9;;;BU)(A;ID;FA;;;BA)(A;ID;FA;;;SY)(A;ID;0x1200a9;;;BU)",
                "O:BAG:SYD:(D;;0xf0007;;;AN)(D;;0xf0007;;;BG)(A;;0xf0007;;;SY)(A;;0x7;;;BA)(A;;0x7;;;SO)(A;;0x3;;;IU)(A;;0x3;;;SU)(A;;0x3;;;S-1-5-3)",
                "O:DAG:DAD:(A;;RPWPCCDCLCRCWOWDSDSW;;;SY)(A;;RPWPCCDCLCRCWOWDSDSW;;;DA)(OA;;CCDC;;;AO)(OA;;CCDC;;;AO)(OA;;CCDC;;;AO)(OA;;CCDC;;;PO)(A;;RPLCRC;;;AU)S:(AU;SAFA;WDWOSDWPCCDCSW;;;WD)",
                "O:S-1-83886080-21-507921405-651377827-725345543-1004G:S-1-83886080-21-507921405-651377827-725345543-513D:(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-21-507921405-651377827-725345543-1004)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-18)",
                "O:S-1-83886080-32-544G:S-1-83886080-18D:(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-18)(A;;RCRPCCSW;;;S-1-83886080-32-544)",
                "O:S-1-83886080-32-544G:S-1-83886080-18D:PAI(A;;RCRPCCSW;;;S-1-83886080-32-545)(A;CIIO;GR;;;S-1-83886080-32-545)(A;;RCRPCCSW;;;S-1-83886080-32-547)(A;CIIO;GR;;;S-1-83886080-32-547)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-32-544)(A;CIIO;GA;;;S-1-83886080-32-544)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-18)(A;CIIO;GA;;;S-1-83886080-18)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-32-544)(A;CIIO;GA;;;S-1-50331648-0)",
                "O:S-1-83886080-32-544G:S-1-83886080-18D:PAI(A;;RCRPCCSW;;;S-1-83886080-32-545)(A;CIIO;GR;;;S-1-83886080-32-545)(A;;RCRPCCSW;;;S-1-83886080-32-547)(A;CIIO;GR;;;S-1-83886080-32-547)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-32-544)(A;CIIO;GA;;;S-1-83886080-32-544)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-18)(A;CIIO;GA;;;S-1-83886080-18)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-32-544)(A;CIIO;GA;;;S-1-50331648-0)",
                "O:S-1-83886080-32-544G:S-1-83886080-18D:PAI(A;;RCRPCCSW;;;S-1-83886080-32-545)(A;CIIO;GR;;;S-1-83886080-32-545)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-32-544)(A;CIIO;GA;;;S-1-83886080-32-544)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-18)(A;CIIO;GA;;;S-1-83886080-18)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-32-544)(A;CIIO;GA;;;S-1-50331648-0)",
                "O:S-1-83886080-32-544G:S-1-83886080-18D:(A;CI;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-21-2511902016-4135317156-846039753-1110)(A;CI;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-21-2511902016-4135317156-846039753-500)(A;CI;RCRPCCSW;;;S-1-83886080-21-2511902016-4135317156-846039753-1111)(A;CI;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-21-2511902016-4135317156-846039753-519)(A;CI;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-21-2511902016-4135317156-846039753-512)(A;;RCRPCCSW;;;S-1-83886080-32-545)(A;CIIO;GR;;;S-1-83886080-32-545)(A;;RCRPCCSW;;;S-1-83886080-32-547)(A;CIIO;GR;;;S-1-83886080-32-547)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-32-544)(A;CIIO;GA;;;S-1-83886080-32-544)(A;;RCSDWDWORPWPCCDCLCSW;;;S-1-83886080-18)(A;CIIO;GA;;;S-1-83886080-18)(A;CIIO;GA;;;S-1-50331648-0)",
            };

            for (int i = 0; i < G_N_ELEMENTS(data); i++)
            {
                SecurityDescriptor *sd1 =
                    secdesc_new_parse_sddl_quick(data[i]);
                SecurityDescriptor *sd2 =
                    secdesc_new_parse_sddl_quick(data[i]);
                SecurityDescriptor *sd3 = secdesc_copy(sd1);
                SecurityDescriptor *sd4 =
                    secdesc_new_parse_sddl_quick(secdesc_to_sddl(sd1));


                assert_true_msg(secdesc_equal(sd1, sd1),
                                (boost::format("Identical: %s vs %s")
                                 % secdesc_to_sddl(sd1)
                                 % secdesc_to_sddl(sd1)).str());
                assert_true_msg(secdesc_equal(sd1, sd2),
                                (boost::format("Same parse: %s vs %s")
                                 % secdesc_to_sddl(sd1)
                                 % secdesc_to_sddl(sd2)).str());
                assert_true_msg(secdesc_equal(sd1, sd3),
                                (boost::format("Copy: %s vs %s")
                                 % secdesc_to_sddl(sd1)
                                 % secdesc_to_sddl(sd3)).str());
                assert_true_msg(secdesc_equal(sd1, sd4),
                                (boost::format("Parse from out: %s vs %s")
                                 % secdesc_to_sddl(sd1)
                                 % secdesc_to_sddl(sd4)).str());

                assert_true(secdesc_free(sd1));
                assert_true(secdesc_free(sd2));
                assert_true(secdesc_free(sd3));
                assert_true(secdesc_free(sd4));
            }

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

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
                assert_equal_msg((int)arr->data[i], (int)data[i],
                                 (boost::format("Differ at byte %d")
                                  % i).str());
            }

            g_byte_array_free(arr, TRUE);
        }

        struct bin_data
        {
            bool has_owner;
            bool has_group;
            bool has_dacl;
            bool has_sacl;

            int dacl_count;
            int sacl_count;

            bool successful;
            int block_size;

            int size;
            int size_remaining;

            guint8 data[2048];
        };


        bool test_parse_binary(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            struct bin_data data[] = {
                {
                    true, true, true, false,
                    2, 0,
                    true, 256,
                    332, 0,
                    { 0x01,0x00,0x04,0x80,0x14,0x01,0x00,0x00,
                      0x30,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x14,0x00,0x00,0x00,0x02,0x00,0x00,0x01,
                      0x02,0x00,0x00,0x00,0x00,0x00,0x18,0x00,

                      0xff,0x01,0x1f,0x00,0x01,0x02,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x20,0x02,0x00,0x00,0x00,0x00,0x24,0x00,
                      0xff,0x01,0x1f,0x00,0x01,0x05,0x00,0x00,

                      0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,
                      0xfd,0x43,0x46,0x1e,0xa3,0x3c,0xd3,0x26,
                      0x07,0xe5,0x3b,0x2b,0xec,0x03,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x00,0x00,0x00,0x00,0x01,0x05,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,

                      0xfd,0x43,0x46,0x1e,0xa3,0x3c,0xd3,0x26,
                      0x07,0xe5,0x3b,0x2b,0xec,0x03,0x00,0x00,
                      0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x15,0x00,0x00,0x00,0xfd,0x43,0x46,0x1e,

                      0xa3,0x3c,0xd3,0x26,0x07,0xe5,0x3b,0x2b,
                      0x01,0x02,0x00,0x00 }
                },
                {
                    true, true, true, false,
                    4, 0,
                    true, 0,
                    188, 0,
                    { 0x01,0x00,0x04,0x84,0x84,0x00,0x00,0x00,
                      0xa0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x14,0x00,0x00,0x00,0x02,0x00,0x70,0x00,
                      0x04,0x00,0x00,0x00,0x00,0x10,0x18,0x00,

                      0xa9,0x00,0x12,0x00,0x01,0x02,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x21,0x02,0x00,0x00,0x00,0x10,0x18,0x00,
                      0xff,0x01,0x1f,0x00,0x01,0x02,0x00,0x00,

                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x20,0x02,0x00,0x00,0x00,0x10,0x14,0x00,
                      0xff,0x01,0x1f,0x00,0x01,0x01,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x12,0x00,0x00,0x00,

                      0x00,0x10,0x24,0x00,0xff,0x01,0x1f,0x00,
                      0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x15,0x00,0x00,0x00,0xfd,0x43,0x46,0x1e,
                      0xa3,0x3c,0xd3,0x26,0x07,0xe5,0x3b,0x2b,

                      0xec,0x03,0x00,0x00,0x01,0x05,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,
                      0xfd,0x43,0x46,0x1e,0xa3,0x3c,0xd3,0x26,
                      0x07,0xe5,0x3b,0x2b,0xec,0x03,0x00,0x00,

                      0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x15,0x00,0x00,0x00,0xfd,0x43,0x46,0x1e,
                      0xa3,0x3c,0xd3,0x26,0x07,0xe5,0x3b,0x2b,
                      0x01,0x02,0x00,0x00 }
                },

                {
                    true, true, true, false,
                    1, 0,
                    true, 0,
                    80, 0,
                    { 0x01,0x00,0x14,0xBC,0x34,0x00,0x00,0x00,
                      0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x14,0x00,0x00,0x00,0x02,0x00,0x20,0x00,
                      0x01,0x00,0x00,0x00,0x00,0x00,0x18,0x00,

                      0x3F,0x00,0x0F,0x00,0x01,0x02,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x20,0x02,0x00,0x00,0x01,0x02,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,

                      0x20,0x02,0x00,0x00,0x01,0x01,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x12,0x00,0x00,0x00, }
                },

                {
                    true, true, true, false,
                    14, 0,
                    true, 0,
                    440, 0,
                    { 0x01,0x00,0x04,0x80,0x9C,0x01,0x00,0x00,
                      0XAC,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x14,0x00,0x00,0x00,0x04,0x00,0x88,0x01,
                      0x0E,0x00,0x00,0x00,0x00,0x02,0x24,0x00,

                      0x3F,0x00,0x0F,0x00,0x01,0x05,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,
                      0x40,0x95,0xB8,0x95,0xA4,0xEE,0x7B,0xF6,
                      0XC9,0x8A,0x6D,0x32,0x56,0x04,0x00,0x00,

                      0x00,0x02,0x24,0x00,0x3F,0x00,0x0F,0x00,
                      0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x15,0x00,0x00,0x00,0x40,0x95,0xB8,0x95,
                      0XA4,0xEE,0x7B,0xF6,0xC9,0x8A,0x6D,0x32,

                      0XF4,0x01,0x00,0x00,0x00,0x02,0x24,0x00,
                      0x19,0x00,0x02,0x00,0x01,0x05,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,
                      0x40,0x95,0xB8,0x95,0xA4,0xEE,0x7B,0xF6,

                      0XC9,0x8A,0x6D,0x32,0x57,0x04,0x00,0x00,
                      0x00,0x02,0x24,0x00,0x3F,0x00,0x0F,0x00,
                      0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x15,0x00,0x00,0x00,0x40,0x95,0xB8,0x95,

                      0XA4,0xEE,0x7B,0xF6,0xC9,0x8A,0x6D,0x32,
                      0x07,0x02,0x00,0x00,0x00,0x02,0x24,0x00,
                      0x3F,0x00,0x0F,0x00,0x01,0x05,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,

                      0x40,0x95,0xB8,0x95,0xA4,0xEE,0x7B,0xF6,
                      0XC9,0x8A,0x6D,0x32,0x00,0x02,0x00,0x00,
                      0x00,0x00,0x18,0x00,0x19,0x00,0x02,0x00,
                      0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,

                      0x20,0x00,0x00,0x00,0x21,0x02,0x00,0x00,
                      0x00,0x0A,0x18,0x00,0x00,0x00,0x00,0x80,
                      0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x20,0x00,0x00,0x00,0x21,0x02,0x00,0x00,

                      0x00,0x00,0x18,0x00,0x19,0x00,0x02,0x00,
                      0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x20,0x00,0x00,0x00,0x23,0x02,0x00,0x00,
                      0x00,0x0A,0x18,0x00,0x00,0x00,0x00,0x80,

                      0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x20,0x00,0x00,0x00,0x23,0x02,0x00,0x00,
                      0x00,0x00,0x18,0x00,0x3F,0x00,0x0F,0x00,
                      0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,

                      0x20,0x00,0x00,0x00,0x20,0x02,0x00,0x00,
                      0x00,0x0A,0x18,0x00,0x00,0x00,0x00,0x10,
                      0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x20,0x00,0x00,0x00,0x20,0x02,0x00,0x00,

                      0x00,0x00,0x14,0x00,0x3F,0x00,0x0F,0x00,
                      0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x12,0x00,0x00,0x00,0x00,0x0A,0x14,0x00,
                      0x00,0x00,0x00,0x10,0x01,0x01,0x00,0x00,

                      0x00,0x00,0x00,0x05,0x12,0x00,0x00,0x00,
                      0x00,0x0A,0x14,0x00,0x00,0x00,0x00,0x10,
                      0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x03,
                      0x00,0x00,0x00,0x00,0x01,0x02,0x00,0x00,

                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x20,0x02,0x00,0x00,0x01,0x01,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x12,0x00,0x00,0x00, }
                },

                {
                    true, true, true, false,
                    17, 0,
                    true, 0,
                    504, 0,
                    { 0x01,0x00,0x04,0x80,0xDC,0x01,0x00,0x00,
                      0XEC,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
                      0x14,0x00,0x00,0x00,0x04,0x00,0xC8,0x01,
                      0x11,0x00,0x00,0x00,0x00,0x02,0x24,0x00,

                      0x3F,0x00,0x0F,0x00,0x01,0x05,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,
                      0x40,0x95,0xB8,0x95,0xA4,0xEE,0x7B,0xF6,
                      0XC9,0x8A,0x6D,0x32,0x56,0x04,0x00,0x00,

                      0x00,0x02,0x24,0x00,0x3F,0x00,0x0F,0x00,
                      0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x15,0x00,0x00,0x00,0x40,0x95,0xB8,0x95,
                      0XA4,0xEE,0x7B,0xF6,0xC9,0x8A,0x6D,0x32,

                      0XF4,0x01,0x00,0x00,0x00,0x02,0x24,0x00,
                      0x19,0x00,0x02,0x00,0x01,0x05,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,
                      0x40,0x95,0xB8,0x95,0xA4,0xEE,0x7B,0xF6,

                      0XC9,0x8A,0x6D,0x32,0x57,0x04,0x00,0x00,
                      0x00,0x02,0x24,0x00,0x3F,0x00,0x0F,0x00,
                      0x01,0x05,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x15,0x00,0x00,0x00,0x40,0x95,0xB8,0x95,

                      0XA4,0xEE,0x7B,0xF6,0xC9,0x8A,0x6D,0x32,
                      0x07,0x02,0x00,0x00,0x00,0x02,0x24,0x00,
                      0x3F,0x00,0x0F,0x00,0x01,0x05,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x15,0x00,0x00,0x00,

                      0x40,0x95,0xB8,0x95,0xA4,0xEE,0x7B,0xF6,
                      0XC9,0x8A,0x6D,0x32,0x00,0x02,0x00,0x00,
                      0x00,0x00,0x14,0x00,0x1F,0x00,0x03,0x00,
                      0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x05,

                      0x0D,0x00,0x00,0x00,0x00,0x1A,0x14,0x00,
                      0x00,0x00,0x01,0xC0,0x01,0x01,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x0D,0x00,0x00,0x00,
                      0x00,0x00,0x18,0x00,0x3F,0x00,0x0F,0x00,

                      0x01,0x02,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x20,0x00,0x00,0x00,0x20,0x02,0x00,0x00,
                      0x00,0x1A,0x14,0x00,0x00,0x00,0x00,0x10,
                      0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x03,

                      0x00,0x00,0x00,0x00,0x00,0x00,0x14,0x00,
                      0x3F,0x00,0x0F,0x00,0x01,0x01,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x12,0x00,0x00,0x00,
                      0x00,0x1A,0x14,0x00,0x00,0x00,0x00,0x10,

                      0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x05,
                      0x12,0x00,0x00,0x00,0x00,0x00,0x18,0x00,
                      0x3F,0x00,0x0F,0x00,0x01,0x02,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,

                      0x20,0x02,0x00,0x00,0x00,0x1A,0x18,0x00,
                      0x00,0x00,0x00,0x10,0x01,0x02,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x20,0x02,0x00,0x00,0x00,0x00,0x18,0x00,

                      0x1F,0x00,0x03,0x00,0x01,0x02,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x23,0x02,0x00,0x00,0x00,0x1A,0x18,0x00,
                      0x00,0x00,0x01,0xC0,0x01,0x02,0x00,0x00,

                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x23,0x02,0x00,0x00,0x00,0x00,0x18,0x00,
                      0x19,0x00,0x02,0x00,0x01,0x02,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,

                      0x21,0x02,0x00,0x00,0x00,0x1A,0x18,0x00,
                      0x00,0x00,0x00,0x80,0x01,0x02,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x21,0x02,0x00,0x00,0x01,0x02,0x00,0x00,

                      0x00,0x00,0x00,0x05,0x20,0x00,0x00,0x00,
                      0x20,0x02,0x00,0x00,0x01,0x01,0x00,0x00,
                      0x00,0x00,0x00,0x05,0x12,0x00,0x00,0x00, }
                },

                {
                    true, true, true, false,
                    0, 0,
                    false, 0,
                    1, 1,
                    { 0x00 }
                },
            };

            for (int i = 0; i < G_N_ELEMENTS(data); i++)
            {
                std::string id = (boost::format("For i = %d") % i).str();

                struct bin_data *d = &data[i];
                gsize sz = d->size;

                SecurityDescriptor *sd1 =
                    secdesc_new_parse_binary(d->data, &sz);

                if (!d->successful)
                {
                    assert_null(sd1);
                }
                else
                {
                    assert_not_null(sd1);

                    assert_equal_msg(d->size_remaining, sz, id);

                    if (d->has_owner)
                    {
                        assert_not_null(secdesc_get_owner(sd1));
                    }
                    else
                    {
                        assert_null(secdesc_get_owner(sd1));
                    }

                    if (d->has_group)
                    {
                        assert_not_null(secdesc_get_group(sd1));
                    }
                    else
                    {
                        assert_null(secdesc_get_group(sd1));
                    }

                    ACList *sacl1 = secdesc_get_sacl(sd1);
                    assert_not_null_msg(sacl1, id);
                    if (d->has_sacl)
                    {
                        assert_equal_msg(d->sacl_count,
                                         aclist_get_acentry_count(sacl1),
                                         id);
                    }

                    ACList *dacl1 = secdesc_get_dacl(sd1);
                    assert_not_null_msg(dacl1, id);
                    if (d->has_dacl)
                    {
                        assert_equal_msg(d->dacl_count,
                                         aclist_get_acentry_count(dacl1),
                                         id);
                    }

                    GByteArray *arr1 = secdesc_to_binary(sd1, d->block_size);

                    assert_bin_data_equal(
                        arr1, d->data, d->size - d->size_remaining);

                    // This can't work because lots of secdescs aren't
                    // cleaned up properly.  They set the sacl offset
                    // to 0, but don't unset the SEC_DESC_SACL_PRESENT
                    // flag.
                    if (true)
                    {
                        const char *sd1_sddl = secdesc_to_sddl(sd1);
                        assert_not_null(sd1_sddl);

                        SecurityDescriptor *sd2 =
                            secdesc_new_parse_sddl_quick(sd1_sddl);

                        int prev_ll = rra_log_get_level();
                        rra_log_set_level(RRA_LOG_LEVEL_DEBUG);
                        assert_warn(
                            assert_true(secdesc_equal(sd1, sd2)));

                        rra_log_set_level(prev_ll);

                        assert_true_msg(secdesc_free(sd2), id);
                    }

                    assert_true_msg(secdesc_free(sd1), id);
                }
            }

            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());
            assert_equal(0, aclist_allocation_count());
            assert_equal(0, secdesc_allocation_count());

            return true;
        }

    public:

        test_security_descriptor(): Melunit::Test("test_security_descriptor")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_security_descriptor:: name)

            REGISTER(test_null);
            REGISTER(test_new);
            REGISTER(test_rev);
            REGISTER(test_type);
            REGISTER(test_owner);
            REGISTER(test_group);
            REGISTER(test_dacl);
            REGISTER(test_sacl);
            REGISTER(test_copy);
            REGISTER(test_compare);
            REGISTER(test_sddl);
            REGISTER(test_parse_binary);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_security_descriptor t1_;
}
