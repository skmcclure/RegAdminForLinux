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

#include <stdlib.h>
#include <errno.h>

#include <iostream>
#include <fstream>

#include <boost/format.hpp>

#include <rregadmin/util/fs.h>

#include "example_registry.h"
#include "rra_test_consts.h"

namespace RRegAdmin
{
    namespace Test
    {
        namespace
        {
            class GroupNames
            {
            public:
                typedef std::pair<std::string, std::string> data_type;
                typedef std::vector<data_type> list_type;

            private:
                list_type group_list;

            public:
                GroupNames()
                {
                    char *env_val = getenv("RRA_TEST_REGISTRY_LIST");
                    if (env_val != NULL)
                    {
                        add_from_env(env_val);
                    }
                    else
                    {
                        add_from_file(RRegAdmin::Test::base_src_test_data_dir
                                      + "/registry.names");
                        add_from_file(RRegAdmin::Test::base_src_test_data_dir
                                      + "/special-registry.names");
                    }
                }

            public:
                int
                size(void)
                {
                    return group_list.size();
                }

                const list_type&
                get_list(void)
                {
                    return group_list;
                }

                const data_type&
                get_data(int in_index)
                {
                    return group_list[in_index];
                }

                void looking_for(const std::string &in_dir)
                {
                    // std::cerr << "Looking for " << in_dir << std::endl;
                }

                void found_dir(const std::string &in_dir)
                {
                    // std::cerr << "Found " << in_dir << std::endl;
                }

                bool add_dir(const std::string &in_dir_part)
                {
                    // Check src dir
                    std::string data_dir =
                        base_src_test_data_dir + "/" + in_dir_part;
                    std::string test_dir = data_dir + "/system32";
                    looking_for(test_dir);
                    if (g_file_test(test_dir.c_str(),
                                    G_FILE_TEST_IS_DIR))
                    {
                        found_dir(test_dir);
                        group_list.push_back(
                            std::make_pair(in_dir_part, data_dir));
                        return true;
                    }
                    // Check build dir
                    data_dir =
                        base_build_test_data_dir + "/" + in_dir_part;
                    test_dir = data_dir + "/system32";
                    looking_for(test_dir);
                    if (g_file_test(test_dir.c_str(),
                                    G_FILE_TEST_IS_DIR))
                    {
                        found_dir(test_dir);
                        group_list.push_back(
                            std::make_pair(in_dir_part, data_dir));
                        return true;
                    }
                    return false;
                }

                void
                add_from_env(const char *env_val)
                {
                    gchar **toks = g_strsplit_set(env_val, ", ", 0);
                    if (toks == NULL)
                    {
                        return;
                    }

                    for (int i = 0; toks[i] != 0; i++)
                    {
                        std::string data_dir;
                        data_dir += toks[i];
                        std::string test_dir =
                            base_src_test_data_dir + "/" + data_dir;
                        looking_for(test_dir);
                        if (g_file_test(test_dir.c_str(),
                                        G_FILE_TEST_IS_DIR))
                        {
                            found_dir(test_dir);
                            add_dir(data_dir);
                            continue;
                        }

                        data_dir = "registries/";
                        data_dir += toks[i];
                        test_dir = base_src_test_data_dir + "/" + data_dir;
                        looking_for(test_dir);
                        if (g_file_test(test_dir.c_str(),
                                        G_FILE_TEST_IS_DIR))
                        {
                            found_dir(test_dir);
                            add_dir(data_dir);
                            continue;
                        }

                        data_dir = "registries/example";
                        data_dir += toks[i];
                        test_dir = base_src_test_data_dir + "/" + data_dir;
                        looking_for(test_dir);
                        if (g_file_test(test_dir.c_str(),
                                        G_FILE_TEST_IS_DIR))
                        {
                            found_dir(test_dir);
                            add_dir(data_dir);
                            continue;
                        }
                    }
                }

                void
                add_from_file(const std::string &in_file)
                {
                    std::ifstream is1(in_file.c_str());

                    while(is1)
                    {
                        std::string str1;
                        getline(is1, str1);

                        if (str1.size() > 0)
                        {
                            if(!add_dir(str1))
                            {
                                // std::cerr << "Unable to add "
                                //           << str1
                                //           << std::endl;
                            }
                        }
                    }
                }
            };

            GroupNames gn;
        }

        int
        ExampleReg::size(void)
        {
            return gn.size();
        }

        const ExampleReg::list_type&
        ExampleReg::get_list(void)
        {
            return gn.get_list();
        }

        ExampleReg::ExampleReg(int in_index, bool in_copy)
        {
            GroupNames::data_type data = gn.get_data(in_index);
            name = data.first;
            source_dir = data.second;
            copied = false;
            if (in_copy)
            {
                copy();
            }
        }

        ExampleReg::~ExampleReg()
        {
            if (copied)
            {
                std::string chmod_cmd = (boost::format("chmod -R +w %1%")
                                         % copy_dir).str();
                checked_system(chmod_cmd);

                std::string rm_cmd = (boost::format("rm -rf %1%")
                                      % copy_dir).str();
                checked_system(rm_cmd);
            }
        }


        const std::string&
        ExampleReg::get_name(void)
        {
            return name;
        }

        const std::string&
        ExampleReg::get_dir(void)
        {
            return copy_dir;
        }

        std::vector<std::string>&
        ExampleReg::get_hive_list(void)
        {
            if (hive_list.size() == 0)
            {
                std::string hives_list_filename(source_dir + "/hives");
                if (g_file_test(hives_list_filename.c_str(),
                                G_FILE_TEST_IS_REGULAR))
                {
                    std::ifstream is1(hives_list_filename.c_str());

                    while(is1)
                    {
                        std::string str1;
                        getline(is1, str1);
                        if (str1.size() > 0)
                        {
                            std::string hive_path = (boost::format("%1%/%2%")
                                                     % copy_dir
                                                     % str1).str();
                            hive_list.push_back(hive_path);
                        }
                    }
                }
                else
                {
                    std::string start_path = copy_dir + "/system32/config/";

                    hive_list.push_back(start_path + "default");
                    hive_list.push_back(start_path + "sam");
                    hive_list.push_back(start_path + "security");
                    hive_list.push_back(start_path + "software");
                    hive_list.push_back(start_path + "system");
                    hive_list.push_back(start_path + "userdiff");
                    hive_list.push_back(start_path
                                        + "systemprofile/ntuser.dat");
                }

            }

            return hive_list;
        }

        std::vector<std::string>&
        ExampleReg::get_orig_hive_list(void)
        {
            if (orig_hive_list.size() == 0)
            {
                std::string hives_list_filename(source_dir + "/hives");
                std::ifstream is1(hives_list_filename.c_str());

                while(is1)
                {
                    std::string str1;
                    getline(is1, str1);
                    if (str1.size() > 0)
                    {
                        std::string hive_path = (boost::format("%1%/%2%")
                                                 % source_dir
                                                 % str1).str();
                        orig_hive_list.push_back(hive_path);
                    }
                }
            }

            return orig_hive_list;
        }

        bool
        ExampleReg::copy(void)
        {
            gchar *target_dir = rra_mkdtemp("test-reg");
            copy_dir = target_dir;
            g_free(target_dir);

            char *cwd = getcwd(0, 0);

            std::string cp_cmd =
                (boost::format("cd %1% ; tar chf - ."
                               "| ( cd %2%/%3% ; tar xf - )")
                 % source_dir
                 % cwd
                 % copy_dir).str();
            checked_system(cp_cmd);

            std::string chmod_cmd = (boost::format("chmod -R +w %1%")
                                     % copy_dir).str();
            checked_system(chmod_cmd);

            copied = true;
            return true;
        }

        bool
        ExampleReg::checked_system(const std::string &in_cmd)
        {
            int sys_ret = system(in_cmd.c_str());

            if (sys_ret == -1)
            {
                std::cerr << "'system' call of "
                          << in_cmd
                          << " failed: "
                          << strerror(errno)
                          << std::endl;
                return false;
            }

            if (WEXITSTATUS(sys_ret) != 0)
            {
                std::cerr << "Call of "
                          << in_cmd
                          << " failed with return value "
                          << WEXITSTATUS(sys_ret)
                          << std::endl;
                return false;
            }
            return true;
        }

    }
}

// Local variables:
// mode: c++
// End:
