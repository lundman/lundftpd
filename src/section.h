// Typedefs/variables

#ifndef SECTION_H_INCLUDED
#define SECTION_H_INCLUDED


#define SECTION_GROUP_DEFAULT "_groups_default_"


struct section_node {

	char *path;
	char *name;
	char *smartpath;
	int dupecheck;
	int announce;
	int filecheck;
	int deletebad;
	int racestats;
	int ratio;
	struct misc_patt *patts;
	int convertmkdspace;
	int convertstorspace;
	int extractmp3genre;
	int extractimdb;
	int uppercasedir;
	int lowercasefile;
	int stealth;
	int request;
	int incompletes;

	char **groups;
	int num_groups;

	char **expand_paths;
	int num_expand_paths;

	// remote section
	unsigned long remote_host;
	int remote_port;
	char *remote_user;
	char *remote_pass;
	int remote_ssl;

	struct section_node *next;
};

typedef struct section_node section_t;

extern struct section_node *section_node_head;

// Prototypes
struct section_node *section_add(char *path);
void section_apply_defaults(void);
struct section_node *section_find(char *path);
char *section_rootfind(char *fullpath);
char *section_rlsfind(char *fullpath);
char *section_find_smartpath(char *smartpath);
char *section_name(char *path);
int section_uppercasedir(char *path);
int section_lowercasefile(char *path);
int section_dupecheck(char *path);
int section_announce(char *path);
int section_filecheck(char *path);
int section_deletebad(char *path);
int section_racestats(char *path);
int section_ratio(char *path);
int section_convertmkdspace(char *path);
int section_convertstorspace(char *path);
int section_extractmp3genre(char *path);
int section_extractimdb(char *path);
int section_hidefile(char *path, char *file);
int section_denyfile(char *file);
int section_denydir(char *dir);
int section_stealth(char *file);
int section_incompletes(char *file);
char *section_request(char *file);
int section_grouped(char *file);
int section_remote(char *file);
struct section_node *section_find_byname(char *name);
//int section_ismember(char *file, );

void section_add_expand(section_t *section, char *path);
void section_add_remote(section_t *section, char *site);
void section_add_groups(struct section_node *, char *group);
char *section_group_ismember(char *path, char *user);

char *section_expand(char *path);

void section_free(void);


// SITE CMDS
void section_site_section(struct login_node *t, char *args);

#endif
