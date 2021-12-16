#ifndef SITECUST_H_INCLUDED
#define SITECUST_H_INCLUDED


/* custom site commands */

#define SITECUST_MAX_LINES 5000


#define SC_TEXT 1
#define SC_EXEC 2

typedef enum sitecust_argument_types {
	SITECUST_ARG_ANY = 0,
	SITECUST_ARG_DIR,
	SITECUST_ARG_FILE,
	SITECUST_ARG_DIRORFILE,
	SITECUST_ARG_SECTION,
	SITECUST_ARG_USER,
	SITECUST_ARG_GROUP,
	SITECUST_ARG_INTEGER,
	SITECUST_ARG_STRING
} sitecust_arg_t;


struct sitecust {
	char *name;
	int type;
	char *exec;
	char *desc;
	int flags;
	int exact; // 0 exact 3 args, +'ve exact or more, -'ve exact or less.
	int args;  // number of arguments. (used with exact above).
	int num_types;
	sitecust_arg_t *types; // array of "args" types. ("int args" above is num)
	struct sitecust *next;
};


//
// While a sitecust is running, in exec form, we have one of these nodes, to
// keep handle, pointer to login, and number-of-lines counters.
//
struct sitecust_exec {
	lion_t *handle;
	struct sitecust *sc;
	struct login_node *t;
	int lines;
};

void sitecust_add(char *name, char *type, char *exec, char *desc, int flags,
				  char *args);
void sitecust_free(void);
int sitecust_site_do(struct login_node *user, char *args);
struct sitecust *sitecust_find(char *name);
void sitecust_help(struct login_node *user);

void sitecust_sendfile(struct login_node *t, struct sitecust *sc, char *file);
void sitecust_sendexec(struct login_node *, struct sitecust *,char *,char *);
int sitecust_send_handler(lion_t *handle, void *user_data, int status, int size, char *line);

void sitecust_disable_read(struct sitecust_exec *);
void sitecust_enable_read(struct sitecust_exec *);
void sitecust_exit(struct sitecust_exec *);


#endif
