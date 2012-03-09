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

#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <glib.h>

#include <example_file.h>

#if defined(__GNUC__)
#  define DECLARE_UNUSED(x) __attribute__((__unused__)) x
#else
#  define DECLARE_UNUSED(x) x
#endif

typedef enum
{
    PRE_E,
    PATH_E,
    NAME_E,
    DONE_E
} parse_state;

struct parse_data_
{
    ef_line *line;
    parse_state state;
};
typedef struct parse_data_ parse_data;

static void
start_e (GMarkupParseContext DECLARE_UNUSED(*context),
         const gchar *element_name,
         const gchar DECLARE_UNUSED(**attribute_names),
         const gchar DECLARE_UNUSED(**attribute_values),
         gpointer user_data,
         GError DECLARE_UNUSED(**error))
{
    parse_data *pd = (parse_data*)user_data;

    if (strcmp(element_name, "value") == 0)
    {
        pd->line = g_new0(ef_line, 1);
        pd->state = PRE_E;
    }
    else if (strcmp(element_name, "path") == 0)
    {
        pd->state = PATH_E;
    }
    else if (strcmp(element_name, "name") == 0)
    {
        pd->state = NAME_E;
    }
}

static void
end_e (GMarkupParseContext DECLARE_UNUSED(*context),
       const gchar *element_name,
       gpointer user_data,
       GError DECLARE_UNUSED(**error))
{
    parse_data *pd = (parse_data*)user_data;

    if (strcmp(element_name, "value") == 0)
    {
        pd->state = DONE_E;
    }
}

static void
text_e (GMarkupParseContext DECLARE_UNUSED(*context),
       const gchar         *text,
       gsize                text_len,
       gpointer             DECLARE_UNUSED(user_data),
       GError             DECLARE_UNUSED(**error))
{
    parse_data *pd = (parse_data*)user_data;

    if (pd->state == PATH_E)
    {
        pd->line->path = g_strndup(text, text_len);
        pd->state = PRE_E;
    }
    else if (pd->state == NAME_E)
    {
        pd->line->name = g_strndup(text, text_len);
        pd->line->type = EF_VALUE;
        pd->state = PRE_E;
    }
}

struct example_file_
{
    char *filename;
    FILE *fl;
    ef_type type;
    gboolean should_close_file;
    gboolean is_reading;
    GMarkupParseContext *ctxt;
    GMarkupParser parser;
    parse_data pdata;
};

example_file*
ef_open(const char *in_filename)
{
    example_file *ret_val = g_new0(example_file, 1);
    ret_val->is_reading = TRUE;
    ret_val->type = EF_UNKNOWN;

    if (in_filename != NULL)
    {
        ret_val->filename = g_strdup(in_filename);
        ret_val->should_close_file = TRUE;
        ret_val->fl = fopen(in_filename, "r");
        if (ret_val->fl == NULL)
        {
            fprintf(stderr, "Cannot open file %s: %s",
                    in_filename, strerror(errno));
            g_free(ret_val);
            return NULL;
        }
    }
    else
    {
        ret_val->filename = g_strdup("-");
        ret_val->should_close_file = FALSE;
        ret_val->fl = stdin;
    }
    ret_val->parser.start_element = start_e;
    ret_val->parser.end_element = end_e;
    ret_val->parser.text = text_e;

    ret_val->ctxt = g_markup_parse_context_new(
        &ret_val->parser, 0, &ret_val->pdata, NULL);

    return ret_val;
}

example_file*
ef_new(const char *in_filename, ef_type in_type)
{
    example_file *ret_val = g_new0(example_file, 1);
    ret_val->is_reading = FALSE;
    ret_val->type = in_type;

    if (in_filename != NULL)
    {
        ret_val->filename = g_strdup(in_filename);
        ret_val->should_close_file = TRUE;
        ret_val->fl = fopen(in_filename, "w");
        if (ret_val->fl == NULL)
        {
            fprintf(stderr, "Cannot open file %s: %s",
                    in_filename, strerror(errno));
            g_free(ret_val);
            return NULL;
        }
    }
    else
    {
        ret_val->filename = g_strdup("-");
        ret_val->should_close_file = FALSE;
        ret_val->fl = stdout;
    }

    fprintf (ret_val->fl,
             "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
             "<values>\n");

    return ret_val;
}

gboolean
ef_free(example_file *in_ef)
{
    if (!in_ef->is_reading)
    {
        fprintf(in_ef->fl, "</values>\n");
    }
    else
    {
        g_markup_parse_context_end_parse(in_ef->ctxt, NULL);
        g_markup_parse_context_free(in_ef->ctxt);
    }

    if (in_ef->should_close_file)
    {
        fclose(in_ef->fl);
    }

    g_free(in_ef->filename);
    g_free(in_ef);
    return TRUE;
}

gboolean
ef_line_free(ef_line *in_line)
{
    if (in_line != NULL)
    {
        if (in_line->path != NULL)
        {
            g_free(in_line->path);
            in_line->path = NULL;
        }
        if (in_line->name != NULL)
        {
            g_free(in_line->name);
            in_line->name = NULL;
        }
        g_free(in_line);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


ef_line*
ef_next(example_file *in_ef)
{
    if (in_ef->is_reading)
    {
        GError *error = NULL;
        char *line = NULL;
        size_t line_size = 0;

        while (getline(&line, &line_size, in_ef->fl) != -1)
        {
            g_markup_parse_context_parse(in_ef->ctxt, line, strlen(line),
                                         &error);
            if (error != NULL)
            {
                fprintf(stderr, "Error2(%d): %s: on line '%s'\n",
                        error->code, error->message, line);
                return NULL;
            }
            if (in_ef->pdata.state == DONE_E)
            {
                ef_line *ret_val = in_ef->pdata.line;
                in_ef->pdata.state = PRE_E;
                in_ef->pdata.line = NULL;
                return ret_val;
            }
        }
    }

    return NULL;
}

gboolean
ef_write_line(example_file *in_ef, const ef_line *in_line)
{
    if (!in_ef->is_reading)
    {
        if (in_ef->type != in_line->type)
        {
            fprintf(stderr, "Mismatched ef and line types\n");
            return FALSE;
        }

        if (in_ef->type == EF_KEY)
        {
            printf("%s",
                   g_markup_printf_escaped(
                       "<value>"
                       "<path>%s</path>"
                       "</value>\n",
                       in_line->path));
        }
        else
        {
            printf("%s",
                   g_markup_printf_escaped(
                       "<value>"
                       "<path>%s</path>"
                       "<name>%s</name>"
                       "</value>\n",
                       in_line->path, in_line->name));
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
