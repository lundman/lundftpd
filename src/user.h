
/* #defines */

#define MAX_CACHE 200  /* reverse (uid -> name) lookup cache */


/* variables */

extern int max_open_files_safe;

/* functions */
void user_load(void);
void user_close(void);
int user_addnew(struct login_node *);
struct login_node *user_findbyname(char *);
struct login_node *user_findbyuid(int);
void user_update(struct login_node *);
int user_pass(struct login_node *, char *);
int user_top(void);
void user_crypt(char *, char *);
void user_listuser(void);
void user_list_top(void);
void user_listuser_long(void);
void user_adduser(void);
void user_deleteuser(void);
void user_edituser(void);
int user_ipmatch(struct login_node *, struct login_node *);
int user_getuid(struct login_node *);
void user_passwd(void);
void user_dump_adduser(void);
char *user_get_username(int);
void user_db_upgrade(int);
void user_correct_db(unsigned long);
void user_sitelist(struct login_node *);
struct login_node *user_dumpall(int);

#define user_getgid(l)	(l->current_group) ? \
						((struct group *)l->current_group)->gid + \
						server_useridstart : -1
int user_getgidbyname(char *user);
int user_getuidbyname(char *user);
char *user_get_name_by_uid(int uid);

// Site Commands
void user_site_user(struct login_node *t, char *args);
