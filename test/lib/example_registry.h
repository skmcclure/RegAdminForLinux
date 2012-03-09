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

#ifndef RREGADMIN_TEST_EXAMPLE_REGISTRY_H
#define RREGADMIN_TEST_EXAMPLE_REGISTRY_H 1

#include <vector>
#include <string>

namespace RRegAdmin
{
    namespace Test
    {
        class ExampleReg
        {
        public:

            typedef std::vector<std::pair<std::string, std::string> >
            list_type;

            static int size(void);
            static const list_type& get_list(void);

        public:
            ExampleReg(int in_index, bool in_copy=false);
            ~ExampleReg();

        private:
            std::string name;
            std::string source_dir;
            std::string copy_dir;
            std::vector<std::string> hive_list;
            std::vector<std::string> orig_hive_list;
            bool copied;

        public:

            const std::string& get_name(void);
            const std::string& get_dir(void);

            std::vector<std::string>& get_hive_list(void);
            std::vector<std::string>& get_orig_hive_list(void);

            bool copy(void);

        private:
            bool checked_system(const std::string &in_cmd);

        };
    }
}

#endif // RREGADMIN_TEST_EXAMPLE_REGISTRY_H
// Local variables:
// mode: c++
// End:
