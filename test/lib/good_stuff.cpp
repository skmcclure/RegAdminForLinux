/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright 2006,2008 Racemi Inc
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
#include <vector>

#include <glib.h>

#include "good_stuff.h"
#include "rra_test_consts.h"

namespace RRegadmin
{
    namespace Test
    {
        namespace
        {
            std::vector<ef_line*> good_keys;

            std::vector<ef_line*> good_vals;

            void
            regenerate_good_keys(void)
            {
                example_file *ef = ef_open(
                    RRegAdmin::Test::good_keys_file.c_str());

                if (ef == NULL)
                {
                    std::cerr << "ef_open(" << RRegAdmin::Test::good_keys_file
                              << ") failed" << std::endl;
                    return;
                }

                for (ef_line *line = ef_next(ef);
                     line != NULL;
                     line = ef_next(ef))
                {
                    good_keys.push_back(line);
                }
            }

            void
            regenerate_good_values(void)
            {
                example_file *ef = ef_open(
                    RRegAdmin::Test::good_values_file.c_str());

                if (ef == NULL)
                {
                    std::cerr << "ef_open("
                              << RRegAdmin::Test::good_values_file
                              << ") failed" << std::endl;
                    return;
                }

                for (ef_line *line = ef_next(ef);
                     line != NULL;
                     line = ef_next(ef))
                {
                    good_vals.push_back(line);
                }
            }

            GRand *rand_generator = NULL;
        }

        const ef_line*
        get_good_key(void)
        {
            if (good_keys.size() == 0)
            {
                regenerate_good_keys();
            }

            if (good_keys.size() == 0)
            {
                return NULL;
            }

            if (rand_generator == NULL)
            {
                rand_generator = g_rand_new();
            }

            int key_num = g_rand_int_range(rand_generator, 0,
                                           good_keys.size());

            return good_keys[key_num];
        }

        const ef_line*
        get_good_value(void)
        {
            if (good_vals.size() == 0)
            {
                regenerate_good_values();
            }

            if (good_vals.size() == 0)
            {
                return NULL;
            }

            if (rand_generator == NULL)
            {
                rand_generator = g_rand_new();
            }

            int val_num = g_rand_int_range(rand_generator, 0,
                                           good_vals.size());

            return good_vals[val_num];
        }

        const std::vector<ef_line*>&
        get_good_keys(void)
        {
            if (good_keys.size() == 0)
            {
                regenerate_good_keys();
            }

            return good_keys;
        }

        const std::vector<ef_line*>&
        get_good_values(void)
        {
            if (good_vals.size() == 0)
            {
                regenerate_good_values();
            }

            return good_vals;
        }
    }
}

// Local variables:
// mode: c++
// End:
