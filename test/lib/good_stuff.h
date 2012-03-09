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

#ifndef RREGADMIN_TEST_GOOD_STUFF_H
#define RREGADMIN_TEST_GOOD_STUFF_H 1

#include <vector>

#include "example_file.h"

namespace RRegadmin
{
    namespace Test
    {
        const ef_line* get_good_key(void);

        const ef_line* get_good_value(void);

        const std::vector<ef_line*>& get_good_keys(void);
        const std::vector<ef_line*>& get_good_values(void);
    }
}

#endif // RREGADMIN_TEST_GOOD_STUFF_H
// Local variables:
// mode: c++
// End:
