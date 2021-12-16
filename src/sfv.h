#ifndef _SFV_H_
#define _SFV_H_

#define SFV_MISSING 0
#define SFV_FAILED	1
#define SFV_UNKNOWN	2
#define SFV_OK		3

#define SFV_TYPE_SFV 0
#define SFV_TYPE_MD5 1

// ce70347b61855405b0d91c9b38255166 *vx-die.hard.4.0.r00^M

struct sfv_state_entry {
	char filename[256];					// Just the filename of the entry
	// 9 -> 33 to handle md5.
	char crc[33];						// Just the CRC from the file
	int status;							// SFV_OK/SFV_FAILED/SFV_UNKNOWN/SFV_MISSING
	struct sfv_state_entry *next;		// Next in list.
};

struct sfv_state {
	char path[256];						// The path and path only
	char filename[256];					// The sfv filename
	char statefilename[256];			// The statefilename
	char user[MAXUSERNAME];				// UID of .sfv owner
	int count;							// Number of files in SFV
	int type;                           // SFV or MD5 file
	struct sfv_state_entry *entry;		// Top of entry list.
	struct sfv_state *next;				// Next in list.
};


struct sfv_state *sfv_file_load(char *pathfile);
void sfv_entry_add(struct sfv_state *sfv_state, char *filename, char *crc);
void sfv_state_save(struct sfv_state *sfv_state);
void sfv_state_free(struct sfv_state *sfv_state);
struct sfv_state *sfv_state_load(char *pathfile);
struct sfv_state *sfv_state_find(char *filename, char *path);
void sfv_state_remove(char *path, char *filename, char *stub);
void sfv_state_create(char *path, char *filename, char *stub, char *user);
void sfv_state_clean(char *path);
int sfv_test(char *pathfile, char *crc);
unsigned long SFV_MakeCRC32(unsigned long iCRC, const char *buff, long count);
unsigned long SFV_GetFileCRC(int fd);
char *SFV_GetFileMD5(int fd);
struct sfv_state *sfv_state_update(char *filename, char *path, int status);
char *sfv_state_crc(char *filename, char *path);
int sfv_state_count(struct sfv_state *ss, int status);

// SITE COMMANDS
void sfv_site_incompletes(struct login_node *t, char *args);
void sfv_incomplete(char *state_path, int method);

#endif
