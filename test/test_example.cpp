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


#include <melunit/melunit-cxx.h>

namespace
{
    class test_example : public Melunit::Test
    {
    private:

        bool test_empty(void)
        {
            return true;
        }

    public:

        test_example(): Melunit::Test("test_example")
        {
#define REGISTER(name) register_test(#name, &test_example:: name)

            REGISTER(test_empty);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_example t1_;
}
