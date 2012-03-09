/*
 * Authors:     James LewisMoss <jlm@racemi.com>
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

#include <iostream>

#include <glib.h>

#include <rregadmin/basic.h>

#include "example_registry.h"

using namespace RRegAdmin::Test;

int
main(int argc, char **argv)
{
    const ExampleReg::list_type &reg_dirs =
        ExampleReg::get_list();

    for (ExampleReg::list_type::const_iterator i = reg_dirs.begin();
         i != reg_dirs.end();
         i++)
    {
        std::cout << i->second << std::endl;
    }

    return 0;
}
