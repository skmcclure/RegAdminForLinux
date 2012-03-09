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

#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <melunit/melunit-cxx.h>

#include <rregadmin/hive/security_descriptor_cell.h>
#include <rregadmin/hive/hive.h>
#include <rregadmin/util/options.h>
#include <rregadmin/util/init.h>
#include <rregadmin/util/log.h>
#include <rregadmin/secdesc/security_descriptor.h>

#include "example_registry.h"

namespace
{
    class test_security_descriptor_cell : public Melunit::Test
    {
    private:

        bool test_get_root(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                SecurityDescriptorCell *sdc1 =
                    hive_get_first_security_descriptor_cell(hv1);
                assert_not_null(sdc1);

                assert_compare(std::less, 0,
                               security_descriptor_cell_get_use_count(sdc1));
                assert_null(security_descriptor_cell_get_prev(sdc1));

                const SecurityDescriptor *sd1 =
                    security_descriptor_cell_get_secdesc(sdc1);
                assert_not_null(sd1);
                assert_equal(SECURITY_DESCRIPTOR_REVISION_1,
                             secdesc_get_rev(sd1));

                assert_true(hive_free(hv1));
            }

            return true;
        }

        bool test_list_ends(int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                SecurityDescriptorCell *sdc1 =
                    hive_get_first_security_descriptor_cell(hv1);
                assert_not_null(sdc1);

                int count = 0;

                while (sdc1 != NULL)
                {
                    sdc1 = security_descriptor_cell_get_next(sdc1);
                    count++;

                    /* We gotta quit at some point.  There is unlikely
                       to be > 1000 secdesc cells */
                    if (count == 1000)
                    {
                        break;
                    }
                }

                assert_compare(std::greater, 1000, count);

                assert_true(hive_free(hv1));
            }

            return true;
        }

        bool test_alloc_new1 (int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                SecurityDescriptorCell *sdc1 =
                    hive_get_first_security_descriptor_cell(hv1);
                SecurityDescriptorCell *last1 = sdc1;

                if (last1 != NULL)
                {
                    while (security_descriptor_cell_get_next(last1) != NULL)
                    {
                        last1 = security_descriptor_cell_get_next(last1);
                    }
                }

                const char *sddl1 =
                    "O:AOG:DAD:(A;;RPWPCCDCLCSWRCWDWOGA;;;S-1-0-0)";

                SecurityDescriptor *sd1 = secdesc_new_parse_sddl(&sddl1);
                SecurityDescriptorCell *sdc2 =
                    security_descriptor_cell_alloc(hv1, INVALID_OFFSET, sd1);

                assert_equal(last1, security_descriptor_cell_get_prev(sdc2));
                assert_equal(security_descriptor_cell_get_next(last1), sdc2);
                assert_equal(1, security_descriptor_cell_get_use_count(sdc2));

                assert_true(hive_free(hv1));
            }

            return true;
        }

        void alloc_existing_checks(RRegAdmin::Test::ExampleReg &in_er,
                                   Hive *in_hive,
                                   SecurityDescriptorCell *in_sdc)
        {
            int in_sdc_uc = security_descriptor_cell_get_use_count(in_sdc);
            SecurityDescriptor *in_sdc_sddl =
                (SecurityDescriptor*)security_descriptor_cell_get_secdesc(
                    in_sdc);
            const char *in_sddl = secdesc_to_sddl(in_sdc_sddl);

            // Gotta create a temp one to leave sddl1 in original state.
            const char *tmp_sddl = in_sddl;
            SecurityDescriptor *sd1 = secdesc_new_parse_sddl(&tmp_sddl);

            // int prev_ll = rra_log_get_level();
            // rra_log_set_level(RRA_LOG_LEVEL_DEBUG);
            SecurityDescriptorCell *sdc2 =
                security_descriptor_cell_alloc(in_hive, INVALID_OFFSET, sd1);
            // rra_log_set_level(prev_ll);

            const char *sddl2 = secdesc_to_sddl(
                security_descriptor_cell_get_secdesc(sdc2));

            std::string err_msg =
                (boost::format("For %1% sds:\nOrig: %2%\nNew:  %3%")
                 % in_er.get_name()
                 % in_sddl
                 % sddl2).str();

            //! @todo We don't handle transforming back and forth with
            // ACLIST_REVISION_ADS ACLists, so don't test those.
            if (aclist_get_rev(secdesc_get_dacl(in_sdc_sddl))
                == ACLIST_REVISION_NT4
                && aclist_get_rev(secdesc_get_sacl(in_sdc_sddl))
                == ACLIST_REVISION_NT4)
            {
                int prev_ll = rra_log_get_level();
                rra_log_set_level(RRA_LOG_LEVEL_DEBUG);
                do_if_fail(
                    assert_true_msg(
                        secdesc_equal(
                            security_descriptor_cell_get_secdesc(in_sdc),
                            security_descriptor_cell_get_secdesc(sdc2)),
                        err_msg),
                    do
                    {
                        rra_log_set_level(prev_ll);
                        security_descriptor_cell_debug_print(in_sdc);
                    } while(0));
                rra_log_set_level(prev_ll);

                assert_equal_msg(in_sddl, sddl2, err_msg);

                assert_equal_msg(in_sdc, sdc2, err_msg);
                assert_equal_msg(in_sdc_uc + 1,
                                 security_descriptor_cell_get_use_count(sdc2),
                                 err_msg);
            }


            assert_true(secdesc_free(sd1));
        }

        bool test_alloc_existing1 (int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                SecurityDescriptorCell *sdc1 =
                    hive_get_first_security_descriptor_cell(hv1);
                alloc_existing_checks(er1, hv1, sdc1);

                assert_true(hive_free(hv1));
            }

            return true;
        }

        bool test_alloc_existing2 (int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                SecurityDescriptorCell *sdc1 =
                    hive_get_first_security_descriptor_cell(hv1);
                SecurityDescriptorCell *last1 = sdc1;

                if (last1 != NULL)
                {
                    while (security_descriptor_cell_get_next(last1) != NULL)
                    {
                        last1 = security_descriptor_cell_get_next(last1);
                    }
                }

                // OK now look back through again to see if we have a dup
                sdc1 = hive_get_first_security_descriptor_cell(hv1);
                if (sdc1 != NULL)
                {
                    while (security_descriptor_cell_get_next(sdc1) != NULL)
                    {
                        const SecurityDescriptor *sd1 =
                            security_descriptor_cell_get_secdesc(sdc1);
                        const SecurityDescriptor *sd2 =
                            security_descriptor_cell_get_secdesc(last1);

                        if (secdesc_equal(sd1, sd2))
                        {
                            last1 = sdc1;
                            break;
                        }
                        sdc1 = security_descriptor_cell_get_next(sdc1);
                    }
                }

                alloc_existing_checks(er1, hv1, last1);

                assert_true(hive_free(hv1));
            }

            return true;
        }

        bool has_earlier_secdesc(Hive *in_hive,
                                 SecurityDescriptorCell *in_sdc)
        {
            // OK now look back through again to see if we have a dup
            SecurityDescriptorCell *sdc1 =
                hive_get_first_security_descriptor_cell(in_hive);
            if (sdc1 != NULL)
            {
                while (security_descriptor_cell_get_next(sdc1) != NULL)
                {
                    if (sdc1 == in_sdc)
                    {
                        return false;
                    }

                    const SecurityDescriptor *sd1 =
                        security_descriptor_cell_get_secdesc(sdc1);
                    const SecurityDescriptor *sd2 =
                        security_descriptor_cell_get_secdesc(in_sdc);

                    if (secdesc_equal(sd1, sd2))
                    {
                        return true;
                    }
                    sdc1 = security_descriptor_cell_get_next(sdc1);
                }
            }
            return false;
        }

        bool test_alloc_existing3 (int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                SecurityDescriptorCell *last1;
                for (last1 = hive_get_first_security_descriptor_cell(hv1);
                     last1 != NULL;
                     last1 = security_descriptor_cell_get_next(last1))
                {
                    if (!has_earlier_secdesc(hv1, last1))
                    {
                        alloc_existing_checks(er1, hv1, last1);
                    }
                }
                assert_true(hive_free(hv1));
            }

            return true;
        }

        bool test_output_type (int index)
        {
            RRegAdmin::Test::ExampleReg er1(index, true);
            std::vector<std::string> hlist = er1.get_hive_list();

            for (int i = 0; i < hlist.size(); i++)
            {
                Hive *hv1 = hive_read(hlist[i].c_str());
                if (hv1 == NULL)
                    continue;

                SecurityDescriptorCell *last1;
                for (last1 = hive_get_first_security_descriptor_cell(hv1);
                     last1 != NULL;
                     last1 = security_descriptor_cell_get_next(last1))
                {
                    const SecurityDescriptor *sd =
                        security_descriptor_cell_get_secdesc(last1);
                    printf("Type: 0x%04X\n", secdesc_get_type(sd));
                }
                assert_true(hive_free(hv1));
            }

            return true;
        }

    public:

        test_security_descriptor_cell()
            : Melunit::Test("test_security_descriptor_cell")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_security_descriptor_cell:: name)
#define REG_BOOSTB(name, ehindex) \
            register_test(#name,                                        \
                          boost::bind(&test_security_descriptor_cell:: name, \
                                      this, ehindex))

            for (int i = 0; i < RRegAdmin::Test::ExampleReg::size(); i++)
            {
                REG_BOOSTB(test_get_root, i);
                REG_BOOSTB(test_list_ends, i);
                REG_BOOSTB(test_alloc_new1, i);
                REG_BOOSTB(test_alloc_existing1, i);
                REG_BOOSTB(test_alloc_existing2, i);
                REG_BOOSTB(test_alloc_existing3, i);
                // REG_BOOSTB(test_output_type, i);
            }

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
#undef REG_BOOSTB
        }

    };

    test_security_descriptor_cell t1_;
}
