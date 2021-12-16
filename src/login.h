#ifndef LOGIN_H_INCLUDED
#define LOGIN_H_INCLUDED

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "lion.h"
#include <sys/types.h>

#ifdef WIN32
#define WINDOWS_MEAN_AND_LEAN
#include <winsock2.h>
#include "win32.h"
#endif


/* #defines */
#define ST_NONE 0        /* No status */
#define ST_LOGIN 1       /* Has logged in */
#define ST_NEWLOGIN 2    /* Issued USER again */
#define ST_MESSAGE  4    /* Message added (while we were logged on) */
#define ST_STATLIST 8    /* Directory listing with STAT command */
#define ST_DELETEME  32  /* Remove should be free'd and removed */
// Free
//#define ST_PENDING 64  /* Connection pending */
#define ST_ACTIVE 128    /* Connection active, altsocket should be closed too */
#define ST_AUTHOK 256    /* AUTH command received, and store for later test */
#define ST_CRYPT  512    /* SRP Crypt is now ACTIVE */
#define ST_DUPELOGIN 1024  /* Logout all other logins    (!username) */
#define ST_DUPEDOWN  2048  /* Logout all other downloads (%username) */
#define ST_SAFETP    4096  /* SAFETP connection used */
#define ST_TLS       8192  /* SSL/TLS connection used */
// Free
//#define ST_TLS_NEG  16384   /* SSL/TLS negotiation in progress */
//#define ST_SRP      32768   /* AUTH SRP issued */
#define ST_NOCAP    65536     /* From a no capping IP */
#define ST_SEND_226 131072    /* we sent 150, so send 226 */
#define ST_DATA_SECURE 262144 /* PROT P is set */


#define UO_COLOUR   1    /* User wants colour */
#define UO_RESET    2    /* Lund's colour reset scheme */
#define UO_FANCY    4    /* Add fancy stuff */
#define UO_XDUPE_2 16    /* XDUPE mode 2 */
#define UO_XDUPE_3 32    /* XDUPE mode 3 */


#define MODE_TYPE_ASCII  0
#define MODE_TYPE_BINARY 1
#define MODE_TYPE_CCSN   2  // CCSN Client ON.
#define MODE_TYPE_SSCN   4  // SSCN Client ON.





/* variables */

struct login_node {
  char user[MAXUSERNAME+2];           /* login name */
  char pass[MAXPASSNAME+2];           /* may be used (see userpasswd) */
  char ident[MAXUSERNAME+2];

  unsigned long host;
  unsigned short int port;

  unsigned long port_host;            /* PORT's host */
  unsigned short int port_port;       /* PORT's port */

  time_t time;

  unsigned int status;  /* Status bits of this login. */

	int cap_group;      /* lion_group used for user cap */

  unsigned int level;   /* Login level */
  int mode;             /* xfer mode */

  char *cwd;            /* Current working directory */

  int waiting_for;

  off_t offset;         /* On disk offset */

  time_t idle;          /* Last active */

/* IP MATCH PATTERNS */
  char matchip[IPMATCHMAX][IPMATCHLEN];
/* END OF IPMATCH */

	// old data_rest
	unsigned long notused1;

  int data_active;      /* # of active data connections */
  void *data;           /* Data connections, actually a struct data_node * */

  struct login_node *next;

	// now free
  unsigned int inbuffer;   /* Number of bytes already in buffer */
	// now free
  unsigned char *buffer;/* Input buffer */

	// now free
  unsigned int outbuffer;  /* Bytes in write() buffer */

	lion_t *handle;


  int last_command;      /* last called command */

  unsigned int options;   /* user settable options */
  void *current_group;          /* assigned to point to active group */

  /* ^^ V1.7 ^^ */

  char tagline[MAXTAGLINE+2];    /* User's tagline */
  char last_ident[MAXUSERNAME+2]; /* last login ident */

  unsigned long last_host;        /* last login's IP */

  time_t logoff_time;             /* Time of logoff */
  time_t last_login;              /* Last login date, not this session */
  time_t last_logout;             /* Last logout */

  unsigned int num_logins;        /* allowed concurrent logins */
  unsigned int num_uploads;       /* allowed concurrent uploads */
  unsigned int num_downloads;     /* allowed concurrent downloads */

  char *auth;                     /* ShadowFTPD Auth reply, if any */

  void *quota;                    /* Pointer to Quota Node, IF ANY! */

  dev_t cwd_devid;                /* ID of last CWDirectory */




  /* These values go here cos they are login dependent, not quota */
  unsigned int files_up_login;   /* files uploaded this login */

	// FIX ME FIX ME FIXME, not all systems have unsigned long == void *
	// damnit!
	void *remote_slave; /* while in section_remote .. it is really remote_t */
	//unsigned long notused2;
	unsigned long notused3;


  unsigned int seconds_up_login;

	unsigned int files_down_login;
	lion64u_t data_restart;

  unsigned int seconds_down_login;

	// During sitecust, we have this set for alt-handle etc.
	void *sitecust;


	// ^^^^^ v3.3 ^^^^^^
	lion64u_t bytes_up_login;    // We don't fill these in yet, FIXME!!
	lion64u_t bytes_down_login;

	lion64u_t notused4;
	lion64u_t notused5;
	lion64u_t notused6;
	lion64u_t notused7;

	unsigned long netused8;
	unsigned long netused9;
	unsigned long netused10;
	unsigned long netused11;
	unsigned long netused12;
	unsigned long netused13;
	unsigned long netused14;
	unsigned long netused15;

};



/* This now goes into its own file. */



struct quota_node {
  off_t offset;                  /* On disk offset, also usernum */

  int dirty;                     /* Has been updated and need's writing */
  int ref_count;                 /* Number of references */

  unsigned long daily;           /* Daily limit (global) */
  lion64u_t bytes_down;      /* # of bytes downloaded */
  lion64u_t bytes_up;        /* " "    "   uploaded   */
  int ratio;                     /* user's ratio settings */
  unsigned int time_limit;       /* logout time (global) */

  struct quota_node *next;

  /* ^^ 1.7 ^^ */
  /* ALL-TIME (effectively) */

  unsigned int files_up;         /* Num files uploaded */
  unsigned int files_down;       /* Num files downed */

  unsigned int seconds_up;       /* Secs spent uploading (for a/g) */
  unsigned int seconds_down;     /* Secs spent downing (for a/g) */

  unsigned int nuked_files;      /* num nuked files */
  lion64u_t nuked_bytes;     /* num bytes nuked */

  unsigned int got_nuked_files;  /* num files of ours nuked */
  lion64u_t got_nuked_bytes; /* num bytes of ours nuked */


  /* DAY */

  unsigned int files_up_day;     /* files uploaded today */
  lion64u_t bytes_up_day;    /* bytes uploaded today */
  unsigned int seconds_up_day;   /* seconds uploading today */

  unsigned int highest_files_up_day;  /* record files upped in one day */
  lion64u_t highest_bytes_up_day; /* record bytes upped in one day */

  unsigned int times_topup_day;  /* num times we were #1 in topup_day */

  unsigned int files_down_day;   /* files downed today */
  lion64u_t bytes_down_day;  /* bytes downed today */
  unsigned int seconds_down_day; /* seconds downing today */

  unsigned int highest_files_down_day;  /* record files downed in one day */
  lion64u_t highest_bytes_down_day;
  /* record bytes downed in one day */

  unsigned int times_topdown_day;/* num times we were #1 in topdown_day */

  /* WEEK */

  unsigned int files_up_week;     /* files uploaded today */
  lion64u_t bytes_up_week;    /* bytes uploaded today */
  unsigned int seconds_up_week;   /* seconds uploading today */

  unsigned int highest_files_up_week;  /* record files upped in one day */
  lion64u_t highest_bytes_up_week; /* record bytes upped in one day */

  unsigned int times_topup_week;  /* num times we were #1 in topup_day */

  unsigned int files_down_week;   /* files downed today */
  lion64u_t bytes_down_week;  /* bytes downed today */
  unsigned int seconds_down_week; /* seconds downing today */

  unsigned int highest_files_down_week;  /* record files downed in one day */
  lion64u_t highest_bytes_down_week;
  /* record bytes downed in one day */

  unsigned int times_topdown_week;/* num times we were #1 in topdown_day */

  /* MONTH */

  unsigned int files_up_month;     /* files uploaded today */
  lion64u_t bytes_up_month;    /* bytes uploaded today */
  unsigned int seconds_up_month;   /* seconds uploading today */

  unsigned int highest_files_up_month;  /* record files upped in one day */
  lion64u_t highest_bytes_up_month;/* record bytes upped in one day */

  unsigned int times_topup_month;  /* num times we were #1 in topup_day */

  unsigned int files_down_month;   /* files downed today */
  lion64u_t bytes_down_month;  /* bytes downed today */
  unsigned int seconds_down_month; /* seconds downing today */

  unsigned int highest_files_down_month;  /* record files downed in one day */
  lion64u_t highest_bytes_down_month;
  /* record bytes downed in one day */

  unsigned int times_topdown_month;/* num times we were #1 in topdown_day */

  /* EXTRA */

  time_t time_on_day;            /* time spent online today */
  time_t time_on_week;           /* time spent online this week */
  time_t time_on_month;          /* time spent online this month */
  time_t time_on_alltime;        /* time spent online all time */

  time_t last_nuked;              /* Last time nuked */

  unsigned int login_times;       /* Number of logins to date */

  int num_active_retr;           /* Active uploads */
  int num_active_stor;           /* Active downloads. */

	// This is now signed as we allow for -'ve credits.
  lion64_t credits;    /* Users credit */


	// ^^^^^ v3.3 ^^^^^^^

	lion64u_t notused1;
	lion64u_t notused2;
	lion64u_t notused3;
	lion64u_t notused4;

	unsigned long notused5;
	unsigned long notused6;
	unsigned long notused7;
	unsigned long notused8;
	unsigned long notused9;
	unsigned long notused10;
	unsigned long notused11;
	unsigned long notused12;
	unsigned long notused13;

};




struct command_s {
  char *name;
  void (*function)(struct login_node *,char *);
};

extern struct login_node *login_node_head;

extern int server_num_logins;
extern int server_num_data;



extern struct command_s commands[];
extern struct command_s commands2[];


struct auth_s {
  char *name;
  void (*function)(struct login_node *,char *);
  int crypt_type;
};








/* functions */
void login_setfd_set(fd_set *, fd_set *);
int login_getline(struct login_node *, char **);
void login_command(struct login_node *, char *);
void login_input(fd_set *, fd_set *);
struct login_node *login_new(lion_t *);
void login_exit(struct login_node *);
struct login_node *login_isloggedin(char *);
struct login_node *login_isloggedin_notus(char *, struct login_node *);

void login_ident_reply(char *);

/* ftpd function */
void login_help(struct login_node *, char *);
void login_user(struct login_node *, char *);
void login_pass(struct login_node *, char *);
void login_syst(struct login_node *, char *);
void login_quit(struct login_node *, char *);
void login_port(struct login_node *, char *);
void login_list(struct login_node *, char *);
void login_type(struct login_node *, char *);
void login_retr(struct login_node *, char *);
void login_rest(struct login_node *, char *);
void login_stor(struct login_node *, char *);
void login_appe(struct login_node *, char *);
void login_cdup(struct login_node *, char *);
void login_noop(struct login_node *, char *);
void login_stat(struct login_node *, char *);
void login_nlst(struct login_node *, char *);
void login_abor(struct login_node *, char *);
void login_dele(struct login_node *, char *);
void login_pasv(struct login_node *, char *);
void login_rnfr(struct login_node *, char *);
void login_rnto(struct login_node *, char *);
void login_cwd(struct login_node *, char *);
void login_pwd(struct login_node *, char *);
void login_mkd(struct login_node *, char *);
void login_rmd(struct login_node *, char *);
void login_size(struct login_node *, char *);
void login_xpwd(struct login_node *, char *);
void login_clnt(struct login_node *, char *);
void login_mdtm(struct login_node *, char *);
void login_countstats();
void login_iden(struct login_node *, char *);
struct login_node *login_console_user(void);
void login_auth(struct login_node *, char *);
void login_pbsz(struct login_node *, char *);
void login_prot(struct login_node *, char *);
void login_feat(struct login_node *, char *);
void login_pret(struct login_node *, char *);
void login_ccsn(struct login_node *, char *);
void login_sscn(struct login_node *, char *);

// new lion stuff
int  login_handler( lion_t *, void *, int, int, char *);
void login_listen(void);
void login_listen_free(void);


char *login_uid_lookup( int );
char *login_gid_lookup( int );

void login_loop(void);

#endif
