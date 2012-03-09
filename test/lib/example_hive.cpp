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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <fstream>

#include <glib.h>

#include <rregadmin/hive/hive.h>

#include "example_hive.h"
#include "example_registry.h"
#include "rra_test_consts.h"
#include "example_hive.h"

namespace RRegAdmin
{
    namespace Test
    {
        namespace
        {
            std::vector<std::string>
            get_all_hive_list(void)
            {
                std::vector<std::string> ret_val;

                for(int i = 0; i < ExampleReg::size(); i++)
                {
                    ExampleReg er(i);
                    std::vector<std::string>& group_list =
                        er.get_orig_hive_list();
                    ret_val.insert(ret_val.end(), group_list.begin(),
                                   group_list.end());
                }

                return ret_val;
            }

            std::vector<std::string>
            get_hive_list(void)
            {
                std::vector<std::string> ret;
                if (false)
                {
                    std::ifstream is1(
                        RRegAdmin::Test::single_hives_list_file.c_str());

                    while(is1)
                    {
                        std::string str1;
                        getline(is1, str1);
                        if (str1.size() > 0
                            && g_file_test(str1.c_str(), G_FILE_TEST_EXISTS))
                        {
                            ret.push_back(str1);
                        }
                    }
                }

                std::vector<std::string> ehives = get_all_hive_list();
                ret.insert(ret.end(), ehives.begin(), ehives.end());

                return ret;
            }
        }

        const std::vector<std::string>
        ExampleHive::get_available(void)
        {
            return get_hive_list();
        }

        ExampleHive::ExampleHive(const std::string &in_fn, bool in_copy)
            : source(in_fn),
              dest(""),
              copied(false),
              hv(0)
        {
            if (in_copy)
            {
                copy();
            }
        }

        ExampleHive::~ExampleHive()
        {
            if (copied)
            {
                unlink(dest.c_str());
            }

            if (hv != 0)
            {
                hive_free(hv);
            }
        }

        const std::string&
        ExampleHive::get_source(void)
        {
            return source;
        }

        const std::string&
        ExampleHive::get_dest(void)
        {
            return dest;
        }

        Hive*
        ExampleHive::get_hive(void)
        {
            if (hv == 0)
            {
                if (copied)
                {
                    hv = hive_read(dest.c_str());
                }
                else
                {
                    hv = hive_read(source.c_str());
                }
            }

            return hv;
        }

        bool
        ExampleHive::copy(void)
        {
            gchar *th_dest = g_strdup("testhive-XXXXXX");
            gint th_fd = g_mkstemp(th_dest);
            dest = th_dest;
            g_free(th_dest);

            const int buf_size = 512 * 1024;
            char buf[buf_size];

            int hive_fd = open(source.c_str(), O_RDONLY, 0);
            if (hive_fd == -1)
            {
                std::cerr << "Cannot open hive "
                          << source << std::endl;
                close(th_fd);
                return false;
            }

            while (true)
            {
                int read_count = read(hive_fd, buf, buf_size);
                if (read_count == 0)
                {
                    break;
                }
                write(th_fd, buf, read_count);
            }

            close(th_fd);
            close(hive_fd);

            copied = true;

            return true;
        }
    }
}
