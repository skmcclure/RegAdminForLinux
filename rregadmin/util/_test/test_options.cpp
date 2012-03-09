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
#include <rregadmin/config.h>

#include <stdlib.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/util/options.h>
#include <rregadmin/util/log.h>

namespace
{
    const char *env_name = "RRA_UTIL_OPTIONS";

    class test_options : public Melunit::Test
    {
    private:

        int get_default_log_level(void)
        {
            return G_LOG_LEVEL_WARNING;

//          if (std::string(BUILD_TYPE) == std::string("debug"))
//             {
//              return G_LOG_LEVEL_DEBUG;
//             }
//          else if (std::string(BUILD_TYPE) == std::string("test"))
//             {
//              return G_LOG_LEVEL_INFO;
//             }
//          else
//             {
//              return G_LOG_LEVEL_WARNING;
//          }
        }

        bool test_init(void)
        {
            assert_equal(G_LOG_LEVEL_WARNING, rra_log_get_level());

            return true;
        }

        bool test_env1(void)
        {
            assert_equal(0, unsetenv(env_name));
            assert_true(rra_util_options_load_from_env(NULL));
            assert_equal(rra_log_get_level(), get_default_log_level());

            rra_util_options_reset();
            assert_equal(rra_log_get_level(), get_default_log_level());

            return true;
        }

        bool test_env2(void)
        {
            assert_equal(0, setenv(env_name,
                                   "-d", 1));
            assert_true(rra_util_options_load_from_env(NULL));
            assert_equal(G_LOG_LEVEL_DEBUG, rra_log_get_level());

            rra_util_options_reset();
            assert_equal(rra_log_get_level(), get_default_log_level());

            return true;
        }

        bool test_env3(void)
        {
            assert_equal(0, setenv(env_name, "--debug", 1));
            assert_true(rra_util_options_load_from_env(NULL));
            assert_equal(G_LOG_LEVEL_DEBUG, rra_log_get_level());

            rra_util_options_reset();
            assert_equal(rra_log_get_level(), get_default_log_level());

            assert_equal(0, setenv(env_name, "--debug", 1));
            assert_true(rra_util_options_load_from_env(NULL));
            assert_equal(G_LOG_LEVEL_DEBUG, rra_log_get_level());

            rra_util_options_reset();

            return true;
        }

    public:

        test_options(): Melunit::Test("test_options")
        {
#define REGISTER(name) register_test(#name, &test_options:: name)

            REGISTER(test_init);
            REGISTER(test_env1);
            REGISTER(test_env2);
            REGISTER(test_env3);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_options t1_;
}
