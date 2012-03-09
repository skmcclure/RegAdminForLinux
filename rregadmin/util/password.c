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

#include <string.h>
#include <ctype.h>

#include <openssl/des.h>
#include <openssl/md4.h>

#include <rregadmin/util/password.h>
#include <rregadmin/util/ustring.h>
#include <rregadmin/util/log.h>
#include <rregadmin/util/malloc.h>

#ifdef HAVE_UNDERSCORE_M4_FUNCS
#  define MD4Init MD4_Init
#  define MD4Update MD4_Update
#  define MD4Final MD4_Final
#endif

#ifdef HAVE_UPPER_CASE_DES_FUNCS
#  ifndef des_set_key
#    define des_set_key DES_set_key
#  endif
#  ifndef des_ecb_encrypt
#    define des_ecb_encrypt DES_ecb_encrypt
#  endif
#endif

#if 0
static void rid_pw_decrypt (int rid, guint8 *dest, char *src);
#endif
static void rid_pw_encrypt (int rid, guint8 *dest, char *src);
static void E1 (char * k, const char * d, char * out);
static void make_lanmpw (const char *p, char *lm, int len);
static void str_to_key (unsigned char *str, unsigned char *key);
static void sid_to_key1 (guint32 sid, guint8 deskey[8]);
static void sid_to_key2 (guint32 sid, guint8 deskey[8]);
static void initialize_schedule (int rid, des_key_schedule * ks);

struct win_password_
{
    rid_type rid;
    ustring *pw;
    guint8 *nthash;
    guint8 *lmhash;
};

static win_password*
winpw_new_base(rid_type in_id)
{
    win_password *ret_val = rra_new_type(win_password);
    ret_val->rid = in_id;
    ret_val->pw = ustr_new();
    return ret_val;
}

win_password*
winpw_new(rid_type in_id, const char *in_pw)
{
    win_password *ret_val = winpw_new_base(in_id);
    ustr_strset(ret_val->pw, in_pw);
    return ret_val;
}

win_password*
winpw_new_encoded(rid_type in_id, const guint8 *in_nthash,
                  const guint8 *in_lmhash)
{
    if (in_nthash == NULL
        || in_lmhash == NULL)
    {
        return NULL;
    }
    int i;
    win_password *ret_val = winpw_new_base(in_id);
    ret_val->nthash = (guint8*)rra_new(sizeof(guint8) * 16);
    ret_val->lmhash = (guint8*)rra_new(sizeof(guint8) * 16);
    for (i = 0; i < 16; i++)
    {
        ret_val->nthash[i] = in_nthash[i];
        ret_val->lmhash[i] = in_lmhash[i];
    }
    return ret_val;
}

gboolean
winpw_free(win_password *in_wp)
{
    if (in_wp == NULL)
    {
        return FALSE;
    }

    ustr_free(in_wp->pw);

    if (in_wp->lmhash != NULL)
    {
        rra_free(sizeof(guint8) * 16, in_wp->lmhash);
        in_wp->lmhash = NULL;
    }
    if (in_wp->nthash != NULL)
    {
        rra_free(sizeof(guint8) * 16, in_wp->nthash);
        in_wp->nthash = NULL;
    }

    rra_free_type(win_password, in_wp);

    return TRUE;
}

const ustring*
winpw_get_pw(const win_password *in_wp)
{
    if (in_wp == NULL)
    {
        return NULL;
    }

    return in_wp->pw;
}

rid_type
winpw_get_rid(const win_password *in_wp)
{
    if (in_wp == NULL)
    {
        return RID_INVALID;
    }

    return in_wp->rid;
}

static void
generate_lmhash (win_password *in_wp)
{
    int len;

    static const char x1[] = { 0x4B, 0x47, 0x53, 0x21,
                               0x40, 0x23, 0x24, 0x25 };

    char buf[16];
    char buf2[16];

    len = ustr_strlen (in_wp->pw);

    if (len >= 15)
    {
        rra_warning("Password too long for lmhash");
        return;
    }

    in_wp->lmhash = (guint8*)rra_new(sizeof(guint8) * 16);

    make_lanmpw (ustr_as_utf8(in_wp->pw), buf, len);

    E1 (buf, x1, buf2);
    E1 (buf + 7, x1, buf2 + 8);

    rid_pw_encrypt (in_wp->rid, in_wp->lmhash, buf2);
}

const guint8*
winpw_get_lmhash(win_password *in_wp)
{
    if (in_wp == NULL)
    {
        return NULL;
    }

    if (in_wp->lmhash == NULL)
    {
        generate_lmhash(in_wp);
    }

    return in_wp->lmhash;
}

static void
generate_nthash (win_password *in_wp)
{
    MD4_CTX context;
    unsigned char digest[16];
    GByteArray *arr;

    arr = ustr_encode(in_wp->pw, USTR_TYPE_UTF16LE);

    MD4Init (&context);
    MD4Update (&context, arr->data, arr->len - 2);
    MD4Final (digest, &context);

    g_byte_array_free(arr, TRUE);

    in_wp->nthash = (guint8*)rra_new(sizeof(guint8) * 16);

    rid_pw_encrypt (in_wp->rid, in_wp->nthash, (char*)digest);
}

const guint8*
winpw_get_nthash(win_password *in_wp)
{
    if (in_wp == NULL)
    {
        return NULL;
    }

    if (in_wp->nthash == NULL)
    {
        generate_nthash((win_password*)in_wp);
    }

    return in_wp->nthash;
}

#if 0
static void
rid_pw_decrypt (int rid, guint8 *dest, char *src)
{
    des_key_schedule ks[2];
    initialize_schedule (rid, ks);

    /* Decrypt the NT md4 password hash as two 8 byte blocks. */
    des_ecb_encrypt ((des_cblock *) src, (des_cblock *) dest, ks[0],
                     DES_DECRYPT);
    des_ecb_encrypt ((des_cblock *) (src + 8), (des_cblock *) (dest + 8),
                     ks[1], DES_DECRYPT);
}
#endif

static void
rid_pw_encrypt (int rid, guint8 *dest, char *src)
{
    des_key_schedule ks[2];
    initialize_schedule (rid, ks);

    des_ecb_encrypt ((des_cblock *) src, (des_cblock *) dest, ks[0],
                     DES_ENCRYPT);
    des_ecb_encrypt ((des_cblock *) (src + 8), (des_cblock *) (dest + 8),
                     ks[1], DES_ENCRYPT);
}

static void
make_lanmpw (const char *p, char *lm, int len)
{
    int i;

    for (i = 0; i < 15; i++)
        lm[i] = 0;
    for (i = 0; i < len; i++)
        lm[i] = toupper (p[i]);
}

// Convert a 7 byte array into an 8 byte des key with odd parity.
static void
str_to_key (unsigned char *str, unsigned char *key)
{
    int i;

    key[0] = str[0] >> 1;
    key[1] = ((str[0] & 0x01) << 6) | (str[1] >> 2);
    key[2] = ((str[1] & 0x03) << 5) | (str[2] >> 3);
    key[3] = ((str[2] & 0x07) << 4) | (str[3] >> 4);
    key[4] = ((str[3] & 0x0F) << 3) | (str[4] >> 5);
    key[5] = ((str[4] & 0x1F) << 2) | (str[5] >> 6);
    key[6] = ((str[5] & 0x3F) << 1) | (str[6] >> 7);
    key[7] = str[6] & 0x7F;
    for (i = 0; i < 8; i++)
    {
        key[i] = (key[i] << 1);
    }
    DES_set_odd_parity ((des_cblock *) key);
}

// DES encrypt, for LANMAN
static void
E1 (char * k, const char * d, char * out)
{
    des_key_schedule ks;
    des_cblock deskey;

    str_to_key ((unsigned char*)k, (unsigned char *) &deskey);
    des_set_key (&deskey, ks);
    des_ecb_encrypt ((des_cblock *) d, (des_cblock *) out, ks, DES_ENCRYPT);
}


// Function to convert the RID to the first decrypt key.
static void
sid_to_key1 (guint32 sid, guint8 deskey[8])
{
    unsigned char s[7];

    s[0] = (unsigned char) (sid & 0xFF);
    s[1] = (unsigned char) ((sid >> 8) & 0xFF);
    s[2] = (unsigned char) ((sid >> 16) & 0xFF);
    s[3] = (unsigned char) ((sid >> 24) & 0xFF);
    s[4] = s[0];
    s[5] = s[1];
    s[6] = s[2];

    str_to_key (s, deskey);
}

// Function to convert the RID to the second decrypt key.
static void
sid_to_key2 (guint32 sid, guint8 deskey[8])
{
    unsigned char s[7];

    s[0] = (unsigned char) ((sid >> 24) & 0xFF);
    s[1] = (unsigned char) (sid & 0xFF);
    s[2] = (unsigned char) ((sid >> 8) & 0xFF);
    s[3] = (unsigned char) ((sid >> 16) & 0xFF);
    s[4] = s[0];
    s[5] = s[1];
    s[6] = s[2];

    str_to_key (s, deskey);
}

static void
initialize_schedule (int rid, des_key_schedule *ks)
{
    des_cblock deskey1;
    des_cblock deskey2;

    /* Get the two decrypt keys. */
    sid_to_key1 (rid, (unsigned char *) deskey1);
    des_set_key ((des_cblock *) deskey1, ks[0]);
    sid_to_key2 (rid, (unsigned char *) deskey2);
    des_set_key ((des_cblock *) deskey2, ks[1]);
}
