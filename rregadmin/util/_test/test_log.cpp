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

#include <melunit/melunit-cxx.h>

#include <rregadmin/util/log.h>

namespace
{
    class test_log : public Melunit::Test
    {
    private:

        int get_default_log_level(void)
        {
            if (std::string(BUILD_TYPE) == std::string("debug"))
            {
                return G_LOG_LEVEL_DEBUG;
            }
            else if (std::string(BUILD_TYPE) == std::string("test"))
            {
                return G_LOG_LEVEL_DEBUG;
            }
            else
            {
                return G_LOG_LEVEL_WARNING;
            }
        }

        bool test_comp_level1(void)
        {
            // assert_equal(rra_comp_loglevel, get_default_log_level());

            return true;
        }

        bool test_default_level1(void)
        {
            assert_equal(G_LOG_LEVEL_WARNING, rra_log_get_level());

            assert_true(rra_warning("test message 1"));
            assert_false(rra_message("test message 2"));
            assert_false(rra_info("test message 3"));
            assert_false(rra_debug("test_message 4"));

            return true;
        }

        bool test_change_level1(void)
        {
            assert_equal(G_LOG_LEVEL_WARNING, rra_log_get_level());
            assert_true(rra_log_set_level(G_LOG_LEVEL_MESSAGE));
            assert_equal(G_LOG_LEVEL_MESSAGE, rra_log_get_level());

            assert_true(rra_warning("test message 1"));
            assert_true(rra_message("test message 2"));
            assert_false(rra_info("test message 3"));
            assert_false(rra_debug("test_message 4"));

            return true;
        }

        bool test_change_level2(void)
        {
            assert_equal(G_LOG_LEVEL_MESSAGE, rra_log_get_level());
            assert_true(rra_log_set_level(G_LOG_LEVEL_INFO));
            assert_equal(G_LOG_LEVEL_INFO, rra_log_get_level());

            assert_true(rra_warning("test message 1"));
            assert_true(rra_message("test message 2"));
            assert_true(rra_info("test message 3"));
            assert_false(rra_debug("test_message 4"));

            return true;
        }

        bool test_change_level3(void)
        {
            assert_equal(G_LOG_LEVEL_INFO, rra_log_get_level());
            assert_true(rra_log_set_level(G_LOG_LEVEL_DEBUG));
            assert_equal(G_LOG_LEVEL_DEBUG, rra_log_get_level());

            if (rra_log_is_supported_level(RRA_LOG_LEVEL_WARNING))
            {
                assert_true(rra_warning("test message 1"));
            }
            if (rra_log_is_supported_level(RRA_LOG_LEVEL_MESSAGE))
            {
                assert_true(rra_message("test message 2"));
            }
            if (rra_log_is_supported_level(RRA_LOG_LEVEL_INFO))
            {
                assert_true(rra_info("test message 3"));
            }
            if (rra_log_is_supported_level(RRA_LOG_LEVEL_DEBUG))
            {
                assert_true(rra_debug("test_message 4"));
            }

            return true;
        }

    public:

        test_log(): Melunit::Test("test_log")
        {
            rra_log_initialize();

#define REGISTER(name) register_test(#name, &test_log:: name)

            REGISTER(test_comp_level1);
            REGISTER(test_default_level1);
            REGISTER(test_change_level1);
            REGISTER(test_change_level2);
            REGISTER(test_change_level3);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_log t1_;
}
