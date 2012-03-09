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

#ifndef RREGADMIN_TEST_LOG_LEVEL_SETTER_H
#define RREGADMIN_TEST_LOG_LEVEL_SETTER_H 1

#include <rregadmin/util/log.h>

namespace RRegAdmin
{
    namespace Test
    {
        class LogLevelSetter
        {
        public:
            LogLevelSetter(int in_ll)
            {
                last_ll = rra_log_get_level();
                rra_log_set_level(in_ll);
            }

            ~LogLevelSetter()
            {
                rra_log_set_level(last_ll);
            }


        private:
            int last_ll;
        };
    }
}

#endif // RREGADMIN_TEST_LOG_LEVEL_SETTER_H
// Local variables:
// mode: c++
// End:
