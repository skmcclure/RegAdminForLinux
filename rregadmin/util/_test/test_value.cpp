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


#include <rregadmin/config.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <functional>

#include <melunit/melunit-cxx.h>

#include <glib/gmem.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <rregadmin/util/value.h>
#include <rregadmin/util/intutils.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_value : public Melunit::Test
    {
    private:

        bool test_free()
        {
            assert_equal(0, value_allocated_count());
            Value *v1 = NULL;

            assert_false(value_free(v1));
            assert_equal(0, value_allocated_count());

            v1 = value_create_dword(10);
            assert_equal(1, value_allocated_count());

            assert_not_null(v1);

            assert_true(value_free(v1));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_can_handle()
        {
            assert_true(value_can_handle(REG_NONE));
            assert_true(value_can_handle(REG_SZ));
            assert_true(value_can_handle(REG_EXPAND_SZ));
            assert_true(value_can_handle(REG_MULTI_SZ));
            assert_true(value_can_handle(REG_BINARY));
            assert_true(value_can_handle(REG_DWORD));
            assert_true(value_can_handle(REG_DWORD_LITTLE_ENDIAN));
            assert_true(value_can_handle(REG_DWORD_BIG_ENDIAN));
            assert_true(value_can_handle(REG_QWORD));
            assert_true(value_can_handle(REG_QWORD_LITTLE_ENDIAN));

            assert_false(value_can_handle(REG_LINK));
            assert_false(value_can_handle(REG_RESOURCE_LIST));
            assert_false(value_can_handle(REG_FULL_RESOURCE_DESCRIPTOR));
            assert_false(value_can_handle(REG_RESOURCE_REQUIREMENTS_LIST));

            return true;
        }

        void do_none_tests(Value *v1)
        {
            assert_not_null(v1);

            assert_equal(REG_NONE, value_get_type(v1));
            assert_false(value_is_integral_type(v1));
            assert_true(value_is_binary_type(v1));
            assert_false(value_is_string_type(v1));

            assert_equal(std::string("NONE"),
                         value_get_type_str(v1));
            assert_equal(0, value_get_as_dword(v1));
            assert_equal(0, value_get_raw_length(v1));
            ustring *s_ret = value_get_as_string(v1);
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(s_ret)));
            ustr_free(s_ret);

            ustring *hex_ret = value_get_as_hexstring(v1);
            assert_not_null(hex_ret);
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(hex_ret)));
            ustr_free(hex_ret);

            GPtrArray *str_arr = value_get_as_string_array(v1);
            assert_not_null(str_arr);
            assert_equal(1, str_arr->len);
            assert_equal(
                std::string(""),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 0))));
            assert_true(value_free_str_array(str_arr));

            guint8 *test_bin = value_get_as_binary(v1);
            assert_null(test_bin);
            g_free(test_bin);

            GByteArray *encod = value_encode(v1, 0);
            assert_not_null(encod);
            assert_equal(0, encod->len);
            g_byte_array_free(encod, TRUE);
        }

        bool test_create_none()
        {
            assert_equal(0, value_allocated_count());
            Value *v1 = value_create_none();
            assert_equal(1, value_allocated_count());

            do_none_tests(v1);

            Value *v2 = value_copy(v1);
            assert_true(value_free(v1));
            do_none_tests(v2);

            assert_true(value_free(v2));
            assert_equal(0, value_allocated_count());

            return true;
        }

#define BYTE_OF_INT(in_int, in_index) ((guint8*)&(in_int))[in_index]

        void do_dword_tests(Value *v1, dword_type in_val)
        {
            assert_not_null(v1);

            ustring *s_val = ustr_new();
            ustr_printf(s_val, "%u", in_val);

            assert_equal(REG_DWORD, value_get_type(v1));
            assert_true(value_is_integral_type(v1));
            assert_false(value_is_binary_type(v1));
            assert_false(value_is_string_type(v1));

            assert_equal(std::string("DWORD"),
                         value_get_type_str(v1));
            assert_equal(in_val, value_get_as_dword(v1));
            assert_equal(4, value_get_raw_length(v1));
            ustring *s_ret = value_get_as_string(v1);
            assert_not_null(s_ret);
            assert_equal(std::string(ustr_as_utf8(s_val)),
                         std::string(ustr_as_utf8(s_ret)));
            ustr_free(s_ret);

            ustring *hex_ret = value_get_as_hexstring(v1);
            assert_not_null(hex_ret);
            ustring *hex_val = ustr_new();
            ustr_hexdump(hex_val, (guint8*)&in_val, 0, 4, TRUE, TRUE);
            assert_equal(std::string(ustr_as_utf8(hex_val)),
                         std::string(ustr_as_utf8(hex_ret)));
            ustr_free(hex_ret);
            ustr_free(hex_val);

            GPtrArray *str_arr = value_get_as_string_array(v1);
            assert_not_null(str_arr);
            assert_equal(1, str_arr->len);
            assert_equal(
                std::string(ustr_as_utf8(s_val)),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 0))));
            assert_true(value_free_str_array(str_arr));

            guint8 *test_bin = value_get_as_binary(v1);
            assert_equal(BYTE_OF_INT(in_val, 0), test_bin[0]);
            assert_equal(BYTE_OF_INT(in_val, 1), test_bin[1]);
            assert_equal(BYTE_OF_INT(in_val, 2), test_bin[2]);
            assert_equal(BYTE_OF_INT(in_val, 3), test_bin[3]);
            g_free(test_bin);

            GByteArray *encod = value_encode(v1, 0);
            assert_not_null(encod);
            assert_equal(4, encod->len);
            assert_equal(BYTE_OF_INT(in_val, 0), encod->data[0]);
            assert_equal(BYTE_OF_INT(in_val, 1), encod->data[1]);
            assert_equal(BYTE_OF_INT(in_val, 2), encod->data[2]);
            assert_equal(BYTE_OF_INT(in_val, 3), encod->data[3]);
            g_byte_array_free(encod, TRUE);

            ustr_free(s_val);
        }

        void one_create_dword_test(dword_type in_val)
        {
            Value *v1 = value_create_dword(in_val);
            assert_equal(1, value_allocated_count());
            do_dword_tests(v1, in_val);
            Value *v2 = value_copy(v1);
            assert_true(value_free(v1));
            do_dword_tests(v2, in_val);

            assert_true(value_free(v2));
            assert_equal(0, value_allocated_count());
        }

        bool test_create_dword()
        {
            one_create_dword_test(20);
            one_create_dword_test(1233451);
            one_create_dword_test(0);
            one_create_dword_test(1);
            one_create_dword_test(120957);
            one_create_dword_test(G_MAXUINT32);

            return true;
        }

        bool test_create_dword_be()
        {
            guint32 val1 = 0xffa00010;
            guint32 val1_le = 0x1000a0ff;

            assert_equal(0, value_allocated_count());
            Value *v1 = value_create_dword_be(val1);
            assert_not_null(v1);
            assert_equal(1, value_allocated_count());

            assert_equal(REG_DWORD_BIG_ENDIAN, value_get_type(v1));
            assert_true(value_is_integral_type(v1));
            assert_false(value_is_binary_type(v1));
            assert_false(value_is_string_type(v1));

            assert_equal(std::string("DWORD_BIG_ENDIAN"),
                         value_get_type_str(v1));
            assert_equal(val1_le, value_get_as_dword(v1));
            assert_equal(4, value_get_raw_length(v1));
            ustring *s_ret = value_get_as_string(v1);
            assert_equal(std::string("268476671"),
                         std::string(ustr_as_utf8(s_ret)));
            ustr_free(s_ret);

            ustring *hex_ret = value_get_as_hexstring(v1);
            assert_not_null(hex_ret);
            assert_equal(std::string("00000000:  10 00 A0 FF                                       ....\n"),
                         std::string(ustr_as_utf8(hex_ret)));
            ustr_free(hex_ret);

            GPtrArray *str_arr = value_get_as_string_array(v1);
            assert_not_null(str_arr);
            assert_equal(1, str_arr->len);
            assert_equal(
                std::string("268476671"),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 0))));
            assert_true(value_free_str_array(str_arr));

            guint8 *test_bin = value_get_as_binary(v1);
            assert_equal(0x10, test_bin[0]);
            assert_equal(0x00, test_bin[1]);
            assert_equal(0xa0, test_bin[2]);
            assert_equal(0xff, test_bin[3]);
            g_free(test_bin);

            GByteArray *encod = value_encode(v1, 0);
            assert_not_null(encod);
            assert_equal(4, encod->len);
            assert_equal(0x10, encod->data[0]);
            assert_equal(0x00, encod->data[1]);
            assert_equal(0xa0, encod->data[2]);
            assert_equal(0xff, encod->data[3]);
            g_byte_array_free(encod, TRUE);

            assert_true(value_free(v1));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool do_qword_tests(Value *v1, qword_type in_val)
        {
            assert_not_null(v1);

            ustring *s_val = ustr_new();
            ustr_printf(s_val, "%" G_GINT64_FORMAT, in_val);

            assert_equal(REG_QWORD, value_get_type(v1));
            assert_true(value_is_integral_type(v1));
            assert_false(value_is_binary_type(v1));
            assert_false(value_is_string_type(v1));

            assert_equal(std::string("QWORD"),
                         value_get_type_str(v1));

            if (llabs(in_val) > G_MAXINT32)
            {
                if (in_val > 0)
                {
                    assert_equal(G_MAXINT32, value_get_as_dword(v1));
                }
                else
                {
                    assert_equal(G_MININT32, value_get_as_dword(v1));
                }
            }
            else
            {
                assert_equal((dword_type)in_val, value_get_as_dword(v1));
            }

            assert_equal(in_val, value_get_as_qword(v1));
            assert_equal(8, value_get_raw_length(v1));

            ustring *s_ret = value_get_as_string(v1);
            assert_equal(std::string(ustr_as_utf8(s_val)),
                         std::string(ustr_as_utf8(s_ret)));
            ustr_free(s_ret);

            ustring *hex_ret = value_get_as_hexstring(v1);
            assert_not_null(hex_ret);
            ustring *hex_val = ustr_new();
            ustr_hexdump(hex_val, (guint8*)&in_val, 0, 8, TRUE, TRUE);
            assert_equal(std::string(ustr_as_utf8(hex_val)),
                         std::string(ustr_as_utf8(hex_ret)));
            ustr_free(hex_ret);
            ustr_free(hex_val);

            GPtrArray *str_arr = value_get_as_string_array(v1);
            assert_not_null(str_arr);
            assert_equal(1, str_arr->len);
            assert_equal(
                std::string(ustr_as_utf8(s_val)),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 0))));
            assert_true(value_free_str_array(str_arr));

            guint8 *test_bin = value_get_as_binary(v1);
            for (int i = 0; i < 8; i++)
            {
                assert_equal(BYTE_OF_INT(in_val, i), test_bin[i]);
            }
            g_free(test_bin);

            GByteArray *encod = value_encode(v1, 0);
            assert_not_null(encod);
            assert_equal(8, encod->len);
            for (int i = 0; i < 8; i++)
            {
                assert_equal(BYTE_OF_INT(in_val, i), encod->data[i]);
            }
            g_byte_array_free(encod, TRUE);

            ustr_free(s_val);

            return true;
        }

        bool test_create_qword()
        {
            gint64 val1 = 0x00ffe0aa1100aaf0ll;

            assert_equal(0, value_allocated_count());
            Value *v1 = value_create_qword(val1);
            assert_equal(1, value_allocated_count());
            do_qword_tests(v1, val1);
            Value *v2 = value_copy(v1);
            assert_true(value_free(v1));
            do_qword_tests(v2, val1);
            assert_true(value_free(v2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_create_binary1()
        {
            guint8 bin_data[] = { 0x07, 0x10, 0x22 };

            assert_equal(0, value_allocated_count());
            Value *v1 = value_create_binary(bin_data, 3);
            assert_not_null(v1);
            assert_equal(1, value_allocated_count());

            assert_equal(REG_BINARY, value_get_type(v1));
            assert_false(value_is_integral_type(v1));
            assert_true(value_is_binary_type(v1));
            assert_false(value_is_string_type(v1));

            assert_equal(std::string("BINARY"),
                         value_get_type_str(v1));
            assert_equal(0, value_get_as_dword(v1));
            assert_equal(3, value_get_raw_length(v1));

            ustring *s_ret = value_get_as_string(v1);
            assert_equal(std::string("00000000:  07 10 22                                          ..\"\n"),
                         std::string(ustr_as_utf8(s_ret)));
            ustring *hex_ret = value_get_as_hexstring(v1);
            assert_not_null(hex_ret);
            assert_equal(std::string("00000000:  07 10 22                                          ..\"\n"),
                         std::string(ustr_as_utf8(hex_ret)));
            assert_equal(std::string(ustr_as_utf8(s_ret)),
                         std::string(ustr_as_utf8(hex_ret)));
            ustr_free(hex_ret);
            ustr_free(s_ret);

            GPtrArray *str_arr = value_get_as_string_array(v1);
            assert_not_null(str_arr);
            assert_equal(1, str_arr->len);
            assert_equal(
                std::string("00000000:  07 10 22                                          ..\"\n"),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 0))));
            assert_true(value_free_str_array(str_arr));

            guint8 *test_bin = value_get_as_binary(v1);
            assert_equal(0x07, test_bin[0]);
            assert_equal(0x10, test_bin[1]);
            assert_equal(0x22, test_bin[2]);
            g_free(test_bin);

            GByteArray *encod = value_encode(v1, 0);
            assert_not_null(encod);
            assert_equal(3, encod->len);
            assert_equal(0x07, encod->data[0]);
            assert_equal(0x10, encod->data[1]);
            assert_equal(0x22, encod->data[2]);
            g_byte_array_free(encod, TRUE);

            assert_true(value_free(v1));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_create_binary2()
        {
            guint8 bin_data[] = { 0x07, 0x10, 0x22, 0x00, 0x55,
                                  0x11, 0x89, 0xfe, 0xab, 0x99,
                                  0x7f, 0x6d, 0xae, 0xff, 0x00,
                                  0xee
            };

            Value *v1 = value_create_binary(bin_data, 16);
            assert_not_null(v1);

            assert_equal(REG_BINARY, value_get_type(v1));
            assert_false(value_is_integral_type(v1));
            assert_true(value_is_binary_type(v1));
            assert_false(value_is_string_type(v1));

            assert_equal(std::string("BINARY"),
                         value_get_type_str(v1));
            assert_equal(0, value_get_as_dword(v1));
            assert_equal(16, value_get_raw_length(v1));

            ustring *s_ret = value_get_as_string(v1);
            assert_not_null(s_ret);
            assert_equal(std::string("00000000:  07 10 22 00 55 11 89 FE  AB 99 7F 6D AE FF 00 EE  ..\".U......m....\n"),
                         std::string(ustr_as_utf8(s_ret)));
            ustring *hex_ret = value_get_as_hexstring(v1);
            assert_not_null(hex_ret);
            assert_equal(std::string("00000000:  07 10 22 00 55 11 89 FE  AB 99 7F 6D AE FF 00 EE  ..\".U......m....\n"),
                         std::string(ustr_as_utf8(hex_ret)));
            assert_equal(std::string(ustr_as_utf8(s_ret)),
                         std::string(ustr_as_utf8(hex_ret)));
            ustr_free(hex_ret);
            ustr_free(s_ret);

            GPtrArray *str_arr = value_get_as_string_array(v1);
            assert_not_null(str_arr);
            assert_equal(1, str_arr->len);
            assert_equal(
                std::string("00000000:  07 10 22 00 55 11 89 FE  AB 99 7F 6D AE FF 00 EE  ..\".U......m....\n"),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 0))));
            assert_true(value_free_str_array(str_arr));

            guint8 *test_bin = value_get_as_binary(v1);
            assert_equal(0x07, test_bin[0]);
            assert_equal(0x10, test_bin[1]);
            assert_equal(0x22, test_bin[2]);
            assert_equal(0x00, test_bin[3]);
            assert_equal(0x55, test_bin[4]);

            assert_equal(0x11, test_bin[5]);
            assert_equal(0x89, test_bin[6]);
            assert_equal(0xfe, test_bin[7]);
            assert_equal(0xab, test_bin[8]);
            assert_equal(0x99, test_bin[9]);

            assert_equal(0x7f, test_bin[10]);
            assert_equal(0x6d, test_bin[11]);
            assert_equal(0xae, test_bin[12]);
            assert_equal(0xff, test_bin[13]);
            assert_equal(0x00, test_bin[14]);

            assert_equal(0xee, test_bin[15]);

            g_free(test_bin);

            GByteArray *encod = value_encode(v1, 0);
            assert_not_null(encod);
            assert_equal(16, encod->len);
            assert_equal(0x07, encod->data[0]);
            assert_equal(0x10, encod->data[1]);
            assert_equal(0x22, encod->data[2]);
            assert_equal(0x00, encod->data[3]);
            assert_equal(0x55, encod->data[4]);

            assert_equal(0x11, encod->data[5]);
            assert_equal(0x89, encod->data[6]);
            assert_equal(0xfe, encod->data[7]);
            assert_equal(0xab, encod->data[8]);
            assert_equal(0x99, encod->data[9]);

            assert_equal(0x7f, encod->data[10]);
            assert_equal(0x6d, encod->data[11]);
            assert_equal(0xae, encod->data[12]);
            assert_equal(0xff, encod->data[13]);
            assert_equal(0x00, encod->data[14]);

            assert_equal(0xee, encod->data[15]);
            g_byte_array_free(encod, TRUE);

            assert_true(value_free(v1));

            assert_equal(0, value_allocated_count());
            return true;
        }

        void do_string_tests(Value *v1, const char *in_str)
        {
            int in_str_len = strlen(in_str) + 1;

            assert_not_null(v1);

            assert_equal(REG_SZ, value_get_type(v1));
            assert_false(value_is_integral_type(v1));
            assert_false(value_is_binary_type(v1));
            assert_true(value_is_string_type(v1));

            assert_equal(std::string("SZ"),
                         value_get_type_str(v1));
            assert_equal(0, value_get_as_dword(v1));
            assert_equal(in_str_len, value_get_raw_length(v1));

            ustring *s_ret = value_get_as_string(v1);
            assert_equal(std::string(in_str),
                         std::string(ustr_as_utf8(s_ret)));
            ustr_free(s_ret);

            ustring *hex_ret = value_get_as_hexstring(v1);
            assert_not_null(hex_ret);
            ustring *hex_val = ustr_new();
            ustr_hexdump(hex_val, (const guint8*)in_str,
                         0, in_str_len, TRUE, TRUE);
            assert_equal_msg(std::string(ustr_as_utf8(hex_val)),
                             std::string(ustr_as_utf8(hex_ret)),
                             in_str);
            ustr_free(hex_ret);
            ustr_free(hex_val);

            GPtrArray *str_arr = value_get_as_string_array(v1);
            assert_not_null(str_arr);
            assert_equal(1, str_arr->len);
            assert_equal(
                std::string(in_str),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 0))));
            assert_true(value_free_str_array(str_arr));

            guint8 *test_bin = value_get_as_binary(v1);
            for (int i = 0; i < in_str_len; i++)
            {
                assert_equal(in_str[i], test_bin[i]);
            }
            g_free(test_bin);

            GByteArray *encod = value_encode(v1, 0);
            assert_not_null(encod);
            assert_equal(in_str_len, encod->len);
            for (int i = 0; i < in_str_len; i++)
            {
                assert_equal_msg(in_str[i], encod->data[i], in_str);
            }
            g_byte_array_free(encod, TRUE);

            GByteArray *encod16 = value_encode(v1, VALUE_STR_UTF16);
            assert_not_null(encod16);
            assert_equal((in_str_len) * 2, encod16->len);
            for (int i = 0; i < in_str_len; i++)
            {
                assert_equal(in_str[i], encod16->data[i * 2]);
                assert_equal('\0', encod16->data[i * 2 + 1]);
            }
            g_byte_array_free(encod16, TRUE);
        }

        void one_test_create_string(const char *in_val)
        {
            assert_equal(0, value_allocated_count());
            Value *v1 = value_create_string(in_val, strlen(in_val) + 1,
                                            USTR_TYPE_UTF8);
            assert_equal(1, value_allocated_count());
            do_string_tests(v1, in_val);
            Value *v2 = value_copy(v1);
            assert_true(value_free(v1));
            do_string_tests(v2, in_val);
            assert_true(value_free(v2));
            assert_equal(0, value_allocated_count());
        }

        bool test_create_string1()
        {
            one_test_create_string("blah on you\twheee\n");
            one_test_create_string("");
            one_test_create_string("                     ");
            one_test_create_string("f");

            return true;
        }

        bool test_create_expanded_string1()
        {
            const char *str_data = "blah on you\twheee\n";

            assert_equal(0, value_allocated_count());
            Value *v1 = value_create_expanded_string(str_data,
                                                     strlen(str_data) + 1,
                                                     USTR_TYPE_UTF8);
            assert_not_null(v1);
            assert_equal(1, value_allocated_count());

            assert_equal(REG_EXPAND_SZ, value_get_type(v1));
            assert_false(value_is_integral_type(v1));
            assert_false(value_is_binary_type(v1));
            assert_true(value_is_string_type(v1));

            assert_equal(std::string("EXPAND_SZ"),
                         value_get_type_str(v1));
            assert_equal(0, value_get_as_dword(v1));
            assert_equal(19, value_get_raw_length(v1));

            ustring *s_ret = value_get_as_string(v1);
            assert_equal(std::string("blah on you\twheee\n"),
                         std::string(ustr_as_utf8(s_ret)));
            ustr_free(s_ret);

            ustring *hex_ret = value_get_as_hexstring(v1);
            assert_not_null(hex_ret);
            assert_equal(std::string("00000000:  62 6C 61 68 20 6F 6E 20  79 6F 75 09 77 68 65 65  blah on you.whee\n"
                                     "00000010:  65 0A 00                                          e..\n"),
                         std::string(ustr_as_utf8(hex_ret)));
            ustr_free(hex_ret);

            GPtrArray *str_arr = value_get_as_string_array(v1);
            assert_not_null(str_arr);
            assert_equal(1, str_arr->len);
            assert_equal(
                std::string("blah on you\twheee\n"),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 0))));
            assert_true(value_free_str_array(str_arr));

            guint8 *test_bin = value_get_as_binary(v1);
            assert_equal('b', test_bin[0]);
            assert_equal('l', test_bin[1]);
            assert_equal('a', test_bin[2]);
            assert_equal('h', test_bin[3]);
            assert_equal(' ', test_bin[4]);

            assert_equal('o', test_bin[5]);
            assert_equal('n', test_bin[6]);
            assert_equal(' ', test_bin[7]);
            assert_equal('y', test_bin[8]);
            assert_equal('o', test_bin[9]);

            assert_equal('u', test_bin[10]);
            assert_equal('\t', test_bin[11]);
            assert_equal('w', test_bin[12]);
            assert_equal('h', test_bin[13]);
            assert_equal('e', test_bin[14]);

            assert_equal('e', test_bin[15]);
            assert_equal('e', test_bin[16]);
            assert_equal('\n', test_bin[17]);
            assert_equal('\0', test_bin[18]);

            g_free(test_bin);

            assert_true(value_free(v1));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_create_multi_string1()
        {
            const char *str_data = "bla\0h on \0you\twheee\n\0";

            assert_equal(0, value_allocated_count());
            Value *v1 = value_create_multi_string_bin(str_data, 22,
                                                      USTR_TYPE_UTF8);
            assert_not_null(v1);
            assert_equal(1, value_allocated_count());

            assert_equal(REG_MULTI_SZ, value_get_type(v1));
            assert_false(value_is_integral_type(v1));
            assert_false(value_is_binary_type(v1));
            assert_true(value_is_string_type(v1));

            assert_equal(std::string("MULTI_SZ"),
                         value_get_type_str(v1));
            assert_equal(0, value_get_as_dword(v1));
            assert_equal(22, value_get_raw_length(v1));

            ustring *s_ret = value_get_as_string(v1);
            assert_equal(std::string("bla, h on , you\twheee\n"),
                         std::string(ustr_as_utf8(s_ret)));
            ustr_free(s_ret);

            GPtrArray *str_arr = value_get_as_string_array(v1);
            assert_not_null(str_arr);
            assert_equal(3, str_arr->len);
            assert_equal(
                std::string("bla"),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 0))));
            assert_equal(
                std::string("h on "),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 1))));
            assert_equal(
                std::string("you\twheee\n"),
                std::string(ustr_as_utf8(
                                (ustring*)g_ptr_array_index(str_arr, 2))));
            assert_equal(
                4,
                ustr_size((ustring*)g_ptr_array_index(str_arr, 0)));
            assert_equal(
                6,
                ustr_size((ustring*)g_ptr_array_index(str_arr, 1)));
            assert_equal(
                11,
                ustr_size((ustring*)g_ptr_array_index(str_arr, 2)));
            assert_true(value_free_str_array(str_arr));

            ustring *hex_ret = value_get_as_hexstring(v1);
            assert_not_null(hex_ret);
            assert_equal(std::string("00000000:  62 6C 61 00 68 20 6F 6E  20 00 79 6F 75 09 77 68  bla.h on .you.wh\n"
                                     "00000010:  65 65 65 0A 00 00                                 eee...\n"),
                         std::string(ustr_as_utf8(hex_ret)));
            ustr_free(hex_ret);

            guint8 *test_bin = value_get_as_binary(v1);
            assert_not_null(test_bin);
            int test_bin_loc = 0;
            assert_equal('b', test_bin[test_bin_loc++]);
            assert_equal('l', test_bin[test_bin_loc++]);
            assert_equal('a', test_bin[test_bin_loc++]);
            assert_equal('\0', test_bin[test_bin_loc++]);
            assert_equal('h', test_bin[test_bin_loc++]);
            assert_equal(' ', test_bin[test_bin_loc++]);

            assert_equal('o', test_bin[test_bin_loc++]);
            assert_equal('n', test_bin[test_bin_loc++]);
            assert_equal(' ', test_bin[test_bin_loc++]);
            assert_equal('\0', test_bin[test_bin_loc++]);
            assert_equal('y', test_bin[test_bin_loc++]);
            assert_equal('o', test_bin[test_bin_loc++]);

            assert_equal('u', test_bin[test_bin_loc++]);
            assert_equal('\t', test_bin[test_bin_loc++]);
            assert_equal('w', test_bin[test_bin_loc++]);
            assert_equal('h', test_bin[test_bin_loc++]);
            assert_equal('e', test_bin[test_bin_loc++]);

            assert_equal('e', test_bin[test_bin_loc++]);
            assert_equal('e', test_bin[test_bin_loc++]);
            assert_equal('\n', test_bin[test_bin_loc++]);
            assert_equal('\0', test_bin[test_bin_loc++]);
            assert_equal('\0', test_bin[test_bin_loc++]);

            g_free(test_bin);

            assert_true(value_free(v1));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_equal_none(void)
        {
            assert_equal(0, value_allocated_count());
            Value *val1 = value_create_none();
            assert_equal(1, value_allocated_count());
            Value *val2 = value_create_none();
            assert_equal(2, value_allocated_count());

            assert_true(value_equal(val1, val2));
            assert_false(value_equal(NULL, val1));
            assert_false(value_equal(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_equal_dword(void)
        {
            Value *val1 = value_create_dword(20);
            Value *val2 = value_create_dword(20);

            assert_true(value_equal(val1, val2));
            assert_false(value_equal(NULL, val1));
            assert_false(value_equal(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_equal_dword_be(void)
        {
            Value *val1 = value_create_dword_be(20);
            Value *val2 = value_create_dword_be(20);

            assert_true(value_equal(val1, val2));
            assert_false(value_equal(NULL, val1));
            assert_false(value_equal(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_equal_qword(void)
        {
            Value *val1 = value_create_qword(5600);
            Value *val2 = value_create_qword(5600);

            assert_true(value_equal(val1, val2));
            assert_false(value_equal(NULL, val1));
            assert_false(value_equal(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_equal_binary(void)
        {
            guint8 data1[] = { 0xff, 0xee, 0xae };
            guint8 data2[] = { 0xff, 0xee, 0xae };

            Value *val1 = value_create_binary(data1, 3);
            Value *val2 = value_create_binary(data2, 3);

            assert_true(value_equal(val1, val2));
            assert_false(value_equal(NULL, val1));
            assert_false(value_equal(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_equal_string(void)
        {
            Value *val1 = value_create_string("foobar", 6, USTR_TYPE_UTF8);
            Value *val2 = value_create_string("foobar", 6, USTR_TYPE_UTF8);

            assert_not_null(val1);
            assert_not_null(val2);

            assert_true(value_equal(val1, val2));
            assert_false(value_equal(NULL, val1));
            assert_false(value_equal(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_compare_null(void)
        {
            assert_true(value_equal(NULL, NULL));
            assert_equal(0, value_compare(NULL, NULL));

            return true;
        }

        bool test_compare_none(void)
        {
            Value *val1 = value_create_none();
            Value *val2 = value_create_none();

            assert_equal(0, value_compare(val1, val2));
            assert_true(0 != value_compare(NULL, val1));
            assert_true(0 != value_compare(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_compare_dword(void)
        {
            Value *val1 = value_create_dword(20);
            Value *val2 = value_create_dword(20);

            assert_equal(0, value_compare(val1, val2));
            assert_true(0 != value_compare(NULL, val1));
            assert_true(0 != value_compare(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            val1 = value_create_dword(30);
            val2 = value_create_dword(20);

            assert_compare(std::less, 0, value_compare(val1, val2));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_compare_dword_be(void)
        {
            Value *val1 = value_create_dword_be(20);
            Value *val2 = value_create_dword_be(20);

            assert_equal(0, value_compare(val1, val2));
            assert_true(0 != value_compare(NULL, val1));
            assert_true(0 != value_compare(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_compare_qword(void)
        {
            Value *val1 = value_create_qword(5600);
            Value *val2 = value_create_qword(5600);

            assert_equal(0, value_compare(val1, val2));
            assert_true(0 != value_compare(NULL, val1));
            assert_true(0 != value_compare(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            val1 = value_create_qword(10000);
            val2 = value_create_qword(100001);

            assert_compare(std::greater, 0, value_compare(val1, val2));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_compare_binary(void)
        {
            guint8 data1[] = { 0xff, 0xee, 0xae };
            guint8 data2[] = { 0xff, 0xee, 0xae };

            Value *val1 = value_create_binary(data1, 3);
            Value *val2 = value_create_binary(data2, 3);

            assert_equal(0, value_compare(val1, val2));
            assert_true(0 != value_compare(NULL, val1));
            assert_true(0 != value_compare(val1, NULL));

            guint8 data3[] = { 0xff, 0xe1, 0xae };
            guint8 data4[] = { 0xff, 0xee, 0xae };

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            val1 = value_create_binary(data3, 3);
            val2 = value_create_binary(data4, 3);

            assert_compare(std::greater, 0, value_compare(val1, val2));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_compare_string(void)
        {
            Value *val1 = value_create_string("foobar", 6, USTR_TYPE_UTF8);
            Value *val2 = value_create_string("foobar", 6, USTR_TYPE_UTF8);

            assert_not_null(val1);
            assert_not_null(val2);

            assert_equal(0, value_compare(val1, val2));
            assert_true(0 != value_compare(NULL, val1));
            assert_true(0 != value_compare(val1, NULL));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            val1 = value_create_string("afoobar", 6, USTR_TYPE_UTF8);
            val2 = value_create_string("foobar", 6, USTR_TYPE_UTF8);

            assert_not_null(val1);
            assert_not_null(val2);

            assert_compare(std::greater, 0, value_compare(val1, val2));

            assert_true(value_free(val1));
            assert_equal(1, value_allocated_count());
            assert_true(value_free(val2));
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_create_fs_none1(void)
        {
            Value *v1 = value_create_from_string(NULL, REG_NONE);

            do_none_tests(v1);

            value_free(v1);

            v1 = value_create_from_string("", REG_NONE);

            do_none_tests(v1);

            value_free(v1);

            v1 = value_create_from_string("foobar", REG_NONE);

            do_none_tests(v1);

            value_free(v1);

            assert_equal(0, value_allocated_count());

            return true;
        }

        void one_test_create_fs_dword(const char *in_val)
        {
            Value *v1 = value_create_from_string(in_val, REG_DWORD);
            guint32 conv_val;
            assert_true(str_to_guint32(in_val, &conv_val));
            do_dword_tests(v1, conv_val);
            value_free(v1);
        }

        void one_failed_test_create_dword_test(const char *in_val)
        {
            assert_null_msg(value_create_from_string(in_val, REG_DWORD),
                            in_val);
        }

        bool test_create_fs_dword1(void)
        {
            assert_null(value_create_from_string(NULL, REG_DWORD));
            one_failed_test_create_dword_test("");
            one_failed_test_create_dword_test("999999999999999999999999999");
            one_failed_test_create_dword_test("10 foo");
            one_failed_test_create_dword_test("10ab");

            one_test_create_fs_dword("10");
            one_test_create_fs_dword("1");
            one_test_create_fs_dword("0");
            one_test_create_fs_dword("0xffffffff");
            one_test_create_fs_dword("   20");
            one_test_create_fs_dword("20  ");
            one_test_create_fs_dword("  20  ");
            one_test_create_fs_dword("1023181");
            one_test_create_fs_dword("0x10ab");
            one_test_create_fs_dword("07701");

            assert_equal(0, value_allocated_count());

            return true;
        }

        void one_test_create_fs_qword(const char *in_val)
        {
            Value *v1 = value_create_from_string(in_val, REG_QWORD);
            qword_type conv_val = strtoll(in_val, NULL, 0);
            do_qword_tests(v1, conv_val);
            value_free(v1);
        }

        void one_failed_test_create_qword_test(const char *in_val)
        {
            assert_null_msg(value_create_from_string(in_val, REG_QWORD),
                            in_val);
        }

        bool test_create_fs_qword1(void)
        {
            assert_null(value_create_from_string(NULL, REG_QWORD));
            one_failed_test_create_qword_test("");
            one_failed_test_create_qword_test("999999999999999999999999999");
            one_failed_test_create_qword_test("10 foo");
            one_failed_test_create_qword_test("10ab");

            one_test_create_fs_qword("10");
            one_test_create_fs_qword("-1");
            one_test_create_fs_qword("0");
            one_test_create_fs_qword("-0");
            one_test_create_fs_qword("   20");
            one_test_create_fs_qword("20  ");
            one_test_create_fs_qword("  20  ");
            one_test_create_fs_qword("1023181");
            one_test_create_fs_qword("0x10ab");
            one_test_create_fs_qword("07701");

            assert_equal(0, value_allocated_count());

            return true;
        }

        void one_test_create_fs_string(const char *in_val)
        {
            Value *v1 = value_create_from_string(in_val, REG_SZ);
            do_string_tests(v1, in_val);
            value_free(v1);
        }

        bool test_create_fs_string1(void)
        {
            assert_null(value_create_from_string(NULL, REG_SZ));

            one_test_create_fs_string("");
            one_test_create_fs_string("woop");
            one_test_create_fs_string("                                                                                                                                                                 ");
            one_test_create_fs_string("10");
            one_test_create_fs_string("-1");
            one_test_create_fs_string("0");
            one_test_create_fs_string("-0");
            one_test_create_fs_string("   20");
            one_test_create_fs_string("20  ");
            one_test_create_fs_string("  20  ");
            one_test_create_fs_string("1023181");
            one_test_create_fs_string("0x10ab");
            one_test_create_fs_string("07701");

            assert_equal(0, value_allocated_count());

            return true;
        }

        void one_test_create_fs_binary(const char *in_str,
                                       guint8 *in_data,
                                       guint32 in_data_len)
        {
            Value *v1 = value_create_from_string(in_str, REG_BINARY);
            assert_not_null_msg(v1, in_str);

            assert_equal_msg(in_data_len, value_get_raw_length(v1), in_str);

            GByteArray *arr = value_encode(v1, 0);
            assert_not_null(arr);
            assert_equal_msg(in_data_len, arr->len, in_str);

            for (int i = 0; i < in_data_len; i++)
            {
                assert_equal_msg(in_data[i], arr->data[i], in_str);
            }

            g_byte_array_free(arr, TRUE);
            assert_true(value_free(v1));
        }

        bool test_create_fs_binary1(void)
        {
            assert_null(value_create_from_string(NULL, REG_BINARY));
            assert_null(value_create_from_string("", REG_BINARY));
            assert_null(value_create_from_string("0 x", REG_BINARY));
            assert_null(value_create_from_string("HEX", REG_BINARY));
            assert_null(value_create_from_string("HEX)", REG_BINARY));
            assert_null(value_create_from_string("HEX(", REG_BINARY));

            guint8 data1[] = { 0xff, 0xee };

            one_test_create_fs_binary("0xffee", data1, G_N_ELEMENTS(data1));
            one_test_create_fs_binary("0x", NULL, 0);
            one_test_create_fs_binary("HEX()", NULL, 0);

            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_failing_create_fs1(void)
        {
            assert_null(value_create_from_string("foo", REG_DWORD_BIG_ENDIAN));
            assert_null(value_create_from_string("foo", REG_LINK));
            assert_null(value_create_from_string("foo", REG_RESOURCE_LIST));
            assert_null(
                value_create_from_string("foo", REG_FULL_RESOURCE_DESCRIPTOR));
            assert_null(value_create_from_string(
                            "foo", REG_RESOURCE_REQUIREMENTS_LIST));

            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_create_fb_none1(void)
        {
            Value *v1 = value_create_from_string(NULL, REG_NONE);

            do_none_tests(v1);

            value_free(v1);

            v1 = value_create_from_string("", REG_NONE);

            do_none_tests(v1);

            value_free(v1);

            v1 = value_create_from_string("foobar", REG_NONE);

            do_none_tests(v1);

            value_free(v1);

            assert_equal(0, value_allocated_count());

            return true;
        }

        void one_test_create_fb_dword(dword_type in_val)
        {
            Value *v1 = value_create_from_binary((guint8*)&in_val, 4,
                                                 REG_DWORD);
            do_dword_tests(v1, in_val);
            value_free(v1);
        }

        bool test_create_fb_dword1(void)
        {
            assert_null(value_create_from_binary(NULL, 0, REG_DWORD));

            one_test_create_fb_dword(10);

            assert_equal(0, value_allocated_count());

            return true;
        }

        void one_test_create_fb_qword(qword_type in_val)
        {
            Value *v1 = value_create_from_binary((guint8*)&in_val, 8,
                                                 REG_QWORD);
            do_qword_tests(v1, in_val);
            value_free(v1);
        }

        bool test_create_fb_qword1(void)
        {
            assert_null(value_create_from_binary(NULL, 0, REG_QWORD));

            one_test_create_fb_qword(10);

            assert_equal(0, value_allocated_count());

            return true;
        }

        void one_test_create_fb_string(const char *in_val)
        {
            Value *v1 = value_create_from_binary((const guint8*)in_val,
                                                 strlen(in_val) + 1,
                                                 REG_SZ);
            do_string_tests(v1, in_val);
            value_free(v1);
        }

        bool test_create_fb_string1(void)
        {
            assert_null(value_create_from_string(NULL, REG_SZ));

            one_test_create_fb_string("");
            one_test_create_fb_string("woop");
            one_test_create_fb_string("                                                                                                                                                                 ");
            one_test_create_fb_string("10");
            one_test_create_fb_string("-1");
            one_test_create_fb_string("0");
            one_test_create_fb_string("-0");
            one_test_create_fb_string("   20");
            one_test_create_fb_string("20  ");
            one_test_create_fb_string("  20  ");
            one_test_create_fb_string("1023181");
            one_test_create_fb_string("0x10ab");
            one_test_create_fb_string("07701");

            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_clear_bits_dword1(void)
        {
            Value *val1 = value_create_dword(0x00015501);
            assert_not_null(val1);

            GByteArray *arr1 = value_encode(val1, 0);
            assert_equal(arr1->data[2], 0x01);

            assert_true(value_clear_bits(val1, 2, 0xff));

            GByteArray *arr2 = value_encode(val1, 0);
            assert_equal(arr2->data[2], 0x00);

            g_byte_array_free(arr1, TRUE);
            g_byte_array_free(arr2, TRUE);

            value_free(val1);
            assert_equal(0, value_allocated_count());

            return true;
        }

        bool test_clear_bits_dword_fail1(void)
        {
            Value *val1 = value_create_dword(0x00015501);
            assert_not_null(val1);

            assert_false(value_clear_bits(val1, 10, 0xff));

            value_free(val1);
            assert_equal(0, value_allocated_count());
            return true;
        }

        bool test_set_bits_dword1(void)
        {
            Value *val1 = value_create_dword(0x00015501);
            assert_not_null(val1);

            GByteArray *arr1 = value_encode(val1, 0);
            assert_equal(arr1->data[2], 0x01);

            assert_true(value_set_bits(val1, 2, 0xaf));

            GByteArray *arr2 = value_encode(val1, 0);
            assert_equal(arr2->data[2], 0xaf);

            g_byte_array_free(arr1, TRUE);
            g_byte_array_free(arr2, TRUE);

            value_free(val1);
            assert_equal(0, value_allocated_count());
            return true;
        }

        bool test_set_bits_dword_fail1(void)
        {
            Value *val1 = value_create_dword(0x00015501);
            assert_not_null(val1);

            assert_false(value_set_bits(val1, 10, 0xff));

            value_free(val1);
            assert_equal(0, value_allocated_count());
            return true;
        }

        Value* create_clear_set_binary(void)
        {
            guint8 data1[] = { 0xff, 0xee, 0xae, 0x00,
                               0xff, 0xee, 0xae, 0x00,
                               0xff, 0xee, 0xae, 0x00,
                               0xff, 0xee, 0xae, 0x00,
            };
            Value *val1 = value_create_binary(data1, G_N_ELEMENTS(data1));
            assert_not_null(val1);
            return val1;
        }

        bool test_clear_bits_binary1(void)
        {
            Value *val1 = create_clear_set_binary();

            GByteArray *arr1 = value_encode(val1, 0);
            assert_equal(arr1->data[2], 0xae);

            assert_true(value_clear_bits(val1, 2, 0xff));

            GByteArray *arr2 = value_encode(val1, 0);
            assert_equal(arr2->data[2], 0x00);

            g_byte_array_free(arr1, TRUE);
            g_byte_array_free(arr2, TRUE);

            value_free(val1);
            assert_equal(0, value_allocated_count());
            return true;
        }

        bool test_clear_bits_binary_fail1(void)
        {
            Value *val1 = create_clear_set_binary();

            assert_false(value_clear_bits(val1, 350, 0xff));

            value_free(val1);
            assert_equal(0, value_allocated_count());
            return true;
        }

        bool test_set_bits_binary1(void)
        {
            Value *val1 = create_clear_set_binary();

            GByteArray *arr1 = value_encode(val1, 0);
            assert_equal(arr1->data[2], 0xae);

            assert_true(value_set_bits(val1, 2, 0xaf));

            GByteArray *arr2 = value_encode(val1, 0);
            assert_equal(arr2->data[2], 0xaf);

            g_byte_array_free(arr1, TRUE);
            g_byte_array_free(arr2, TRUE);

            value_free(val1);
            assert_equal(0, value_allocated_count());
            return true;
        }

        bool test_set_bits_binary_fail1(void)
        {
            Value *val1 = create_clear_set_binary();

            assert_false(value_set_bits(val1, 350, 0xff));

            value_free(val1);
            assert_equal(0, value_allocated_count());
            return true;
        }

        bool test_alloc_count1(void)
        {
            const int count1 = 10;
            Value *vs1[count1];
            int i;

            assert_equal(0, value_allocated_count());

            for (i = 0; i < count1; i++)
            {
                vs1[i] = value_create_dword(10);
                assert_equal(i + 1, value_allocated_count());
            }

            assert_equal(count1, value_allocated_count());

            for (i = 0; i < count1; i++)
            {
                value_free(vs1[i]);
                assert_equal(count1 - 1 - i, value_allocated_count());
            }

            assert_equal(0, value_allocated_count());

            return true;
        }

    public:

        test_value(): Melunit::Test("test_value")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_value:: name)

            REGISTER(test_can_handle);
            REGISTER(test_create_none);
            REGISTER(test_create_dword);
            REGISTER(test_create_dword_be);
            REGISTER(test_create_qword);
            REGISTER(test_free);
            REGISTER(test_create_binary1);
            REGISTER(test_create_binary2);
            REGISTER(test_create_string1);
            REGISTER(test_create_expanded_string1);
            REGISTER(test_create_multi_string1);

            REGISTER(test_equal_none);
            REGISTER(test_equal_dword);
            REGISTER(test_equal_dword_be);
            REGISTER(test_equal_qword);
            REGISTER(test_equal_binary);
            REGISTER(test_equal_string);

            REGISTER(test_compare_null);
            REGISTER(test_compare_none);
            REGISTER(test_compare_dword);
            REGISTER(test_compare_dword_be);
            REGISTER(test_compare_qword);
            REGISTER(test_compare_binary);
            REGISTER(test_compare_string);

            REGISTER(test_create_fs_none1);
            REGISTER(test_create_fs_dword1);
            REGISTER(test_create_fs_qword1);
            REGISTER(test_create_fs_string1);
            REGISTER(test_create_fs_binary1);
            REGISTER(test_failing_create_fs1);

            REGISTER(test_create_fb_none1);
            REGISTER(test_create_fb_dword1);
            REGISTER(test_create_fb_qword1);
            REGISTER(test_create_fb_string1);

            REGISTER(test_clear_bits_dword1);
            REGISTER(test_clear_bits_dword_fail1);
            REGISTER(test_set_bits_dword1);
            REGISTER(test_set_bits_dword_fail1);
            REGISTER(test_clear_bits_binary1);
            REGISTER(test_clear_bits_binary_fail1);
            REGISTER(test_set_bits_binary1);
            REGISTER(test_set_bits_binary_fail1);

            REGISTER(test_alloc_count1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_value t1_;
}
