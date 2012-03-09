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

#include <glib/gmem.h>

#include <rregadmin/util/offsets.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_offsets : public Melunit::Test
    {
    private:

        bool test_new_holder()
        {
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1);
            for (int i = 0; i < size1; i++)
            {
                buf1[i] = i;
            }

            offset_holder *h1 = offset_holder_new(buf1, size1);
            assert_not_null(h1);
            assert_equal(size1, offset_holder_size(h1));

            assert_true(offset_holder_free(h1));

            return true;
        }

        bool test_create_offset1()
        {
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1);
            for (int i = 0; i < size1; i++)
            {
                buf1[i] = i;
            }

            offset_holder *h1 = offset_holder_new(buf1, size1);
            assert_not_null(h1);

            const offset o1 = offset_holder_make_offset(h1, 10);
            assert_true(offset_is_valid(o1));

            assert_equal(10, offset_to_begin(o1));
            guint8 *data1 = offset_get_data(o1);
            assert_equal(10, *data1);
            assert_equal(h1, offset_get_holder(o1));

            const offset o2 = offset_holder_make_offset(h1, 10);
            assert_true(offset_is_valid(o2));

            const offset o3 = offset_holder_make_offset(h1, 100);
            assert_false(offset_is_valid(o3));

            assert_true(offset_holder_free(h1));

            return true;
        }

        bool test_create_offset2()
        {
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1);
            for (int i = 0; i < size1; i++)
            {
                buf1[i] = i;
            }

            offset_holder *h1 = offset_holder_new(buf1, size1);
            assert_not_null(h1);

            for (int i = 0; i < size1; i++)
            {
                offset_holder_make_offset(h1, i);
            }

            const offset o1 = offset_holder_make_offset(h1, 10);
            assert_true(offset_is_valid(o1));

            assert_equal(10, offset_to_begin(o1));
            guint8 *data1 = offset_get_data(o1);
            assert_equal(10, *data1);
            assert_equal(h1, offset_get_holder(o1));

            assert_true(offset_holder_free(h1));

            return true;
        }

        bool test_new_with_header()
        {
            int header_size1 = 10;
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1 + header_size1);
            for (int i = 0; i < header_size1; i++)
            {
                buf1[i] = i;
            }
            for (int i = header_size1; i < header_size1 + size1; i++)
            {
                buf1[i] = i + 40;
            }

            offset_holder *h1 =
                offset_holder_new_with_header(buf1, header_size1,
                                              size1 + header_size1);
            assert_not_null(h1);

            assert_equal(size1, offset_holder_size(h1));
            assert_equal(10, offset_holder_header_size(h1));

            guint8* header1 = offset_holder_get_header(h1);
            assert_not_null(header1);
            assert_equal(0, header1[0]);
            assert_equal(1, header1[1]);
            assert_equal(2, header1[2]);
            assert_equal(3, header1[3]);
            assert_equal(4, header1[4]);
            assert_equal(5, header1[5]);
            assert_equal(6, header1[6]);
            assert_equal(7, header1[7]);
            assert_equal(8, header1[8]);
            assert_equal(9, header1[9]);
            // beyond the end of the header
            assert_equal(50, (int)header1[10]);

            const offset o1 = offset_holder_make_offset(h1, 10);
            assert_true(offset_is_valid(o1));

            assert_equal(10, offset_to_begin(o1));
            guint8 *data1 = offset_get_data(o1);
            assert_equal(60, *data1);

            assert_true(offset_holder_free(h1));

            return true;
        }

        bool test_grow1()
        {
            int header_size1 = 10;
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1 + header_size1);
            for (int i = 0; i < header_size1; i++)
            {
                buf1[i] = i;
            }
            for (int i = header_size1; i < header_size1 + size1; i++)
            {
                buf1[i] = i + 40;
            }

            offset_holder *h1 =
                offset_holder_new_with_header(buf1, header_size1,
                                              size1 + header_size1);
            assert_not_null(h1);

            assert_equal(size1, offset_holder_size(h1));
            assert_equal(header_size1, offset_holder_header_size(h1));

            const offset ofs1 = offset_holder_make_offset(h1, 30);
            assert_equal(70, offset_to_end(ofs1));

            const offset bad_ofs1 = offset_holder_make_offset(h1, 101);
            assert_false(offset_is_valid(bad_ofs1));

            int grow_size1 = 30;
            const offset ofs2 = offset_holder_grow(h1, grow_size1);
            assert_true(offset_is_valid(ofs2));
            assert_equal(30, offset_to_end(ofs2));
            assert_equal(100, offset_to_end(ofs1));

            for (int i = 0; i < grow_size1; i++)
            {
                assert_equal(0, offset_get_data(ofs2)[i]);
                offset_get_data(ofs2)[i] = i + 100;
            }

            assert_equal(size1 + grow_size1, offset_holder_size(h1));
            assert_equal(header_size1, offset_holder_header_size(h1));

            guint8* header1 = offset_holder_get_header(h1);
            assert_not_null(header1);
            assert_equal(0, header1[0]);
            assert_equal(1, header1[1]);
            assert_equal(2, header1[2]);
            assert_equal(3, header1[3]);
            assert_equal(4, header1[4]);
            assert_equal(5, header1[5]);
            assert_equal(6, header1[6]);
            assert_equal(7, header1[7]);
            assert_equal(8, header1[8]);
            assert_equal(9, header1[9]);
            // beyond the end of the header
            assert_equal(50, (int)header1[10]);

            const offset good_ofs1 = offset_holder_make_offset(h1, 101);
            assert_true(offset_is_valid(good_ofs1));
            assert_equal(101, offset_get_data(good_ofs1)[0]);

            assert_true(offset_holder_free(h1));

            return true;
        }

        bool test_grow2()
        {
            int size1 = 0x2000;
            guint8 *buf1 = g_new0(guint8, size1);

            offset_holder *h1 = offset_holder_new_with_header(buf1,
                                                              0x1000, size1);
            assert_not_null(h1);
            assert_equal(0x1000, offset_holder_size(h1));

            const offset o1 = offset_holder_grow(h1, 0x1000);
            assert_equal(0x1000, offset_to_begin(o1));
            assert_equal(0x2000, offset_holder_size(h1));

            const offset o2 = offset_holder_grow(h1, 0x2000);
            assert_equal(0x2000, offset_to_begin(o2));
            assert_equal(0x4000, offset_holder_size(h1));

            return true;
        }

        bool test_grow3()
        {
            int size1 = 0x2000;
            guint8 *buf1 = g_new0(guint8, size1);

            offset_holder *h1 =
                offset_holder_new_with_header(buf1, 0x1000, size1);
            assert_not_null(h1);
            assert_equal(0x1000, offset_holder_size(h1));

            const offset o1 = offset_holder_grow(h1, 0);
            assert_false(offset_is_valid(o1));

            return true;
        }

        bool test_truncate1()
        {
            int header_size1 = 10;
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1 + header_size1);
            for (int i = 0; i < header_size1; i++)
            {
                buf1[i] = i;
            }
            for (int i = header_size1; i < header_size1 + size1; i++)
            {
                buf1[i] = i + 40;
            }

            offset_holder *h1 =
                offset_holder_new_with_header(buf1, header_size1,
                                              size1 + header_size1);
            assert_not_null(h1);

            assert_equal(size1, offset_holder_size(h1));
            assert_equal(header_size1, offset_holder_header_size(h1));

            const offset ofs1 = offset_holder_make_offset(h1, 99);
            assert_true(offset_is_valid(ofs1));
            assert_equal(99 + 40 + 10, offset_get_data(ofs1)[0]);

            assert_true(offset_holder_truncate(h1, 10));
            assert_equal(size1 - 10, offset_holder_size(h1));

            assert_false(offset_is_valid(ofs1));

            const offset ofs2 = offset_holder_make_offset(h1, 99);
            assert_false(offset_is_valid(ofs2));

            assert_true(offset_holder_free(h1));

            return true;
        }

        bool test_truncate2()
        {
            int header_size1 = 10;
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1 + header_size1);
            for (int i = 0; i < header_size1; i++)
            {
                buf1[i] = i;
            }
            for (int i = header_size1; i < header_size1 + size1; i++)
            {
                buf1[i] = i + 40;
            }

            offset_holder *h1 =
                offset_holder_new_with_header(buf1, header_size1,
                                              size1 + header_size1);
            assert_not_null(h1);

            assert_equal(size1, offset_holder_size(h1));
            assert_equal(header_size1, offset_holder_header_size(h1));

            const offset ofs1 = offset_holder_make_offset(h1, 99);
            assert_true(offset_is_valid(ofs1));
            assert_equal(99 + 40 + 10, offset_get_data(ofs1)[0]);

            assert_false(offset_holder_truncate(h1, 101));
            assert_true(offset_holder_truncate(h1, 100));
            assert_equal(0, offset_holder_size(h1));
            assert_equal(header_size1, offset_holder_header_size(h1));

            assert_true(offset_holder_free(h1));

            return true;
        }

        bool test_get_all_data()
        {
            int header_size1 = 10;
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1 + header_size1);
            for (int i = 0; i < header_size1; i++)
            {
                buf1[i] = i;
            }
            for (int i = header_size1; i < header_size1 + size1; i++)
            {
                buf1[i] = i + 40;
            }

            offset_holder *h1 =
                offset_holder_new_with_header(buf1, header_size1,
                                              size1 + header_size1);
            assert_not_null(h1);

            assert_equal(size1, offset_holder_size(h1));
            assert_equal(header_size1, offset_holder_header_size(h1));
            assert_equal(size1 + header_size1, offset_holder_buf_size(h1));

            guint8 *buf2 = offset_holder_get_buf(h1);
            assert_not_null(buf2);
            assert_equal(buf2, offset_holder_get_header(h1));

            assert_true(offset_holder_free(h1));

            return true;
        }

        bool test_make_relative()
        {
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1);
            for (int i = 0; i < size1; i++)
            {
                buf1[i] = i;
            }

            offset_holder *h1 = offset_holder_new(buf1, size1);
            assert_not_null(h1);

            const offset o1 = offset_holder_make_offset(h1, 10);
            assert_true(offset_is_valid(o1));

            assert_equal(10, offset_to_begin(o1));
            guint8 *data1 = offset_get_data(o1);
            assert_equal(10, *data1);
            assert_equal(h1, offset_get_holder(o1));

            const offset o2 = offset_make_relative(o1, 10);
            assert_true(offset_is_valid(o2));

            assert_equal(10, offset_diff(o1, o2));
            assert_equal(0, offset_diff(o1, o1));

            assert_equal(20, offset_to_begin(o2));
            guint8 *data2 = offset_get_data(o2);
            assert_equal(20, *data2);
            assert_equal(h1, offset_get_holder(o2));

            const offset o3 = offset_make_relative(o2, -5);
            assert_true(offset_is_valid(o3));

            assert_equal(5, offset_diff(o1, o3));
            assert_equal(-5, offset_diff(o2, o3));
            assert_equal(0, offset_diff(o2, o2));
            assert_equal(0, offset_diff(o3, o3));

            assert_equal(15, offset_to_begin(o3));
            guint8 *data3 = offset_get_data(o3);
            assert_equal(15, *data3);
            assert_equal(h1, offset_get_holder(o3));

            assert_true(offset_holder_free(h1));

            return true;
        }

        bool test_compare()
        {
            int size1 = 100;
            guint8 *buf1 = g_new0(guint8, size1);
            for (int i = 0; i < size1; i++)
            {
                buf1[i] = i;
            }

            offset_holder *h1 = offset_holder_new(buf1, size1);
            assert_not_null(h1);

            const offset o1 = offset_holder_make_offset(h1, 10);
            assert_true(offset_is_valid(o1));
            const offset o2 = offset_holder_make_offset(h1, 20);
            assert_true(offset_is_valid(o2));

            assert_true(offset_compare(o1, o2) < 0);
            assert_true(offset_compare(o2, o1) > 0);

            const offset o3 = offset_holder_make_offset(h1, 10);
            assert_true(offset_is_valid(o3));

            assert_true(offset_compare(o3, o2) < 0);
            assert_true(offset_compare(o2, o3) > 0);
            assert_equal(0, offset_compare(o1, o3));

            return true;
        }

    public:

        test_offsets(): Melunit::Test("test_offsets")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_offsets:: name)

            REGISTER(test_new_holder);
            REGISTER(test_create_offset1);
            REGISTER(test_create_offset2);
            REGISTER(test_new_with_header);
            REGISTER(test_grow1);
            REGISTER(test_grow2);
            REGISTER(test_grow3);
            REGISTER(test_truncate1);
            REGISTER(test_truncate2);
            REGISTER(test_get_all_data);
            REGISTER(test_make_relative);
            REGISTER(test_compare);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_offsets t1_;
}
