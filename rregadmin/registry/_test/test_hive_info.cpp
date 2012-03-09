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

#include <boost/algorithm/string.hpp>

#include <melunit/melunit-cxx.h>

#include <rregadmin/registry/hive_info.h>
#include <rregadmin/registry/init.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;
    namespace ba = boost::algorithm;

    class test_hive_info : public Melunit::Test
    {
    private:

        bool test_fail1(void)
        {
            hive_info *hi1 = hive_info_new("/some/bogus/file/name/blah");
            assert_null(hi1);

            // not a hive
            hi1 = hive_info_new("test_hive_info");
            assert_null(hi1);

            assert_false(hive_info_free(NULL));

            return true;
        }

        bool test_hives1(void)
        {
            std::vector<std::string> hlist = ExampleHive::get_available();

            for (int i = 0; i < hlist.size(); i++)
            {
                hive_info *hi1 = hive_info_new(hlist[i].c_str());
                if (hi1 == NULL)
                    continue;

                assert_equal(hlist[i].c_str(),
                             std::string(hive_info_get_filename(hi1)));
                assert_equal(-1, hive_info_get_id(hi1));

                if (ba::iends_with(hlist[i], "SAM"))
                {
                    assert_equal(HTYPE_SAM,
                                 hive_info_get_type(hi1));
                }
                else if (ba::iends_with(hlist[i], "SECURITY"))
                {
                    assert_equal(HTYPE_SECURITY,
                                 hive_info_get_type(hi1));
                }
                else if (ba::iends_with(hlist[i], "default"))
                {
                    assert_equal(HTYPE_DEFAULT,
                                 hive_info_get_type(hi1));
                }
                else if (ba::iends_with(hlist[i], "ntuser.dat"))
                {
                    assert_equal(HTYPE_USER,
                                 hive_info_get_type(hi1));
                }
                else if (ba::iends_with(hlist[i], "software"))
                {
                    assert_equal(HTYPE_SOFTWARE,
                                 hive_info_get_type(hi1));
                }
                else if (ba::iends_with(hlist[i], "system"))
                {
                    assert_equal(HTYPE_SYSTEM,
                                 hive_info_get_type(hi1));
                }
                else if (ba::iends_with(hlist[i], "userdiff"))
                {
                    assert_equal(HTYPE_USERDIFF,
                                 hive_info_get_type(hi1));
                }
                else
                {
                    assert_equal(HTYPE_SYSTEM,
                                 hive_info_get_type(hi1));
                }

                assert_true(hive_info_unload(hi1, FALSE));
                assert_false(hive_info_is_valid(hi1));
                assert_null(hive_info_get_hive(hi1));
                assert_null(hive_info_get_filename(hi1));
                assert_equal(HTYPE_UNLOADED, hive_info_get_id(hi1));

                assert_true(hive_info_free(hi1));
            }
            return true;
        }

    public:

        test_hive_info(): Melunit::Test("test_hive_info")
        {
            rra_registry_init();
            rra_registry_init_from_env();

#define REGISTER(name) register_test(#name, &test_hive_info:: name)

            REGISTER(test_fail1);
            REGISTER(test_hives1);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_hive_info t1_;
}
