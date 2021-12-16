#ifndef _GROUPS_H_
#define _GROUPS_H_



#define GROUPS_VERSION_STR   "GR01"
#define GROUPS_VERSION_VALUE (('G'<<24) | ('R'<<16) | ('0'<<8)|'1')










#define GROUP_CURRENT	(1<<0)
#define GROUP_ADMIN		(1<<1)


/* Unimplemented */
#define GROUP_HIDDEN	(1<<0)

struct group_member {
	char name[MAXUSERNAME+2];
	int flags;
	int ratio;

	unsigned long unused1;
	unsigned long unused2;
	lion64u_t unused3;

	struct group_member *next;
};

struct group {
	char name[MAXUSERNAME+2];
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

	// ^^^^^ 3.3

	unsigned long unused1;
	unsigned long unused2;
	unsigned long unused3;
	unsigned long unused4;
	lion64u_t unused5;
	lion64u_t unused6;
	lion64u_t unused7;
	lion64u_t unused8;
	void *unused9;
	void *unused10;
	void *unused11;
	void *unused12;

};


/* Prototypes */

// Basic Group Functions 
void group_load();
void group_save();
void group_time_init();
int group_newgid();
int group_add(char *, int, int, int, int, int);
void group_del(struct group *g);
void group_del_byname(char *group);
int group_count_flag(struct group *g, int flag);
int group_count_ratio(struct group *g, int ratio);

// Group Find Functions 
struct group *group_find_byname(char *);
struct group *group_find_bygid(int);
char *group_findname_bygid(int);

// Member Functions
#define group_member_is_gadmin(g)	(g->flags & GROUP_ADMIN)
#define group_member_is_current(g)	(g->flags & GROUP_CURRENT)
#define group_member_is_leech(g)	(g->ration == -1)

void group_member_add(struct group *g, char *username);
void group_member_del(struct group *g, struct group_member *gm);
struct group_member *group_hasuser(struct group *g, char *user);
struct group_member *group_hasgadmin(struct group *g, char *user);
struct group_member *group_iscurrent(struct group *g, char *user);
void group_member_setcurrent(struct group *g, char *user);
void group_member_clearcurrent(char *user);
struct group_member *group_member_findcurr(char *user);
char *group_list_groups(char *user);
void group_auto_section(struct login_node *t, char *name);




// Calls for LundFTPD to use
int group_hasuser_byname(char *group, char *user);
int group_hasgadmin_byname(char *group, char *user);
char *group_listgroups(char *user);
struct group *group_findany_byname(char *user);
struct group *group_findcurr_byname(char *user);


// Site Commands
void group_site_list(struct login_node *t, char *args);
void group_site_info(struct login_node *t, char *args);
void group_site_add(struct login_node *t, char *args);
void group_site_mod(struct login_node *t, char *args);
void group_site_del(struct login_node *t, char *args);
void group_site_user(struct login_node *t, char *args);
void group_site_admin(struct login_node *t, char *args);
void group_site_adduser(struct login_node *t, char *args);

#endif
