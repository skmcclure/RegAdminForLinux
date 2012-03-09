/*
 * Authors:     James LewisMoss <jlm@racemi.com>
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

#include <glib/gmem.h>
#include <glib/gmacros.h>

#include <rregadmin/util/ustring.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_ustring : public Melunit::Test
    {
    private:

        bool testCreateDelete_null()
        {
            assert_equal(0, ustr_allocated_count());

            ustring *u1 = ustr_new();
            assert_equal(1, ustr_allocated_count());
            assert_not_null(u1);
            assert_true(ustr_is_valid(u1));
            assert_equal(1, ustr_size(u1));
            assert_equal(0, ustr_strlen(u1));
            assert_true(ustr_free(u1));
            assert_equal(0, ustr_allocated_count());

            ustring *v2 = NULL;
            assert_false(ustr_is_valid(v2));

            return true;
        }

        bool testCreateDelete_small1()
        {
            assert_equal(0, ustr_allocated_count());
            ustring *u1 = ustr_new();
            assert_equal(1, ustr_allocated_count());
            assert_not_null(u1);
            assert_equal(1, ustr_size(u1));
            assert_equal(0, ustr_strlen(u1));
            ustr_free(u1);
            assert_equal(0, ustr_allocated_count());

            return true;
        }

        bool testCreateDelete_small2()
        {
            ustring *u1 = ustr_new();
            assert_not_null(u1);
            assert_equal(1, ustr_size(u1));
            assert_equal(0, ustr_strlen(u1));
            ustr_free(u1);

            return true;
        }

        bool test_free_null()
        {
            assert_equal(FALSE, ustr_free(NULL));

            return true;
        }

        bool test_create1()
        {
            ustring *u1 = ustr_create("foobar");
            assert_not_null(u1);
            assert_equal(7, ustr_size(u1));
            assert_equal(6, ustr_strlen(u1));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            ustr_free(u1);

            return true;
        }

        bool test_create_type_ascii1()
        {
            ustring *u1 = ustr_create_type("foobar", 7, USTR_TYPE_ASCII);
            assert_not_null(u1);
            assert_equal(7, ustr_size(u1));
            assert_equal(6, ustr_strlen(u1));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            ustr_free(u1);

            return true;
        }

        bool test_create_type_iso8859_1_1()
        {
            ustring *u1 = ustr_create_type("foobar", 7, USTR_TYPE_ISO8859_1);
            assert_not_null(u1);
            assert_equal(7, ustr_size(u1));
            assert_equal(6, ustr_strlen(u1));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            ustr_free(u1);

            return true;
        }

        bool test_create_type_utf8_1()
        {
            ustring *u1 = ustr_create_type("foobar", 7, USTR_TYPE_UTF8);
            assert_not_null(u1);
            assert_equal(7, ustr_size(u1));
            assert_equal(6, ustr_strlen(u1));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            ustr_free(u1);

            return true;
        }

        bool test_create_type_utf16_1()
        {
            char data[] = {
                'f', '\0',
                'o', '\0',
                'o', '\0',
                'b', '\0',
                'a', '\0',
                'r', '\0',
                '\0', '\0'
            };
            int data_len = G_N_ELEMENTS(data);

            ustring *u1 = ustr_create_type(data, data_len, USTR_TYPE_UTF16LE);
            assert_not_null(u1);
            assert_equal(7, ustr_size(u1));
            assert_equal(6, ustr_strlen(u1));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            ustr_free(u1);

            return true;
        }

        bool test_ustr_printf1()
        {
            ustring *u1 = ustr_new();

            ustr_printf(u1, "%d", 45);

            assert_equal(std::string("45"),
                         std::string(ustr_as_utf8(u1)));

            ustr_printf(u1, "%s\n", "foobar");

            assert_equal(std::string("foobar\n"),
                         std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_ustr_printfa1()
        {
            ustring *u1 = ustr_new();

            assert_equal(1, ustr_size(u1));
            assert_equal(0, ustr_strlen(u1));

            ustr_printfa(u1, "foobar, %d", 10);

            assert_equal(11, ustr_size(u1));
            assert_equal(10, ustr_strlen(u1));

            assert_equal(std::string("foobar, 10"),
                         std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_ustr_printfa2()
        {
            ustring *u1 = ustr_new();

            assert_equal(1, ustr_size(u1));

            ustr_printfa(u1, "foobar, %d", 10);

            assert_equal(11, ustr_size(u1));

            assert_equal(std::string("foobar, 10"),
                         std::string(ustr_as_utf8(u1)));

            ustr_printfa(u1, "wheee op %s", "mighty");
            assert_equal(26, ustr_size(u1));
            assert_equal(std::string("foobar, 10wheee op mighty"),
                         std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_encode_utf8_1()
        {
            ustring *u1 = ustr_create("foobar");
            assert_not_null(u1);

            GByteArray *encod = ustr_encode(u1, USTR_TYPE_UTF8);
            assert_not_null(encod);
            assert_equal(7, encod->len);
            int encod_loc = 0;
            assert_equal('f', encod->data[encod_loc++]);
            assert_equal('o', encod->data[encod_loc++]);
            assert_equal('o', encod->data[encod_loc++]);
            assert_equal('b', encod->data[encod_loc++]);
            assert_equal('a', encod->data[encod_loc++]);
            assert_equal('r', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            g_byte_array_free(encod, TRUE);

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_encode_utf16_1()
        {
            ustring *u1 = ustr_create("foobar");
            assert_not_null(u1);

            GByteArray *encod = ustr_encode(u1, USTR_TYPE_UTF16LE);
            assert_not_null(encod);
            assert_equal(14, encod->len);
            int encod_loc = 0;
            assert_equal('f', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            assert_equal('o', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            assert_equal('o', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            assert_equal('b', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            assert_equal('a', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            assert_equal('r', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            g_byte_array_free(encod, TRUE);

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_encode_iso8859_1_1()
        {
            ustring *u1 = ustr_create("foobar");
            assert_not_null(u1);

            GByteArray *encod = ustr_encode(u1, USTR_TYPE_ISO8859_1);
            assert_not_null(encod);
            assert_equal(7, encod->len);
            int encod_loc = 0;
            assert_equal('f', encod->data[encod_loc++]);
            assert_equal('o', encod->data[encod_loc++]);
            assert_equal('o', encod->data[encod_loc++]);
            assert_equal('b', encod->data[encod_loc++]);
            assert_equal('a', encod->data[encod_loc++]);
            assert_equal('r', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            g_byte_array_free(encod, TRUE);

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_encode_ascii_1()
        {
            ustring *u1 = ustr_create("foobar");
            assert_not_null(u1);

            GByteArray *encod = ustr_encode(u1, USTR_TYPE_ASCII);
            assert_not_null(encod);
            assert_equal(7, encod->len);
            int encod_loc = 0;
            assert_equal('f', encod->data[encod_loc++]);
            assert_equal('o', encod->data[encod_loc++]);
            assert_equal('o', encod->data[encod_loc++]);
            assert_equal('b', encod->data[encod_loc++]);
            assert_equal('a', encod->data[encod_loc++]);
            assert_equal('r', encod->data[encod_loc++]);
            assert_equal('\0', encod->data[encod_loc++]);
            g_byte_array_free(encod, TRUE);

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strnset_type_ascii()
        {
            ustring *u1 = ustr_new();

            assert_true(ustr_strnset_type(u1, USTR_TYPE_ASCII,
                                          "foobar", 7));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strnset_type_iso8859()
        {
            ustring *u1 = ustr_new();

            assert_true(ustr_strnset_type(u1, USTR_TYPE_ISO8859_1,
                                          "foobar", 7));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strnset_type_utf8()
        {
            ustring *u1 = ustr_new();

            assert_true(ustr_strnset_type(u1, USTR_TYPE_UTF8,
                                          "foobar", 7));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strnset_type_utf16()
        {
            char data[] = {
                'f', '\0',
                'o', '\0',
                'o', '\0',
                'b', '\0',
                'a', '\0',
                'r', '\0',
                '\0', '\0'
            };
            int data_len = G_N_ELEMENTS(data);
            ustring *u1 = ustr_new();

            assert_true(ustr_strnset_type(u1, USTR_TYPE_UTF16LE,
                                          data, data_len));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_invalid_strnset_type_utf16()
        {
            char data[] = {
                'f', '\0',
                0x3f, 0xd8,
                0xfe, 0xdf,
                'o', '\0',
                'b', '\0',
                'a', '\0',
                'r', '\0',
                '\0', '\0'
            };
            int data_len = G_N_ELEMENTS(data);
            ustring *u1 = ustr_new();

            assert_false(ustr_strnset_type(u1, USTR_TYPE_UTF16LE,
                                          data, data_len));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strnset_type_null_utf8()
        {
            ustring *u1 = ustr_new();

            assert_true(ustr_strnset_type(u1, USTR_TYPE_UTF8,
                                          NULL, 0));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strnset1()
        {
            char data1[] = {'f', 'o', 'o', 'b', 'a', 'r'};

            ustring *u1 = ustr_new();

            assert_true(ustr_strnset(u1, data1, 6));
            assert_equal(7, ustr_size(u1));

            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strset_null1()
        {
            ustring *u1 = ustr_new();

            assert_true(ustr_strset(u1, NULL));
            assert_equal(1, ustr_size(u1));
            assert_equal(0, ustr_strlen(u1));

            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strset_null2()
        {
            ustring *u1 = ustr_create("Whee");

            assert_true(ustr_strset(u1, NULL));
            assert_equal(1, ustr_size(u1));
            assert_equal(0, ustr_strlen(u1));

            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strcat1()
        {
            ustring *u1 = ustr_new();

            assert_true(ustr_strset(u1, ""));
            assert_equal(1, ustr_size(u1));

            assert_true(ustr_strcat(u1, "blah"));
            assert_equal(5, ustr_size(u1));

            assert_true(ustr_strcat(u1, "feee"));
            assert_equal(9, ustr_size(u1));

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strcat2()
        {
            ustring *u1 = ustr_create("blah");

            assert_equal(5, ustr_size(u1));

            assert_true(ustr_strcat(u1, "blah"));
            assert_equal(9, ustr_size(u1));

            assert_true(ustr_strcat(u1, "feee"));
            assert_equal(13, ustr_size(u1));

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_charcat1()
        {
            ustring *u1 = ustr_new();

            assert_equal(1, ustr_size(u1));
            assert_equal(0, ustr_strlen(u1));

            assert_true(ustr_charcat(u1, 'p'));
            assert_equal(2, ustr_size(u1));
            assert_equal(1, ustr_strlen(u1));
            assert_equal(std::string("p"),
                         std::string(ustr_as_utf8(u1)));

            assert_true(ustr_charcat(u1, 'e'));
            assert_equal(3, ustr_size(u1));
            assert_equal(2, ustr_strlen(u1));
            assert_equal(std::string("pe"),
                         std::string(ustr_as_utf8(u1)));

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_trim1()
        {
            ustring *u1;

            u1 = ustr_create("foo");
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_front(u1, 0));
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_back(u1, 0));
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(u1)));
            assert_false(ustr_trim_front(u1, 4));
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(u1)));
            assert_false(ustr_trim_back(u1, 4));
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(u1)));

            assert_true(ustr_trim_front(u1, 1));
            assert_equal(std::string("oo"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_back(u1, 1));
            assert_equal(std::string("o"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            u1 = ustr_create("foo");
            assert_true(ustr_trim_back(u1, 3));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            u1 = ustr_create("foo");
            assert_true(ustr_trim_front(u1, 3));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_trimws1()
        {
            ustring *u1;

            u1 = ustr_create("foo");
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_front(u1));
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_back(u1));
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws(u1));
            assert_equal(std::string("foo"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_trimws2()
        {
            ustring *u1;

            u1 = ustr_create(" \tf\n ");
            assert_equal(std::string(" \tf\n "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_front(u1));
            assert_equal(std::string("f\n "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            u1 = ustr_create(" \tf\n ");
            assert_equal(std::string(" \tf\n "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_back(u1));
            assert_equal(std::string(" \tf"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            u1 = ustr_create(" \tf\n ");
            assert_equal(std::string(" \tf\n "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws(u1));
            assert_equal(std::string("f"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_trimws3()
        {
            ustring *u1;

            // only whitespace
            u1 = ustr_create(" \t\n ");
            assert_equal(std::string(" \t\n "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_front(u1));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            u1 = ustr_create(" \t\n ");
            assert_equal(std::string(" \t\n "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_back(u1));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            u1 = ustr_create(" \t\n ");
            assert_equal(std::string(" \t\n "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws(u1));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_trimws4()
        {
            ustring *u1;

            // only whitespace
            u1 = ustr_create(" ");
            assert_equal(std::string(" "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_front(u1));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            u1 = ustr_create(" ");
            assert_equal(std::string(" "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_back(u1));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            u1 = ustr_create(" ");
            assert_equal(std::string(" "),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws(u1));
            assert_equal(std::string(""),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_trimws5()
        {
            ustring *u1;

            u1 = ustr_create("f");
            assert_equal(std::string("f"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_front(u1));
            assert_equal(std::string("f"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws_back(u1));
            assert_equal(std::string("f"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_trim_ws(u1));
            assert_equal(std::string("f"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strup1()
        {
            ustring *u1;

            u1 = ustr_create("f");

            assert_equal(TRUE, ustr_strup(u1));
            assert_equal(std::string("F"),
                         std::string(ustr_as_utf8(u1)));

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strdown1()
        {
            ustring *u1;

            u1 = ustr_create("F");

            assert_equal(TRUE, ustr_strdown(u1));
            assert_equal(std::string("f"),
                         std::string(ustr_as_utf8(u1)));

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_index1()
        {
            ustring *u1;

            u1 = ustr_create("f");
            assert_equal('f', ustr_index(u1, 0));
            assert_equal('\0', ustr_index(u1, 1));
            assert_equal('\0', ustr_index(u1, 100));
            assert_equal('\0', ustr_index(u1, 1000));
            assert_equal('\0', ustr_index(u1, 10000));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_index_unsafe1()
        {
            ustring *u1;

            u1 = ustr_create("foobar");

            assert_equal('f', ustr_index_unsafe(u1, 0));
            assert_equal('o', ustr_index_unsafe(u1, 1));
            assert_equal('o', ustr_index_unsafe(u1, 2));
            assert_equal('b', ustr_index_unsafe(u1, 3));
            assert_equal('a', ustr_index_unsafe(u1, 4));
            assert_equal('r', ustr_index_unsafe(u1, 5));
            assert_equal('\0', ustr_index_unsafe(u1, 6));

            assert_true(ustr_free(u1));

            return true;
        }

        bool test_wrap1()
        {
            ustring *u1 = ustr_new();

            ustr_wrap(u1, "foobar", NULL, 80);

            assert_equal(std::string("foobar\n"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_wrap2()
        {
            ustring *u1 = ustr_new();

            ustr_wrap(u1, "foobar", NULL, 5);

            assert_equal(std::string("foobar\n"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_wrap3()
        {
            ustring *u1 = ustr_new();

            ustr_wrap(u1, "foo bar baz 111 222 333 444 555 666 777 888",
                      NULL, 30);

            assert_equal(
                std::string("foo bar baz 111 222 333 444\n555 666 777 888\n"),
                std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_wrap4(void)
        {
            ustring *u1 = ustr_new();

            ustr_wrap(u1, "foo bar baz 111 222 333 444 555 666 777 888",
                      "    ", 30);

            assert_equal(
                std::string(
                    "    foo bar baz 111 222 333\n    444 555 666 777 888\n"),
                std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_strcat_perc1(void)
        {
            ustring *u1 = ustr_new();

            ustr_strcat(u1, "%i %s");
            assert_equal(std::string("%i %s"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_printf_perc1(void)
        {
            ustring *u1 = ustr_new();

            ustr_printfa (u1, "%s\n", "%i %s");
            assert_equal(std::string("%i %s\n"),
                         std::string(ustr_as_utf8(u1)));
            assert_true(ustr_free(u1));

            return true;
        }

        bool test_alloc_count1(void)
        {
            const int count1 = 10;
            ustring *us1[count1];
            int i;

            int start_count = ustr_allocated_count();

            for (i = 0; i < count1; i++)
            {
                us1[i] = ustr_new();
                assert_equal(start_count + i + 1, ustr_allocated_count());
            }

            assert_equal(start_count + count1, ustr_allocated_count());

            for (i = 0; i < count1; i++)
            {
                ustr_free(us1[i]);
                assert_equal(start_count + count1 - 1 - i,
                             ustr_allocated_count());
            }

            assert_equal(start_count, ustr_allocated_count());

            return true;
        }

        bool test_compare1(void)
        {
            int start_count = ustr_allocated_count();
            ustring *u1 = ustr_create("aaa");
            ustring *u1_1 = ustr_create("aaa");
            ustring *u2 = ustr_create("bbb");
            ustring *u2_1 = ustr_create("bbb");
            ustring *u3 = NULL;
            ustring *u3_1 = NULL;
            ustring *u4 = ustr_create("");
            ustring *u4_1 = ustr_create("");

            assert_equal(0, ustr_compare(u1, u1_1));
            assert_equal(0, ustr_compare(u2, u2_1));
            assert_equal(0, ustr_compare(u3, u3_1));
            assert_equal(0, ustr_compare(u4, u4_1));

            assert_compare(std::greater, 0, ustr_compare(u1, u2));
            assert_compare(std::less, 0, ustr_compare(u2, u1));

            assert_compare(std::greater, 0, ustr_compare(u3, u1));
            assert_compare(std::less, 0, ustr_compare(u1, u3));

            assert_compare(std::greater, 0, ustr_compare(u3, u2));
            assert_compare(std::less, 0, ustr_compare(u2, u3));

            assert_compare(std::greater, 0, ustr_compare(u4, u1));
            assert_compare(std::less, 0, ustr_compare(u1, u4));

            assert_compare(std::greater, 0, ustr_compare(u4, u2));
            assert_compare(std::less, 0, ustr_compare(u2, u4));

            assert_compare(std::less, 0, ustr_compare(u4, u3));
            assert_compare(std::greater, 0, ustr_compare(u3, u4));

            ustr_free(u1);
            ustr_free(u1_1);
            ustr_free(u2);
            ustr_free(u2_1);
            ustr_free(u4);
            ustr_free(u4_1);

            assert_equal(start_count, ustr_allocated_count());
            return true;
        }

        bool test_casecmp1(void)
        {
            int start_count = ustr_allocated_count();
            ustring *u1 = ustr_create("aAa");
            ustring *u1_1 = ustr_create("aaa");
            ustring *u2 = ustr_create("bBb");
            ustring *u2_1 = ustr_create("bbb");
            ustring *u3 = NULL;
            ustring *u3_1 = NULL;
            ustring *u4 = ustr_create("");
            ustring *u4_1 = ustr_create("");

            assert_equal(0, ustr_casecmp(u1, u1_1));
            assert_equal(0, ustr_casecmp(u2, u2_1));
            assert_equal(0, ustr_casecmp(u3, u3_1));
            assert_equal(0, ustr_casecmp(u4, u4_1));

            assert_compare(std::greater, 0, ustr_casecmp(u1, u2));
            assert_compare(std::less, 0, ustr_casecmp(u2, u1));

            assert_compare(std::greater, 0, ustr_casecmp(u3, u1));
            assert_compare(std::less, 0, ustr_casecmp(u1, u3));

            assert_compare(std::greater, 0, ustr_casecmp(u3, u2));
            assert_compare(std::less, 0, ustr_casecmp(u2, u3));

            assert_compare(std::greater, 0, ustr_casecmp(u4, u1));
            assert_compare(std::less, 0, ustr_casecmp(u1, u4));

            assert_compare(std::greater, 0, ustr_casecmp(u4, u2));
            assert_compare(std::less, 0, ustr_casecmp(u2, u4));

            assert_compare(std::less, 0, ustr_casecmp(u4, u3));
            assert_compare(std::greater, 0, ustr_casecmp(u3, u4));

            ustr_free(u1);
            ustr_free(u1_1);
            ustr_free(u2);
            ustr_free(u2_1);
            ustr_free(u4);
            ustr_free(u4_1);

            assert_equal(start_count, ustr_allocated_count());
            return true;
        }

        bool test_compare_str1(void)
        {
            int start_count = ustr_allocated_count();
            ustring *u1 = ustr_create("aaa");
            ustring *u2 = ustr_create("bbb");
            ustring *u3 = NULL;
            ustring *u4 = ustr_create("");

            assert_equal(0, ustr_compare_str(u1, "aaa"));
            assert_equal(0, ustr_compare_str(u2, "bbb"));
            assert_equal(0, ustr_compare_str(u3, NULL));
            assert_equal(0, ustr_compare_str(u4, ""));

            assert_compare(std::greater, 0, ustr_compare_str(u1, "bbb"));
            assert_compare(std::less, 0, ustr_compare_str(u2, "aaa"));

            assert_compare(std::greater, 0, ustr_compare_str(u3, "aaa"));
            assert_compare(std::less, 0, ustr_compare_str(u1, NULL));

            assert_compare(std::greater, 0, ustr_compare_str(u3, "bbb"));
            assert_compare(std::less, 0, ustr_compare_str(u2, NULL));

            assert_compare(std::greater, 0, ustr_compare_str(u4, "aaa"));
            assert_compare(std::less, 0, ustr_compare_str(u1, ""));

            assert_compare(std::greater, 0, ustr_compare_str(u4, "bbb"));
            assert_compare(std::less, 0, ustr_compare_str(u2, ""));

            assert_compare(std::less, 0, ustr_compare_str(u4, NULL));
            assert_compare(std::greater, 0, ustr_compare_str(u3, ""));

            ustr_free(u1);
            ustr_free(u2);
            ustr_free(u4);

            assert_equal(start_count, ustr_allocated_count());
            return true;
        }

        bool test_casecmp_str1(void)
        {
            int start_count = ustr_allocated_count();
            ustring *u1 = ustr_create("aAa");
            ustring *u2 = ustr_create("bBb");
            ustring *u3 = NULL;
            ustring *u4 = ustr_create("");

            assert_equal(0, ustr_casecmp_str(u1, "aaa"));
            assert_equal(0, ustr_casecmp_str(u2, "bbb"));
            assert_equal(0, ustr_casecmp_str(u3, NULL));
            assert_equal(0, ustr_casecmp_str(u4, ""));

            assert_compare(std::greater, 0, ustr_casecmp_str(u1, "bbb"));
            assert_compare(std::less, 0, ustr_casecmp_str(u2, "aaa"));

            assert_compare(std::greater, 0, ustr_casecmp_str(u3, "aaa"));
            assert_compare(std::less, 0, ustr_casecmp_str(u1, NULL));

            assert_compare(std::greater, 0, ustr_casecmp_str(u3, "bbb"));
            assert_compare(std::less, 0, ustr_casecmp_str(u2, NULL));

            assert_compare(std::greater, 0, ustr_casecmp_str(u4, "aaa"));
            assert_compare(std::less, 0, ustr_casecmp_str(u1, ""));

            assert_compare(std::greater, 0, ustr_casecmp_str(u4, "bbb"));
            assert_compare(std::less, 0, ustr_casecmp_str(u2, ""));

            assert_compare(std::less, 0, ustr_casecmp_str(u4, NULL));
            assert_compare(std::greater, 0, ustr_casecmp_str(u3, ""));

            ustr_free(u1);
            ustr_free(u2);
            ustr_free(u4);

            assert_equal(start_count, ustr_allocated_count());
            return true;
        }

        bool test_hexstream1(void)
        {
            guint8 data[] = {
                0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
                0xD2,0x32,0x1E,0x41,0x58,0xBF,0xC3,0x01,
                0x2B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
            };

            ustring *u1 = ustr_new();

            assert_true(ustr_hexstream(u1, data, 0, G_N_ELEMENTS(data), ','));

            assert_equal(
                std::string("02,00,01,00,00,00,00,00,"
                            "D2,32,1E,41,58,BF,C3,01,"
                            "2B,00,00,00,00,00,00,00,"
                            "00,00,00,00,00,00,00,80"),
                std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_hexdump1(void)
        {
            guint8 data[] = {
                0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
                0xD2,0x32,0x1E,0x41,0x58,0xBF,0xC3,0x01,
                0x2B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
            };

            ustring *u1 = ustr_new();

            assert_true(ustr_hexdump(u1, data, 0, G_N_ELEMENTS(data), TRUE,
                                     TRUE));

            assert_equal(
                std::string("00000000:  02 00 01 00 00 00 00 00  D2 32 1E 41 58 BF C3 01  .........2.AX...\n"
                            "00000010:  2B 00 00 00 00 00 00 00  00 00 00 00 00 00 00 80  +...............\n"),
                std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_hexdump2(void)
        {
            guint8 data[] = {
                0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
                0xD2,0x32,0x1E,0x41,0x58,0xBF,0xC3,0x01,
                0x2B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,
            };

            ustring *u1 = ustr_new();

            assert_true(ustr_hexdump(u1, data, 0, G_N_ELEMENTS(data), TRUE,
                                     TRUE));

            assert_equal(
                std::string("00000000:  02 00 01 00 00 00 00 00  D2 32 1E 41 58 BF C3 01  .........2.AX...\n"
                            "00000010:  2B 00 00 00 00 00 00 00  00 00 00                 +..........\n"),
                std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_hexdump3(void)
        {
            guint8 data[] = {
                0x51,0xEA,0xEC,0x00,0x54,0xEA,0xEC,0x00,
                0x57,0xEA,0xEC,0x00,0x5B,0xEB,0xEC,0x00,
                0x5E,0xEB,0xED,0x00,0x61,0xEB,0xED,0x00,
                0x65,0xEB,0xED,0x00,0x68,0xEB,0xED,0x00,
                0x6C,0xEB,0xEE,0x00,0x6F,0xEB,0xEE,0x00,
                0x72,0xEB,0xEE,0x00,0x76,0xEB,0xEE,0x00,
            };

            ustring *u1 = ustr_new();

            assert_true(ustr_hexdump(u1, data, 0, G_N_ELEMENTS(data), TRUE,
                                     TRUE));

            assert_equal(
                std::string("00000000:  51 EA EC 00 54 EA EC 00  57 EA EC 00 5B EB EC 00  Q...T...W...[...\n"
                            "00000010:  5E EB ED 00 61 EB ED 00  65 EB ED 00 68 EB ED 00  ^...a...e...h...\n"
                            "00000020:  6C EB EE 00 6F EB EE 00  72 EB EE 00 76 EB EE 00  l...o...r...v...\n"),
                std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_hexdump4(void)
        {
            guint8 data[256];
            for (int i = 0; i < 256; i++)
            {
                data[i] = i;
            }

            ustring *u1 = ustr_new();

            assert_true(ustr_hexdump(u1, data, 0, 256, TRUE, TRUE));

            ustring *examp_string = ustr_new();

            for (int i = 0; i < 16; i++)
            {
                ustr_printfa(examp_string, "%08X:  ", 16 * i);
                for (int j = 0; j < 8; j++)
                {
                    ustr_printfa(examp_string, "%02X ", data[i * 16 + j]);
                }
                ustr_printfa(examp_string, " ");
                for (int j = 8; j < 16; j++)
                {
                    ustr_printfa(examp_string, "%02X ", data[i * 16 + j]);
                }
                ustr_printfa(examp_string, " ");
                for (int j = 0; j < 16; j++)
                {
                    guint8 c = data[i * 16 + j];
                    ustr_printfa(examp_string, "%c",
                                 isprint(c) ? (char)c : '.');
                }
                ustr_printfa(examp_string, "\n");
            }

            assert_equal(std::string(ustr_as_utf8(examp_string)),
                         std::string(ustr_as_utf8(u1)));

            ustr_free(examp_string);
            ustr_free(u1);

            return true;
        }

        bool test_hexdump_no_colon1(void)
        {
            guint8 data[] = {
                0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
                0xD2,0x32,0x1E,0x41,0x58,0xBF,0xC3,0x01,
                0x2B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
            };

            ustring *u1 = ustr_new();

            assert_true(ustr_hexdump(u1, data, 0, G_N_ELEMENTS(data), TRUE,
                                     FALSE));

            assert_equal(
                std::string("00000000   02 00 01 00 00 00 00 00  D2 32 1E 41 58 BF C3 01  .........2.AX...\n"
                            "00000010   2B 00 00 00 00 00 00 00  00 00 00 00 00 00 00 80  +...............\n"),
                std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_hexdump_no_colon2(void)
        {
            guint8 data[] = {
                0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,
                0xD2,0x32,0x1E,0x41,0x58,0xBF,0xC3,0x01,
                0x2B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                0x00,0x00,0x00,
            };

            ustring *u1 = ustr_new();

            assert_true(ustr_hexdump(u1, data, 0, G_N_ELEMENTS(data), TRUE,
                                     FALSE));

            assert_equal(
                std::string("00000000   02 00 01 00 00 00 00 00  D2 32 1E 41 58 BF C3 01  .........2.AX...\n"
                            "00000010   2B 00 00 00 00 00 00 00  00 00 00                 +..........\n"),
                std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_hexdump_no_colon3(void)
        {
            guint8 data[] = {
                0x51,0xEA,0xEC,0x00,0x54,0xEA,0xEC,0x00,
                0x57,0xEA,0xEC,0x00,0x5B,0xEB,0xEC,0x00,
                0x5E,0xEB,0xED,0x00,0x61,0xEB,0xED,0x00,
                0x65,0xEB,0xED,0x00,0x68,0xEB,0xED,0x00,
                0x6C,0xEB,0xEE,0x00,0x6F,0xEB,0xEE,0x00,
                0x72,0xEB,0xEE,0x00,0x76,0xEB,0xEE,0x00,
            };

            ustring *u1 = ustr_new();

            assert_true(ustr_hexdump(u1, data, 0, G_N_ELEMENTS(data), TRUE,
                                     FALSE));

            assert_equal(
                std::string("00000000   51 EA EC 00 54 EA EC 00  57 EA EC 00 5B EB EC 00  Q...T...W...[...\n"
                            "00000010   5E EB ED 00 61 EB ED 00  65 EB ED 00 68 EB ED 00  ^...a...e...h...\n"
                            "00000020   6C EB EE 00 6F EB EE 00  72 EB EE 00 76 EB EE 00  l...o...r...v...\n"),
                std::string(ustr_as_utf8(u1)));

            ustr_free(u1);

            return true;
        }

        bool test_hexdump_no_colon4(void)
        {
            guint8 data[256];
            for (int i = 0; i < 256; i++)
            {
                data[i] = i;
            }

            ustring *u1 = ustr_new();

            assert_true(ustr_hexdump(u1, data, 0, 256, TRUE, FALSE));

            ustring *examp_string = ustr_new();

            for (int i = 0; i < 16; i++)
            {
                ustr_printfa(examp_string, "%08X   ", 16 * i);
                for (int j = 0; j < 8; j++)
                {
                    ustr_printfa(examp_string, "%02X ", data[i * 16 + j]);
                }
                ustr_printfa(examp_string, " ");
                for (int j = 8; j < 16; j++)
                {
                    ustr_printfa(examp_string, "%02X ", data[i * 16 + j]);
                }
                ustr_printfa(examp_string, " ");
                for (int j = 0; j < 16; j++)
                {
                    guint8 c = data[i * 16 + j];
                    ustr_printfa(examp_string, "%c",
                                 isprint(c) ? (char)c : '.');
                }
                ustr_printfa(examp_string, "\n");
            }

            assert_equal(std::string(ustr_as_utf8(examp_string)),
                         std::string(ustr_as_utf8(u1)));

            ustr_free(examp_string);
            ustr_free(u1);

            return true;
        }

        bool test_regcmp_case1(void)
        {
            ustring *u1 = ustr_create("FoObAR");
            ustring *u2 = ustr_create("foobar");

            assert_equal(0, ustr_regcmp(u1, u2));

            ustr_free(u2);
            ustr_free(u1);

            return true;
        }

        struct ex_data
        {
            ustring *s1;
            ustring *s2;
            int res;

            ex_data(const char *in_s1, const char *in_s2, int in_res)
                : s1(ustr_create(in_s1)),
                  s2(ustr_create(in_s2)),
                  res(in_res)
            {
            }
        };

        bool test_regcmp_examples(void)
        {
            ex_data test_data1[] = {
                ex_data("Applets", "App Management", 1),
                ex_data("App Management", "Applets", -1),
                ex_data("*pnp0f13", "acpi#dockdevice", -1),
                ex_data(".Default", "AppGPFault", -1),
                ex_data("REALTEK Semiconductor Corp.",
                        "REALTEK Semiconductor Corporation",
                        -1),
                ex_data("symcb", "sym_hi", -1),
                ex_data("_foo", "foo", 1),

                ex_data("foobar", "foobar", 0)
            };

            guint i;
            for (i = 0; i < G_N_ELEMENTS(test_data1); i++)
            {
#define my_assert_compare_msg(func, o1, o2, msg)                        \
                assert_compare_internal<func>((o1), (o2), __FILE__, __LINE__, \
                                              std::string("assert_compare(") \
                                              + #func + ", " + #o1 + ", " \
                                              + #o2 + "): " + msg)

                if (test_data1[i].res > 0)
                {
                    my_assert_compare_msg(std::less, 0,
                                          ustr_regcmp(test_data1[i].s1,
                                                      test_data1[i].s2),
                                          ustr_as_utf8(test_data1[i].s1));
                    my_assert_compare_msg(std::greater, 0,
                                          ustr_regcmp(test_data1[i].s2,
                                                      test_data1[i].s1),
                                          ustr_as_utf8(test_data1[i].s1));
                }
                else if (test_data1[i].res < 0)
                {
                    my_assert_compare_msg(std::greater, 0,
                                          ustr_regcmp(test_data1[i].s1,
                                                      test_data1[i].s2),
                                          ustr_as_utf8(test_data1[i].s1));
                    my_assert_compare_msg(std::less, 0,
                                          ustr_regcmp(test_data1[i].s2,
                                                      test_data1[i].s1),
                                          ustr_as_utf8(test_data1[i].s1));
                }
                else
                {
                    assert_equal_msg(0,
                                     ustr_regcmp(test_data1[i].s1,
                                                 test_data1[i].s2),
                                     ustr_as_utf8(test_data1[i].s1));
                    assert_equal_msg(0,
                                     ustr_regcmp(test_data1[i].s2,
                                                 test_data1[i].s1),
                                     ustr_as_utf8(test_data1[i].s1));
                }
            }

            return true;
        }

    public:

        test_ustring(): Melunit::Test("test_ustring")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_ustring:: name)

            REGISTER(testCreateDelete_null);
            REGISTER(testCreateDelete_small1);
            REGISTER(testCreateDelete_small2);
            REGISTER(test_free_null);
            REGISTER(test_create1);
            REGISTER(test_create_type_ascii1);
            REGISTER(test_create_type_iso8859_1_1);
            REGISTER(test_create_type_utf8_1);
            REGISTER(test_create_type_utf16_1);
            REGISTER(test_ustr_printf1);
            REGISTER(test_ustr_printfa1);
            REGISTER(test_ustr_printfa2);
            REGISTER(test_encode_utf8_1);
            REGISTER(test_encode_utf16_1);
            REGISTER(test_encode_ascii_1);
            REGISTER(test_encode_iso8859_1_1);
            REGISTER(test_strnset_type_utf8);
            REGISTER(test_strnset_type_null_utf8);
            REGISTER(test_strnset_type_utf16);
            // REGISTER(test_invalid_strnset_type_utf16);
            REGISTER(test_strnset_type_ascii);
            REGISTER(test_strnset_type_iso8859);
            REGISTER(test_strnset1);
            REGISTER(test_strset_null1);
            REGISTER(test_strset_null2);
            REGISTER(test_strcat1);
            REGISTER(test_strcat2);
            REGISTER(test_charcat1);
            REGISTER(test_trim1);
            REGISTER(test_trimws1);
            REGISTER(test_trimws2);
            REGISTER(test_trimws3);
            REGISTER(test_trimws4);
            REGISTER(test_trimws5);
            REGISTER(test_strup1);
            REGISTER(test_strdown1);
            REGISTER(test_index1);
            REGISTER(test_index_unsafe1);
            REGISTER(test_wrap1);
            REGISTER(test_wrap2);
            REGISTER(test_wrap3);
            REGISTER(test_wrap4);
            REGISTER(test_strcat_perc1);
            REGISTER(test_printf_perc1);
            REGISTER(test_alloc_count1);
            REGISTER(test_compare1);
            REGISTER(test_casecmp1);
            REGISTER(test_compare_str1);
            REGISTER(test_casecmp_str1);
            REGISTER(test_hexstream1);
            REGISTER(test_hexdump1);
            REGISTER(test_hexdump2);
            REGISTER(test_hexdump3);
            REGISTER(test_hexdump4);
            REGISTER(test_hexdump_no_colon1);
            REGISTER(test_hexdump_no_colon2);
            REGISTER(test_hexdump_no_colon3);
            REGISTER(test_hexdump_no_colon4);
            REGISTER(test_regcmp_case1);
            REGISTER(test_regcmp_examples);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_ustring t1_;
}
