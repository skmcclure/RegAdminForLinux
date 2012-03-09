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

#include <time.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/util/timestamp.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_timestamp : public Melunit::Test
    {
    private:

        bool test_timet1(void)
        {
            time_t tt1 = time(NULL);

            NTTIME time1 = nttime_from_time_t(tt1, 0);

            assert_false(nttime_is_error(time1));
            assert_compare(std::less, (NTTIME)0, time1);

            time_t tt2;
            nanosecs ns2;
            assert_true(nttime_to_time_t(time1, &tt2, &ns2));

            assert_equal(tt1, tt2);

            return true;
        }

        bool test_tm1(void)
        {
            time_t da_time_t = time(NULL);
            struct tm da_time;
            assert_not_null(localtime_r(&da_time_t, &da_time));

            NTTIME time1 = nttime_from_tm(&da_time, 0);

            struct tm da_time2;
            guint32 da_nano2 = -1;
            assert_true(nttime_to_tm(time1, &da_time2, &da_nano2));

            assert_equal(0, da_nano2);
            assert_equal(da_time.tm_sec, da_time2.tm_sec);
            assert_equal(da_time.tm_min, da_time2.tm_min);
            assert_equal(da_time.tm_hour, da_time2.tm_hour);
            assert_equal(da_time.tm_mday, da_time2.tm_mday);
            assert_equal(da_time.tm_mon, da_time2.tm_mon);
            assert_equal(da_time.tm_year, da_time2.tm_year);
            // assert_equal(da_time.tm_wday, da_time2.tm_wday);
            assert_equal(da_time.tm_yday, da_time2.tm_yday);

            return true;
        }

        bool test_now(void)
        {
            time_t da_time_t = time(NULL);
            struct tm da_time;
            assert_not_null(localtime_r(&da_time_t, &da_time));

            NTTIME time1 = nttime_now();

            assert_compare(std::greater, time1, (NTTIME)0);

            assert_false(nttime_is_relative(time1));

            // All these could theoretically fail if everything is
            // timed perfectly, but I'm not going to worry about that.
            assert_equal(da_time.tm_year, nttime_year(time1));
            assert_equal(da_time.tm_mon, nttime_month(time1));
            assert_equal(da_time.tm_mday, nttime_month_day(time1));
            assert_equal(da_time.tm_yday, nttime_year_day(time1));
            assert_equal(da_time.tm_hour, nttime_hour(time1));
            assert_equal(da_time.tm_min, nttime_minute(time1));

            ustring *time_str1 = nttime_to_str(time1);
            assert_not_null(time_str1);
            // std::cerr << "Time: " << ustr_as_utf8(time_str1) << std::endl;
            ustr_free(time_str1);

            return true;
        }

        bool test_no_str1(void)
        {
            gint64 step = G_MAXINT64 / 10000;

            for (gint64 i = G_MININT64; i < G_MAXINT64 - step; i += step)
            {
                NTTIME time1 = i;
                ustring *u1 = nttime_to_str(time1);
                assert_not_null(u1);
                // std::cout << ustr_as_utf8(u1) << std::endl;
                ustr_free(u1);
            }

            return true;
        }

        bool test_error(void)
        {
            assert_true(nttime_is_error(nttime_get_error()));
            return true;
        }

    public:

        test_timestamp(): Melunit::Test("test_timestamp")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_timestamp:: name)

            REGISTER(test_timet1);
            REGISTER(test_tm1);
            REGISTER(test_now);
            REGISTER(test_no_str1);
            REGISTER(test_error);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_timestamp t1_;
}
