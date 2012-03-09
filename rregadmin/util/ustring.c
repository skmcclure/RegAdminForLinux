/*
 * Authors:     Sean Loaring
 *              Petter Nordahl-Hagen
 *              James LewisMoss <jlm@racemi.com>
 *
 * Copyright (c) 2006 Racemi Inc
 * Copyright (c) 2005-2006 Sean Loaring
 * Copyright (c) 1997-2004 Petter Nordahl-Hagen
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

#include <ctype.h>
#include <string.h>
#include <iconv.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <glib.h>
#include <glib/gi18n-lib.h>

#include <rregadmin/util/ustring.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/malloc.h>
#include <rregadmin/util/iconv_helper.h>

/** Variable-length unicode handling string type.
 *
 * All strings are converted to UTF-8 for internal storage.
 *
 * @ingroup ustring_util_group
 */
struct ustring_
{
    /** Magic signature.
     */
    guint32 signature;

    /** Buffer containing the char data.
     */
    char *buffer;
    /** The length of the buffer (may be larger than needed).
     */
    gsize buffer_len;

    /** Where the string begins.
     */
    char *start_of_data;
    /** Amount of valid data in the buffer.
     */
    gsize data_len;

    /** Length of string in buffer.
     *
     * @note cached data
     */
    gsize my_strlen;

    /** Collate key for case sensitive comparisons.
     *
     * @note cached data
     */
    gchar *collate_key;

    /** Collate key for case insensitive comparisons.
     *
     * @note cached data
     */
    gchar *casei_collate_key;
};

#undef DEBUG_USTRING

#if defined(RRA_INCLUDE_DEBUG_CODE) && defined(DEBUG_USTRING)

#  define USTR_PRECONDITION(in_ustr)                                    \
    do {                                                                \
        if (in_ustr == NULL)                                            \
        {                                                               \
            rra_warning(#in_ustr " is NULL");                           \
            abort();                                                    \
        }                                                               \
        if (in_ustr->buffer != NULL                                     \
            && in_ustr->start_of_data[in_ustr->data_len - 1] != '\0')   \
        {                                                               \
            rra_warning(#in_ustr " has no \\0 ending");                 \
            abort();                                                    \
        }                                                               \
    } while(0)
#  define USTR_POSTCONDITION(in_ustr)                                   \
    do {                                                                \
        if (in_ustr->buffer != NULL                                     \
            && in_ustr->start_of_data[in_ustr->data_len - 1] != '\0')   \
        {                                                               \
            rra_warning(#in_ustr " has no \\0 ending");                 \
            abort();                                                    \
        }                                                               \
    } while(0)

#else

#  define USTR_PRECONDITION(in_ustr)
#  define USTR_POSTCONDITION(in_ustr)

#endif

void
ustr_debug_print (const ustring *in_ustr)
{
    fprintf (stderr, "<ustring>\n");
    fprintf (stderr, "  <buffer value=\"%p\"/>\n", in_ustr->buffer);
    fprintf (stderr, "  <buffer_len value=\"%" G_GSIZE_FORMAT "\"/>\n",
             in_ustr->buffer_len);
    fprintf (stderr, "  <start_of_data value=\"%p\"/>\n",
             in_ustr->start_of_data);
    fprintf (stderr, "  <data_len value=\"%" G_GSIZE_FORMAT "\"/>\n",
             in_ustr->data_len);
    fprintf (stderr, "</ustring>\n");
}

const guint32 USTR_SIG_DYNAMIC = 0xff0e1243;
const guint32 USTR_SIG_STATIC =  0x43120eff;

/**
 * Initialize a new statically created ustring.
 */
void ustr_init(ustring *in_ustr);
char* ustr_get_end_of_data (ustring *in_ustr);
static char hex_dump_char(char c) G_GNUC_PURE;
static gboolean generic_convert(const guint8 *in_data, int in_len,
                                const char *from_type, const char *to_type,
                                GByteArray *out_data);

static const char * get_type_char_code(UStringType in_type) G_GNUC_PURE;
static void assure_null_ending(ustring *in_ustr);
static int ustr_avail (const ustring *in_ustr) G_GNUC_PURE;
static char* ustr_get_start_of_data (ustring *in_ustr);
static gboolean ustr_assuresize_clear (ustring *in_ustr, size_t len);
static gboolean ustr_assuresize (ustring *in_ustr, gsize in_wanted_size);
static gboolean ustr_grow (ustring *in_ustr, gint32 len);
static const char* ustr_get_data (const ustring *in_ustr) G_GNUC_PURE;
static void ustr_clear_buffer(ustring *in_ustr);
static void clear_cached_data(ustring *in_ustr);
static gint32 get_new_size(ustring *in_ustr, gint32 len);
static gunichar ustr_unichar_at_index(const ustring *in_ustr, gssize in_index);

static int allocated_count = 0;

int
ustr_allocated_count(void)
{
    return allocated_count;
}

static void
clear_cached_data(ustring *in_ustr)
{
    in_ustr->my_strlen = (gsize)-1;
    if (in_ustr->collate_key != NULL)
    {
        g_free(in_ustr->collate_key);
        in_ustr->collate_key = NULL;
    }
    if (in_ustr->casei_collate_key != NULL)
    {
        g_free(in_ustr->casei_collate_key);
        in_ustr->casei_collate_key = NULL;
    }
}


void
ustr_init(ustring *in_ustr)
{
    in_ustr->signature = USTR_SIG_STATIC;
    in_ustr->data_len = 0;
    in_ustr->buffer_len = 0;
    in_ustr->buffer = NULL;
    in_ustr->start_of_data = NULL;
    clear_cached_data(in_ustr);
}

ustring*
ustr_new (void)
{
    ustring *result;
    result = rra_new_type(ustring);
    ustr_init(result);
    result->signature = USTR_SIG_DYNAMIC;
    assure_null_ending(result);
    allocated_count++;

    USTR_POSTCONDITION(result);

    return result;
}

static void
ustr_clear_buffer(ustring *in_ustr)
{
    if (in_ustr->buffer != NULL)
    {
        g_free (in_ustr->buffer);
    }
    in_ustr->data_len = 0;
    in_ustr->buffer_len = 0;
    in_ustr->buffer = NULL;
    in_ustr->start_of_data = NULL;
    clear_cached_data(in_ustr);
}

ustring *
ustr_create(const char *in_str)
{
    if (in_str == NULL)
    {
        return ustr_new();
    }
    else
    {
        return ustr_create_type(in_str, strlen(in_str) + 1, USTR_TYPE_UTF8);
    }
}

ustring *
ustr_create_type(const char *in_str, guint32 in_str_len,
                 UStringType in_type)
{
    ustring *ret_val = ustr_new();
/*     rra_debug("Creating str=\"%s\", len=\"%d\", type=\"%d\"", */
/*               in_str, in_str_len, in_type); */
    if (!ustr_strnset_type(ret_val, in_type, in_str, in_str_len))
    {
        ustr_free(ret_val);
        ret_val = NULL;
    }
    else
    {
        USTR_POSTCONDITION(ret_val);
    }

    return ret_val;
}

ustring *
ustr_copy (const ustring *in_ustr)
{
    USTR_PRECONDITION(in_ustr);
/*     rra_debug("Copying %s", ustr_as_utf8(in_ustr)); */
    return ustr_create(ustr_as_utf8(in_ustr));
}

gboolean
ustr_is_valid(const ustring *in_ustr)
{
    if (in_ustr == NULL)
    {
        return FALSE;
    }

    if (in_ustr->signature == USTR_SIG_STATIC
        || in_ustr->signature == USTR_SIG_DYNAMIC)
    {
        return TRUE;
    }

    return FALSE;
}

gboolean
ustr_free (ustring *in_ustr)
{
    if (in_ustr == NULL)
    {
        return FALSE;
    }

    if (!ustr_is_valid(in_ustr))
    {
        rra_warning(N_("Invalid ustring passed to ustr_free"));
        return FALSE;
    }

    clear_cached_data(in_ustr);
    ustr_clear_buffer(in_ustr);
    in_ustr->signature = 0;
    rra_free_type(ustring, in_ustr);
    allocated_count--;
    return TRUE;
}

static const gint32 MIN_GROWTH_SIZE = 512;

static gint32
get_new_size(ustring *in_ustr, gint32 len)
{
    gint32 ret_val =
        MAX(len, MAX(MIN_GROWTH_SIZE, in_ustr->buffer_len * 1.5));
    ret_val += (ret_val % MIN_GROWTH_SIZE);
    return ret_val;
}

static gboolean
ustr_assuresize (ustring *in_ustr, gsize in_wanted_size)
{
    if (in_wanted_size <= in_ustr->buffer_len)
    {
        return TRUE;
    }
    else
    {
        guint32 new_len = get_new_size(in_ustr, in_wanted_size);
        int start_offset = abs(in_ustr->start_of_data - in_ustr->buffer);
        char *new_buffer = g_renew (char, in_ustr->buffer, new_len);
        rra_debug(N_("OrigSize %6" G_GSIZE_FORMAT
                     ", WantedSize %6" G_GSIZE_FORMAT ", "
                     "Size increased to %" G_GSIZE_FORMAT ".\n"),
                  in_ustr->buffer_len, in_wanted_size, new_len);
        in_ustr->buffer_len = new_len;
        in_ustr->buffer = new_buffer;
        in_ustr->start_of_data = in_ustr->buffer + start_offset;
        return TRUE;
    }
}


static gboolean
ustr_assuresize_clear (ustring *in_ustr, size_t len)
{
    /*
     * Don't do this
     * USTR_PRECONDITION(in_ustr);
     */
    gboolean ret_val = TRUE;

    if (!ustr_assuresize(in_ustr, len + 1))
    {
        return FALSE;
    }

    in_ustr->data_len = 0;
    in_ustr->start_of_data = in_ustr->buffer;
    memset(in_ustr->buffer, 0, in_ustr->buffer_len);
    clear_cached_data(in_ustr);

    /*
     * Don't do this
     * USTR_POSTCONDITION(in_ustr);
     */
    return ret_val;
}

static gboolean
ustr_grow (ustring *in_ustr, gint32 len)
{
    /*
     * Don't do this
     * USTR_PRECONDITION(in_ustr);
     */
    int start_offset;
    int wanted_size;

    if (len == 0)
        return FALSE;

    if (in_ustr->buffer_len == 0)
    {
        ustr_assuresize_clear (in_ustr, len);
        return TRUE;
    }

    start_offset = abs(in_ustr->start_of_data - in_ustr->buffer);
    wanted_size = in_ustr->data_len + len + start_offset;
    if (!ustr_assuresize(in_ustr, wanted_size))
    {
        return FALSE;
    }

    if (ustr_avail (in_ustr) < len)
        return FALSE;

    /*
     * Don't do this
     * USTR_POSTCONDITION(in_ustr);
     */
    return TRUE;
}

static int
ustr_avail (const ustring *in_ustr)
{
    return (in_ustr->buffer_len
            - (((char *) in_ustr->start_of_data - (char *) in_ustr->buffer)
               + in_ustr->data_len));
}

int
ustr_compare(const ustring *in_ustra, const ustring *in_ustrb)
{
    if (in_ustra == NULL)
    {
        if (in_ustrb == NULL)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (in_ustrb == NULL)
        {
            return 1;
        }
        else
        {
#ifdef HAVE_GOOD_G_GUTF8_COLLATE_KEY
            if (in_ustra->collate_key == NULL)
            {
                ((ustring*)in_ustra)->collate_key =
                    g_utf8_collate_key(ustr_as_utf8(in_ustra), -1);
            }
            if (in_ustrb->collate_key == NULL)
            {
                ((ustring*)in_ustrb)->collate_key =
                    g_utf8_collate_key(ustr_as_utf8(in_ustrb), -1);
            }

            return strcmp(in_ustra->collate_key, in_ustrb->collate_key);
#else
            return g_utf8_collate(ustr_as_utf8(in_ustra),
                                  ustr_as_utf8(in_ustrb));
#endif
        }
    }
}

int
ustr_casecmp(const ustring *in_ustra, const ustring *in_ustrb)
{
    if (in_ustra == NULL)
    {
        if (in_ustrb == NULL)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (in_ustrb == NULL)
        {
            return 1;
        }
        else
        {
#ifdef HAVE_GOOD_G_GUTF8_COLLATE_KEY
            if (in_ustra->casei_collate_key == NULL)
            {
                gchar *a_lc = g_utf8_casefold(ustr_as_utf8(in_ustra), -1);
                ((ustring*)in_ustra)->casei_collate_key =
                    g_utf8_collate_key(a_lc, -1);
                g_free(a_lc);
            }
            if (in_ustrb->casei_collate_key == NULL)
            {
                gchar *b_lc = g_utf8_casefold(ustr_as_utf8(in_ustrb), -1);
                ((ustring*)in_ustrb)->casei_collate_key =
                    g_utf8_collate_key(b_lc, -1);
                g_free(b_lc);
            }
            return strcmp(in_ustra->casei_collate_key
                          in_ustrb->casei_collate_key);
#else
            gchar *a_lc = g_utf8_casefold(ustr_as_utf8(in_ustra), -1);
            gchar *b_lc = g_utf8_casefold(ustr_as_utf8(in_ustrb), -1);
            int ret_val = g_utf8_collate(a_lc, b_lc);
            g_free(a_lc);
            g_free(b_lc);
            return ret_val;
#endif
        }
    }
}

int
ustr_regcmp(const ustring *in_ustra, const ustring *in_ustrb)
{
    if (TRUE)
    {
        int ret_val = 0;
        size_t index = 0;
        gunichar s1_c = ustr_index_unsafe(in_ustra, index);
        gunichar s2_c = ustr_index_unsafe(in_ustrb, index);

        while (s1_c != (gunichar)'\0' && s2_c != (gunichar)'\0')
        {
            s1_c = g_unichar_toupper(s1_c);
            s2_c = g_unichar_toupper(s2_c);
            ret_val = (s1_c - s2_c);
            if (ret_val != 0)
            {
                break;
            }

            index++;
            s1_c = ustr_index_unsafe(in_ustra, index);
            s2_c = ustr_index_unsafe(in_ustrb, index);
        }

        if (ret_val == 0)
        {
            // equal except for length
            // longer is >
            ret_val = ustr_strlen(in_ustra) - ustr_strlen(in_ustrb);
        }

        return ret_val;
    }
    else
    {
        /* This doesn't work. */
        return strcasecmp(ustr_as_utf8(in_ustra),
                          ustr_as_utf8(in_ustrb));
    }
}


gboolean
ustr_equal(const ustring *in_ustra, const ustring *in_ustrb)
{
    return ustr_compare(in_ustra, in_ustrb) == 0;
}

int
ustr_compare_str(const ustring *in_ustra, const char *in_str)
{
    if (in_ustra == NULL)
    {
        if (in_str == NULL)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (in_str == NULL)
        {
            return 1;
        }
        else
        {
            return strcmp(ustr_as_utf8(in_ustra), in_str);
        }
    }
}

int
ustr_casecmp_str(const ustring *in_ustra, const char *in_str)
{
    if (in_ustra == NULL)
    {
        if (in_str == NULL)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (in_str == NULL)
        {
            return 1;
        }
        else
        {
            gchar *a_lc = g_utf8_casefold(ustr_as_utf8(in_ustra), -1);
            gchar *b_lc = g_utf8_casefold(in_str, -1);

            gint ret_val = g_utf8_collate(a_lc, b_lc);

            g_free(a_lc);
            g_free(b_lc);

            return ret_val;
        }
    }
}

gboolean
ustr_equal_str(const ustring *in_ustra, const char *in_str)
{
    return ustr_compare_str(in_ustra, in_str) == 0;
}

int
ustr_size (const ustring *in_ustr)
{
    USTR_PRECONDITION(in_ustr);
    return in_ustr->data_len;
}

int
ustr_strlen(const ustring *in_ustr)
{
    USTR_PRECONDITION(in_ustr);
    if (ustr_size(in_ustr) == 0)
    {
        return 0;
    }
    else
    {
        if (in_ustr->my_strlen == (gsize)-1)
        {
            ((ustring*)in_ustr)->my_strlen =
                g_utf8_strlen(in_ustr->start_of_data, -1);
        }
        return in_ustr->my_strlen;
    }
}

static char*
ustr_get_start_of_data (ustring *in_ustr)
{
    return (char*)in_ustr->start_of_data;
}

static const char*
ustr_get_data (const ustring *in_ustr)
{
    return (char*)in_ustr->start_of_data;
}

const char*
ustr_as_utf8 (const ustring *in_ustr)
{
    USTR_PRECONDITION(in_ustr);
    // \todo actually make this do what is expected.
    return (char*)in_ustr->start_of_data;
}

static gunichar
ustr_unichar_at_index(const ustring *in_ustr, gssize in_index)
{
    return g_utf8_get_char(g_utf8_offset_to_pointer(in_ustr->start_of_data,
                                                    in_index));
    /* return (gunichar)in_ustr->start_of_data[in_index]; */
}

gunichar
ustr_index(const ustring *in_ustr, gssize in_index)
{
    USTR_PRECONDITION(in_ustr);
    if (in_index >= ustr_strlen(in_ustr))
    {
        return (gunichar)'\0';
    }
    else
    {
        return ustr_unichar_at_index(in_ustr, in_index);
    }
}

gunichar
ustr_index_unsafe(const ustring *in_ustr, gssize in_index)
{
    USTR_PRECONDITION(in_ustr);
    return ustr_unichar_at_index(in_ustr, in_index);
}

static gboolean
generic_convert(const guint8 *in_data, int in_len,
                const char *from_type, const char *to_type,
                GByteArray *out_data)
{
    static iconv_helper *ihelper = NULL;
    if (ihelper == NULL)
    {
        ihelper = iconv_helper_new();
    }

    gboolean ret_val = TRUE;
    size_t inbytesleft = in_len;
    const char *inbuf = (const char*)in_data;
    if (out_data == NULL)
    {
        rra_warning(N_("Passed NULL byte array to generic_convert"));
        return FALSE;
    }

    size_t outbytesleft;
    char *outbuf;
    iconv_t iconv_handle;

    if (in_len == 0)
    {
        return TRUE;
    }

    iconv_handle = iconv_helper_get (ihelper, to_type, from_type);

    if (iconv_handle == (iconv_t)-1)
    {
        rra_warning(N_("Got invalid iconv_handle from iconv_open"));
        return FALSE;
    }

    g_byte_array_set_size(out_data, in_len);
    outbytesleft = in_len;
    outbuf = (char*)out_data->data;

    while(1)
    {
        size_t icresult =
            iconv (iconv_handle, (char**)&inbuf, &inbytesleft,
                   (char**)&outbuf, &outbytesleft);

        /* rra_debug("iconv returned %d", icresult); */

        if (icresult == (size_t)-1)
        {
            if (errno == E2BIG)
            {
                /*
                 * The buffer wasn't large enough. Make it bigger and
                 * try again.
                 */
                int written = out_data->len - outbytesleft;
                g_byte_array_set_size(out_data, out_data->len * 2);
                outbytesleft = out_data->len - written;
                outbuf = (char*)out_data->data + written;
            }
            else
            {
                rra_message(N_("iconv error(%d): %s"), errno, strerror(errno));
                rra_message(N_("%d bytes left of %d"), inbytesleft, in_len);
                ret_val = FALSE;
                break;
            }
        }
        else if (icresult > 0)
        {
            rra_warning(N_("%d unreversible conversions done"), icresult);
            break;
        }
        else
        {
            break;
        }
    }

    iconv_helper_release(ihelper, iconv_handle, to_type, from_type);

    if (outbytesleft > 0)
    {
        g_byte_array_set_size(out_data, out_data->len - outbytesleft);
    }

    return ret_val;
}

GByteArray*
ustr_generic_encode(const ustring *in_ustr, const char *in_code_name)
{
    USTR_PRECONDITION(in_ustr);
    if (in_code_name == NULL
        || (strncasecmp(in_code_name, "UTF-8", 6) == 0)
        || (strncasecmp(in_code_name, "", 2) == 0))
    {
        GByteArray *ret_val = g_byte_array_new();
        g_byte_array_set_size(ret_val, ustr_size(in_ustr));
        memcpy(ret_val->data, ustr_get_data(in_ustr), ustr_size(in_ustr));
        return ret_val;
    }
    else
    {
        GByteArray *ret_val = g_byte_array_new();
        if (generic_convert((guint8*)ustr_get_data(in_ustr),
                            ustr_size(in_ustr),
                            "UTF-8", in_code_name, ret_val))
        {
            return ret_val;
        }
        else
        {
            g_byte_array_free(ret_val, TRUE);
            return NULL;
        }
    }
}

static const char *
get_type_char_code(UStringType in_type)
{
    if (in_type == USTR_TYPE_UTF8)
    {
        return "UTF-8";
    }
    else if (in_type == USTR_TYPE_ISO8859_1)
    {
        return "ISO_8859-1";
    }
    else if (in_type == USTR_TYPE_ASCII)
    {
        return "ASCII";
    }
    else if (in_type == USTR_TYPE_UTF16LE)
    {
        return "UTF-16LE";
    }
    else
    {
        return NULL;
    }
}

GByteArray*
ustr_encode (const ustring *in_ustr, UStringType in_type)
{
    return ustr_generic_encode(in_ustr, get_type_char_code(in_type));
}

char*
ustr_get_end_of_data (ustring *in_ustr)
{
    USTR_PRECONDITION(in_ustr);
    return (char*)in_ustr->start_of_data + ustr_size(in_ustr);
}

int
ustr_last_char (const ustring *in_ustr)
{
    USTR_PRECONDITION(in_ustr);
    if (in_ustr->data_len < 1)
        return -1;
    if (in_ustr->start_of_data[in_ustr->data_len - 1] != '\0')
        return -1;
    if (in_ustr->data_len == 1)
        return '\0';
    return in_ustr->start_of_data[in_ustr->data_len - 2];
}


gboolean
ustr_clear (ustring *in_ustr)
{
    ustr_strset(in_ustr, "");
    return TRUE;
}

gboolean
ustr_trim_ws_front (ustring *in_ustr)
{
    USTR_PRECONDITION(in_ustr);
    if (in_ustr == NULL)
    {
        return FALSE;
    }

    if (ustr_strlen(in_ustr) == 0)
    {
        return TRUE;
    }

    char *p = ustr_get_start_of_data(in_ustr);
    while (p < ustr_get_end_of_data(in_ustr) && isspace (*p))
    {
        p++;
    }

    return ustr_trim_front (in_ustr, p - ustr_get_start_of_data(in_ustr));
}

gboolean
ustr_trim_ws_back (ustring *in_ustr)
{
    USTR_PRECONDITION(in_ustr);
    if (in_ustr == NULL)
    {
        return FALSE;
    }

    if (ustr_strlen(in_ustr) == 0)
    {
        return TRUE;
    }

    int count = 0;
    char *p = ustr_get_start_of_data(in_ustr) + ustr_strlen(in_ustr) - 1;

    while (p >= ustr_get_start_of_data(in_ustr) && isspace (*p))
    {
        p--;
        count++;
    }

    return ustr_trim_back (in_ustr, count);
}

gboolean
ustr_trim_ws (ustring *in_ustr)
{
    return ustr_trim_ws_front(in_ustr) && ustr_trim_ws_back(in_ustr);
}

gboolean
ustr_trim_back (ustring *in_ustr, size_t in_skip)
{
    USTR_PRECONDITION(in_ustr);
    if (in_ustr == NULL || in_skip >= in_ustr->data_len)
    {
        return FALSE;
    }

/*     rra_debug("Before: %s('%s', %d)", __func__, */
/*               ustr_as_utf8(in_ustr), in_skip); */

    in_ustr->start_of_data[in_ustr->data_len - in_skip - 1] = '\0';
    in_ustr->data_len -= in_skip;
    clear_cached_data(in_ustr);

/*     rra_debug("After:  %s('%s', %d)", __func__, */
/*               ustr_as_utf8(in_ustr), in_skip); */

    USTR_POSTCONDITION(in_ustr);
    return TRUE;
}

gboolean
ustr_trim_front (ustring *in_ustr, size_t in_skip)
{
    USTR_PRECONDITION(in_ustr);
    if (in_ustr == NULL || in_skip >= in_ustr->data_len)
    {
        return FALSE;
    }

/*     rra_debug("Before: %s('%s', %d)", __func__, */
/*               ustr_as_utf8(in_ustr), in_skip); */

    in_ustr->start_of_data += in_skip;
    in_ustr->data_len -= in_skip;
    clear_cached_data(in_ustr);

/*     rra_debug("After:  %s('%s', %d)", __func__, */
/*               ustr_as_utf8(in_ustr), in_skip); */

    USTR_POSTCONDITION(in_ustr);
    return TRUE;
}

gboolean
ustr_strup(ustring *in_ustr)
{
    gchar *up_str = g_utf8_strup(ustr_as_utf8(in_ustr), -1);
    ustr_strset(in_ustr, up_str);
    g_free(up_str);
    return TRUE;
}

gboolean
ustr_strdown(ustring *in_ustr)
{
    gchar *down_str = g_utf8_strdown(ustr_as_utf8(in_ustr), -1);
    ustr_strset(in_ustr, down_str);
    g_free(down_str);
    return TRUE;
}

static void
assure_null_ending(ustring *in_ustr)
{
    if (in_ustr->start_of_data == NULL
        || in_ustr->start_of_data[in_ustr->data_len - 1] != '\0')
    {
        ustr_grow(in_ustr, 1);
        in_ustr->start_of_data[in_ustr->data_len] = '\0';
        in_ustr->data_len++;
    }
}

gboolean
ustr_wrapa(ustring *in_ustr, const char *in_str, const char *in_prefix,
           int in_column)
{
    USTR_PRECONDITION(in_ustr);
    if (in_ustr == NULL || in_str == NULL)
    {
        return FALSE;
    }

    int prefix_len = in_prefix ? strlen(in_prefix) : 0;
    int line_len = in_column - prefix_len;

    const char *cur_pos = in_str;
    int cur_len = strlen(cur_pos);
    clear_cached_data(in_ustr);
    while (cur_len > 0)
    {
        if (prefix_len > 0)
        {
            ustr_printfa(in_ustr, in_prefix);
        }

        /* strip leading whitespace */
        while (isspace(*cur_pos))
        {
            cur_pos++;
            cur_len--;
        }

        if (cur_len > line_len)
        {
            int loc;
            for (loc = line_len;
                 !isspace(cur_pos[loc]) && loc > (line_len / 5);
                 loc--)
            {
            }
            if (isspace(cur_pos[loc]))
            {
                ustr_strncat(in_ustr, cur_pos, loc);
                ustr_strcat(in_ustr, "\n");
                cur_pos += loc;
                cur_len = strlen(cur_pos);
            }
            else
            {
                for (loc = line_len;
                     !isspace(cur_pos[loc]) && loc < cur_len;
                     loc++)
                {
                }
                ustr_strncat(in_ustr, cur_pos, loc);
                ustr_strcat(in_ustr, "\n");
                if (loc == cur_len)
                {
                    cur_len = 0;
                }
                else
                {
                    cur_pos += loc;
                    cur_len = strlen(cur_pos);
                }
            }
        }
        else
        {
            ustr_printfa(in_ustr, "%s\n", cur_pos);
            cur_len = 0;
        }
    }

    USTR_POSTCONDITION(in_ustr);
    return TRUE;
}

gboolean
ustr_wrap(ustring *in_ustr, const char *in_str, const char *in_prefix,
          int in_column)
{
    if (in_ustr == NULL || in_str == NULL)
    {
        return FALSE;
    }

    ustr_clear(in_ustr);

    return ustr_wrapa(in_ustr, in_str, in_prefix, in_column);
}


gboolean
ustr_strnset_type(ustring *in_ustr, UStringType in_type, const char *buffer,
                  int buffer_len)
{
    USTR_PRECONDITION(in_ustr);
    gboolean ret_val = TRUE;

    if (buffer == NULL
        || buffer_len == 0)
    {
        ustr_assuresize_clear(in_ustr, 1);
        in_ustr->start_of_data[0] = '\0';
        in_ustr->data_len = 1;
    }
    else if (FALSE && in_type == USTR_TYPE_UTF8)
    {
        ustr_assuresize_clear(in_ustr, buffer_len + 1);
        memcpy(in_ustr->start_of_data, buffer, buffer_len);
        in_ustr->data_len = buffer_len;
        assure_null_ending(in_ustr);
    }
    else
    {
        GByteArray *arr = g_byte_array_new();
        if(generic_convert((guint8*)buffer, buffer_len,
                           get_type_char_code(in_type), "UTF-8", arr))
        {
            if (arr->len > 0)
            {
                ustr_assuresize_clear (in_ustr, arr->len + 1);
                memcpy (in_ustr->start_of_data, arr->data, arr->len);
                in_ustr->data_len = arr->len;
                assure_null_ending(in_ustr);
            }
        }
        else
        {
            rra_info(_("generic_convert failed"));
            ret_val = FALSE;
        }

        g_byte_array_free(arr, TRUE);

    }

    clear_cached_data(in_ustr);

/*     rra_debug("set to %s", ustr_as_utf8(in_ustr)); */

    USTR_POSTCONDITION(in_ustr);
    return ret_val;
}

gboolean
ustr_strset (ustring *in_ustr, const char *s)
{
    int slen = 0;
    if (s != NULL)
        slen = strlen(s);

    return ustr_strnset_type (in_ustr, USTR_TYPE_UTF8, s, slen);
}

gboolean
ustr_strnset (ustring *in_ustr, const char *s, int n)
{
    return ustr_strnset_type (in_ustr, USTR_TYPE_UTF8, s, n);
}

gboolean
ustr_overwrite (ustring *in_ustr_dest, const ustring *in_ustr_src)
{
    USTR_PRECONDITION(in_ustr_dest);
    USTR_PRECONDITION(in_ustr_src);

    ustr_assuresize_clear (in_ustr_dest, in_ustr_src->buffer_len);
    memcpy (in_ustr_dest->start_of_data,
            in_ustr_src->start_of_data,
            in_ustr_src->data_len);
    in_ustr_dest->buffer_len = in_ustr_src->buffer_len;
    in_ustr_dest->data_len = in_ustr_src->data_len;

    clear_cached_data(in_ustr_dest);

    USTR_POSTCONDITION(in_ustr_dest);
    return TRUE;
}

gboolean
ustr_strcat (ustring *in_ustr, const char *in_str)
{
    return ustr_strncat (in_ustr, in_str, strlen (in_str) + 1);
}

gboolean
ustr_strncat (ustring *in_ustr, const char *str, int len)
{
    USTR_PRECONDITION(in_ustr);
    if (in_ustr->data_len == 0)
    {
        ustr_clear(in_ustr);
    }

    if (in_ustr->start_of_data[in_ustr->data_len - 1] != '\0')
        return FALSE;

    int sl = strlen (str);
    int n = (sl < len) ? sl : len;
    //string append
    ustr_grow (in_ustr, n + 1);
    memcpy (in_ustr->start_of_data + in_ustr->data_len - 1, str, n);
    //strncat( in_ustr->start_of_data, str, n );

    in_ustr->data_len += n;
    in_ustr->start_of_data[in_ustr->data_len - 1] = '\0';

    clear_cached_data(in_ustr);

    USTR_POSTCONDITION(in_ustr);
    return TRUE;
}

gboolean
ustr_charcat (ustring *in_ustr, int ch)
{
    USTR_PRECONDITION(in_ustr);
    if (in_ustr->data_len == 0)
    {
        ustr_clear(in_ustr);
    }

    ustr_grow (in_ustr, 1);
    in_ustr->start_of_data[in_ustr->data_len - 1] = ch;
    in_ustr->start_of_data[in_ustr->data_len] = '\0';
    in_ustr->data_len++;

    clear_cached_data(in_ustr);

    USTR_POSTCONDITION(in_ustr);
    return TRUE;
}

gboolean
ustr_strnprepend (ustring *in_ustr, const char *str, int len)
{
    USTR_PRECONDITION(in_ustr);
    if (in_ustr->start_of_data[in_ustr->data_len - 1] != '\0')
        return FALSE;

    int sl = strlen (str);
    int n = (sl < len) ? sl : len;
    //string append
    ustr_grow (in_ustr, n + 1);
    if (in_ustr->data_len - 1 > 0)
    {
        memmove (in_ustr->start_of_data + n, in_ustr->start_of_data,
                 in_ustr->data_len - 1);
    }

    memcpy (in_ustr->start_of_data, str, n);
    //strncat( in_ustr->start_of_data, str, n );

    in_ustr->data_len += n;
    in_ustr->start_of_data[in_ustr->data_len - 1] = '\0';

    clear_cached_data(in_ustr);

    USTR_POSTCONDITION(in_ustr);
    return TRUE;
}

gboolean
ustr_printf (ustring *in_ustr, const char *fmt, ...)
{
    gboolean ret_val;
    va_list ap;
    va_start (ap, fmt);

    ret_val = ustr_vprintf(in_ustr, fmt, ap);

    va_end(ap);

    return ret_val;
}

gboolean
ustr_printfa (ustring *in_ustr, const char *fmt, ...)
{
    gboolean ret_val;
    va_list ap;
    va_start (ap, fmt);

    ret_val = ustr_vprintfa(in_ustr, fmt, ap);

    va_end(ap);

    return ret_val;
}

gboolean
ustr_vprintf (ustring *in_ustr, const char *fmt, va_list args)
{
    ustr_clear(in_ustr);
    return ustr_vprintfa(in_ustr, fmt, args);
}

gboolean
ustr_vprintfa (ustring *in_ustr, const char *fmt, va_list args)
{
    gchar *text = g_strdup_vprintf(fmt, args);

    if (text == NULL)
    {
        return FALSE;
    }

    ustr_strcat(in_ustr, text);

    g_free(text);

    clear_cached_data(in_ustr);

    return TRUE;
}

static char
hex_dump_char(char c)
{
    static char null_char = '.';

    if (c == '\n'
        || c == '\r')
        return null_char;
    if (isprint(c))
        return c;
    return null_char;
}

gboolean
ustr_hexstream (ustring *in_ustr, const guint8 *buffer,
                int start, int stop, char in_sep)
{
    ustr_clear(in_ustr);
    return ustr_hexstreama(in_ustr, buffer, start, stop, in_sep);
}

gboolean
ustr_hexstreama (ustring *in_ustr, const guint8 *buffer,
                 int start, int stop, char in_sep)
{
    USTR_PRECONDITION(in_ustr);
    int i;
    for (i = start; i < stop; i++)
    {
        if ((i + 1) == stop)
        {
            ustr_printfa (in_ustr, "%02X", *(buffer + i));
        }
        else
        {
            ustr_printfa (in_ustr, "%02X%c", *(buffer + i), in_sep);
        }
    }
    USTR_POSTCONDITION(in_ustr);
    return TRUE;
}

gboolean
ustr_hexdump (ustring *in_ustr, const guint8 *buffer, int start,
              int stop, gboolean ascii, gboolean in_include_colon)
{
    ustr_clear(in_ustr);
    return ustr_hexdumpa(in_ustr, buffer, start, stop, ascii,
                         in_include_colon);
}

gboolean
ustr_hexdumpa (ustring *in_ustr, const guint8 *buffer, int start,
               int stop, gboolean ascii, gboolean in_include_colon)
{
    USTR_PRECONDITION(in_ustr);
    int i;

    const char *line_start;
    if (in_include_colon)
    {
        line_start = "%08X:  ";
    }
    else
    {
        line_start = "%08X   ";
    }

    while (start < stop)
    {
        int diff = stop - start;
        if (diff > 16)
        {
            diff = 16;
        }

        ustr_printfa (in_ustr, line_start, start);
        ustr_hexstreama(in_ustr, buffer,
                        start, MIN(stop, start + 8),
                        ' ');
        if ((start + 8) < stop)
        {
            ustr_printfa (in_ustr, "  ");
            ustr_hexstreama(in_ustr, buffer,
                            start + 8, MIN(stop, start + 16),
                            ' ');
        }

        if (ascii)
        {
            for (i = diff; i < 16; i++)
            {
                if (i == 8)
                {
                    ustr_printfa (in_ustr, "    ");
                }
                else if (i == 15)
                {
                    ustr_printfa (in_ustr, "   ");
                }
                else
                {
                    ustr_printfa (in_ustr, "   ");
                }
            }

            ustr_printfa (in_ustr, "  ");

            for (i = 0; i < diff; i++)
            {
                char c = *(buffer + start + i);
                ustr_printfa (in_ustr, "%c", hex_dump_char(c));
            }
        }

        ustr_printfa (in_ustr, "\n");
        start += 16;
    }
    USTR_POSTCONDITION(in_ustr);
    return TRUE;
}
