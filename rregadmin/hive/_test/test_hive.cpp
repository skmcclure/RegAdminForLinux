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


#include <rregadmin/config.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include <functional>

#include <melunit/melunit-cxx.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>

#include <rregadmin/hive/hive.h>
#include <rregadmin/hive/key_cell.h>
#include <rregadmin/hive/bin.h>
#include <rregadmin/hive/defines.h>
#include <rregadmin/hive/cell.h>
#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>

#include "example_hive.h"

namespace
{
    using namespace RRegAdmin::Test;

    class test_hive : public Melunit::Test
    {
    private:

        bool test_open_bad1(void)
        {
            Hive* hv1 = hive_read("/some/boguse/file/nam/blah");
            assert_null(hv1);

            // not a hive
            hv1 = hive_read("test_hive");
            assert_null(hv1);

            return true;
        }

        bool test_is_hive(void)
        {
            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                struct stat fo_stat;
                if (stat(i->c_str(), &fo_stat) == 0)
                {
                    if (fo_stat.st_size == 0)
                    {
                        assert_false(hive_is_hive(i->c_str()));
                        continue;
                    }
                }

                assert_true(hive_is_hive(i->c_str()));
            }

            assert_false(hive_is_hive("some-bogus-filename-foo"));

            return true;
        }

        bool test_new1(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            assert_not_null(hv1);
            assert_true(hive_is_valid(hv1));
            ustring *name2 = hive_get_name(hv1);
            assert_equal(std::string(ustr_as_utf8(name1)),
                         std::string(ustr_as_utf8(name2)));
            ustr_free(name1);
            ustr_free(name2);
            assert_compare(std::less_equal, (guint32)(4096 * 2),
                           hive_size(hv1));
            assert_equal(1, hive_get_version(hv1)->major);
            assert_equal(5, hive_get_version(hv1)->minor);
            assert_equal(0, hive_get_version(hv1)->release);
            assert_equal(1, hive_get_version(hv1)->build);

            Bin* bin1 = hive_get_first_bin(hv1);
            assert_not_null(bin1);

            assert_not_null(hive_get_last_bin(hv1));

            KeyCell *root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_true(key_cell_is_valid(root1));
            assert_equal(0, key_cell_get_number_of_values(root1));

            ustring *name = key_cell_get_name(root1);
            assert_not_null(name);
            assert_equal(std::string("$$$PROTO.HIV"),
                         std::string(ustr_as_utf8(name)));
            ustr_free(name);
            assert_equal(std::string("root"),
                         std::string(key_cell_get_type_str(root1)));

            hive_free(hv1);
            return true;
        }

        bool test_new2(void)
        {
            ustring *name1 = ustr_create("foobar");
            Hive *hv1 = hive_new(name1);
            assert_not_null(hv1);
            assert_true(hive_is_valid(hv1));
            ustring *name2 = hive_get_name(hv1);
            assert_equal(std::string(ustr_as_utf8(name1)),
                         std::string(ustr_as_utf8(name2)));
            ustr_free(name1);
            ustr_free(name2);
            assert_compare(std::less_equal, (guint32)(4096 * 2),
                           hive_size(hv1));
            assert_equal(1, hive_get_version(hv1)->major);
            assert_equal(5, hive_get_version(hv1)->minor);
            assert_equal(0, hive_get_version(hv1)->release);
            assert_equal(1, hive_get_version(hv1)->build);

            int max_block_size = 50 * (int)pow(2, 8) + 5;

            for (guint32 block_size = 50;
                 block_size < max_block_size;
                 block_size *= 2)
            {
                Cell* block1 = cell_alloc(hv1, INVALID_OFFSET, block_size);
                assert_not_null_msg(block1,
                                    (boost::format("block_size = %d")
                                     % block_size).str());
                assert_compare(std::less, block_size, cell_size(block1));
                assert_true(cell_is_allocd(block1));
                assert_true(cell_is_valid(block1));
                guint8 *cdata1 = cell_get_data(block1);
                for (int j = 0; j < block_size; j++)
                {
                    int tmp = cdata1[j];
                    cdata1[j] = 0;
                    cdata1[j] = tmp;
                }
            }

            hive_free(hv1);
            return true;
        }

        bool test_open_close(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();
            // This isn't true with the hive smart file name loading.
            // std::string tmp1(*i);
            // std::string tmp2(hive_get_filename(hv1));
            // boost::trim(tmp1);
            // boost::trim(tmp2);
            // assert_equal(tmp1, tmp2);

            ustring *name = hive_get_name(hv1);
            assert_not_null(name);

            // name could be many things.
            // assert_equal(std::string("SYSTEM"),
            //              std::string(ustr_as_utf8(name)));
            ustr_free(name);

            return true;
        }

        bool test_get_root_key(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            KeyCell* root1 = hive_get_root_key(hv1);
            assert_not_null(root1);
            assert_compare(
                std::less, (guint32)0,
                offset_to_begin(cell_get_offset(
                                    key_cell_to_cell(root1))));

            return true;
        }

        bool test_get_first_bin(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            Bin* page1 = hive_get_first_bin(hv1);
            assert_not_null(page1);
            assert_equal(0, offset_to_begin(bin_get_offset(page1)));

            return true;
        }

        bool test_checksum(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            assert_true(hive_is_valid(hv1));

            return true;
        }

        bool test_offsets(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            Bin* bin1 = hive_get_first_bin(hv1);
            assert_not_null(bin1);

            int current_in = 0;
            while (bin1 != NULL)
            {
                assert_equal(current_in,
                             offset_to_begin(bin_get_offset(bin1)));
                assert_equal(hive_size(hv1) - current_in
                             - HIVE_PAGE_LEN,
                             offset_to_end(bin_get_offset(bin1)));
                current_in += bin_size(bin1);
                bin1 = bin_get_next_bin(bin1);
            }

            return true;
        }

        bool test_pages(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            assert_compare(std::greater, hive_get_page_count(hv1),
                           hive_get_bin_count(hv1));

            // Have to use filename from hive since case can be
            // different.
            std::string tmp_fn(hive_get_filename(hv1));
            struct stat stat1;
            assert_equal(0, stat(tmp_fn.c_str(), &stat1));

            assert_compare(std::greater_equal,
                           (uint32_t)stat1.st_size / 0x1000,
                           hive_get_page_count(hv1));

            return true;
        }

        bool test_stats(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            struct used_stats stats;
            hive_collect_stats(hv1, &stats);

            uint32_t total =
                stats.used + stats.free + stats.data + stats.lost;
            assert_equal_msg(hive_size(hv1), total,
                             (boost::format("Difference %#x")
                              % labs(hive_size(hv1) - total)
                                 ).str());

            return true;
        }

        bool test_find_free_cell(const std::string &in_hivename)
        {
            ExampleHive eh1(in_hivename);
            Hive* hv1 = eh1.get_hive();

            Cell* block1 = hive_find_free_cell(hv1, 30);
            assert_not_null(block1);

            return true;
        }

        bool test_find_free_cell_more(const std::string &in_hivename)
        {
            int max_block_size = 50 * (int)pow(2, 8) + 5;

            for (guint32 block_size = 50;
                 block_size < max_block_size;
                 block_size *= 2)
            {
                ExampleHive eh1(in_hivename);
                Hive* hv1 = eh1.get_hive();

                Cell* block1 = cell_alloc(hv1, INVALID_OFFSET, block_size);
                assert_not_null_msg(block1,
                                    (boost::format("block_size = %d")
                                     % block_size).str());
                assert_compare(std::less, block_size, cell_size(block1));
                assert_true(cell_is_allocd(block1));
                assert_true(cell_is_valid(block1));
                guint8 *cdata1 = cell_get_data(block1);
                for (int j = 0; j < block_size; j++)
                {
                    int tmp = cdata1[j];
                    cdata1[j] = 0;
                    cdata1[j] = tmp;
                }
            }

            return true;
        }

    public:

        test_hive(): Melunit::Test("test_hive")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_hive:: name)

            REGISTER(test_open_bad1);
            REGISTER(test_new1);
            REGISTER(test_new2);

#define REG_BOOSTB(name, hive_name)             \
            register_test(#name, \
                          boost::bind(&test_hive:: name, this, \
                                      hive_name))

            std::vector<std::string> hives = ExampleHive::get_available();

            for(std::vector<std::string>::iterator i = hives.begin();
                i != hives.end();
                i++)
            {
                if (hive_is_hive(i->c_str()))
                {
                    REG_BOOSTB(test_open_close, *i);
                    REG_BOOSTB(test_get_root_key, *i);
                    REG_BOOSTB(test_get_first_bin, *i);
                    REG_BOOSTB(test_checksum, *i);
                    REG_BOOSTB(test_offsets, *i);
                    REG_BOOSTB(test_pages, *i);
                    REG_BOOSTB(test_stats, *i);
                    REG_BOOSTB(test_find_free_cell, *i);
                    REG_BOOSTB(test_find_free_cell_more, *i);
                }
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_hive t1_;
}
