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


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <functional>

#include <melunit/melunit-cxx.h>

#include <boost/format.hpp>
#include <boost/bind.hpp>

#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/defines.h>

#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;

    class test_bin : public Melunit::Test
    {
    private:

        bool test_bin_list(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            Bin* bin1 = hive_get_first_bin(hv1);
            assert_not_null(bin1);

            Bin* bin2 = hive_get_first_bin(hv1);
            assert_not_null(bin2);

            assert_equal(bin1, bin2);

            while (bin1 != NULL)
            {
                do_if_fail((assert_true(bin_is_valid(bin1))),
                           (bin_debug_print(bin1, TRUE)));
                bin1 = bin_get_next_bin(bin1);
            }

            return true;
        }

        bool test_ofs_from1(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            Bin* bin1 = hive_get_first_bin(hv1);
            assert_not_null(bin1);

            uint32_t tot_offset = 0;

            while (bin1 != NULL)
            {
                assert_equal_msg(offset_to_begin(bin_get_offset(bin1)),
                                 tot_offset,
                                 (boost::format("Difference %#x")
                                  % (tot_offset
                                     - offset_to_begin(
                                         bin_get_offset(bin1)))).str());
                tot_offset += bin_size(bin1);

                bin1 = bin_get_next_bin(bin1);
            }

            return true;
        }

        bool test_offset_calcs(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            Bin* bin1 = hive_get_first_bin(hv1);
            assert_not_null(bin1);

            while (bin1 != NULL)
            {
                uint32_t tot_offset = offset_to_end(bin_get_offset(bin1))
                    + offset_to_begin(bin_get_offset(bin1))
                    // for header
                    + HIVE_PAGE_LEN;

                assert_equal_msg(hive_size(hv1), tot_offset,
                                 (boost::format("Difference %#x")
                                  % (tot_offset -
                                     hive_size(hv1))).str());
                bin1 = bin_get_next_bin(bin1);
            }

            return true;
        }

        bool test_bin_sizes(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            Bin* bin1 = hive_get_first_bin(hv1);
            while (bin1 != NULL)
            {
                assert_compare(std::greater_equal,
                               bin_size(bin1),
                               (uint32_t)0x1000);
                // AFter dealing with a few more hives these tests
                // just don't mean anything.
                if (0)
                {
                    if (hive_get_version(hv1)->minor == 5)
                    {
                        // They must have noticed that ofs_next and
                        // len_page always had the same data so in the
                        // new version of the file len_page can
                        // contain 0, ofs_next, or the number
                        // 0x10c000.  I'm betting this has some deep
                        // meaning that'll bite me at some point.
                        assert_true_msg(
                            (bin_get_len_page(bin1) == 0)
                            || (bin_get_ofs_next(bin1)
                                == bin_get_len_page(bin1))
                            || (0x10c000 == bin_get_len_page(bin1)),
                            (boost::format(
                                 "len_page (%d) vs ofs_next (%d)")
                             % bin_get_len_page(bin1)
                             % bin_get_ofs_next(bin1)).str());
                    }
                    else
                    {
                        assert_true_msg(
                            (bin_get_len_page(bin1) == 0)
                            || (bin_get_ofs_next(bin1)
                                == bin_get_len_page(bin1)),
                            (boost::format(
                                 "len_page (%d) vs ofs_next (%d)")
                             % bin_get_len_page(bin1)
                             % bin_get_ofs_next(bin1)).str());
                    }
                }

                bin1 = bin_get_next_bin(bin1);
            }

            return true;
        }

    public:

        test_bin(): Melunit::Test("test_bin")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_bin:: name)

#define REG_BOOSTB(name, hive_name) \
            register_test(#name, \
                          boost::bind(&test_bin:: name, this, \
                                      hive_name))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_bin_list, *i);
                    REG_BOOSTB(test_ofs_from1, *i);
                    REG_BOOSTB(test_offset_calcs, *i);
                    REG_BOOSTB(test_bin_sizes, *i);
                }
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_bin t1_;
}
