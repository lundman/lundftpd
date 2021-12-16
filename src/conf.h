// DEFINES
#define CONF_MAXFIELD	256	
#define CONF_MAXVALUE	512	

#define CONF_TRUE			1
#define CONF_FALSE			0

#define CONF_STRING			(1<<0)		// Pointer to a char *
#define CONF_INT			(1<<1)		// Pointer to an int
#define CONF_BOOL			(1<<2)		// Pointer to a boolean (int really)
#define CONF_BLOCK			(1<<3)		// Pointer to a conf_item struct


// TYPEDEFS/STRUCTURES
struct conf_item {
	char name[CONF_MAXFIELD];
	void *value;
	int type;
	int line;
	struct conf_item *next;
};


// EXTERNS
extern struct conf_item *conf_item_head;


// PROTOTYPES
void conf_init(char *configfile);
int conf_load(char *configfile);
void conf_mandatory(void);
struct conf_item *conf_parseblock(int *linecnt, FILE *fp);
void conf_add(struct conf_item **top, char *field, void *value, int type, int linecnt);
struct conf_item *conf_getitem(char *field);
struct conf_item *conf_getblock(struct conf_item *ci, char *name);
struct conf_item *conf_getblock_from(struct conf_item *ci);
char *conf_getstring(struct conf_item *ci, char *field);
char *conf_getstring_from(struct conf_item *ci);
int conf_getint(struct conf_item *ci, char *field);
int conf_getint_from(struct conf_item *ci);
int conf_getbool(struct conf_item *ci, char *field);
int conf_getbool_from(struct conf_item *ci);
int conf_getline(struct conf_item *ci, char *field);

void conf_free(void);
