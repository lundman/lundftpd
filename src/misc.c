
#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef LINUX
#define __USE_BSD
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <time.h>
#include <time.h>
#include <ctype.h>

#include <sys/types.h>


#if HAVE_DIRENT_H
# include <dirent.h>
#endif

#include "help.h"
#include "global.h"
#include "userflag.h"
#include "log.h"
#include "lfnmatch.h"
#include "misc.h"


char genbuf[2048];              // General scratch buffer
char formatted_data[16];
struct misc_patt *global_patts = NULL; // Pattern head




void chop(char *s)
{
  char *r;

  if ((r=(char *) strchr(s, '\r')))
    *r = (char) 0;
  if ((r=(char *) strchr(s, '\n')))
    *r = (char) 0;
}

void chop2(char *s)
{
  char *r;

  while((r=(char *) strchr(s, '\r')))
    *r = (char) ' ';

  if ((r=(char *) strchr(s, '\n')))
    *r = (char) 0;
}

char *digtoken(char **string,char *match)
{
  if(string && *string && **string) {

    while(**string && strchr(match,**string))
      (*string)++;

    if(**string) { /* got something */
      char *token=*string;

      if((*string=strpbrk(*string,match))) {

	*(*string)++=(char)0;
	while(**string && strchr(match,**string))
	  (*string)++;

      }  else
	*string = ""; /* must be at the end */

      return(token);
    }
  }
  return((char *)0);
}



static unsigned char itoa64[] =         /* 0 ... 63 => ascii - 64 */
"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void to64(s, v, n)
     register char *s;
     register long v;
     register int n;
{
  while (--n >= 0) {
    *s++ = itoa64[v&0x3f];
    v >>= 6;
  }
}


char *idletime(time_t time)
{ /* This support functionw as written by George Shearer (Dr_Delete) */

  static char workstr[100];
  unsigned short int days=(time/86400),hours,mins,secs;
  hours=((time-(days*86400))/3600);
  mins=((time-(days*86400)-(hours*3600))/60);
  secs=(time-(days*86400)-(hours*3600)-(mins*60));

  workstr[0]=(char)0;
  if(days)
    sprintf(workstr,"%dd",days);
  if(hours)
    sprintf(workstr,"%s%s%dh",workstr,(workstr[0])?", ":"",hours);
  if(mins)
    sprintf(workstr,"%s%s%dm",workstr,(workstr[0])?", ":"",mins);
  if(secs)
    sprintf(workstr,"%s%s%ds",workstr,(workstr[0])?", ":"",secs);
  if (!days && !hours && !mins && !secs)
    sprintf(workstr,"0 seconds");

  return(workstr);
}

char *idletime2(time_t time)
{ /* This support functionw as written by George Shearer (Dr_Delete) */

  static char workstr[100];
  unsigned short int days=(time/86400),hours,mins,secs;
  hours=((time-(days*86400))/3600);
  mins=((time-(days*86400)-(hours*3600))/60);
  secs=(time-(days*86400)-(hours*3600)-(mins*60));

  workstr[0]=(char)0;
  if(days)
    sprintf(workstr,"%dd",days);
  if(hours)
    sprintf(workstr,"%s%s%2dh",workstr,(workstr[0])?" ":"",hours);
  if(mins)
    sprintf(workstr,"%s%s%2dm",workstr,(workstr[0])?" ":"",mins);
  if(secs)
    sprintf(workstr,"%s%s%2ds",workstr,(workstr[0])?" ":"",secs);
  if (!days && !hours && !mins && !secs)
    sprintf(workstr,"0 seconds");

  return(workstr);
}


// "2 gigs 576 megs 123 ks."
char *byte_str(lion64u_t bytes)
{
  static char work[100];
  int gigs, megs, kays;

  work[0] = 0;

  gigs = bytes / ((lion64u_t)1024 * 1024 * 1024);

  megs = (bytes -
	  ((lion64u_t)gigs * 1024 * 1024 * 1024)) /
    ((lion64u_t)1024 * 1024);

  kays = (bytes -
	  ((lion64u_t) gigs * 1024 * 1024 * 1024) -
	  ((lion64u_t) megs * 1024 * 1024)) /
    (lion64u_t)1024;

  if (gigs)
    sprintf(work, "%d gig%s",
	    gigs, gigs > 1 ? "s" : "");
  if (megs)
    sprintf(work, "%s%s%d meg%s", work, work[0] ? ", ":"",
	    megs, megs > 1 ? "s" : "");
  if (kays)
    sprintf(work, "%s%s%d k%s", work, work[0] ? ", ":"",
	    kays, kays > 1 ? "'s" : "");

  if (!gigs && !megs && !kays)
    sprintf(work, "0");

  return work;
}

// "325.2 MB"
// "12.5 GB"
// Since we return a static string, but might call this function multiple
// times, we add hacky code to use a different buffer. Blergh.
//
char *misc_bytestr(lion64u_t bytes)
{
  static char works[4][100];
  char *work;
  static int last = 0;

  if (last >= 3) last = 0;
  else last++;

  work = works[last];

  if (bytes >= 1099511627776LL)
	  snprintf(work, 100, "%.1f TB",
			   ((float)bytes) / 1099511627776.0);
  else if (bytes >= 1073741824)
	  snprintf(work, 100, "%.1f GB",
			   ((float)bytes) / 1073741824.0);
  else if (bytes >= 1048576)
	  snprintf(work, 100, "%.1f MB",
			   ((float)bytes) / 1048576.0);
  else if (bytes >= 1024)
	  snprintf(work, 100, "%.1f KB",
			   ((float)bytes) / 1024.0);
  else
	  snprintf(work, 100, "%.1f B",
			   ((float)bytes));

  return work;
}




int cmptail(char *string, char *sub)
{
  int l1 = strlen(string), l2 = strlen(sub);
  if (l1 < l2)
    return 0;
  string = string + l1 - l2;
  if (strcasecmp(string, sub))
    return 0;
  return 1;
}


void misc_getargs(char *full, char *args, char *path)
{
  /*
   * Take 'full' and extract the arguments into args and leave the path
   * in 'path'.
   */

  char *r;

  r = full;
  while(*r && (*r == ' ')) r++;

  while(*r == '-') {  /* Ooohh, we have an argument */

    if (r[1] == '-') {
      while(*r && ((*r=='-') || (*r == ' '))) r++;
      break;
    }

    while(*r && (*r != ' '))  /* Copy over argument bit */
      *args++ = *r++;

    while(*r && (*r == ' ')) r++; /* skip white spaces */
  }

  *args = (char) 0;

  strcpy(path, r); /* Copy the rest over */

  consolef("getargs(): '%s' to '%s' and '%s'\n", full, args, path);
}


char *path2file(char *s)
{ /* Just returns the last bit of a path, ie after last '/' */
  char *r;

  if (!(r = (char *)strrchr(s, '/')))
    return s;
  return &r[1];
}



/*
  Take a string, of format XXX|YYY|ZZZ|~AAA and produce uint.
  XXX etc are defined in hmm...userflag.h?
 */


#define TOINT(q,w,e,r) (((q)<<24) + ((w)<<16) + ((e)<<8) + (r))


unsigned int atoflags(char *s)
{
  char *part, par[50];
  int neg;
  unsigned int flag, returnflag;

  returnflag = 0;

  while(( part = digtoken(&s, "|"))) {
    neg = 0;
    flag = 0;

    if (*part == '~') {
      neg = 1;
      part++;
    }

	// Skip past UF_ if it was given.
    if (strncmp(part, "UF_", 3))
		help_get4letter(&part[0], par);
	else
		help_get4letter(&part[3], par);

    /*
      ALL, KICK, OVERWRITE, DELETE, DIRDELETE, STAT, PASSWORD(*), RESTART(*),
      PUNISH(*), MOVE, LOGINS
     */

    switch(TOINT(par[0], par[1], par[2], par[3])) {
    case TOINT('A','L','L',' '):
      flag = UF_ALL;
      break;
    case TOINT('N','O','N','E'):
      flag = UF_NONE;
      break;
    case TOINT('O','V','E','R'):
      flag = UF_OVERWRITE;
      break;
    case TOINT('K','I','C','K'):
      flag = UF_KICK;
      break;
    case TOINT('D','E','L','E'):
      flag = UF_DELETE;
      break;
    case TOINT('D','I','R','D'):
      flag = UF_DIRDELETE;
      break;
    case TOINT('G','R','O','U'):
      flag = UF_GROUP;
      break;
    case TOINT('P','A','S','S'):
      flag = UF_PASSWORD;
      break;
    case TOINT('S','T','A','T'):
      flag = UF_STAT;
      break;
    case TOINT('D','U','P','E'):
      flag = UF_DUPE;
      break;
    case TOINT('C','R','E','D'):
      flag = UF_CREDITS;
      break;
    case TOINT('A','D','D','U'):
      flag = UF_ADDUSER;
      break;
    case TOINT('S','E','T','I'):
      flag = UF_SETIP;
      break;
    case TOINT('S','E','T','R'):
      flag = UF_SETRATIO;
      break;
    case TOINT('S','E','T','L'):
      flag = UF_SETLIMIT;
      break;
    case TOINT('P','U','N','I'):
      flag = UF_PUNISH;
      break;
    case TOINT('M','O','V','E'):
      flag = UF_MOVE;
      break;
    case TOINT('S','E','E','A'):
      flag = UF_SEEALL;
      break;
    case TOINT('S','E','T','F'):
      flag = UF_SETFLAGS;
      break;
    default:
      consolef("Unknown user flag '%s'\n", part);
      continue; /* Continue works inside switch() to mean the while() */
    }

    /* Now, apply the current flag,
       They are being silly if we have ~NONE to be ALL, or ~ALL to be none
       But I don't care */

    if (neg) { /* Remove, not add */
      returnflag &= ~flag;
    } else {   /* Add, not remove */
      returnflag |= flag;
    }
  }

  return returnflag;

}



char *flagstoa(unsigned int flags)
{
  char *r;

  /* Uses genbuf */
  genbuf[0] = (char) 0;

  if (flags == UF_NONE)
    strcpy(genbuf, "UF_NONE");
  else if (flags == UF_ALL)    strcpy(genbuf, "UF_ALL");
  else {
    if (flags & UF_OVERWRITE) strcat(genbuf, "UF_OVERWRITE|");
    if (flags & UF_DELETE)    strcat(genbuf, "UF_DELETE|");
    if (flags & UF_DIRDELETE) strcat(genbuf, "UF_DIRDELETE|");
    if (flags & UF_GROUP)     strcat(genbuf, "UF_GROUP|");
    if (flags & UF_PASSWORD)  strcat(genbuf, "UF_PASSWORD|");
    if (flags & UF_STAT)      strcat(genbuf, "UF_STAT|");
    if (flags & UF_PUNISH)    strcat(genbuf, "UF_PUNISH|");
    if (flags & UF_MOVE)      strcat(genbuf, "UF_MOVE|");
    if (flags & UF_DUPE)      strcat(genbuf, "UF_DUPE|");
    if (flags & UF_CREDITS)   strcat(genbuf, "UF_CREDITS|");
    if (flags & UF_ADDUSER)   strcat(genbuf, "UF_ADDUSER|");
    if (flags & UF_SETIP)     strcat(genbuf, "UF_SETIP|");
    if (flags & UF_SETRATIO)  strcat(genbuf, "UF_SETRATIO|");
    if (flags & UF_SETLIMIT)  strcat(genbuf, "UF_SETLIMIT|");
    if (flags & UF_SEEALL)    strcat(genbuf, "UF_SEEALL|");
    if (flags & UF_SETFLAGS)  strcat(genbuf, "UF_SETFLAGS|");
    if (flags & UF_KICK)      strcat(genbuf, "UF_KICK|");
  }

  r =&genbuf[strlen(genbuf) -1];
  if (*r == '|') *r = (char) 0;

  return genbuf;
}





char *hide_path(char *s)
{
  char *r;

  if (server_usechroot && !strncmp(s, server_usechroot, strlen(server_usechroot))) {
    r = &s[strlen(server_usechroot)];

    while(*r == '/') r++;

    return r;

  }

  return s;
}



char *fixstring(char *src, int max)
{
  static char buffers[3][256];
  static int index = 0;

  index++;
  if (index >= 3) index = 0;


  strncpy(buffers[index], src, max);
  buffers[index][max] = 0;
  return buffers[index];
}


void strlower(char *s)
{

  for ( ; *s; s++) *s = tolower(*s);

}


void strcpylower(char *d, char *s)
{

  for ( ; *s; s++, d++) *d = tolower(*s);

  *d = 0;
}

char *postnumber(int i)
{

  switch (i % 10) {

  case 1:
    return "st";
  case 2:
    return "nd";
  case 3:
    return "rd";
  default:
    return "th";
  }
}



#ifdef WIN32
char *crypt(const char *key, const char *salt)
{

  return key;

}
#endif




int myfree(void **mem)
{


#undef free

  free(*mem);

  mem = (void *)-1;

  return 0;
}


int  misc_ishex(char c)
{

  switch(c) {

  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
  case 'A':
  case 'B':
  case 'C':
  case 'D':
  case 'E':
  case 'F':
  case 'a':
  case 'b':
  case 'c':
  case 'd':
  case 'e':
  case 'f':
    return 1;
  default:
    return 0;
  }

  return 0;

}





char *misc_itoa( int i )
{
	static char buf[40]; // longest int is only 9 chars.

	snprintf(buf, sizeof(buf), "%d", i);

	return buf;
}


int fnmatch_func(char *patt, char *path, int (*func)(const char *, void *), int flags,
				 void *arg)
{
	int i = 0;
	DIR *dh;
	struct dirent *dp;
	char buf[1024];

	if (!(dh = opendir(path)))
		return -1;

	while ((dp = readdir(dh)) != NULL) {

		//consolef("[misc] file '%s' type %d\n", dp->d_name, dp->d_type);

		// Skip "." and ".." ?
		if ((dp->d_name[0] == '.') && !dp->d_name[1])
			continue;
		if ((dp->d_name[0] == '.') && (dp->d_name[1] == '.') && !dp->d_name[2])
			continue;


#if !HAVE_STRUCT_DIRENT_D_TYPE
		//#if defined ( __sun__ ) || defined ( __linux__ ) || defined ( WIN32 )

		// DAMN! Solaris DON'T have the ->d_type field.
		// Linux has/defines them, but sets the to 0 always. "typical".

		if ( flags & (FNMATCH_FILESONLY | FNMATCH_DIRSONLY)) {
			char ptmp[8192];
			DIR *fake;

			// Which is more efficient, stat it, or attempt to opendir it?
			// opendir calls open(), then fstat(). So, stat it is.
			// One day we can change this code to stat().
			snprintf(ptmp, sizeof(ptmp), "%s/%s", path, dp->d_name);

			fake = opendir(ptmp);

			if (fake) { // directory

				closedir(fake);

				if (flags & FNMATCH_FILESONLY)
					continue;

			} else { // file

				if (flags & FNMATCH_DIRSONLY)
					continue;

			}

		}


#else  // BSD OS has d_type. If you get error here, add your OS above.
		// HAVE_STRUCT_DIRENT_D_TYPE

		if (dp->d_type && (flags & FNMATCH_FILESONLY) &&
			(dp->d_type != DT_REG)) continue;

		if (dp->d_type && (flags & FNMATCH_DIRSONLY) &&
			(dp->d_type != DT_DIR)) continue;

#endif

		//consolef("[misc] fnmatch comparing '%s' to '%s' flags %d\n",
		//dp->d_name, patt, flags);

		if (!lfnmatch(patt, dp->d_name, flags)) {

			//consolef("    match - calling\n");

			if (flags & FNMATCH_NOPRECAT) {

				i += func(dp->d_name, arg);

			} else {

				snprintf(buf, sizeof(buf), "%s/%s", path, dp->d_name);
				i += func(buf, arg);

			}
        }
    }
    closedir(dh);
    return i;
}


/* Most of these support routines were written by Simon McAuliffe */

static int get_char(FILE *f)
{
  int c;
  c = getc(f);
  if (c=='#')
    while ((c = getc(f)) != '\n') ;
  if (c == EOF)
    return EOF;
  if (c < ' ' && (c != 1))
    c = ' ';
  return c;
}


int get_word(FILE *f, char *w)
{
  int p = 0;
  int c;
  c = get_char(f);
  while (c == ' ')
    c = get_char(f);
  if (c == EOF)
    return 0;
  while (p < WORDLEN && c != ' ') {
    w[p++] = c;
    c = get_char(f);
    if (c == EOF)
      break;
  }
  w[p] = 0;
  return 1;
}

int get_int(FILE *f)
{
  char word[WORDLEN];
  get_word(f, word);
  if (word[0] == '*')
    return -1;
  return atoi(word);
}


long get_long(FILE *f)
{
  char word[WORDLEN];
  get_word(f, word);
  if (word[0] == '*')
    return -1;

#if 1   /* If you don't have stroul, change 1 into 0 */
  return strtoul(word, NULL, 10);
#else
  return atol(word);
#endif
}


lion64_t get_longlong(FILE *f)
{
  char word[WORDLEN];
  get_word(f, word);
  if (word[0] == '*')
    return -1;

#if HAVE_STRTOUQ       // unsigned 64 bit
  return strtouq(word, NULL, 10);
#elif HAVE_STRTOULL    // unsigned 64 bit
  return strtoull(word, NULL, 10);
#elif HAVE_STRTOLL     //   signed 64 bit
  return strtoll(word, NULL, 10);
#elif HAVE_STRTOUL     // unsigned 32 bit
  return (lion64u_t)strtoul(word, NULL, 10);
#else
  return atol(word);   //   signed 32 bit
#endif

}



int get_keyword(FILE *in, char *buf)
{
  int c;
  int pos = 0;

  while ((c = get_char(in)) != EOF) {

    if (!pos && (c == ' ')) continue;

    if ((c=='"') || (c=='\'') ) {

      while ((c = get_char(in)) != EOF) {
        if (c == '\\') {
          buf[pos++] = get_char(in);
          continue;
        }
        if ((c == '"') || (c=='\'')) {
          c = get_char(in);
          break;
        }
        buf[pos++] = c;
      }
    }

    if (c=='{' || c=='}' || c==',' || c==';') {
      if (!pos) {
        buf[pos++] = c;
        buf[pos] = 0;
        return 0;
      }
      ungetc(c, in);
    }

    if (!isalnum(c) && c != ':' && c != '_' && c != '|' && c!= '~') {
      buf[pos] = 0;
      return 0;
    }
    buf[pos++] = c;
  }
  return EOF;
}


char *get_string(FILE *f)
{
  char word[MAXKEYWORD], *w;

  if (get_keyword(f, word) != EOF)
    if ((w = (char *)malloc(strlen(word)+1))) {
      strcpy(w, word);
      return w;
    }

  fprintf(stderr,"Error parsing %s - in get_string()\r\n", CONFIG);
  return NULL;
}


// Pattern lists
void misc_patt_add(struct misc_patt **top, int type, char *pattern)
{

	struct misc_patt *mp;

	if ((mp = malloc(sizeof(struct misc_patt))) == NULL) {
		perror("malloc");
		exit(-1);
	}
	bzero(mp, sizeof(struct misc_patt));

	mp->type = type;
	mp->patt = pattern;
	mp->next = *top;
	*top = mp;

}

int misc_patt_match(struct misc_patt *top, int type, char *value)
{

	struct misc_patt *run;

	for (run = top; run; run = run->next) {
		if (run->type & type && !lfnmatch(run->patt, value, LFNM_CASEFOLD))
			return 1;
	}

	return 0;

}

char *misc_patt_list(struct misc_patt *top, int type)
{

	struct misc_patt *run;
	static char retstr[512];

	bzero(retstr, 512);

	for (run = top; run; run = run->next) {
		if (run->type & type)
			snprintf(retstr, 512, "%s %s", retstr, run->patt);
	}

	if (retstr[0])
		return retstr;

	return NULL;

}


char *path2deepest(char *path, int level)
{

	char *p;
	int slashes = 0;

	p = strrchr(path, 0);
	for (p = strrchr(path, 0); p != path; p--) {
		if (*p == '/')
			slashes++;
		if (slashes == level)
			return p+1;
	}

	return path;
}


void stripslashes(char *string)
{
	char *p;

	for (p = (string + strlen(string)) - 1; *p == '/' && p != string; p--)
		*p = 0;
}

char *pathsplit(char *s)
{
	char *r;

	if (!(r = (char *)strrchr(s, '/')))
		return s;
	*(&r[0]) = 0;
	return &r[1];
}

char *pathfixsplit(char *s, char *t)
{
	if (t > s)
		*(&t[-1]) = '/';
	return s;
}

char *misc_format_ratio(int ratio)
{
	bzero(formatted_data, 16);
	switch (ratio) {
		case -1:
			sprintf(formatted_data, "leech");
			break;
		case 0:
			sprintf(formatted_data, "none");
			break;
		default:
			snprintf(formatted_data, 15, "1:%d", ratio);
	}

	return formatted_data;
}



char *flagtowhat(int flag)
{

	if (!flag)
		return "No Flags";

	switch(flag) {
		case UF_OVERWRITE:
			return "Can Overwrite Files";
			break;
		case UF_DELETE:
			return "Can Delete Files";
			break;
		case UF_DIRDELETE:
			return "Can Delete Directories";
			break;
		case UF_GROUP:
			return "Can Add/Remove Groups";
			break;
		case UF_PASSWORD:
			return "Can Change Passwords";
			break;
		case UF_STAT:
			return "Can Stat?";
			break;
		case UF_PUNISH:
			return "Can Nuke";
			break;
		case UF_MOVE:
			return "Can Move Files";
			break;
		case UF_DUPE:
			return "Can Dupe/Undupe";
			break;
		case UF_CREDITS:
			return "Can Give Credits";
			break;
		case UF_ADDUSER:
			return "Can Add/Edit/Del Users";
			break;
		case UF_SETIP:
			return "Can Add/Edit/Del IPs";
			break;
		case UF_SETRATIO:
			return "Can Set Ratios";
			break;
		case UF_SETLIMIT:
			return "Can Set Limits";
			break;
		case UF_KICK:
			return "Can Kick Users";
			break;
#ifdef RETIRED
		case UF_LOGINS:
			return "Can Login Multiple Times";
			break;
#endif
	case UF_SEEALL:
		return "Can See Users In Stealth Sections";
		break;

		case UF_ALL:
			return "Godly Siteop";
			break;
		default:
			return "Unknown Flag";
	}

}
