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

#ifndef RREGADMIN_CLI_BASIC_H
#define RREGADMIN_CLI_BASIC_H 1

#include <rregadmin/util/macros.h>
#include <rregadmin/util/path.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/registry/types.h>
#include <rregadmin/cli/basic_result.h>

/**
 * @defgroup basic_cli_group RRACliState Object
 *
 * This class provides a simplistic command-line-like interface to a
 * standard group of hives.
 *
 * @ingroup cli_group
 */

G_BEGIN_DECLS

typedef struct RRACliState_ RRACliState;

/** Create a RRACliState object.
 *
 * @ingroup basic_cli_group
 */
RRACliState* rra_cli_state_new(void)
    G_GNUC_WARN_UNUSED_RESULT G_GNUC_MALLOC;

/** Create a RRACliState object and load the standard group of hives.
 *
 * @see rra_cli_state_load_win_dir()
 *
 * @ingroup basic_cli_group
 */
RRACliState* rra_cli_state_new_from_win_dir(const char *in_dir)
    G_GNUC_WARN_UNUSED_RESULT G_GNUC_MALLOC;

/** Free a RRACliState object.
 *
 * @ingroup basic_cli_group
 */
gboolean rra_cli_state_free(RRACliState *in_state);

/** Load the standard group of hives in a windows systemroot.
 *
 * @see registry_load_win_dir
 *
 * @ingroup basic_cli_group
 */
gboolean rra_cli_state_load_win_dir(RRACliState* in_state,
                                    const char *in_dir);

/** Get the Registry object.
 *
 * @ingroup basic_cli_group
 */
Registry* rra_cli_state_get_registry(RRACliState *in_state);

/** Get the current path.
 *
 * @ingroup basic_cli_group
 */
const rra_path* rra_cli_state_get_path(const RRACliState *in_state);

/** Return text describing the available commands.
 *
 * @ingroup basic_cli_group
 */
const char* rra_cli_state_help_message(RRACliState *in_state);

/** Apply a command from the string list.
 *
 * Possible commands are; cd, has_key, show_key, delete_key, add_key,
 * has_value, show_value, cat_value, add_value, modify_value,
 * and delete_value.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_state_apply(RRACliState *in_state, int argc,
                                  const char **argv);

/*
 * The following are the handlers for the basic operations.
 * They can be called individually or the apply above will look at its
 * first argument to determine the operation and call one for you.
 */

/** Typedef for command functions.
 *
 * @ingroup basic_cli_group
 */
typedef RRACliResult* (*HTBasicOperation)(RRACliState *in_state, int argc,
                                          const char **argv);

/** Set the exit status type and return.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_exit(RRACliState *in_state, int argc,
                           const char **argv);

/** Change the current path.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_cd(RRACliState *in_state, int argc,
                         const char **argv);

/** Check whether a key exists.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_has_key(RRACliState *in_state, int argc,
                              const char **argv);

/** Show a simple text representation of a key.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_show_key(RRACliState *in_state, int argc,
                               const char **argv);

/** Show a parseable text representation of a key.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_show_key_parseable(RRACliState *in_state, int argc,
                                         const char **argv);

/** Delete a key.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_delete_key(RRACliState *in_state, int argc,
                                 const char **argv);

/** Check to see if the key can be deleted.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_is_deletable_key(RRACliState *in_state, int argc,
                                       const char **argv);

/** Add a key.
 *
 * If the key already exists this function exits successfully.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_add_key(RRACliState *in_state, int argc,
                              const char **argv);

/** Check whether a value exists.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_has_value(RRACliState *in_state, int argc,
                                const char **argv);

/** Show a simple text representation of a value
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_show_value(RRACliState *in_state, int argc,
                                 const char **argv);

/** Output a binary representation ofa value.
 *
 * Like rra_cli_show_value() but instead outputs the raw data from the value.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_cat_value(RRACliState *in_state, int argc,
                                const char **argv);

/** Delete a value.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_delete_value(RRACliState *in_state, int argc,
                                   const char **argv);

/** Add or replace a value.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_add_value(RRACliState *in_state, int argc,
                                const char **argv);

/** Modify a value.
 *
 * Allows bit operations to be done on the value.  The first argument
 * should be clear_bit or set_bit followed by the bytenum and the mask.
 *
 * @ingroup basic_cli_group
 */
RRACliResult* rra_cli_modify_value(RRACliState *in_state, int argc,
                                   const char **argv);

G_END_DECLS

#endif // RREGADMIN_CLI_BASIC_H
