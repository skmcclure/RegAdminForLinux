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

#include <boost/format.hpp>

#include <melunit/melunit-cxx.h>

#include <rregadmin/secdesc/privilege.h>
#include <rregadmin/util/init.h>

#include <rregadmin/util/flag_map.h>

namespace
{

    class test_flag_map : public Melunit::Test
    {
    private:

        bool test_single1(void)
        {
            FlagMap single1[] = {
                { "one", "1OnE1", "it's a one", 1 },
                { "two", "2TwO2", "it's a two", 2 },
                { "three", "3ThreE3", "it's a three", 3 },
                { "four", "4FouR4", "it's a four", 4 },
                { "five", "5FivE5", "it's a five", 5 },
                { NULL, NULL, NULL, 0 }
            };

            ustring *names = ustr_new();
            int i;
            for (i = 0; i < G_N_ELEMENTS(single1); i++)
            {
                if (single1[i].name == NULL)
                {
                    break;
                }

                fm_flag_type flag = single1[i].flag;
                assert_true(flag_map_flags_to_names(
                                single1, &flag, names, TRUE));
                assert_true(ustr_equal_str(names, single1[i].name));
                assert_equal(0, flag);

                ustr_clear(names);

                flag = 0;
                const char *name_str = single1[i].name;
                assert_true(flag_map_names_to_flags(single1, &name_str,
                                                    &flag, '\0', TRUE));
                assert_equal(single1[i].flag, flag);
            }

            ustr_free(names);
            return true;
        }

        struct m1_test_data
        {
            fm_flag_type flag;
            const char *names;
        };

        void two_byte_multiple_test_helper(FlagMap *in_map,
                                           struct m1_test_data *in_byte1,
                                           struct m1_test_data *in_byte2)
        {
            fm_flag_type da_flag;
            ustring *da_names = ustr_new();
            ustr_strcat(da_names, in_byte1->names);
            ustr_strcat(da_names, in_byte2->names);

            std::string error_msg = (boost::format("TD(%1%, %2%)TD(%3%, %4%)")
                                     % in_byte1->flag
                                     % in_byte1->names
                                     % in_byte2->flag
                                     % in_byte2->names).str();

            ustring *names = ustr_new();
            da_flag = in_byte1->flag + in_byte2->flag;
            assert_true_msg(flag_map_flags_to_names(in_map, &da_flag,
                                                    names, TRUE),
                            error_msg);
            assert_equal_msg(0, da_flag, error_msg);
            assert_true_msg(ustr_equal(da_names, names),
                            error_msg
                            + ustr_as_utf8(da_names)
                            + " vs "
                            + ustr_as_utf8(names));

            const char *name_str = ustr_as_utf8(da_names);
            da_flag = 0;
            assert_true_msg(flag_map_names_to_flags(in_map, &name_str,
                                                    &da_flag, '\0', TRUE),
                            error_msg);
            assert_equal_msg(in_byte1->flag + in_byte2->flag, da_flag,
                             error_msg);
            assert_equal_msg("", name_str, error_msg);

            ustr_strcat(da_names, ")");
            da_flag = 0;
            name_str = ustr_as_utf8(da_names);
            assert_true_msg(flag_map_names_to_flags(in_map, &name_str,
                                                    &da_flag, ')', TRUE),
                            error_msg);
            assert_equal_msg(in_byte1->flag + in_byte2->flag, da_flag,
                             error_msg);
            assert_equal_msg(')', name_str[0], error_msg);
            assert_equal_msg(")", name_str, error_msg);

            ustr_free(da_names);
            ustr_free(names);
        }

        bool test_multiple1(void)
        {
            FlagMap multiple_data[] = {
                { "foo", "FFooOO", "It's a foo", 0x01 },
                { "bar", "BBaaRR", "It's a bar", 0x02 },
                { "baz", "BBaaZZ", "It's a baz", 0x04 },
                { "bee", "BBeeEE", "It's a bee", 0x08 },
                { "1foo", "1FFooOO1", "It's a 1foo", 0x10 },
                { "1bar", "1BBaaRR1", "It's a 1bar", 0x20 },
                { "1baz", "1BBaaZZ1", "It's a 1baz", 0x40 },
                { "1bee", "1BBeeEE'", "It's a 1bee", 0x80 },
                { NULL, NULL, NULL, 0 }
            };

            struct m1_test_data byte1[] = {
                { 0x00, "" },
                { 0x01, "foo" },
                { 0x02, "bar" },
                { 0x03, "foobar" },
                { 0x04, "baz" },
                { 0x05, "foobaz" },
                { 0x06, "barbaz" },
                { 0x07, "foobarbaz" },
                { 0x08, "bee" },
                { 0x09, "foobee" },
                { 0x0A, "barbee" },
                { 0x0B, "foobarbee" },
                { 0x0C, "bazbee" },
                { 0x0D, "foobazbee" },
                { 0x0E, "barbazbee" },
                { 0x0F, "foobarbazbee" },
            };

            struct m1_test_data byte2[] = {
                { 0x00, "" },
                { 0x10, "1foo" },
                { 0x20, "1bar" },
                { 0x30, "1foo1bar" },
                { 0x40, "1baz" },
                { 0x50, "1foo1baz" },
                { 0x60, "1bar1baz" },
                { 0x70, "1foo1bar1baz" },
                { 0x80, "1bee" },
                { 0x90, "1foo1bee" },
                { 0xA0, "1bar1bee" },
                { 0xB0, "1foo1bar1bee" },
                { 0xC0, "1baz1bee" },
                { 0xD0, "1foo1baz1bee" },
                { 0xE0, "1bar1baz1bee" },
                { 0xF0, "1foo1bar1baz1bee" },
            };

            int ib1;
            int ib2;

            for (ib1 = 0; ib1 < 16; ib1++)
            {
                for (ib2 = 0; ib2 < 16; ib2++)
                {
                    two_byte_multiple_test_helper(
                        multiple_data, &byte1[ib1], &byte2[ib2]);
                }
            }

            // error : ending string not found.
            {
                fm_flag_type da_flag = 0;

                const char *name_str = "foobar1bar";
                assert_false(flag_map_names_to_flags(multiple_data, &name_str,
                                                     &da_flag, ')', TRUE));
                assert_equal(0, da_flag);
                assert_equal("foobar1bar", name_str);
            }

            return true;
        }

        bool test_multiple2(void)
        {
            FlagMap multiple_data[] = {
                { "whoop", "WWhhooooPP", "It's a whoop", 0x03 },
                { "foo", "FFooOO", "It's a foo", 0x01 },
                { "bar", "BBaaRR", "It's a bar", 0x02 },
                { "baz", "BBaaZZ", "It's a baz", 0x04 },
                { "bee", "BBeeEE", "It's a bee", 0x08 },
                { "1whoop", "1WWhhooooPP1", "It's a 1whoop", 0x30 },
                { "1foo", "1FFooOO1", "It's a 1foo", 0x10 },
                { "1bar", "1BBaaRR1", "It's a 1bar", 0x20 },
                { "1baz", "1BBaaZZ1", "It's a 1baz", 0x40 },
                { "1bee", "1BBeeEE'", "It's a 1bee", 0x80 },
                { NULL, NULL, NULL, 0 }
            };

            struct m1_test_data byte1[] = {
                { 0x00, "" },
                { 0x01, "foo" },
                { 0x02, "bar" },
                { 0x03, "whoop" },
                { 0x04, "baz" },
                { 0x05, "foobaz" },
                { 0x06, "barbaz" },
                { 0x07, "whoopbaz" },
                { 0x08, "bee" },
                { 0x09, "foobee" },
                { 0x0A, "barbee" },
                { 0x0B, "whoopbee" },
                { 0x0C, "bazbee" },
                { 0x0D, "foobazbee" },
                { 0x0E, "barbazbee" },
                { 0x0F, "whoopbazbee" },
            };

            struct m1_test_data byte2[] = {
                { 0x00, "" },
                { 0x10, "1foo" },
                { 0x20, "1bar" },
                { 0x30, "1whoop" },
                { 0x40, "1baz" },
                { 0x50, "1foo1baz" },
                { 0x60, "1bar1baz" },
                { 0x70, "1whoop1baz" },
                { 0x80, "1bee" },
                { 0x90, "1foo1bee" },
                { 0xA0, "1bar1bee" },
                { 0xB0, "1whoop1bee" },
                { 0xC0, "1baz1bee" },
                { 0xD0, "1foo1baz1bee" },
                { 0xE0, "1bar1baz1bee" },
                { 0xF0, "1whoop1baz1bee" },
            };

            int ib1;
            int ib2;

            for (ib1 = 0; ib1 < 16; ib1++)
            {
                for (ib2 = 0; ib2 < 16; ib2++)
                {
                    two_byte_multiple_test_helper(
                        multiple_data, &byte1[ib1], &byte2[ib2]);
                }
            }

            return true;
        }

    public:

        test_flag_map(): Melunit::Test("test_flag_map")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_flag_map:: name)

            REGISTER(test_single1);
            REGISTER(test_multiple1);
            REGISTER(test_multiple2);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_flag_map t1_;
}
