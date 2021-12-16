/**
 ** This source file is lifted directly from the FiSH project.
 ** I'm not happy having to use this, as not only are we linked
 ** against libcrypto, but lion has base64. But for some reason
 ** they went with non-standard functions for this.
 **/
#if HAVE_CONFIG_H
#include <config.h>
#endif



#include "blowfish.h"

/* #define S(x,i) (bf_S[i][x.w.byte##i]) */
#define S0(x) (bf_S[0][x.w.byte0])
#define S1(x) (bf_S[1][x.w.byte1])
#define S2(x) (bf_S[2][x.w.byte2])
#define S3(x) (bf_S[3][x.w.byte3])
#define bf_F(x) (((S0(x) + S1(x)) ^ S2(x)) + S3(x))
#define ROUND(a,b,n) (a.word ^= bf_F(b) ^ bf_P[n])

/* Each box takes up 4k so be very careful here */
//#define BOXES 3
/* Keep a set of rotating P & S boxes
static struct box_t {
  u_32bit_t *P;
  u_32bit_t **S;
  char key[81];
  char keybytes;
} box[BOXES];*/
//static u_32bit_t *bf_P;
//static u_32bit_t **bf_S;

static u_32bit_t bf_P[bf_N+2];
static u_32bit_t bf_S[4][256];



static void blowfish_encipher(u_32bit_t * xl, u_32bit_t * xr)
{
  union aword Xl;
  union aword Xr;

  Xl.word = *xl;
  Xr.word = *xr;

  Xl.word ^= bf_P[0];
  ROUND(Xr, Xl, 1);
  ROUND(Xl, Xr, 2);
  ROUND(Xr, Xl, 3);
  ROUND(Xl, Xr, 4);
  ROUND(Xr, Xl, 5);
  ROUND(Xl, Xr, 6);
  ROUND(Xr, Xl, 7);
  ROUND(Xl, Xr, 8);
  ROUND(Xr, Xl, 9);
  ROUND(Xl, Xr, 10);
  ROUND(Xr, Xl, 11);
  ROUND(Xl, Xr, 12);
  ROUND(Xr, Xl, 13);
  ROUND(Xl, Xr, 14);
  ROUND(Xr, Xl, 15);
  ROUND(Xl, Xr, 16);
  Xr.word ^= bf_P[17];

  *xr = Xl.word;
  *xl = Xr.word;
}



static void blowfish_decipher(u_32bit_t * xl, u_32bit_t * xr)
{
  union aword Xl;
  union aword Xr;

  Xl.word = *xl;
  Xr.word = *xr;

  Xl.word ^= bf_P[17];
  ROUND(Xr, Xl, 16);
  ROUND(Xl, Xr, 15);
  ROUND(Xr, Xl, 14);
  ROUND(Xl, Xr, 13);
  ROUND(Xr, Xl, 12);
  ROUND(Xl, Xr, 11);
  ROUND(Xr, Xl, 10);
  ROUND(Xl, Xr, 9);
  ROUND(Xr, Xl, 8);
  ROUND(Xl, Xr, 7);
  ROUND(Xr, Xl, 6);
  ROUND(Xl, Xr, 5);
  ROUND(Xr, Xl, 4);
  ROUND(Xl, Xr, 3);
  ROUND(Xr, Xl, 2);
  ROUND(Xl, Xr, 1);
  Xr.word ^= bf_P[0];

  *xl = Xr.word;
  *xr = Xl.word;
}


static void blowfish_init(u_8bit_t * key, int keybytes)
{
  int i, j;
  u_32bit_t data;
  u_32bit_t datal;
  u_32bit_t datar;
  union aword temp;

  // Fixes crash if key is longer than 80 char. This may cause the key
  // to not end with \00 but that's no problem.
  if (keybytes > 80) keybytes = 80;

  // Reset blowfish boxes to initial state
  for (i = 0; i < bf_N + 2; i++) bf_P[i] = initbf_P[i];
  for (i = 0; i < 4; i++)
    for (j = 0; j < 256; j++)
      bf_S[i][j] = initbf_S[i][j];

  j = 0;
  if (keybytes > 0) {
    for (i = 0; i < bf_N + 2; ++i) {
      temp.word = 0;
      temp.w.byte0 = key[j];
      temp.w.byte1 = key[(j + 1) % keybytes];
      temp.w.byte2 = key[(j + 2) % keybytes];
      temp.w.byte3 = key[(j + 3) % keybytes];
      data = temp.word;
      bf_P[i] = bf_P[i] ^ data;
      j = (j + 4) % keybytes;
    }
  }
  datal = 0x00000000;
  datar = 0x00000000;
  for (i = 0; i < bf_N + 2; i += 2) {
    blowfish_encipher(&datal, &datar);
    bf_P[i] = datal;
    bf_P[i + 1] = datar;
  }
  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 256; j += 2) {
      blowfish_encipher(&datal, &datar);
      bf_S[i][j] = datal;
      bf_S[i][j + 1] = datar;
    }
  }
}


/* decode base64 string */

static int base64dec(char c)
{
  int i;

  for (i = 0; i < 64; i++)
    if (B64[i] == c) return i;

  return 0;
}


/* Returned string must be freed when done with it! */
int encrypt_string(char *key, char *str, char *dest, int len)
{
  u_32bit_t left, right;
  unsigned char *p;
  char *s, *d;
  int i;

  /* Pad fake string with 8 bytes to make sure there's enough */
  s = (char *) malloc(len + 9);
  strncpy(s, str, len);
  s[len]=0;
  if ((!key) || (!key[0])) return 0;
  p = s;
  while (*p) p++;
  for (i = 0; i < 8; i++) *p++ = 0;
  blowfish_init((unsigned char *) key, strlen(key));
  p = s;
  d = dest;
  while (*p) {
    left = ((*p++) << 24);
    left += ((*p++) << 16);
    left += ((*p++) << 8);
    left += (*p++);
    right = ((*p++) << 24);
    right += ((*p++) << 16);
    right += ((*p++) << 8);
    right += (*p++);
    blowfish_encipher(&left, &right);
    for (i = 0; i < 6; i++) {
      *d++ = B64[right & 0x3f];
      right = (right >> 6);
    }
    for (i = 0; i < 6; i++) {
      *d++ = B64[left & 0x3f];
      left = (left >> 6);
    }
  }
  *d = 0;
  free(s);
  return 1;
}



int decrypt_string(char *key, char *str, char *dest, int len)
{
  u_32bit_t left, right;
  char *p, *s, *d;
  int i;

  /* Pad encoded string with 0 bits in case it's bogus */
  if ((!key) || (!key[0])) return 0;
  s = (char *) malloc(len + 12);
  strncpy(s, str, len);
  s[len]=0;
  p = s;
  while (*p) p++;
  for (i = 0; i < 12; i++) *p++ = 0;
  blowfish_init((unsigned char *) key, strlen(key));
  p = s;
  d = dest;
  while (*p) {
    right = 0L;
    left = 0L;
    for (i = 0; i < 6; i++) right |= (base64dec(*p++)) << (i * 6);
    for (i = 0; i < 6; i++) left |= (base64dec(*p++)) << (i * 6);
    blowfish_decipher(&left, &right);
    for (i = 0; i < 4; i++) *d++ = (left & (0xff << ((3 - i) * 8))) >> ((3 - i) * 8);
    for (i = 0; i < 4; i++) *d++ = (right & (0xff << ((3 - i) * 8))) >> ((3 - i) * 8);
  }
  *d = 0;
  free(s);
  return 1;
}


#if 0
int decrypt_key(char *theData)
{
	int i;
	unsigned char theContactKey[500];

	i=strlen(theData);
	if(i >= 500) return;

	if(strncmp(theData, "+OK ", 4) != 0) return;	// key is stored plain-text

	strcpy(theContactKey, theData+4);
	decrypt_string(iniKey, theContactKey, theData, i-4);

	ZeroMemory(theContactKey, sizeof(theContactKey));
}


int encrypt_key(char *theData)
{
	int i;
	unsigned char theContactKey[500];

	i=strlen(theData);
	if(i >= 500) return;

	strcpy(theContactKey, theData);

	strcpy(theData, "+OK ");
	encrypt_string(iniKey, theContactKey, theData+4, i);

	ZeroMemory(theContactKey, sizeof(theContactKey));
}
#endif
