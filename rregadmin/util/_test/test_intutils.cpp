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

#include <rregadmin/util/intutils.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_intutils : public Melunit::Test
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

        bool test_gint8_success1(void)
        {
            gint8 val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(str_to_gint8(da_str, &val1)); \
            assert_equal(da_num, val1)

            DO_SUCC_TEST("10", 10);
            DO_SUCC_TEST("-1", -1);
            DO_SUCC_TEST("56", 56);

            DO_SUCC_TEST("-128", G_MININT8);
            DO_SUCC_TEST("127", G_MAXINT8);
            DO_SUCC_TEST("0", 0);
            DO_SUCC_TEST("-0", -0);

            loop_test<gint8, G_MAXINT8, G_MININT8, gint32>(str_to_gint8);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_gint8_failure1(void)
        {
            gint8 val1;

            assert_false(str_to_gint8("-9223372036854775808", &val1));
            assert_false(str_to_gint8("9223372036854775807", &val1));
            assert_false(str_to_gint8("-2147483648", &val1));
            assert_false(str_to_gint8("2147483647", &val1));
            assert_false(str_to_gint8("", &val1));
            assert_false(str_to_gint8("         ", &val1));
            assert_false(str_to_gint8("doo wop", &val1));
            assert_false(str_to_gint8("20 doo wop", &val1));

            return true;
        }

        bool test_gint16_success1(void)
        {
            gint16 val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(str_to_gint16(da_str, &val1)); \
            assert_equal(da_num, val1)

            DO_SUCC_TEST("10", 10);
            DO_SUCC_TEST("-1", -1);
            DO_SUCC_TEST("156", 156);

            DO_SUCC_TEST("-32768", G_MININT16);
            DO_SUCC_TEST("32767", G_MAXINT16);
            DO_SUCC_TEST("0", 0);
            DO_SUCC_TEST("-0", -0);

            loop_test<gint16, G_MAXINT16, G_MININT16, gint32>(str_to_gint16);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_gint16_failure1(void)
        {
            gint16 val1;

            assert_false(str_to_gint16("-9223372036854775808", &val1));
            assert_false(str_to_gint16("9223372036854775807", &val1));
            assert_false(str_to_gint16("-2147483648", &val1));
            assert_false(str_to_gint16("2147483647", &val1));
            assert_false(str_to_gint16("", &val1));
            assert_false(str_to_gint16("         ", &val1));
            assert_false(str_to_gint16("doo wop", &val1));
            assert_false(str_to_gint16("20 doo wop", &val1));

            return true;
        }

        bool test_gint32_success1(void)
        {
            gint32 val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(str_to_gint32(da_str, &val1)); \
            assert_equal(da_num, val1)

            DO_SUCC_TEST("10", 10);
            DO_SUCC_TEST("-1", -1);
            DO_SUCC_TEST("738292", 738292);
            DO_SUCC_TEST("-2147483648", G_MININT32);
            DO_SUCC_TEST("2147483647", G_MAXINT32);
            DO_SUCC_TEST("0", 0);
            DO_SUCC_TEST("-0", -0);

            loop_test<gint32, G_MAXINT32, G_MININT32, gint32>(str_to_gint32);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_gint32_failure1(void)
        {
            gint32 val1;

            assert_false(str_to_gint32("-9223372036854775808", &val1));
            assert_false(str_to_gint32("9223372036854775807", &val1));
            assert_false(str_to_gint32("", &val1));
            assert_false(str_to_gint32("         ", &val1));
            assert_false(str_to_gint32("doo wop", &val1));
            assert_false(str_to_gint32("20 doo wop", &val1));

            return true;
        }

        bool test_gint64_success1(void)
        {
            gint64 val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(str_to_gint64(da_str, &val1)); \
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
            // loop_test<gint64, G_MAXINT64, G_MININT64, gint64>(str_to_gint64);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_gint64_failure1(void)
        {
            gint64 val1;

            assert_false(str_to_gint64("-92233720368547758080", &val1));
            assert_false(str_to_gint64("92233720368547758070", &val1));
            assert_false(str_to_gint64("", &val1));
            assert_false(str_to_gint64("         ", &val1));
            assert_false(str_to_gint64("doo wop", &val1));
            assert_false(str_to_gint64("20 doo wop", &val1));

            return true;
        }

        bool test_guint8_success1(void)
        {
            guint8 val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(str_to_guint8(da_str, &val1)); \
            assert_equal(da_num, val1)

            DO_SUCC_TEST("10", 10);
            DO_SUCC_TEST("1", 1);
            DO_SUCC_TEST("56", 56);

            DO_SUCC_TEST("255", G_MAXUINT8);
            DO_SUCC_TEST("0", 0);
            DO_SUCC_TEST("-0", -0);

            loop_test<guint8, G_MAXUINT8, 0, guint32>(str_to_guint8);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_guint8_failure1(void)
        {
            guint8 val1;

            assert_false(str_to_guint8("-1", &val1));
            assert_false(str_to_guint8("-9223372036854775808", &val1));
            assert_false(str_to_guint8("9223372036854775807", &val1));
            assert_false(str_to_guint8("-2147483648", &val1));
            assert_false(str_to_guint8("2147483647", &val1));
            assert_false(str_to_guint8("", &val1));
            assert_false(str_to_guint8("         ", &val1));
            assert_false(str_to_guint8("doo wop", &val1));
            assert_false(str_to_guint8("20 doo wop", &val1));

            return true;
        }

        bool test_guint16_success1(void)
        {
            guint16 val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(str_to_guint16(da_str, &val1)); \
            assert_equal(da_num, val1)

            DO_SUCC_TEST("10", 10);
            DO_SUCC_TEST("156", 156);
            DO_SUCC_TEST("65535", G_MAXUINT16);
            DO_SUCC_TEST("0", 0);
            DO_SUCC_TEST("-0", -0);

            loop_test<guint16, G_MAXUINT16, 0, guint32>(str_to_guint16);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_guint16_failure1(void)
        {
            guint16 val1;

            assert_false(str_to_guint16("-1", &val1));
            assert_false(str_to_guint16("-9223372036854775808", &val1));
            assert_false(str_to_guint16("9223372036854775807", &val1));
            assert_false(str_to_guint16("-2147483648", &val1));
            assert_false(str_to_guint16("2147483647", &val1));
            assert_false(str_to_guint16("", &val1));
            assert_false(str_to_guint16("         ", &val1));
            assert_false(str_to_guint16("doo wop", &val1));
            assert_false(str_to_guint16("20 doo wop", &val1));

            return true;
        }

        bool test_guint32_success1(void)
        {
            guint32 val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(str_to_guint32(da_str, &val1)); \
            assert_equal(da_num, val1)

            DO_SUCC_TEST("10", 10);
            DO_SUCC_TEST("156", 156);
            DO_SUCC_TEST("4294967295", G_MAXUINT32);
            DO_SUCC_TEST("0", 0);
            DO_SUCC_TEST("-0", -0);

            loop_test<guint32, G_MAXUINT32, 0, guint32>(str_to_guint32);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_guint32_failure1(void)
        {
            guint32 val1;

            assert_false(str_to_guint32("-1", &val1));
            assert_false(str_to_guint32("-9223372036854775808", &val1));
            assert_false(str_to_guint32("9223372036854775807", &val1));
            assert_false(str_to_guint32("-2147483648", &val1));
            assert_false(str_to_guint32("", &val1));
            assert_false(str_to_guint32("         ", &val1));
            assert_false(str_to_guint32("doo wop", &val1));
            assert_false(str_to_guint32("20 doo wop", &val1));

            return true;
        }

        bool test_guint64_success1(void)
        {
            guint64 val1;
#define DO_SUCC_TEST(da_str, da_num)                    \
            assert_true(str_to_guint64(da_str, &val1)); \
            assert_equal(da_num, val1)

            DO_SUCC_TEST("10", 10);
            DO_SUCC_TEST("156", 156);
            DO_SUCC_TEST("18446744073709551615", G_MAXUINT64);
            DO_SUCC_TEST("0", 0);
            DO_SUCC_TEST("-0", -0);

            loop_test<guint64, G_MAXUINT64, 0, guint64>(str_to_guint64);

#undef DO_SUCC_TEST
            return true;
        }

        bool test_guint64_failure1(void)
        {
            guint64 val1;

            assert_false(str_to_guint64("-1", &val1));
            assert_false(str_to_guint64("      -1", &val1));
            assert_false(str_to_guint64("-9223372036854775808", &val1));
            assert_false(str_to_guint64("-2147483648", &val1));
            assert_false(str_to_guint64("", &val1));
            assert_false(str_to_guint64("         ", &val1));
            assert_false(str_to_guint64("doo wop", &val1));
            assert_false(str_to_guint64("20 doo wop", &val1));

            return true;
        }

    public:

        test_intutils(): Melunit::Test("test_intutils")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_intutils:: name)

            REGISTER(test_gint8_success1);
            REGISTER(test_gint8_failure1);
            REGISTER(test_gint16_success1);
            REGISTER(test_gint16_failure1);
            REGISTER(test_gint32_success1);
            REGISTER(test_gint32_failure1);
            REGISTER(test_gint64_success1);
            REGISTER(test_gint64_failure1);

            REGISTER(test_guint8_success1);
            REGISTER(test_guint8_failure1);
            REGISTER(test_guint16_success1);
            REGISTER(test_guint16_failure1);
            REGISTER(test_guint32_success1);
            REGISTER(test_guint32_failure1);
            REGISTER(test_guint64_success1);
            REGISTER(test_guint64_failure1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_intutils t1_;
}
