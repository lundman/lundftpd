
/* Defines */

#define DUPE_MAX_FILENAME 47
#define DUPE_MAX_PATH 20

#define DUPE_IS_FILE 0
#define DUPE_IS_DIR  0xfffffffe

/* Variables */

struct dupe_node {
  char          filename[DUPE_MAX_FILENAME + 1];
  char          username[MAXUSERNAME+2];
  unsigned long filesize; 
  time_t        time;
  unsigned int  flags;
  unsigned int  not_used1;
  unsigned int  not_used2;
};





/* Functions */

void dupe_init(void);
void dupe_quit(void);
struct dupe_node *dupe_find(char *);
int dupe_add(char *, char *, unsigned long, time_t);
void dupe_sync(void);
int dupe_del(char *);
struct dupe_node *dupe_fnmatch(char *);
void dupe_pathadd(char *);
int dupe_isdupepath(char *);
int dupe_isdupepathtop(char *);
struct dupe_node *dupe_purge(time_t);

