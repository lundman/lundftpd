struct login_node_3_3 {
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

	// NO LONGER USED, SHOULD BE RELEASED INTO THE WILD.
  unsigned int num_logins;        /* allowed concurrent logins */
  unsigned int num_uploads;       /* allowed concurrent uploads */
  unsigned int num_downloads;     /* allowed concurrent downloads */

  char *auth;                     /* ShadowFTPD Auth reply, if any */

  void *quota;                    /* Pointer to Quota Node, IF ANY! */

  dev_t cwd_devid;                /* ID of last CWDirectory */


	

  /* These values go here cos they are login dependent, not quota */
  unsigned int files_up_login;   /* files uploaded this login */

	// We needed a lion64u_t for REST, so changed this one to two floats
	// and the other is the new REST.
	//lion64u_t bytes_up_login;  
	float bytes_up_login;
	float bytes_down_login;

  unsigned int seconds_up_login;   

  unsigned int files_down_login;     
	//lion64u_t bytes_down_login; 
	lion64u_t data_restart;

  unsigned int seconds_down_login;

	// During sitecust, we have this set for alt-handle etc.
	void *sitecust;


};




struct login_node_2_4 {
  char user[MAXUSERNAME+2];           /* login name */
  char pass[MAXPASSNAME+2];           /* may be used (see userpasswd) */
  char ident[MAXUSERNAME+2];

  unsigned long host;
  unsigned short int port;
  
  unsigned long port_host;            /* PORT's host */
  unsigned short int port_port;       /* PORT's port */

  time_t time;

  unsigned int status;  /* Status bits of this login. */
  
  int socket;

  unsigned int level;   /* Login level */
  int mode;             /* xfer mode */

  char *cwd;            /* Current working directory */

  int waiting_for;

  off_t offset;         /* On disk offset */

  time_t idle;          /* Last active */

/* IP MATCH PATTERNS */
  char matchip[IPMATCHMAX][IPMATCHLEN];
/* END OF IPMATCH */

  unsigned long data_restart; /* REST command, lseek first */

  int data_active;      /* # of active data connections */
  void *data;           /* Data connections, actually a struct data_node * */

  struct login_node *next;

  unsigned int inbuffer;   /* Number of bytes already in buffer */
  unsigned char *buffer;/* Input buffer */

  unsigned int outbuffer;  /* Bytes in write() buffer */
  unsigned char *obuffer;  /* ptr to write buffer. often 0. */

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
  lion64u_t bytes_up_login;  
  unsigned int seconds_up_login;   

  unsigned int files_down_login;     
  lion64u_t bytes_down_login; 
  unsigned int seconds_down_login;

};


struct login_node_1_4 {
  int unknown;
};


struct login_node_1_7 {
  char user[MAXUSERNAME+2];           /* login name */
  char pass[MAXPASSNAME+2];           /* may be used (see userpasswd) */
  char ident[MAXUSERNAME+2];

  unsigned long host;
  unsigned short int port;
  
  unsigned long port_host;            /* PORT's host */
  unsigned short int port_port;       /* PORT's port */

  time_t time;

  unsigned int status;  /* Status bits of this login. */
  
  int socket;

  unsigned int level;   /* Login level */
  int mode;             /* xfer mode */

  char *cwd;            /* Current working directory */

  int waiting_for;

  off_t offset;         /* On disk offset */

  time_t idle;          /* Last active */

/* IP MATCH PATTERNS */
  char matchip[IPMATCHMAX][IPMATCHLEN];
/* END OF IPMATCH */

  unsigned long data_restart; /* REST command, lseek first */

  int data_active;      /* # of active data connections */
  void *data;           /* Data connections, actually a struct data_node * */

  struct login_node *next;

  unsigned int inbuffer;   /* Number of bytes already in buffer */
  unsigned char *buffer;/* Input buffer */

  unsigned int outbuffer;  /* Bytes in write() buffer */
  unsigned char *obuffer;  /* ptr to write buffer. often 0. */

  int last_command;      /* last called command */

  unsigned int options;   /* user settable options */
  void *current_group;          /* assigned to point to active group */
};



struct login_node_1_8 {
  char user[MAXUSERNAME+2];           /* login name */
  char pass[MAXPASSNAME+2];           /* may be used (see userpasswd) */
  char ident[MAXUSERNAME+2];

  unsigned long host;
  unsigned short int port;
  
  unsigned long port_host;            /* PORT's host */
  unsigned short int port_port;       /* PORT's port */

  time_t time;

  unsigned int status;  /* Status bits of this login. */
  
  int socket;

  unsigned int level;   /* Login level */
  int mode;             /* xfer mode */

  char *cwd;            /* Current working directory */

  int waiting_for;

  off_t offset;         /* On disk offset */

  time_t idle;          /* Last active */

/* IP MATCH PATTERNS */
  char matchip[IPMATCHMAX][IPMATCHLEN];
/* END OF IPMATCH */

  unsigned long data_restart; /* REST command, lseek first */

  int data_active;      /* # of active data connections */
  void *data;           /* Data connections, actually a struct data_node * */

  struct login_node *next;

  unsigned int inbuffer;   /* Number of bytes already in buffer */
  unsigned char *buffer;/* Input buffer */

  unsigned int outbuffer;  /* Bytes in write() buffer */
  unsigned char *obuffer;  /* ptr to write buffer. often 0. */

  int last_command;      /* last called command */

  unsigned int options;   /* user settable options */
  void *current_group;          /* assigned to point to active group */

  /* ^^ V1.7 ^^ */

  char tagline[MAXPASSNAME+2];    /* User's tagline */
  char last_ident[MAXUSERNAME+2]; /* last login ident */

  unsigned long last_host;        /* last login's IP */

  time_t logoff_time;             /* Time of logoff */
  time_t last_login;              /* Last login date, not this session */
  time_t last_logout;             /* Last logout */

  unsigned int num_logins;        /* allowed concurrent logins */
  unsigned int num_uploads;       /* allowed concurrent uploads */
  unsigned int num_downloads;     /* allowed concurrent downloads */

  char *auth;                     /* ShadowFTPD Auth reply, if any */

};



struct quota_node_1_4 {
  int unknown;
};


struct quota_node_1_7 {
  off_t offset;                  /* On disk offset, also usernum */

  int dirty;                     /* Has been updated and need's writing */
  int ref_count;                 /* Number of references */
  
  unsigned long daily;           /* Daily limit (global) */
  unsigned long bytes_down;      /* # of bytes downloaded */
  unsigned long bytes_up;        /* " "    "   uploaded   */
  int ratio;                     /* user's ratio settings */
  unsigned int time_limit;       /* logout time (global) */

  struct quota_node *next;
};




struct quota_node_1_8 {
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

  unsigned int files_up;         /* Num files uploaded */
  unsigned int files_down;       /* Num files downed */

  unsigned int seconds_up;       /* Secs spent uploading (for a/g) */
  unsigned int seconds_down;     /* Secs spent downing (for a/g) */

  unsigned int nuked_files;      /* num nuked files */
  lion64u_t nuked_bytes;     /* num bytes nuked */
  
  unsigned int got_nuked_files;  /* num files of ours nuked */
  lion64u_t got_nuked_bytes; /* num bytes of ours nuked */

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

  time_t time_on_day;            /* time spent online today */
  time_t time_on_week;           /* time spent online this week */
  time_t time_on_month;          /* time spent online this month */
  time_t time_on_alltime;        /* time spent online all time */
  
  time_t last_nuked;              /* Last time nuked */

  unsigned int login_times;       /* Number of logins to date */
};






struct login_node_2_1 {
  char user[MAXUSERNAME+2];           /* login name */
  char pass[MAXPASSNAME+2];           /* may be used (see userpasswd) */
  char ident[MAXUSERNAME+2];

  unsigned long host;
  unsigned short int port;
  
  unsigned long port_host;            /* PORT's host */
  unsigned short int port_port;       /* PORT's port */

  time_t time;

  unsigned int status;  /* Status bits of this login. */
  
  int socket;

  unsigned int level;   /* Login level */
  int mode;             /* xfer mode */

  char *cwd;            /* Current working directory */

  int waiting_for;

  off_t offset;         /* On disk offset */

  time_t idle;          /* Last active */

/* IP MATCH PATTERNS */
  char matchip[IPMATCHMAX][IPMATCHLEN];
/* END OF IPMATCH */

  unsigned long data_restart; /* REST command, lseek first */

  int data_active;      /* # of active data connections */
  void *data;           /* Data connections, actually a struct data_node * */

  struct login_node *next;

  unsigned int inbuffer;   /* Number of bytes already in buffer */
  unsigned char *buffer;/* Input buffer */

  unsigned int outbuffer;  /* Bytes in write() buffer */
  unsigned char *obuffer;  /* ptr to write buffer. often 0. */

  int last_command;      /* last called command */

  unsigned int options;   /* user settable options */
  void *current_group;          /* assigned to point to active group */

  /* ^^ V1.7 ^^ */

  char tagline[MAXPASSNAME+2];    /* User's tagline */
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

};



/* This now goes into its own file. */



struct quota_node_2_1 {
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


};




struct quota_node_3_3 {
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

};














// GROUPS

struct group_member_3_3 {
	char name[MAXUSERNAME];
	int flags;
	int ratio;
	struct group_member *next;
};

struct group_3_3 {
	char name[MAXUSERNAME];
    int gid;					// Unique GID for filesystem permissions.
	int flags;

    int totalslots;				// Total slots including leech slots.
	int usedslots;				// Total slots used.
    int leechslots;				// Maximum # of users that may have leech.
	int usedleechslots;			// Total # of users that have leech.
    int maxlogins;				// Maximum logins group has.
    int maxratio;				// Max ratio GADMIN may set member to.
    int idle;					// Max idle time of group.

	// If you are to use 64bit variables, please use the lion one.
    lion64u_t bytes_uploaded;
    lion64u_t bytes_downloaded;
    lion64u_t bytes_uploaded_month;
    lion64u_t bytes_downloaded_month;
    lion64u_t bytes_uploaded_week;
    lion64u_t bytes_downloaded_week;
    lion64u_t bytes_uploaded_day;
    lion64u_t bytes_downloaded_day;

	unsigned long last_month;	// Last month above stats were updated.
	unsigned long last_week;	// Last week above stats were updated.
	unsigned long last_day;		// Last day above stats were updated.

    struct group_member *members;
    struct group *next;
};








