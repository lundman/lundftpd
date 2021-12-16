
/* #defines */

/* DATA_BUFSZ moved into socket.h */

#define DATA_LIST      1
#define DATA_RETR      2
#define DATA_STOR      4
#define DATA_FILECHECK 8
#define DATA_NLST      16
#define DATA_PASSIVE   32     /* This node is in passive */
#define DATA_FREE      64     /* This xfer is free */
#define DATA_RENAME    128    /* This node is for RNFR RNTO */
#define DATA_APPE      256    /* This to call APPE not STOR */
#define DATA_TLS       512    /* Have we called SSL yet? */
#define DATA_WANTCONNECTED 1024 /* Call data_connected for node */
#define DATA_QUEUE     0x8000

#define DATA_SORT_NAME 1
#define DATA_SORT_SIZE 2
#define DATA_SORT_TIME 4
#define DATA_SORT_REVERSE 8
#define DATA_SORT_DELETE 16
#define DATA_SORT_RECURSIVE 32
#define DATA_SORT_PARSE 64  /* jeffe asked for a easy-to-parse output */

#define DATA_SORT_TYPES (DATA_SORT_NAME|DATA_SORT_SIZE|DATA_SORT_TIME|DATA_SORT_PARSE)


// Idle timeout of data sessions in seconds.
#define DATA_IDLE_SEC 3600



/* variables */
struct data_node {

	lion_t *handle;
	lion_t *althandle;

	int type;
	int status;
	int mode;

	int ret;
	int sort_by;

	time_t time;

	unsigned long host;
	 int port;

	lion64u_t bytes;

	char *name;
	char *list_precat;

	void *shadow;

	lion64u_t rest_value;

	struct data_node *next;
	struct login_node *login;

    timers_t *timer_handle;
};

extern int data_global_rate_cap_group;



/* functions */

int data_outpad(struct data_node *, int, char *, int);
void data_close(struct data_node *);
void data_process(struct data_node *);
int data_get(struct data_node *, int, int, int);


void data_connected(struct data_node *d);
int data_handler( lion_t *handle,
				  void *user_data, int status, int size, char *line);


struct data_node *data_init(struct login_node *, int );
struct data_node *data_list(struct login_node *, char *, char *);
struct data_node *data_retr(struct login_node *, char *);
struct data_node *data_stor(struct login_node *, char *, int );
struct data_node *data_nlst(struct login_node *, char *, char *);
struct data_node *data_pasv(struct login_node *, lion_t *);
struct data_node *data_rnfr(struct login_node *, char *);
struct data_node *data_rnto(struct login_node *, char *);
struct data_node *data_findtype(struct login_node *, int);
void              data_clearall(struct login_node *);


int data_cps_retr( struct data_node * );
int data_cps_stor( struct data_node * );
void data_set_rate_in ( struct data_node * );
void data_set_rate_out( struct data_node * );
void data_update_stats( struct data_node * );
