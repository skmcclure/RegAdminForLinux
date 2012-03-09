/*
 * Authors:	James Lewismoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi
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

#include <sys/time.h>
#include <time.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/timestamp.h>

static const NTTIME nttime_error = G_MININT64;
static const NTTIME nttime_largest = G_MAXINT64;

#define nano100_per_microsecond (10000ll)
#define nano100_per_second (1000ll * nano100_per_microsecond)
#define nano100_per_minute (60ll * nano100_per_second)
#define nano100_per_hour (60ll * nano100_per_minute)
#define nano100_per_day (60ll * nano100_per_hour)
#define nano100_per_year (365ll * nano100_per_day)

/*
 * I grabbed this constant and the time_t conversion functions from the
 * samba source.
 */
#define TIME_FIXUP_CONSTANT 11644473600LL
#ifndef TIME_T_MIN
/* we use 0 here, because (time_t)-1 means error */
#define TIME_T_MIN 0
#endif

/*
 * we use the INT32_MAX here as on 64 bit systems,
 * gmtime() fails with INT64_MAX
 */
#ifndef TIME_T_MAX
#define TIME_T_MAX G_MAXINT32
#endif


static int
count_leap_years(const struct tm *in_tm)
{
    int last_leapy = in_tm->tm_year - 1 - (in_tm->tm_year % 4);

    // This isn't correct, but it's close enough for government work.
    return (last_leapy + 1 - 1601) / 4;
}

static NTTIME
get_year_nanos(const struct tm *in_tm)
{
    if (in_tm->tm_year < 1601)
    {
        return nttime_error;
    }

    int year_count = 1900 + in_tm->tm_year - 1601;

    NTTIME ret_val = year_count * nano100_per_year;

    ret_val += count_leap_years(in_tm) * nano100_per_day;

    return ret_val;
}

static NTTIME
get_day_of_year_nanos(const struct tm *in_tm)
{
    return in_tm->tm_yday * nano100_per_day;
}

static NTTIME
get_hour_nanos(const struct tm *in_tm)
{
    return in_tm->tm_hour * nano100_per_hour;
}

static NTTIME
get_minute_nanos(const struct tm *in_tm)
{
    return in_tm->tm_hour * nano100_per_minute;
}

static NTTIME
get_second_nanos(const struct tm *in_tm)
{
    return in_tm->tm_hour * nano100_per_second;
}

NTTIME
nttime_from_tm(const struct tm *in_tm, nanosecs in_ns)
{
    if (in_tm == NULL)
    {
        return nttime_now();
    }

    /* Short circuit this thing if possible */
    NTTIME ret_val = nttime_from_time_t(mktime((struct tm*)in_tm), in_ns);
    if (ret_val != nttime_error)
    {
        return ret_val;
    }
    else
    {
        /* This doesn't work yet */
        ret_val = 0;

        NTTIME years_nanos = get_year_nanos(in_tm);
        if (years_nanos == nttime_error)
        {
            return nttime_error;
        }
        ret_val += years_nanos;

        ret_val += get_day_of_year_nanos(in_tm);
        ret_val += get_hour_nanos(in_tm);
        ret_val += get_minute_nanos(in_tm);
        ret_val += get_second_nanos(in_tm);

        ret_val += in_ns;

        return ret_val;
    }
}

NTTIME
nttime_from_time_t(time_t in_tt, nanosecs in_ns)
{
    NTTIME ret_val;

    if (in_tt == (time_t)-1)
    {
        return nttime_error;
    }
    if (in_tt == 0)
    {
        return nttime_error;
    }

    ret_val = in_tt;
    ret_val += TIME_FIXUP_CONSTANT;
    ret_val *= 1000*1000*10;

    ret_val += in_ns;

    return ret_val;
}

NTTIME
nttime_now(void)
{
    return nttime_from_time_t(time(NULL), 0);
}

NTTIME
nttime_get_error(void)
{
    return nttime_error;
}

NTTIME
nttime_get_largest_date(void)
{
    return nttime_largest;
}

gboolean
nttime_to_tm(NTTIME in_time, struct tm *out_tm, nanosecs *out_nano)
{
    time_t my_tt;

    if (nttime_to_time_t(in_time, &my_tt, out_nano))
    {
        if (localtime_r(&my_tt, out_tm) == NULL)
        {
            return FALSE;
        }
        return TRUE;
    }

    return FALSE;
}

gboolean
nttime_to_time_t(NTTIME in_time, time_t *out_tt, nanosecs *out_nano)
{
    if (in_time == 0)
    {
        return FALSE;
    }

    if (in_time == -1LL)
    {
        return FALSE;
    }

    in_time += 1000*1000*10/2;
    in_time /= 1000*1000*10;
    in_time -= TIME_FIXUP_CONSTANT;

    if (TIME_T_MIN > in_time || in_time > TIME_T_MAX)
    {
        return FALSE;
    }

    *out_tt = in_time;
    *out_nano = 0;

    return TRUE;
}

ustring*
nttime_to_str(NTTIME in_time)
{
    struct tm my_tm;
    guint32 my_nanos;
    ustring *ret_val = ustr_new();

    if (!nttime_to_tm(in_time, &my_tm, &my_nanos))
    {
        ustr_printf(ret_val, _("unable to represent time: %#"
                               G_GINT64_MODIFIER "x"),
                    in_time);
    }
    else
    {
        ustr_strset(ret_val, asctime(&my_tm));
        ustr_trim_ws(ret_val);
    }
    return ret_val;
}

gboolean
nttime_is_error(NTTIME in_time)
{
    return in_time == nttime_error;
}

gboolean
nttime_is_relative(NTTIME in_time)
{
    return (in_time < 0);
}

int
nttime_year(NTTIME in_time)
{
    struct tm my_tm;
    guint32 my_nanos;
    if (!nttime_to_tm(in_time, &my_tm, &my_nanos))
    {
        return 0;
    }

    return my_tm.tm_year;
}

int
nttime_month(NTTIME in_time)
{
    struct tm my_tm;
    guint32 my_nanos;
    if (!nttime_to_tm(in_time, &my_tm, &my_nanos))
    {
        return 0;
    }

    return my_tm.tm_mon;
}

int
nttime_month_day(NTTIME in_time)
{
    struct tm my_tm;
    guint32 my_nanos;
    if (!nttime_to_tm(in_time, &my_tm, &my_nanos))
    {
        return 0;
    }

    return my_tm.tm_mday;
}

int
nttime_year_day(NTTIME in_time)
{
    struct tm my_tm;
    guint32 my_nanos;
    if (!nttime_to_tm(in_time, &my_tm, &my_nanos))
    {
        return 0;
    }

    return my_tm.tm_yday;
}

int
nttime_hour(NTTIME in_time)
{
    struct tm my_tm;
    guint32 my_nanos;
    if (!nttime_to_tm(in_time, &my_tm, &my_nanos))
    {
        return 0;
    }

    return my_tm.tm_hour;
}

int
nttime_minute(NTTIME in_time)
{
    struct tm my_tm;
    guint32 my_nanos;
    if (!nttime_to_tm(in_time, &my_tm, &my_nanos))
    {
        return 0;
    }

    return my_tm.tm_min;
}

int
nttime_second(NTTIME in_time)
{
    struct tm my_tm;
    guint32 my_nanos;
    if (!nttime_to_tm(in_time, &my_tm, &my_nanos))
    {
        return 0;
    }

    return my_tm.tm_sec;
}

nanosecs
nttime_nanosecond(NTTIME in_time)
{
    struct tm my_tm;
    guint32 my_nanos;
    if (!nttime_to_tm(in_time, &my_tm, &my_nanos))
    {
        return 0;
    }

    return my_nanos;
}
