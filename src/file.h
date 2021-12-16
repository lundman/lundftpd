
/* #defines */
#define FILE_OLD	0 /* Open a new file */
#define FILE_APPE	1 /* Open a possible new file and append */
#define FILE_SET	2 /* Open file and lseek to set. */

#define  FILE_NLST		1 /* This is an nlst situation */
#define FILE_FINDFIRST	2 /* First first occurance of matching pattern */
#define FILE_NEWSCAN	4 /* Check for directories newer than XXXX */  

#define GID_COUNT_OFFSET 3000 /* d/l count offset */

#define MAX_PATHLEN	255


/* variables */
struct wipedata {
	char user[MAXUSERNAME+2];    
	char group[MAXUSERNAME+2];    
	lion64u_t bytes;
	int files;
	struct wipedata *next;
};


/* functions */
void file_retr(struct data_node *);
void file_stor(struct data_node *, int);
void file_load(void);
int file_pathcheck(struct data_node *);
int file_pathchecklogin(struct login_node *, char *);
void file_undot(char *);
int file_isdir(struct login_node *, char *);
void file_size(struct login_node *, char *);
void file_mkdir(struct login_node *, char *);
void file_rmdir(struct login_node *, char *);
void file_delete(struct login_node *, char *);
void file_checkmessage(struct login_node *, int);
char *file_parsedir(struct login_node *, char *, unsigned int, int, int);
void file_gononroot(void);
void file_goroot(void);
void file_gononuser(void);
void file_gouser(struct login_node *);
void file_chown(struct login_node *, char *);
void file_chown_byname(char *, char *);
int file_isowner(struct login_node *, char *);
int file_num_dir_entries(char *path, char *dir, unsigned long *);
int file_wipe_recursive(char *fullpath, struct wipedata **wdata, int del);
void file_bump_gid(struct data_node *);
int file_hasmessages(struct login_node *);
void file_readwall(void);
void file_writewall(void);
void file_free(struct login_node *, char *, int);
