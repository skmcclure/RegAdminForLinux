/*
 * Authors:	James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2008 Racemi Inc
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

#ifndef RREGADMIN_TEST_EXAMPLE_HIVE_H
#define RREGADMIN_TEST_EXAMPLE_HIVE_H 1

#include <vector>
#include <string>
#include <rregadmin/hive/types.h>

namespace RRegAdmin
{
    namespace Test
    {
        class ExampleHive
        {
        public:
            static const std::vector<std::string> get_available(void);

        public:
            ExampleHive(const std::string &in_fn, bool in_copy=true);
            ~ExampleHive();

        private:
            std::string source;
            std::string dest;
            bool copied;
            Hive *hv;

        public:

            const std::string& get_source(void);
            const std::string& get_dest(void);

            bool copy(void);

            Hive* get_hive(void);

        };
    }
}

#endif // RREGADMIN_TEST_EXAMPLE_HIVE_H
// Local variables:
// mode: c++
// End:
