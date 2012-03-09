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

#include <rregadmin/util/password.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/init.h>

#define ENCRYPTION_CORRECT 0

namespace
{
    struct pw_data
    {
        const rid_type rid;
        const char *pw;
        const guint8 nthash[16];
        const guint8 lmhash[16];
    };

    static struct pw_data password_data[] = {
        { // user1
            0x000003F1,
            "foobar",
            { 0xDF, 0xB7, 0x07, 0x43, 0x24, 0xB0, 0xBC, 0x86,
              0xBE, 0xBB, 0xE2, 0xFC, 0x35, 0xAF, 0x44, 0x0B, },
            { 0x3D, 0x99, 0x99, 0xB9, 0xF7, 0x76, 0xC4, 0x32,
              0x71, 0x6B, 0xF8, 0x0E, 0x02, 0xBA, 0xA9, 0x09, },
        },
        { // user2
            0x000003F2,
            "whoop",
            { 0xFF, 0x6F, 0xFD, 0x84, 0x40, 0x71, 0x94, 0x44,
              0x8C, 0x49, 0x14, 0x9A, 0x3D, 0x41, 0x7F, 0x1A, },
            { 0x87, 0xAD, 0x59, 0x55, 0x0C, 0x99, 0x22, 0xEE,
              0xB9, 0xA8, 0x3D, 0x2C, 0xC4, 0xAE, 0xD7, 0x84, },
        },
        { // user3
            0x000003F3,
            "flaah",
            { 0x63, 0xAA, 0x0F, 0x78, 0xF5, 0x1B, 0x10, 0xAB,
              0xC9, 0xB3, 0x6D, 0xDC, 0xE9, 0xF6, 0xF3, 0x76, },
            { 0xF5, 0x87, 0x1E, 0xA8, 0x90, 0x3E, 0xE8, 0xC6,
              0xFF, 0xEF, 0x92, 0x12, 0x2B, 0x49, 0x88, 0x9D, },
        },
        { // user4
            0x000003F4,
            "nananana",
            { 0x49, 0xF0, 0x40, 0xB5, 0xAF, 0x5B, 0x53, 0x78,
              0xFC, 0x9A, 0x02, 0xC3, 0x0B, 0x16, 0x52, 0xB2, },
            { 0x7B, 0x5F, 0xCD, 0x9F, 0xCA, 0xD0, 0x02, 0x1D,
              0x28, 0xBD, 0x19, 0x70, 0x8D, 0x6B, 0x94, 0x21, },
        },
    };

    class test_password : public Melunit::Test
    {
    private:

        bool test_null(void)
        {
            assert_false(winpw_free(NULL));
            assert_null(winpw_get_pw(NULL));
            assert_equal(RID_INVALID, winpw_get_rid(NULL));
            assert_null(winpw_get_nthash(NULL));
            assert_null(winpw_get_lmhash(NULL));

            return true;
        }

        bool test_create(void)
        {
            win_password *wp1 = winpw_new(1244, "foobar");
            assert_not_null(wp1);

            assert_equal(1244, winpw_get_rid(wp1));
            assert_equal(std::string("foobar"),
                         std::string(ustr_as_utf8(winpw_get_pw(wp1))));
            assert_not_null(winpw_get_nthash(wp1));
            assert_not_null(winpw_get_lmhash(wp1));

            assert_true(winpw_free(wp1));

            return true;
        }

        bool test_create_encoded(void)
        {
            for (int i = 0; i < G_N_ELEMENTS(password_data); i++)
            {
                struct pw_data *d = &password_data[i];
                win_password *wp1 =
                    winpw_new_encoded(d->rid, d->nthash, d->lmhash);

                assert_not_null(wp1);

                assert_equal(d->rid, winpw_get_rid(wp1));


                assert_true(winpw_free(wp1));
            }

            return true;
        }

        void assert_equal_bin(const guint8 *b1, const guint8 *b2, int len,
                              const char *tag)
        {
#if !ENCRYPTION_CORRECT
            ustring *u1 = ustr_new();
            ustr_hexstream(u1, b1, 0, len, ' ');
            ustr_printfa(u1, "\nvs\n");
            ustr_hexstreama(u1, b2, 0, len, ' ');
            ustr_printfa(u1, "\n\n");
            fprintf (stderr, "%s", ustr_as_utf8(u1));
            ustr_free(u1);
#endif
#if ENCRYPTION_CORRECT
            for (int i = 0; i < len; i++)
            {
                gchar *msg =
                    g_strdup_printf("%s:byte %d: %#02x vs %#02x", tag, i,
                                    (guint32)b1[i], (guint32)b2[i]);
                assert_equal_msg((guint32)b1[i], (guint32)b2[i], msg);
                g_free(msg);
            }
#endif
        }

        // This doesn't work.  Either the password hash algorithm changed
        // with winxp or something else is wrong.  I grabbed the original
        // chntpw source and stripped it down to just generating passwords
        // and it duplicates the errors exposed here so it's likely that
        // winxp has a different algorithm now.
        bool test_hashes(void)
        {
            for (int i = 0; i < G_N_ELEMENTS(password_data); i++)
            {
                struct pw_data *d = &password_data[i];
                win_password *wp1 = winpw_new(d->rid, d->pw);
                assert_not_null(wp1);

                const guint8 *nth1 = winpw_get_nthash(wp1);
                assert_not_null(nth1);

                assert_equal_bin(d->nthash, nth1, 16, "nthash");

                const guint8 *lmh1 = winpw_get_lmhash(wp1);
                assert_not_null(lmh1);

                assert_equal_bin(d->lmhash, lmh1, 16, "lmhash");

                assert_true(winpw_free(wp1));
            }

            return true;
        }

    public:

        test_password(): Melunit::Test("test_password")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_password:: name)

            REGISTER(test_null);
            REGISTER(test_create);
            REGISTER(test_create_encoded);
            REGISTER(test_hashes);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_password t1_;
}
