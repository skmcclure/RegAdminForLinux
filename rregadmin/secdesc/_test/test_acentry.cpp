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
#include <stdio.h>

#include <vector>

#include <boost/assign/list_of.hpp>

#include <melunit/melunit-cxx.h>

#include <rregadmin/secdesc/acentry.h>
#include <rregadmin/secdesc/sid.h>
#include <rregadmin/util/init.h>

namespace
{
    using namespace boost::assign;

    class test_acentry : public Melunit::Test
    {
    private:

        bool test_new(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            ACEntry *a1 = acentry_new();
            assert_not_null(a1);

            assert_equal(1, acentry_allocation_count());

            assert_equal(SEC_ACE_TYPE_INVALID, acentry_get_type(a1));
            assert_equal(0, acentry_get_flags(a1));
            assert_equal(0, acentry_get_access_mask(a1));
            assert_null(acentry_get_trustee(a1));

            assert_true(acentry_free(a1));

            return true;
        }

        bool test_set_type(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            ACEntry *a1 = acentry_new();
            assert_not_null(a1);
            assert_equal(SEC_ACE_TYPE_INVALID, acentry_get_type(a1));

            assert_true(acentry_set_type(a1, SEC_ACE_TYPE_ACCESS_ALLOWED));
            assert_equal(SEC_ACE_TYPE_ACCESS_ALLOWED, acentry_get_type(a1));

            assert_true(acentry_free(a1));

            return true;
        }

        bool test_flags(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            ACEntry *a1 = acentry_new();
            assert_not_null(a1);
            assert_equal(0, acentry_get_flags(a1));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_OBJECT_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_CONTAINER_INHERIT));
            assert_false(
                acentry_has_flag(a1, SEC_ACE_FLAG_NO_PROPAGATE_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_INHERIT_ONLY));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_VALID_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_INHERITED_ACE));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_SUCCESSFUL_ACCESS));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_FAILED_ACCESS));

            assert_true(acentry_set_flags(a1, SEC_ACE_FLAG_OBJECT_INHERIT));
            assert_equal(SEC_ACE_FLAG_OBJECT_INHERIT, acentry_get_flags(a1));
            assert_true(acentry_has_flag(a1, SEC_ACE_FLAG_OBJECT_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_CONTAINER_INHERIT));
            assert_false(
                acentry_has_flag(a1, SEC_ACE_FLAG_NO_PROPAGATE_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_INHERIT_ONLY));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_VALID_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_INHERITED_ACE));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_SUCCESSFUL_ACCESS));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_FAILED_ACCESS));

            assert_true(acentry_add_flag(a1, SEC_ACE_FLAG_CONTAINER_INHERIT));
            assert_equal(SEC_ACE_FLAG_CONTAINER_INHERIT
                         | SEC_ACE_FLAG_OBJECT_INHERIT,
                         acentry_get_flags(a1));
            assert_true(acentry_has_flag(a1, SEC_ACE_FLAG_OBJECT_INHERIT));
            assert_true(acentry_has_flag(a1, SEC_ACE_FLAG_CONTAINER_INHERIT));
            assert_false(
                acentry_has_flag(a1, SEC_ACE_FLAG_NO_PROPAGATE_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_INHERIT_ONLY));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_VALID_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_INHERITED_ACE));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_SUCCESSFUL_ACCESS));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_FAILED_ACCESS));

            assert_true(acentry_del_flag(a1, SEC_ACE_FLAG_OBJECT_INHERIT));
            assert_equal(SEC_ACE_FLAG_CONTAINER_INHERIT,
                         acentry_get_flags(a1));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_OBJECT_INHERIT));
            assert_true(acentry_has_flag(a1, SEC_ACE_FLAG_CONTAINER_INHERIT));
            assert_false(
                acentry_has_flag(a1, SEC_ACE_FLAG_NO_PROPAGATE_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_INHERIT_ONLY));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_VALID_INHERIT));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_INHERITED_ACE));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_SUCCESSFUL_ACCESS));
            assert_false(acentry_has_flag(a1, SEC_ACE_FLAG_FAILED_ACCESS));

            assert_true(acentry_free(a1));

            return true;
        }

        bool test_access_mask1(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            std::vector<acentry_access_mask> masks =
                list_of
                (SEC_GENERIC_ALL)
                (SEC_GENERIC_EXECUTE)
                (SEC_GENERIC_WRITE)
                (SEC_GENERIC_READ)

                (SEC_FLAG_SYSTEM_SECURITY)
                (SEC_FLAG_MAXIMUM_ALLOWED)

                (SEC_STD_DELETE)
                (SEC_STD_READ_CONTROL)
                (SEC_STD_WRITE_DAC)
                (SEC_STD_WRITE_OWNER)
                (SEC_STD_SYNCHRONIZE)

//                 (SEC_FILE_READ_DATA)
//                 (SEC_FILE_WRITE_DATA)
//                 (SEC_FILE_APPEND_DATA)
//                 (SEC_FILE_READ_EA)
//                 (SEC_FILE_WRITE_EA)
//                 (SEC_FILE_EXECUTE)
//                 (SEC_FILE_READ_ATTRIBUTE)
//                 (SEC_FILE_WRITE_ATTRIBUTE)

                (SEC_DIR_LIST)
                (SEC_DIR_ADD_FILE)
                (SEC_DIR_ADD_SUBDIR)
                (SEC_DIR_READ_EA)
                (SEC_DIR_WRITE_EA)
                (SEC_DIR_TRAVERSE)
                (SEC_DIR_DELETE_CHILD)
                (SEC_DIR_READ_ATTRIBUTE)
                (SEC_DIR_WRITE_ATTRIBUTE)

//                 (SEC_REG_QUERY_VALUE)
//                 (SEC_REG_SET_VALUE)
//                 (SEC_REG_CREATE_SUBKEY)
//                 (SEC_REG_ENUM_SUBKEYS)
//                 (SEC_REG_NOTIFY)
//                 (SEC_REG_CREATE_LINK)

//                 (SEC_ADS_CREATE_CHILD)
//                 (SEC_ADS_DELETE_CHILD)
//                 (SEC_ADS_LIST)
//                 (SEC_ADS_SELF_WRITE)
//                 (SEC_ADS_READ_PROP)
//                 (SEC_ADS_WRITE_PROP)
//                 (SEC_ADS_DELETE_TREE)
//                 (SEC_ADS_LIST_OBJECT)
//                 (SEC_ADS_CONTROL_ACCESS)
                ;

            ACEntry *a1 = acentry_new();
            assert_not_null(a1);
            assert_equal(0, acentry_get_access_mask(a1));

            for (int i = 0; i < masks.size(); i++)
            {
                for (int j = 0; j < masks.size(); j++)
                {
                    assert_false(acentry_has_access_mask(a1, masks[j]));
                }

                assert_true(acentry_set_access_mask(a1, masks[i]));
                assert_equal(masks[i], acentry_get_access_mask(a1));
                for (int j = 0; j < masks.size(); j++)
                {
                    if (j == i)
                    {
                        assert_true(acentry_has_access_mask(a1, masks[j]));
                    }
                    else
                    {
                        assert_false(acentry_has_access_mask(a1, masks[j]));
                    }
                }

                assert_true(acentry_set_access_mask(a1, 0));
                assert_equal(0, acentry_get_access_mask(a1));
                for (int j = 0; j < masks.size(); j++)
                {
                    assert_false(acentry_has_access_mask(a1, masks[j]));
                }

                assert_true(acentry_add_access_mask(a1, masks[i]));
                assert_equal(masks[i], acentry_get_access_mask(a1));
                for (int j = 0; j < masks.size(); j++)
                {
                    if (j == i)
                    {
                        assert_true(acentry_has_access_mask(a1, masks[j]));
                    }
                    else
                    {
                        assert_false(acentry_has_access_mask(a1, masks[j]));
                    }
                }

                assert_true(acentry_del_access_mask(a1, masks[i]));
                assert_equal(0, acentry_get_access_mask(a1));
                for (int j = 0; j < masks.size(); j++)
                {
                    assert_false(acentry_has_access_mask(a1, masks[j]));
                }
            }

            assert_true(acentry_free(a1));

            return true;
        }

        bool test_access_mask2(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            ACEntry *a1 = acentry_new();
            assert_not_null(a1);
            assert_equal(0, acentry_get_access_mask(a1));

            assert_true(acentry_set_access_mask(a1, SEC_RIGHTS_FILE_READ));
            assert_equal(SEC_RIGHTS_FILE_READ, acentry_get_access_mask(a1));
            assert_true(acentry_has_access_mask(a1, SEC_STD_READ_CONTROL));
            assert_true(acentry_has_access_mask(a1, SEC_STD_SYNCHRONIZE));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_READ_DATA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_READ_ATTRIBUTE));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_READ_EA));

            assert_true(acentry_add_access_mask(a1, SEC_RIGHTS_FILE_WRITE));
            assert_true(acentry_has_access_mask(a1, SEC_STD_READ_CONTROL));
            assert_true(acentry_has_access_mask(a1, SEC_STD_SYNCHRONIZE));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_READ_DATA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_READ_ATTRIBUTE));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_READ_EA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_WRITE_DATA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_WRITE_ATTRIBUTE));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_WRITE_EA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_APPEND_DATA));

            assert_true(acentry_add_access_mask(a1, SEC_RIGHTS_FILE_EXECUTE));
            assert_true(acentry_has_access_mask(a1, SEC_STD_READ_CONTROL));
            assert_true(acentry_has_access_mask(a1, SEC_STD_SYNCHRONIZE));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_READ_DATA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_READ_ATTRIBUTE));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_READ_EA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_WRITE_DATA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_WRITE_ATTRIBUTE));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_WRITE_EA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_APPEND_DATA));
            assert_true(acentry_has_access_mask(a1, SEC_FILE_EXECUTE));

            assert_false(acentry_has_access_mask(a1, SEC_RIGHTS_FILE_ALL));

            assert_true(acentry_add_access_mask(a1, SEC_STD_ALL));
            assert_true(acentry_add_access_mask(a1, SEC_FILE_ALL));
            assert_true(acentry_has_access_mask(a1, SEC_RIGHTS_FILE_ALL));

            assert_true(acentry_free(a1));

            return true;
        }

        bool test_trustee(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            ACEntry *a1 = acentry_new();
            assert_not_null(a1);
            assert_null(acentry_get_trustee(a1));

            Sid *s1 = sid_new_id(10);
            assert_true(acentry_set_trustee(a1, s1));
            assert_not_null(acentry_get_trustee(a1));
            assert_equal(s1, acentry_get_trustee(a1));

            assert_true(acentry_free(a1));

            return true;
        }

        bool test_compare1(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            ACEntry *a1 = acentry_new();
            ACEntry *a2 = acentry_new();

            assert_equal(0, acentry_compare(a1, a2));
            assert_equal(0, acentry_compare(NULL, NULL));
            assert_compare(std::less, 0, acentry_compare(a1, NULL));
            assert_compare(std::greater, 0, acentry_compare(NULL, a2));

            assert_true(acentry_free(a1));
            assert_true(acentry_free(a2));

            return true;
        }

        bool test_copy1(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            ACEntry *a1 = acentry_new();
            assert_not_null(a1);

            Sid *s1 = sid_new_id(10);
            assert_true(acentry_set_trustee(a1, s1));
            assert_true(acentry_set_type(a1, SEC_ACE_TYPE_ACCESS_ALLOWED));
            assert_true(acentry_set_flags(a1, SEC_ACE_FLAG_OBJECT_INHERIT));
            assert_true(acentry_set_access_mask(a1, SEC_RIGHTS_FILE_ALL));

            ACEntry *a2 = acentry_copy(a1);
            assert_not_null(a2);

            assert_equal(0, acentry_compare(a1, a2));
            assert_equal(SEC_ACE_TYPE_ACCESS_ALLOWED, acentry_get_type(a2));
            assert_equal(SEC_ACE_FLAG_OBJECT_INHERIT, acentry_get_flags(a2));
            assert_equal(SEC_RIGHTS_FILE_ALL, acentry_get_access_mask(a2));

            assert_true(acentry_free(a1));
            assert_true(acentry_free(a2));

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

        bool test_parse_sddl1(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            const char *sddl_str = "A;;KA;;;S-1-0-5-10";
            ACEntry *a1 = acentry_new_parse_sddl(&sddl_str);
            assert_not_null(a1);
            assert_equal('\0', sddl_str[0]);

            assert_equal(SEC_ACE_TYPE_ACCESS_ALLOWED, acentry_get_type(a1));
            assert_equal(0, acentry_get_flags(a1));
            assert_equal(SEC_RIGHTS_REG_ALL, acentry_get_access_mask(a1));

            assert_not_null(acentry_get_trustee(a1));

            const Sid *s1 = acentry_get_trustee(a1);
            assert_equal(std::string("S-1-0-5-10"), sid_to_sddl(s1));

            // Duplicate possibilities here
            assert_equal(std::string("A;;FA;;;S-1-0-5-10"),
                         std::string(acentry_to_sddl(a1)));

            const guint8 data[] = {
                0x00,
                0x00,
                0x18, 0x00,
                0xff, 0x01, 0x1f, 0x00,

                0x01,
                0x02,
                0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x05, 0x00, 0x00, 0x00,
                0x0a, 0x00, 0x00, 0x00
            };

            assert_bin_data_equal(acentry_to_binary(a1),
                                  data, G_N_ELEMENTS(data));
            assert_true(acentry_free(a1));

            return true;
        }

        bool test_parse_sddl_quick1(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            const char *sddl_str = "A;;KA;;;S-1-0-5-10";
            ACEntry *a1 = acentry_new_parse_sddl_quick(sddl_str);
            assert_not_null(a1);

            assert_equal(SEC_ACE_TYPE_ACCESS_ALLOWED, acentry_get_type(a1));
            assert_equal(0, acentry_get_flags(a1));
            assert_equal(SEC_RIGHTS_REG_ALL, acentry_get_access_mask(a1));

            assert_not_null(acentry_get_trustee(a1));

            const Sid *s1 = acentry_get_trustee(a1);
            assert_equal(std::string("S-1-0-5-10"), sid_to_sddl(s1));

            // Duplicate possibilities here
            assert_equal(std::string("A;;FA;;;S-1-0-5-10"),
                         std::string(acentry_to_sddl(a1)));

            const guint8 data[] = {
                0x00,
                0x00,
                0x18, 0x00,
                0xff, 0x01, 0x1f, 0x00,

                0x01,
                0x02,
                0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x05, 0x00, 0x00, 0x00,
                0x0a, 0x00, 0x00, 0x00
            };

            assert_bin_data_equal(acentry_to_binary(a1),
                                  data, G_N_ELEMENTS(data));
            assert_true(acentry_free(a1));

            return true;
        }

        struct bin_data
        {
            acentry_type type;
            acentry_flags flags;
            acentry_access_mask mask;
            const char *sddl;
            const char *sid_sddl;

            gssize size;
            gssize size_remaining;
            guint8 data[512];
        };

        bool test_parse_bin1(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            struct bin_data data[] = {
                {
                    SEC_ACE_TYPE_ACCESS_ALLOWED,
                    0x03,
                    0x001f01ff,
                    "A;OICI;FA;;;S-1-83886080-18",
                    "S-1-83886080-18",
                    20,
                    0,
                    {
                        0x00,
                        0x03,
                        0x14,0x00,
                        0xff,0x01,0x1f,0x00,

                        0x01,
                        0x01,
                        0x00,0x00,
                        0x00,0x00,0x00,0x05,
                        0x12,0x00,0x00,0x00,
                    },
                },
                {
                    SEC_ACE_TYPE_ACCESS_ALLOWED,
                    0x03,
                    0x001f01ff,
                    "A;OICI;FA;;;S-1-83886080-18",
                    "S-1-83886080-18",
                    28,
                    8,
                    {
                        0x00,
                        0x03,
                        0x14,0x00,
                        0xff,0x01,0x1f,0x00,

                        0x01,
                        0x01,
                        0x00,0x00,
                        0x00,0x00,0x00,0x05,
                        0x12,0x00,0x00,0x00,

                        0x00,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,
                    },
                }
            };

            for (int i = 0; i < G_N_ELEMENTS(data); i++)
            {
                gssize sz = data[i].size;
                ACEntry *a1 =
                    acentry_new_parse_binary(data[i].data, &sz);
                assert_not_null(a1);

                assert_equal(data[i].size_remaining, sz);

                assert_equal(data[i].type, acentry_get_type(a1));
                assert_equal(data[i].flags, acentry_get_flags(a1));
                assert_equal(data[i].mask, acentry_get_access_mask(a1));

                assert_equal(std::string(data[i].sddl),
                             std::string(acentry_to_sddl(a1)));

                assert_not_null(acentry_get_trustee(a1));

                const Sid *s1 = acentry_get_trustee(a1);
                assert_equal(std::string(data[i].sid_sddl), sid_to_sddl(s1));

                assert_bin_data_equal(acentry_to_binary(a1),
                                      data[i].data,
                                      data[i].size - data[i].size_remaining);

                assert_true(acentry_free(a1));
            }

            return true;
        }

        bool test_to_bin1(void)
        {
            assert_equal(0, sid_allocation_count());
            assert_equal(0, acentry_allocation_count());

            ACEntry *a1 = acentry_new();
            assert_true(acentry_set_type(a1, SEC_ACE_TYPE_ACCESS_ALLOWED));
            assert_true(acentry_set_access_mask(a1,
                                                SEC_RIGHTS_FILE_ALL));
            Sid *s1 = sid_new_id(10);
            assert_true(acentry_set_trustee(a1, s1));

            const guint8 data[] = {
                0x00,
                0x00,
                0x10, 0x00,
                0xff, 0x01, 0x1f, 0x00,

                0x01,
                0x00,
                0x00, 0x00,
                0x0a, 0x00, 0x00, 0x00
            };
            guint32 data_len = G_N_ELEMENTS(data);

            assert_bin_data_equal(acentry_to_binary(a1), data, data_len);

            GByteArray *arr = g_byte_array_new();
            assert_true(acentry_write_to_binary(a1, arr));
            assert_bin_data_equal(arr, data, data_len);

            assert_true(acentry_free(a1));

            return true;
        }

    public:

        test_acentry(): Melunit::Test("test_acentry")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_acentry:: name)

            REGISTER(test_new);
            REGISTER(test_set_type);
            REGISTER(test_flags);
            REGISTER(test_access_mask1);
            REGISTER(test_access_mask2);
            REGISTER(test_trustee);
            REGISTER(test_compare1);
            REGISTER(test_copy1);
            REGISTER(test_parse_sddl1);
            REGISTER(test_parse_sddl_quick1);
            REGISTER(test_parse_bin1);
            REGISTER(test_to_bin1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_acentry t1_;
}
