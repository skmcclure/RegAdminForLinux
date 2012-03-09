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

#ifndef RREGADMIN_CLI_BASIC_RESULT_H
#define RREGADMIN_CLI_BASIC_RESULT_H 1

#include <stdarg.h>

#include <glib/gtypes.h>
#include <glib/garray.h>

#include <rregadmin/util/macros.h>
#include <rregadmin/hive/types.h>

/**
 * @defgroup basic_result_cli_group RRACliResult Object
 *
 * This object contains the results from an execution of one of the
 * RRACliState operations.
 *
 * @ingroup cli_group
 */
G_BEGIN_DECLS

typedef struct RRACliResult_ RRACliResult;

/** Create a new RRACliResult.
 *
 * @ingroup basic_result_cli_group
 */
RRACliResult* rra_cli_result_new(void) G_GNUC_WARN_UNUSED_RESULT;

/** Free a RRACliResult.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_free(RRACliResult *in_res);

/** Set the result as an error.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_set_error(RRACliResult *in_res);

/** Set the result as success.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_set_success(RRACliResult *in_res);

/** Set the result as exit.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_set_exit(RRACliResult *in_res);

/** Is this result an error result?
 *
 * Until a result type is set it is none of error, success or exit.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_is_error(const RRACliResult *in_res);

/** Is this result a success result?
 *
 * Until a result type is set it is none of error, success or exit.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_is_success(const RRACliResult *in_res);

/** Is this result an exit result?
 *
 * Until a result type is set it is none of error, success or exit.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_is_exit(const RRACliResult *in_res);

/** Does the result have any contents to output?
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_has_content(const RRACliResult *in_res);

/** Does the result have any raw data to return?
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_has_data(const RRACliResult *in_res);

/** Add some error text and set the result to the error type.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_add_error(RRACliResult *in_res,
                                   const char *in_message, ...)
    G_GNUC_PRINTF(2, 3);

/** Add some success text and set the result to the success type.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_add_message(RRACliResult *in_res,
                                      const char *in_message);

/** Add some binary data to the result.
 *
 * The byte array that is passed in becomes owned by the result.
 * Only one set of data at a time can be contained by the result.
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_set_data(RRACliResult *in_res,
                                   GByteArray *in_data);

/** Set the cell this operation operated on (if it still exists).
 *
 * @ingroup basic_result_cli_group
 */
gboolean rra_cli_result_set_cell(RRACliResult *in_res, Cell *in_cell);

/** Get the content for either errors or normal textual returns.
 *
 * @ingroup basic_result_cli_group
 */
const char* rra_cli_result_get_content(const RRACliResult *in_res);

/** Get the data contained by the result.
 *
 * @ingroup basic_result_cli_group
 */
const GByteArray* rra_cli_result_get_data(const RRACliResult *in_res);

/** Get the cell this operation operated on.
 *
 * @ingroup basic_result_cli_group
 */
Cell* rra_cli_result_get_cell(const RRACliResult *in_res);

G_END_DECLS

#endif // RREGADMIN_CLI_BASIC_RESULT_H
