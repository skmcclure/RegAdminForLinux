/*
 * chntpw.c - Offline Password Edit Utility for NT 3.51 4.0 5.0 5.1 SAM database.
 * 1999-feb: Now able to browse registry hives. (write support to come)
 * 2000-jan: Attempt to detect and disable syskey
 * 2000-jun: syskey disable works on NT4. Not properly on NT5.
 * 2000-jun: changing passwords regardless of syskey.
 * 2001-jan: patched & changed to use OpenSSL. Thanks to Denis Ducamp
 * 2001-jul: extra blank password logic (when NT or LANMAN hash missing)
 * 2002-dec: New option: blank the pass (zero hash lengths).
 * 2002-dec: New option: Specify user using RID
 * 2003-jan: Support in ntreg for adding keys etc. Editor updated.
 * 2003-jan: Changed to use more of struct based V + some small stuff
 * 2004-jan: Changed some of the verbose/debug stuff
 * 2004-aug: More stuff in regedit. Stringinput bugfixes.
 *
 * Copyright (c) 1997-2004 Petter Nordahl-Hagen.
 * Freely distributable in source or binary for noncommercial purposes,
 * but I allow some exceptions to this.
 * Please see the COPYING file for more details on
 * copyrights & credits.
 * 
 * Part of some routines, information and ideas taken from
 * pwdump by Jeremy Allison.
 *
 * Some stuff from NTCrack by Jonathan Wilkins.
 * 
 *  
 * THIS SOFTWARE IS PROVIDED BY PETTER NORDAHL-HAGEN `AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/des.h>
#include <openssl/md4.h>
#define uchar u_char
#define MD4Init MD4_Init
#define MD4Update MD4_Update
#define MD4Final MD4_Final

const char chntpw_version[] = "chntpw version 0.99.3 040818, (c) Petter N Hagen";

#define MAX_HIVES 10

/* Array of loaded hives */
struct hive *hive[MAX_HIVES+1];
int no_hives = 0;

/* Icky icky... globals used to refer to hives, will be
 * set when loading, so that hives can be loaded in any order
 */

int H_SAM = -1;
int H_SYS = -1;
int H_SEC = -1;
int H_SOF = -1;

int syskeyreset = 0;
int dirty = 0;
int max_sam_lock = 0;

/*
 * of user with RID 500, because silly MS decided
 * to localize the bloody admin-username!! AAAGHH!
 */
char admuser[129]="Administrator";

/* ============================================================== */

/* Crypto-stuff & support for what we'll do in the V-value */

/* Zero out string for lanman passwd, then uppercase
 * the supplied password and put it in here */

static void make_lanmpw(const char *p, char *lm, int len)
{
   int i;

   for (i=0; i < 15; i++) lm[i] = 0;
   for (i=0; i < len; i++) lm[i] = toupper(p[i]);
}

/*
 * Convert a 7 byte array into an 8 byte des key with odd parity.
 */

static void str_to_key(unsigned char *str,unsigned char *key)
{
	int i;

	key[0] = str[0]>>1;
	key[1] = ((str[0]&0x01)<<6) | (str[1]>>2);
	key[2] = ((str[1]&0x03)<<5) | (str[2]>>3);
	key[3] = ((str[2]&0x07)<<4) | (str[3]>>4);
	key[4] = ((str[3]&0x0F)<<3) | (str[4]>>5);
	key[5] = ((str[4]&0x1F)<<2) | (str[5]>>6);
	key[6] = ((str[5]&0x3F)<<1) | (str[6]>>7);
	key[7] = str[6]&0x7F;
	for (i=0;i<8;i++) {
		key[i] = (key[i]<<1);
	}
	DES_set_odd_parity((des_cblock *)key);
}

/*
 * Function to convert the RID to the first decrypt key.
 */

static void sid_to_key1(unsigned long sid,unsigned char deskey[8])
{
	unsigned char s[7];

	s[0] = (unsigned char)(sid & 0xFF);
	s[1] = (unsigned char)((sid>>8) & 0xFF);
	s[2] = (unsigned char)((sid>>16) & 0xFF);
	s[3] = (unsigned char)((sid>>24) & 0xFF);
	s[4] = s[0];
	s[5] = s[1];
	s[6] = s[2];

	str_to_key(s,deskey);
}

/*
 * Function to convert the RID to the second decrypt key.
 */

static void sid_to_key2(unsigned long sid,unsigned char deskey[8])
{
	unsigned char s[7];

	s[0] = (unsigned char)((sid>>24) & 0xFF);
	s[1] = (unsigned char)(sid & 0xFF);
	s[2] = (unsigned char)((sid>>8) & 0xFF);
	s[3] = (unsigned char)((sid>>16) & 0xFF);
	s[4] = s[0];
	s[5] = s[1];
	s[6] = s[2];

	str_to_key(s,deskey);
}

/* DES encrypt, for LANMAN */

static void E1(uchar *k, uchar *d, uchar *out)
{
  des_key_schedule ks;
  des_cblock deskey;

  str_to_key(k,(uchar *)deskey);
#ifdef __FreeBSD__
  des_set_key(&deskey,ks);
#else /* __FreeBsd__ */
  des_set_key((des_cblock *)deskey,ks);
#endif /* __FreeBsd__ */
  des_ecb_encrypt((des_cblock *)d,(des_cblock *)out, ks, DES_ENCRYPT);
}

static void cheap_ascii2uni(const char *src, char *dest, int l)
{
   for (; l > 0; l--) {
      *dest++ = *src++;
      *dest++ = 0;

   }
}

static void hexprnt(const char *s, unsigned char *bytes, int len)
{
int i;

   printf("%s",s);
   for (i = 0; i < len; i++) {
      printf("%02x ",bytes[i]);
   }
   printf("\n");
}


/* Decode the V-struct, and change the password
 * vofs - offset into SAM buffer, start of V struct
 * rid - the users RID, required for the DES decrypt stage
 *
 * Some of this is ripped & modified from pwdump by Jeremy Allison
 *
 */
static void show_pws(int rid, const char *pw)
{
   uchar x1[] = {0x4B,0x47,0x53,0x21,0x40,0x23,0x24,0x25};
   int pl;
   char lanman[32];
   char newunipw[34], despw[20], newlanpw[16], newlandes[20];

   des_key_schedule ks1, ks2;
   des_cblock deskey1, deskey2;

   MD4_CTX context;
   unsigned char digest[16];

   /* Get the two decrpt keys. */
   sid_to_key1(rid,(unsigned char *)deskey1);
   des_set_key((des_cblock *)deskey1,ks1);
   sid_to_key2(rid,(unsigned char *)deskey2);
   des_set_key((des_cblock *)deskey2,ks2);

   pl = strlen(pw);

   cheap_ascii2uni(pw,newunipw,pl);

   make_lanmpw(pw,newlanpw,pl);

   printf("Rid: %d Pw: %s\n", rid, pw);

   MD4Init (&context);
   MD4Update (&context, newunipw, pl<<1);
   MD4Final (digest, &context);

   hexprnt("NEW MD4 hash    : ",digest,16);

   E1((uchar*)newlanpw,   x1, (uchar*)lanman);
   E1((uchar*)newlanpw+7, x1, (uchar*)lanman+8);

   hexprnt("NEW LANMAN hash : ",(uchar*)lanman,16);

   /* Encrypt the NT md4 password hash as two 8 byte blocks. */
   des_ecb_encrypt((des_cblock *)digest,
                   (des_cblock *)despw, ks1, DES_ENCRYPT);
   des_ecb_encrypt((des_cblock *)(digest+8),
                   (des_cblock *)&despw[8], ks2, DES_ENCRYPT);

   des_ecb_encrypt((des_cblock *)lanman,
                   (des_cblock *)newlandes, ks1, DES_ENCRYPT);
   des_ecb_encrypt((des_cblock *)(lanman+8),
                   (des_cblock *)&newlandes[8], ks2, DES_ENCRYPT);

   hexprnt("NEW DES crypt   : ",(uchar*)despw,16);
   hexprnt("NEW LANMAN crypt: ",(uchar*)newlandes,16);

}

int
main (int argc, char **argv)
{
    if (argc != 3)
    {
        return 1;
    }

    int rid = atol(argv[1]);
    const char *pw = argv[2];

    show_pws(rid, pw);

    return 0;
}
