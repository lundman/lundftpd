// Defines

#include "lion_types.h"

#define GIGS(x)	((x) / 1073741824.0)
#define MEGS(x) ((x) / 1048576.0)
#define KBS(x)	((x) / 1024)

#define PATT_NOCAP	(1<<0)
#define PATT_GLOBAL (1<<1)
#define PATT_HIDE	(1<<2)
#define PATT_DENY	(1<<3)
#define PATT_DENYD	(1<<4)

#define FNMATCH_FILESONLY 256
#define FNMATCH_DIRSONLY  512
#define FNMATCH_NOPRECAT  1024


#define misc_patt_add_gbl(type, patt)	misc_patt_add(&global_patts, type, patt)
#define misc_patt_match_gbl(type, patt)	misc_patt_match(global_patts, type, patt)

// Typedefs/structs
struct misc_patt {
	char *patt;
	int type;
	struct misc_patt *next;
};

// Variables
extern struct misc_patt *global_patts;
extern char genbuf[2048];

// Prototypes
void chop(char *);
void chop2(char *);
char *digtoken(char **, char *);
int mystrccmp(register char *, register char *);
char *mystrcpy(char *);
void to64(register char *, register long, register int);
char *idletime(time_t);
char *idletime2(time_t);
int cmptail(char *, char *);
void misc_getargs(char *, char *, char *);
char *path2file(char *);
unsigned int atoflags(char *);
char *flagstoa(unsigned int);
char *misc_file_date(time_t);
char *hide_path(char *);
char *byte_str(lion64u_t);
char *misc_bytestr(lion64u_t);
char *fixstring(char *, int);
void strlower(char *);
char *postnumber(int);
void strcpylower(char *, char *);
int  misc_ishex(char);
char *misc_itoa(int );
int fnmatch_func(char *, char *, int (*)(const char *, void *), int, void *);
char *path2deepest(char *path, int level);

int get_word(FILE *, char *);
int get_keyword(FILE *, char *);int get_int(FILE *);
long get_long(FILE *);
lion64_t get_longlong(FILE *);
char *get_string(FILE *);

// FNMatch Pattern List Functions
void misc_patt_add(struct misc_patt **top, int type, char *pattern);
int misc_patt_match(struct misc_patt *top, int type, char *value);
char *misc_patt_list(struct misc_patt *top, int type);

void stripslashes(char *string);
char *pathsplit(char *s);
char *pathfixsplit(char *s, char *t);
char *misc_format_ratio(int ratio);
char *flagtowhat(int flag);

