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

#include <rregadmin/util/uchar_info.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_uchar_info : public Melunit::Test
    {
    private:

        bool test_unknown(void)
        {
            assert_null(uchar_info_by_name("ZZZZZ"));
            assert_null(uchar_info_by_num(-1));
            return true;
        }

        bool test_known_name1 (void)
        {
            const struct unicode_char_info *i1 =
                uchar_info_by_name("b.alpha");
            assert_not_null(i1);
            assert_equal(std::string("b.alpha"),
                         std::string(i1->name));
            assert_equal(0x03b1, i1->num);

            return true;
        }

        bool test_known_num1 (void)
        {
            const struct unicode_char_info *i1 =
                uchar_info_by_num(0x03b1);
            assert_not_null(i1);
            assert_equal(std::string("b.alpha"),
                         std::string(i1->name));
            assert_equal(0x03b1, i1->num);

            return true;
        }

        bool test_required1 (void)
        {
            assert_not_null(uchar_info_by_num('&'));
            assert_not_null(uchar_info_by_name("amp"));
            assert_equal(uchar_info_by_num('&'),
                         uchar_info_by_name("amp"));

            assert_not_null(uchar_info_by_num('/'));
            assert_not_null(uchar_info_by_name("sol"));
            assert_equal(uchar_info_by_num('/'),
                         uchar_info_by_name("sol"));

            assert_not_null(uchar_info_by_num('\\'));
            assert_not_null(uchar_info_by_name("bsol"));
            assert_equal(uchar_info_by_num('\\'),
                         uchar_info_by_name("bsol"));

            return true;
        }

    public:

        test_uchar_info(): Melunit::Test("test_uchar_info")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_uchar_info:: name)

            REGISTER(test_unknown);
            REGISTER(test_known_name1);
            REGISTER(test_known_num1);
            REGISTER(test_required1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_uchar_info t1_;
}
