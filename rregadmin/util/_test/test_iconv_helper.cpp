/*
 * Authors:	James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2007 Racemi Inc
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
#include <functional>

#include <melunit/melunit-cxx.h>

#include <rregadmin/util/iconv_helper.h>
#include <rregadmin/util/init.h>

namespace
{
    class test_iconv_helper : public Melunit::Test
    {
    private:

        bool test_new1(void)
        {
            iconv_helper *h = iconv_helper_new();
            assert_not_null(h);

            iconv_t it1 = iconv_helper_get(h, "UTF-8", "UTF-8");
            assert_true(it1);

            iconv_t it2 = iconv_helper_get(h, "UTF-8", "UTF-8");
            assert_true(it2);

            assert_compare(std::not_equal_to, it1, it2);

            assert_true(iconv_helper_release(h, it1, "UTF-8", "UTF-8"));

            iconv_t it3 = iconv_helper_get(h, "UTF-8", "UTF-8");
            assert_equal(it1, it3);
            assert_compare(std::not_equal_to, it3, it2);

            assert_true(iconv_helper_release(h, it2, "UTF-8", "UTF-8"));
            assert_true(iconv_helper_release(h, it3, "UTF-8", "UTF-8"));

            assert_true(iconv_helper_free(h));

            return true;
        }

    public:

        test_iconv_helper(): Melunit::Test("test_iconv_helper")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_iconv_helper:: name)

            REGISTER(test_new1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_iconv_helper t1_;
}
