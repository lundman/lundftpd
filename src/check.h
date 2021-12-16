#ifndef CHECK_H_INCLUDED
#define CHECK_H_INCLUDED



/* #defines */

#define CHECK_FREE     0
#define CHECK_NOCHECK  1
#define CHECK_INTERNAL 2
#define CHECK_SAVE_FILENAME ".check-rescan.txt"



/* variables */

struct check_node {
  char *ext;
  char *exe;
  int ret;
  struct check_node *next;
};

struct check_sfv_struct {
  char path[256];
  char user[MAXUSERNAME];
  lion64u_t bytes;
};

/* Various types of defines for this linked list */

#define CD_SMARTPATH 1 /* Is this a CD path ? */
#define CD_NEWPATH   2 /* is this a NEW files path? */

#define SHADOW_IP_PATTERN 3 /* ident@ip-patterns for shadowftpd connects */

#define CD_NOCAPIP   4 /* is this a NEW files path? */



#define CHECK_UNKNOWN 0
#define CHECK_GOOD    1
#define CHECK_BAD     2 // 666



/* end of list */


struct cd_node {
  int type;
  char *abbr;       /* Abbreviated form */
  char *expa;       /* Expanded form */
  struct cd_node *next;
};

struct sfv_state_struct {
  unsigned int total;
  unsigned int good;
  unsigned int failed;
  unsigned int unknown;
};


//
// Basically, this is nearly a copy of a data node, but we define it again
// for the sake of making more sense. Real mess before!
struct check_filetest_struct {

	char user[MAXUSERNAME+2];

	off_t offset;  // sigh, quota code takes login nodes.
	void *quota;
	int ratio;			// Ratio for this file
	void *current_group;

	// Things from data node we want.
	char *name;         // file name, as submitted by client [1] read note
	char *disk_name;    // actual on-disk filename.
	char *dir_name;     // actual on-disk directory containing file.
	char *file_name;    // actual on-disk just filename.
	time_t time;        // transfer start time.
	lion64u_t bytes;

	char *expected_value;   // string of "0" or "4EC129FE" etc.
	char *computed_value;   // if known.
	int   computed_status;  // the good, the bad, the unknown.(new movie title)

	char *sfv_file_name;    // if relevant

	unsigned int id;        // incremental unique identifier.

	long timesec;			// Time file finished - seconds
	long timeusec;			// time file finished - microseconds

	struct check_filetest_struct *next;

};

typedef struct check_filetest_struct checker_t;
// [1]
//    name can be "file.ext" or "/full/path/cwd/file.ext"
//  on_disk   is "/full/path/cwd/file.ext"
//  dir_name  is "/full/path/cwd/"  (slash guaranteed)
//  file_name is "file.ext"

extern struct check_node *check_node_head;




/* functions */

void check_add(char *ext, char *exe, int ret);
void check_cdadd(FILE *);
void check_newadd(FILE *);
int check_filebad(struct data_node *);
//void check_return(struct data_node *, int );
int check_isfree(char *);
char *check_cdmatch(char *);
void check_newscan(struct login_node *, time_t);
void check_shadowadd(FILE *);
int check_shadowcheck(struct login_node *);
void check_free(void);
void check_internal(struct data_node *);
void sfv_award(struct check_sfv_struct *, int);
void sfv_state_control(char *, int, int, int, int);
void sfv_state(char *, int);
void child_sfv_test( struct check_sfv_struct *, char *, int);
void check_child(int, int);
void check_internal(struct data_node *);
void *check_find_exe(char *pathfile);
void create_mp3_info(const char *, const char *);

void check_nocapadd(FILE *config);
int  check_nocapmatch(char *ip);




void          check_action        ( checker_t * );
void          check_return        ( checker_t *, char * );
void          check_releasenode   ( checker_t * );
checker_t    *check_makenode      ( struct data_node * );
int           check_init          ( void );
void          check_free_helper   ( void );

void          check_stats         ( struct login_node * );
void          check_extractimdb   ( checker_t * );
void          check_resume        ( void );
void          check_manual_scan   ( char * );

#endif
