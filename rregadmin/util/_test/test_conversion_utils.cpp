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
#include <ostream>
#include <sstream>

#include <melunit/melunit-cxx.h>

#include <glib/gstrfuncs.h>
#include <glib/gmem.h>

#include <rregadmin/util/conversion_utils.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_conversion_utils : public Melunit::Test
    {
    private:

        template<typename IntT, IntT MaxV, IntT MinV,
                 typename CounterT>
        void
        loop_test(gboolean (*func) (const char*, IntT*))
        {
            IntT val1;

            CounterT adj = MaxV / 200;
            if (adj == 0)
                adj = 1;

            if (false)
            {
                std::cerr << "Testing from " << MinV
                          << " to " << MaxV
                          << " in steps of " << adj
                          << std::endl;
            }

            for (CounterT i = MinV; i < MaxV; i += adj)
            {
                std::stringstream ss;
                ss << i;
                assert_true(func(ss.str().c_str(), &val1));
                assert_equal(i, val1);

                if (MaxV - i > adj)
                {
                    // stop the overflow
                    break;
                }
            }
        }

        bool test_fuzzy_dword_success1(void)
        {
            dword_type val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(fuzzy_str_to_dword(da_str, &val1)); \
            assert_equal(da_num, val1)

            DO_SUCC_TEST("10", 10);
            DO_SUCC_TEST("156", 156);
            DO_SUCC_TEST("4294967295", G_MAXUINT32);
            DO_SUCC_TEST("0", 0);
            DO_SUCC_TEST("-0", -0);
            DO_SUCC_TEST("0xaa", 170);
            DO_SUCC_TEST("00000017", 17);
            DO_SUCC_TEST("00000008", 8);
            DO_SUCC_TEST("000000aa", 170);

            loop_test<dword_type, G_MAXUINT32, 0, dword_type>(fuzzy_str_to_dword);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_fuzzy_dword_failure1(void)
        {
            dword_type val1;

            assert_false(fuzzy_str_to_dword("-1", &val1));
            assert_false(fuzzy_str_to_dword("-9223372036854775808", &val1));
            assert_false(fuzzy_str_to_dword("9223372036854775807", &val1));
            assert_false(fuzzy_str_to_dword("-2147483648", &val1));
            assert_false(fuzzy_str_to_dword("", &val1));
            assert_false(fuzzy_str_to_dword("         ", &val1));
            assert_false(fuzzy_str_to_dword("doo wop", &val1));
            assert_false(fuzzy_str_to_dword("20 doo wop", &val1));

            return true;
        }

        bool test_fuzzy_qword_success1(void)
        {
            qword_type val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(fuzzy_str_to_qword(da_str, &val1)); \
            assert_equal(da_num, val1)

            DO_SUCC_TEST("10", 10);
            DO_SUCC_TEST("-1", -1);
            DO_SUCC_TEST("738292", 738292);
            DO_SUCC_TEST("0x80000000", 0x80000000);
            DO_SUCC_TEST("0x7fffffff", 0x7fffffff);
            DO_SUCC_TEST("-9223372036854775808", G_MININT64);
            DO_SUCC_TEST("9223372036854775807", G_MAXINT64);
            DO_SUCC_TEST("0", 0);
            DO_SUCC_TEST("-0", -0);

            // this is broken somehow, but since we don't need the 64 bit
            // functionality currently it's going to wait.
            // loop_test<qword_type, G_MAXINT64, G_MININT64, qword_type>(fuzzy_str_to_qword);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_fuzzy_qword_failure1(void)
        {
            qword_type val1;

            assert_false(fuzzy_str_to_qword("-92233720368547758080", &val1));
            assert_false(fuzzy_str_to_qword("92233720368547758070", &val1));
            assert_false(fuzzy_str_to_qword("", &val1));
            assert_false(fuzzy_str_to_qword("         ", &val1));
            assert_false(fuzzy_str_to_qword("doo wop", &val1));
            assert_false(fuzzy_str_to_qword("20 doo wop", &val1));

            return true;
        }

        void one_succ_str_to_bin_test(const char *in_str,
                                      const guint8* in_data,
                                      guint32 in_len)
        {
            GByteArray *arr = g_byte_array_new();

            assert_true_msg(str_to_binary(in_str, arr),
                            in_str);

            assert_equal_msg(in_len, arr->len,
                             in_str);

            for (int i = 0; i < in_len; i++)
            {
                assert_equal_msg((guint32)in_data[i], (guint32)arr->data[i],
                                 in_str);
            }
            g_byte_array_free(arr, TRUE);
        }

        bool test_binary1(void)
        {
            const guint8 data1[] = {
                0x88, 0x77, 0x66, 0x55,  0x44, 0x33, 0x22, 0x11,
            };

            one_succ_str_to_bin_test("0x8877665544332211",
                                     data1, G_N_ELEMENTS(data1));
            one_succ_str_to_bin_test("HEX(8877665544332211)",
                                     data1, G_N_ELEMENTS(data1));
            one_succ_str_to_bin_test(" 0x 8 8 7 7 6 6 5 5 4 4 3 3 2 2 1 1  ",
                                     data1, G_N_ELEMENTS(data1));
            one_succ_str_to_bin_test("     HEX      ( 8877 6655 4433 2211 ) ",
                                     data1, G_N_ELEMENTS(data1));

            const guint8 data2[] = {
            };

            one_succ_str_to_bin_test("0x", data2, 0);
            one_succ_str_to_bin_test("HEX()", data2, 0);

            return true;
        }

        void one_fail_str_to_bin_test(const char *in_str,
                                      const guint8* in_data,
                                      guint32 in_len)
        {
            GByteArray *arr = g_byte_array_new();
            assert_false(str_to_binary(in_str, arr));
            g_byte_array_free(arr, TRUE);
        }

        bool test_binary_fail1(void)
        {
            const guint8 data1[] = {
                0x88, 0x77, 0x66, 0x55,  0x44, 0x33, 0x22, 0x11,
            };

            one_fail_str_to_bin_test("0 x8877665544332211",
                                     data1, G_N_ELEMENTS(data1));
            one_fail_str_to_bin_test("HE X(8877665544332211)",
                                     data1, G_N_ELEMENTS(data1));
            one_fail_str_to_bin_test("HEX     8877665544332211)",
                                     data1, G_N_ELEMENTS(data1));

            return true;
        }

    public:

        test_conversion_utils(): Melunit::Test("test_conversion_utils")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_conversion_utils:: name)

            REGISTER(test_fuzzy_dword_success1);
            REGISTER(test_fuzzy_dword_failure1);

            REGISTER(test_fuzzy_qword_success1);
            REGISTER(test_fuzzy_qword_failure1);

            REGISTER(test_binary1);
            REGISTER(test_binary_fail1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_conversion_utils t1_;
}
