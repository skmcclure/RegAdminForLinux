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
#include <stdlib.h>

#include <melunit/melunit-cxx.h>

#include <rregadmin/cli/options.h>

namespace
{
    const char *env_name = "RRA_CLI_OPTIONS";

    class test_options : public Melunit::Test
    {
    private:

        bool test_init(void)
        {
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            return true;
        }

        bool test_env1(void)
        {
            assert_equal(0, unsetenv(env_name));
            assert_true(rra_cli_options_load_from_env(NULL));
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            rra_cli_options_reset();
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            return true;
        }

        bool test_env2(void)
        {
            assert_equal(0, setenv(env_name, "--hive-check-read", 1));
            assert_true(rra_cli_options_load_from_env(NULL));
            assert_true(rra_cli_options_should_make_backup());
            assert_true(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            rra_cli_options_reset();
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            return true;
        }

        bool test_env3(void)
        {
            assert_equal(0, setenv(env_name, "--dont-make-backups", 1));
            assert_true(rra_cli_options_load_from_env(NULL));
            assert_false(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            rra_cli_options_reset();
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            assert_equal(0, setenv(env_name, "--dont-make-backups", 1));
            assert_true(rra_cli_options_load_from_env(NULL));
            assert_false(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());
            rra_cli_options_reset();

            return true;
        }

        bool test_env5(void)
        {
            assert_equal(0, setenv(env_name, "--hive-check-write", 1));
            assert_true(rra_cli_options_load_from_env(NULL));
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_true(rra_cli_options_should_check_hive_write());

            rra_cli_options_reset();
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            assert_equal(0, setenv(env_name, "--hive-check-read", 1));
            assert_true(rra_cli_options_load_from_env(NULL));
            assert_true(rra_cli_options_should_make_backup());
            assert_true(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            rra_cli_options_reset();
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            assert_equal(0, setenv(env_name,
                                   "--hive-check-read --hive-check-write", 1));
            assert_true(rra_cli_options_load_from_env(NULL));
            assert_true(rra_cli_options_should_make_backup());
            assert_true(rra_cli_options_should_check_hive_read());
            assert_true(rra_cli_options_should_check_hive_write());

            rra_cli_options_reset();
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            return true;
        }

        bool test_env6(void)
        {
            assert_equal(0, setenv(env_name,
                                   /* misspelled */
                                   "--dont-make-backup", 1));
            assert_false(rra_cli_options_load_from_env(NULL));
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

            rra_cli_options_reset();
            assert_true(rra_cli_options_should_make_backup());
            assert_false(rra_cli_options_should_check_hive_read());
            assert_false(rra_cli_options_should_check_hive_write());

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
            REGISTER(test_env5);
            REGISTER(test_env6);

            Melunit::Suite::instance().register_test(this);
#undef REGISTER
        }

    };

    test_options t1_;
}
