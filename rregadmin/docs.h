/*
 * Authors:	James Lewismoss <jlm@racemi.com>
 *
 * Copyright (c) 2006,2008 Racemi
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

#ifndef RREGADMIN_DOCS_H
#define RREGADMIN_DOCS_H

/**
 * @mainpage RRegAdmin Project
 *
 * This collection of libraries and programs allows access, and modification
 * of MS Windows configuration files called Hives.
 *
 * The Windows registry is actually a view into several files on the
 * filesystem in the hive format.
 *
 * Some places to look for information:
 *
 * - docs/cifs6.txt in the source
 * - http://www.microsoft.com/technet/archive/winntas/tips/winntmag/inreg.mspx
 * - http://support.microsoft.com/kb/256986/
 * - http://www.windowsitpro.com/Articles/Index.cfm?IssueID=19&ArticleID=122
 * - http://msdn.microsoft.com/library/default.asp?url=/library/en-us/sysinfo/base/registry_functions.asp
 *
 * For a more talky description of the code and and how it fits together
 * check out @ref caf_page.
 *
 * These libraries include:
 *
 * - @ref util_group
 * Some generic code needed by other parts of the programs and libraries.
 *
 * - @ref hive_group
 * Hive file format library.  This handles the low level access and
 * modification of the hive file format.
 *
 * - @ref registry_group
 * This in an interface to a registry which is a group of hives.
 *
 * - @ref sam_group
 * Interface to user data.
 *
 * - @ref secdesc_group
 * Interface to security descriptor data.
 *
 * - @ref diff_group
 * Code allowing differences to be generated between hives and registries.
 *
 * - @ref cli_group
 * A command line interface to manipulating a registry.
 *
 * \todo Put more here.
 */

#endif /* RREGADMIN_DOCS_H */
