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

#include <rregadmin/secdesc/secdesc_type.h>
#include <rregadmin/util/init.h>

#include "ustring_to_string.hpp"

namespace
{
    // In the current (12 March 2008) test data these are the type
    // codes that exist.
    // Type: 0x0000
    // Type: 0x0004
    // Type: 0x0404
    // Type: 0x0814
    // Type: 0x0C14
    // Type: 0x1004
    // Type: 0x1404
    // Type: 0x1814
    // Type: 0x1C14
    // Type: 0x2C04
    // Type: 0x3C04

    struct type_test_data
    {
        secdesc_type type;
        std::string type_str;

        type_test_data(secdesc_type in_type,
                       const std::string &in_type_str)
            : type(in_type),
              type_str(in_type_str)
        {
        }
    };

    type_test_data tdata1[] = {
        type_test_data(0x0000, "SecDescType()"),
        type_test_data(0x0004, "SecDescType(DACL)"),
        type_test_data(0x0404, "SecDescType(DACL(AutoInherited))"),
        type_test_data(0x0814, "SecDescType(DACL,SACL(AutoInherited))"),
        type_test_data(0x0C14, "SecDescType(DACL(AutoInherited),SACL(AutoInherited))"),
        type_test_data(0x1004, "SecDescType(DACL(Protected))"),
        type_test_data(0x1404, "SecDescType(DACL(AutoInherited,Protected))"),
        type_test_data(0x1814, "SecDescType(DACL(Protected),SACL(AutoInherited))"),
        type_test_data(0x1C14, "SecDescType(DACL(AutoInherited,Protected),SACL(AutoInherited))"),
        // These two make no sense.  The say the sacl is
        // auto-inherited and protected, but they don't say the
        // sacl is present.
        type_test_data(0x2C04, "SecDescType(DACL(AutoInherited),SACL(AutoInherited,Protected))"),
        type_test_data(0x3C04, "SecDescType(DACL(AutoInherited,Protected),SACL(AutoInherited,Protected))"),
    };

    class test_secdesc_type : public Melunit::Test
    {
    private:

        bool test_to_str(void)
        {
            for (int i = 0; i < G_N_ELEMENTS(tdata1); i++)
            {
                ustring *type_str = secdesc_type_to_ustr(tdata1[i].type);
                assert_not_null(type_str);
                assert_equal(tdata1[i].type_str, to_string(type_str));
                ustr_free(type_str);
            }

            return true;
        }

        bool test_to_type(void)
        {
            for (int i = 0; i < G_N_ELEMENTS(tdata1); i++)
            {
                ustring *type_str = ustr_create(tdata1[i].type_str.c_str());
                assert_equal(tdata1[i].type,
                             secdesc_type_from_ustr(type_str));
                ustr_free(type_str);
            }

            return true;
        }

    public:

        test_secdesc_type(): Melunit::Test("test_secdesc_type")
        {
            rra_util_init();
            rra_util_init_from_env();

#define REGISTER(name) register_test(#name, &test_secdesc_type:: name)

            REGISTER(test_to_str);
            // REGISTER(test_to_type);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_secdesc_type t1_;
}
