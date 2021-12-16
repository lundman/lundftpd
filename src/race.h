#ifdef RACESTATS
#ifndef _RACE_H_
#define _RACE_H_

#define RACE_HALF	(1<<0)
#define RACE_END	(1<<1)

#include "lion_types.h"

struct racer {
	char who[MAXUSERNAME];
	char group[MAXUSERNAME];
	float topspd;
	float botspd;
	long starttime; // time_t ?
	long endtime;
	lion64_t bytes;
	int files;
	int totaltimesec;
	int totaltimeusec;
	struct racer *next;
};

struct race_file {
	char file[MAX_PATH];				// File these stats are for.
	long starttimesec;
	long starttimeusec;
	long endtimesec;
	long endtimeusec;
	float speed;
	lion64_t size;
	char who[MAXUSERNAME];
	struct racer *racer;
	struct race_file *next;
};

struct race {
	char path[MAX_PATH];				// Path of the race
	char sfvfile[MAX_PATH];				// Name of corresponding SFV
	char statefile[MAX_PATH];			// Name of statefile
	int filestotal;						// Total files in release.
	int filesdone;						// Total files done.
	lion64_t bytes;							// Size completed so far.
	long starttime;						// Starttime of race
	int announced;						// Shown halfway yet?
	struct racer *top_racer;			// Racers.
	struct race_file *top_race_file;	// Files.
	struct race *next;					// Other races.
};

// Race
struct race *race_new(char *path, char *sfvfile);
struct race *race_state_find(char *path, char *filename);
struct race *race_state_load(char *path, char *filename);
struct race *race_state_load_phys(char *pathfile);
void race_state_sync(struct race *race);
void race_state_save(struct race *race);
void race_delete(struct race *race);
void race_state_free(struct race *race);
void race_state_clean(char *path);

// Race information
char *race_who(struct race *race);
int race_count(struct race *race);
struct racer **race_winners(struct race *race);
void race_show(struct login_node *t, char *args);

// Race files
void race_file_new(struct race *race, char *file);
void race_file_start(char *path, char *file);
void race_file_complete(char *path, char *file, char *who, char *group, lion64u_t bytes, long timesec, long timeusec);
void race_file_calc_speed(struct race_file *race_file);
void race_file_deleted(char *path, char *file);
void race_file_reset(struct race *race, struct race_file *rf);
struct race_file *race_file_find_inrace(struct race *race, char *filename);


// Racers
struct racer *race_racer_new(struct race *race, char *who, char *group, float speed, long starttime);
struct racer *race_racer_find_inrace(struct race *race, char *who);
void race_racer_remove(struct race *race, struct racer *racer);

// Announcements
void race_announce_newracer(struct race *race, struct racer *racer, float speed);
void race_announce_half(struct race *race);
void race_announce_end(struct race *race);

void race_xdupe(struct race *race, struct login_node *t, int code, char *name);


#endif
#endif
