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

#ifndef RREGADMIN_REGISTRY_SEARCH_ORDER_H
#define RREGADMIN_REGISTRY_SEARCH_ORDER_H 1

#include <rregadmin/registry/consts.h>

G_BEGIN_DECLS

/**
 * Search order for hives.
 *
 * @ingroup registry_group
 */
static const char *hive_search_order[] = {
    CURRENT_USER_KEY_NAME,
    SAM_KEY_NAME,
    SECURITY_KEY_NAME,
    SOFTWARE_KEY_NAME,
    SYSTEM_KEY_NAME,
    DEFAULT_USER_KEY_NAME,
    SID_RID_CLASSES_USER_KEY_NAME,
    USERDIFF_KEY_NAME,
    UNKNOWN_KEY_NAME,
    NULL
};

G_END_DECLS

#endif // RREGADMIN_REGISTRY_SEARCH_ORDER_H
