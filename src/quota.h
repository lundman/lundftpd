
/* #defines */

/* variables */

/* functions */
void quota_logincheck(struct login_node *, struct login_node *);
void quota_exit(struct login_node *);
void quota_new(struct login_node *);
void quota_quit(void);
void quota_sync(void);
void quota_load(void);
void quota_status(struct login_node *);
void quota_addnew(struct login_node *, int);
//int quota_get(struct data_node *, int);
void quota_timecheck(void);
struct quota_node *quota_getquota(struct login_node *);
void quota_upgrade(int);
void quota_top_clear(void);

int  quota_retr         (struct data_node *, struct stat * );
int  quota_retr_close   (struct data_node * );
void quota_stor_close   (struct data_node * );
void quota_stor_award(struct check_filetest_struct *);

