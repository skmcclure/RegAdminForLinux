/*
 * Authors:     James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
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

#ifndef RREGADMIN_CLI_TEST_HELPERS_H
#define RREGADMIN_CLI_TEST_HELPERS_H 1

#include <rregadmin/util/macros.h>

G_BEGIN_DECLS

#define assert_true_result(res_cmd)                                     \
    do                                                                  \
    {                                                                   \
        RRACliResult *res = res_cmd;                                   \
        assert_not_null(res);                                           \
        if (!rra_cli_result_is_success(res)                            \
            && rra_cli_result_has_content(res))                        \
        {                                                               \
            std::cerr << "ERROR: "                                      \
                      << rra_cli_result_get_content(res)               \
                      << std::endl;                                     \
        }                                                               \
        assert_true(rra_cli_result_is_success(res));                   \
        assert_true(rra_cli_result_free(res));                         \
    } while(0)

#define assert_false_result(res_cmd)                  \
    do                                                \
    {                                                 \
        RRACliResult *res = res_cmd;                 \
        assert_not_null(res);                         \
        assert_true(rra_cli_result_is_error(res));   \
        assert_true(rra_cli_result_free(res));       \
    } while(0)

#define assert_true_result_msg(res_cmd, msg)                            \
    do                                                                  \
    {                                                                   \
        RRACliResult *res = res_cmd;                                   \
        assert_not_null(res);                                           \
        if (!rra_cli_result_is_success(res)                            \
            && rra_cli_result_has_content(res))                        \
        {                                                               \
            std::cerr << "ERROR: "                                      \
                      << rra_cli_result_get_content(res)               \
                      << std::endl;                                     \
        }                                                               \
        assert_true_msg(rra_cli_result_is_success(res), msg);          \
        assert_true(rra_cli_result_free(res));                         \
    } while(0)

#define assert_false_result_msg(res_cmd, msg)                       \
    do                                                              \
    {                                                               \
        RRACliResult *res = res_cmd;                               \
        assert_not_null(res);                                       \
        assert_true_msg(rra_cli_result_is_error(res), msg);        \
        assert_true(rra_cli_result_free(res));                     \
    } while(0)


G_END_DECLS

#endif // RREGADMIN_CLI_TEST_HELPERS_H
