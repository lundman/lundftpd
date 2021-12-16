
#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef RACESTATS

#include <stdlib.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#if HAVE_DIRENT_H
#include <dirent.h>
#endif

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include "lfnmatch.h"

#include "lion.h"

#include "global.h"
#include "misc.h"
#include "socket.h"
#include "login.h"
#include "data.h"
#include "user.h"
#include "userflag.h"
#include "global.h"
#include "groups.h"
#include "log.h"
#include "check.h"
//#include "common.h"
#include "race.h"
#include "section.h"

#ifdef IRCBOT
#include "irc.h"
#endif


struct race *races = NULL;

// Create a new race if the race does not already exist
// in memory in the 'races' list or on disk as a .racestats-xxx
// file.
struct race *race_new(char *path, char *sfvfile)
{

	struct race *new;

	// Strip any trailing from the path.
	stripslashes(path);

	// Search for the existing race in memory
	if ((new = race_state_find(path, sfvfile)))
		return new;

	// Search for the existing race on disk
	if ((new = race_state_load(path, sfvfile)))
		return new;

	//======================================================
	// Hey, looks like we really are a new race.
	if ((new = malloc(sizeof(struct race))) == NULL) {
		consolef("** CRITICAL: malloc: %s\n", strerror(errno));
		_exit(-1);
	}
	bzero(new, sizeof(struct race));


	// Fill out the race details.
	strncpy(new->path, path, MAX_PATH-1);
	strncpy(new->sfvfile, sfvfile, MAX_PATH-1);
	snprintf(new->statefile, MAX_PATH-1, ".racestats-%s", sfvfile);
	if (!lfnmatch("*.sfv", new->statefile, LFNM_CASEFOLD)
#ifdef WITH_SSL
		|| !lfnmatch("*.md5", new->statefile, LFNM_CASEFOLD)
#endif
		)
		new->statefile[strlen(new->statefile) - 4] = '\0';

	new->starttime = global_time_now;

	// Add the the global list.
	new->next = races;
	races = new;

	// Save state to disk.
	race_state_save(new);

	consolef("** NEW RACE: Path=%s Stats=%s\n", hide_path(path), new->statefile);

	return new;
}

//------------------------------------------------------------------------
// Race information starts here!
//------------------------------------------------------------------------

// You must free what is returned!
char *race_who(struct race *race)
{

	struct racer *racer;
	char *who = NULL;
	char *newwho = NULL;

	for (racer = race->top_racer; racer; racer = racer->next) {

		if (who) {
			if ((newwho = malloc(strlen(who) + strlen(racer->who) + strlen(racer->group) + 2)) == NULL)
				abort();
			sprintf(newwho, "%s %s@%s", who, racer->who, racer->group);
			free(who);
		} else {
			if ((newwho = malloc(strlen(racer->who) + strlen(racer->group) + 2)) == NULL)
				abort();
			sprintf(newwho, "%s@%s", racer->who, racer->group);
		}
		who = newwho;
	}

	return who;

}

int race_count(struct race *race)
{
	struct racer *tmp;
	int i = 0;

	for (tmp = race->top_racer; tmp; tmp = tmp->next)
		i++;
	
	return i;
}


// Free this:
struct racer **race_winners(struct race *race)
{

	struct racer *racer;
	struct racer **winners;
	int i, j;

	if ((winners = malloc(sizeof(struct racer) * 8)) == NULL)
		abort();

	bzero(winners, sizeof(struct racer) * 8);

	for (racer = race->top_racer; racer; racer = racer->next) {
		if (winners[0] == NULL) {
			winners[0] = racer;
		} else {
			for (i = 0; i < 8; i++) {
				if (winners[i]) {
					if (winners[i]->bytes < racer->bytes) {
						for (j = 7; j > i; j--)
							winners[j] = winners[j - 1];
						winners[i] = racer;
						break;
					}
				} else {
					winners[i] = racer;
					break;
				}
			}
		}

	}

	return winners;

}

//------------------------------------------------------------------------
// Save the racestats to disk...
void race_state_save(struct race *race)
{

	struct race_file *run;
	struct racer *run2;
	FILE *fp;
	char buf[MAX_PATH];

	// Open and truncate a new file (.racestats-PATHNAME)
	snprintf(buf, MAX_PATH-1, "%s/%s", race->path, race->statefile);
	if ((fp = fopen(buf, "w+b")) == NULL) {
		consolef("** WARNING: Cannot open %s for writing\n", buf);
		return;
	}

	// Write to statefile
	fwrite(race, sizeof(struct race), 1, fp);
	// Write files to disk
	for (run = race->top_race_file; run; run = run->next)
		fwrite(run, sizeof(struct race_file), 1, fp);
	//Write racers to disk
	for (run2 = race->top_racer; run2; run2 = run2->next)
		fwrite(run2, sizeof(struct racer), 1, fp);
	fclose(fp);

}

// Search the in-memory list of states for any that match
// the following path/filename.  filename may be NULL,
// in which case the first path that matches is returned.
struct race *race_state_find(char *path, char *filename)
{
	struct race *run;
	int sfv = 0, state = 0;

	// Strip any trailing from the path.
	stripslashes(path);


	// Handle .sfv files differently.  These are stored as part of
	// the race struct and not as a single race_file.
	if (!lfnmatch("*.sfv", filename, LFNM_CASEFOLD))
		sfv = 1;
#ifdef WITH_SSL
	if (!lfnmatch("*.md5", filename, LFNM_CASEFOLD))
		sfv = 2;
#endif
	if (!lfnmatch(".racestats-*", filename, LFNM_CASEFOLD)) {
		state = 1;
	}

	// Cycle through currently loaded races.
	for (run = races; run; run = run->next) {


		// Is this the right path?
		if (!strcasecmp(run->path, path)) {

			// Check if we have a filename or the filename is a .SFV
			// otherwise, cycle through the race_files looking for a match.
			if (!filename)
				return run;
			else if (sfv) {

				if (!strcasecmp(run->sfvfile, filename))
					return run;

			} else if (state) {

				if (!strcasecmp(run->statefile, filename))
					return run;

			} else {
				struct race_file *rfrun;

				for (rfrun = run->top_race_file; rfrun; rfrun = rfrun->next) {
					if (!strcasecmp(rfrun->file, filename))
						return run;
				}

			}
		}
	}

	return NULL;

}

// This loads a given statefile and returns the struct.
struct race *race_state_load_phys(char *pathfile)
{

	FILE *fp;
	char *work;
	struct race *race;
	struct race_file *race_file, *run;
	struct racer *racer, *run2;
	int i;

	if ((fp = fopen(pathfile, "r+b")) == NULL)
		return NULL;

	if (!(race = malloc(sizeof(struct race)))) {
		consolef("** CRITICAL ERROR: unable to malloc: %s\n", strerror(errno));
		abort();
	}
	bzero(race, sizeof(struct race));

	// Read contents into race struct.
	if (fread(race, sizeof(struct race), 1, fp) != 1) {
		consolef("** RACE WARNING: Corrupt race statefile: %s\n", path2file(pathfile));
		free(race);
		fclose(fp);
		return NULL;
	}

	// NULL out some pointers, repair them later.
	race->top_race_file = NULL;
	race->top_racer = NULL;

	// Copy the current path over, it may have changed.
	strncpy(race->path, pathfile, MAX_PATH-1);
	work = strrchr(race->path, '/');
	*work = 0;

	// Read race files into memory.
	for (i = 0; i < race->filestotal; i++) {

		if (!(race_file = malloc(sizeof(struct race_file)))) {
			perror("malloc");
			abort();
		}

		if (fread(race_file, sizeof(struct race_file), 1, fp) != 1) {
			free(race_file);
			if (feof(fp) || ferror(fp))
				break;
			consolef("** RACE WARNING: Corrupt race statefile fileentry: %s\n", path2file(pathfile));
			break;
		}

		// NULL out racer, fix that later.
		race_file->racer = NULL;

		// Add to list.
		race_file->next = race->top_race_file;
		race->top_race_file = race_file;

	}

	// Read remaining racers
	while (1) {

		if (!(racer = malloc(sizeof(struct racer)))) {
			perror("malloc");
			abort();
			_exit(-1);
		}

		if (fread(racer, sizeof(struct racer), 1, fp) != 1) {
			free(racer);
			if (feof(fp) || ferror(fp))
				break;
			consolef("** RACE WARNING: Corrupt race statefile racer entry: %s\n", path2file(pathfile));
			break;
		}

		// Add to list.
		racer->next = race->top_racer;
		race->top_racer = racer;
	}

	// Fix all the race_file->racer pointers
	for (run = race->top_race_file; run; run = run->next) {
		for (run2 = race->top_racer; run2; run2 = run2->next) {
			if (!strcmp(run->who, run2->who)) {
				run->racer = run2;
				break;
			}
		}
	}

	// Check that the state of the racestats matches the state of the files
	// actually on disk.
	race_state_sync(race);

	consolef("** RACE LOAD: Loaded state '%s/%s' from physical disk\n", race->path, race->statefile);


	// Close the file? Should this be here friar as we appear to leak fds here.
	fclose(fp);

	return race;

}

void race_state_sync(struct race *race)
{

	struct race_file *rf;
	char pathfile[MAX_PATH];

	// Cycle through each file, see if it's still on disk, if not it was fragged
	// while we weren't looking! ;p
	for (rf = race->top_race_file; rf; rf = rf->next) {

		snprintf(pathfile, MAX_PATH-1, "%s/%s", race->path, rf->file);
		if (access(pathfile, F_OK) != 0) {

			// This file is missing!  Was it marked as complete in the race?
			// If so, remove this file from the race.
			if (rf->size) {
				consolef("** race_state_sync: missing file %s, being reset in race!\n", rf->file);
				race_file_reset(race, rf);
			}
		}
	}
}

void race_file_reset(struct race *race, struct race_file *rf)
{

	// No need to reset a file that is already done.
	if (!rf->size)
		return;


	// If this file has a racer associated, reset some of the punks stats.
	if (rf->racer) {

		// Reduce the number of files this racer has contributed.
		rf->racer->files--;
		rf->racer->bytes -= rf->size;

		// Reduce the total time spent racing for this racer.
		rf->racer->totaltimesec -= (rf->endtimesec - rf->starttimesec);
		if (rf->endtimeusec < rf->starttimeusec) {
			rf->racer->totaltimesec--;
			rf->racer->totaltimeusec = (rf->endtimeusec + 1000000) - rf->starttimeusec;
		} else {
			rf->racer->totaltimeusec -= (rf->endtimeusec - rf->starttimeusec);
		}

		// If the racer has no files, they are OUT OF HERE! *chop*
		if (rf->racer->files == 0)
			race_racer_remove(race, rf->racer);

	}

	// Update the race stats for this race.
	race->filesdone--;
	// Looks like we've broken a completed race!
	if (race->filestotal - race->filesdone == 1)
		race->announced &= (~RACE_END);
	// Looks like we're under halfway again!
	if (race->filesdone / (float)race->filestotal < 0.5)
		race->announced &= (~RACE_HALF);
	race->bytes -= rf->size;
	
	// Finally, take the file and set all the stats back to starting points.
	rf->starttimesec = rf->starttimeusec = 0;
	rf->endtimesec = rf->endtimeusec = 0;
	rf->speed = 0;
	rf->size = 0;
	bzero(rf->who, sizeof(rf->who));
	rf->racer = NULL;

	consolef("RACE FILE RESET: Reset file stats, race now %d/%d done\n", race->filesdone, race->filestotal);

	race_state_save(race);

}


struct race *race_state_load(char *path, char *filename)
{

	char statefilepath[MAX_PATH] = {0}, buf[MAX_PATH];
	struct race *race;
	DIR *dir;
	struct dirent *dp;

	// Strip any trailing from the path.
	stripslashes(path);

	// The race we are trying to load is already in memory.
	if ((race = race_state_find(path, filename)))
		return race;

	// -------------------------------------------------------------
	// Race is not in memory, let's begin loading .racestat-* files.

	// Do we know the exact name of the .racestat-* file?
	if (!lfnmatch(".racestat-*", filename, LFNM_CASEFOLD))
		snprintf(statefilepath, MAX_PATH-1, "%s/%s", path, filename);
	else if (!lfnmatch("*.sfv", filename, LFNM_CASEFOLD)
#ifdef WITH_SSL
			 || !lfnmatch("*.md5", filename, LFNM_CASEFOLD)
#endif
			 ) {

		snprintf(statefilepath, MAX_PATH-1, "%s/.racestats-%s", path, filename);
		statefilepath[strlen(statefilepath) - 4] = '\0';
	}

	// Yes, we know exactly which statefile we need
	if (statefilepath[0]) {
		if (!(race = race_state_load_phys(statefilepath)))
			return NULL;
		// Add this race into the master list.
		race->next = races;
		races = race;
		return race;
	}
		

	// ----------------------------------------------------------------
	// We are looking for a normal file, start loading states from disk
	if (!(dir = opendir(path)))
		return NULL;

	// Cycle through the path looking for .racestats files, load each one
	// and ascertain if the file we want is inside it.
	while ((dp = readdir(dir)) != NULL) {

		// File matches
		if (!lfnmatch(".racestats-*", dp->d_name, LFNM_CASEFOLD)) {

			// It isn't already loaded.
			if (!race_state_find(path, dp->d_name)) {

				// Load it
				snprintf(buf, MAX_PATH-1, "%s/%s", path, dp->d_name);
                if ((race = race_state_load_phys(buf))) {
					if (race_file_find_inrace(race, filename))
						break;
					race_state_free(race);
					race = NULL;
				}

            }

        }
	}
	closedir(dir);

	// We found the right file!
	if (race) {
		// Add this race into the master list.
		race->next = races;
		races = race;
	}

	return race;

}


// Simply searches for a filename withing a race's files.
struct race_file *race_file_find_inrace(struct race *race, char *filename)
{

	struct race_file *rf;

	for (rf = race->top_race_file; rf; rf = rf->next) {
		if (!strcasecmp(rf->file, filename))
			return rf;
	}

	return NULL;

}


// Adds a new racefile to the race list
void race_file_new(struct race *race, char *file)
{

	struct race_file *rf;

	// Search for file already in race
	// Malloc space for file
	// Fill out contents
	// Add to list

	// If the file is already added, do not readd it.
	if (race_file_find_inrace(race, file))
		return;

	if ( (rf = malloc(sizeof(struct race_file))) == NULL) {
		consolef("** CRITICAL malloc failure: %s\n", strerror(errno));
		abort();
	}
	bzero(rf, sizeof(struct race_file));

	strncpy(rf->file, file, MAX_PATH-1);

	// Add info to the main race struct
	race->filestotal++;

	// Looks like we've broken a completed race!
	if (race->filestotal - race->filesdone == 1)
		race->announced &= (~RACE_END);
	// Looks like we're under halfway again!
	if (race->filesdone / (float)race->filestotal < 0.5)
		race->announced &= (~RACE_HALF);

	// Add the file into the race list
	rf->next = race->top_race_file;
	race->top_race_file = rf;

	race_state_save(race);

}


void race_file_deleted(char *path, char *file)
{
	struct race *race;
	struct race_file *rf;

	// Strip any trailing from the path.
	stripslashes(path);

	// There was no race for this file.
	if (!(race = race_state_load(path, file)))
		return;

	// File didn't exist in that race...
	if (!(rf = race_file_find_inrace(race, file)))
		return;

	race_file_reset(race, rf);

}

void race_file_complete(char *path, char *file, char *who, char *group, lion64u_t bytes, long timesec, long timeusec)
{

	struct race *race;
	struct race_file *rf;
	struct racer *racer;

	// Strip any trailing from the path.
	stripslashes(path);

	// Find the race.
	if (!(race = race_state_load(path, file)))
		return;

	// Find the race_file.
	if (!(rf = race_file_find_inrace(race, file)))
		return;

	// Update the race_file info
    rf->endtimesec = timesec;
    rf->endtimeusec = timeusec;
	rf->size = bytes;
	strncpy(rf->who, who, MAXUSERNAME-1);

	// Calc. speed for this file.
	race_file_calc_speed(rf);

	// Update race info
	race->filesdone++;
	race->bytes += bytes;
	
	// Find the racer or create a new one.
	if ((racer = race_racer_find_inrace(race, who)) == NULL) {
		racer = race_racer_new(race, who, group, rf->speed, rf->starttimesec);
	}
	rf->racer = racer;

	// Add the size and time taken for the racer.
	rf->racer->bytes += bytes;
	rf->racer->files++;
	rf->racer->endtime = timesec;

	rf->racer->totaltimesec += (rf->endtimesec - rf->starttimesec);
	if (rf->endtimeusec < rf->starttimeusec) {
		rf->racer->totaltimesec--;
		rf->racer->totaltimeusec += (rf->endtimeusec + 1000000) - rf->starttimeusec;
	} else {
		rf->racer->totaltimeusec += (rf->endtimeusec - rf->starttimeusec);
	}
	rf->racer->totaltimesec += (rf->racer->totaltimeusec / 1000000);
	rf->racer->totaltimeusec %= 1000000;
	

	// Announce half of the race if required.
	if (( (race->filesdone / (float)race->filestotal) >= 0.5) && !(race->announced & RACE_HALF))
		race_announce_half(race);

	// Announce end of race if required.
	if (( race->filesdone == race->filestotal) && !(race->announced & RACE_END))
		race_announce_end(race);

	race_state_save(race);

	consolef("** RACE FILE COMPLETE: Path=%s File=%s (%d/%d)\n", hide_path(path), file,
				race->filesdone, race->filestotal);

	if (race->announced & RACE_END)
		race_delete(race);


}

struct racer *race_racer_find_inrace(struct race *race, char *who)
{
	struct racer *racer;

	for (racer = race->top_racer; racer; racer = racer->next) {
		if (!strcasecmp(racer->who, who))
			return racer;
	}

	return NULL;
}

struct racer *race_racer_new(struct race *race, char *who, char *group, float speed, long starttime)
{

	struct racer *racer;

	if ((racer = malloc(sizeof(struct racer))) == NULL) {
		consolef("** CRITICAL malloc failed: %s\n", strerror(errno));
		abort();
	}
	bzero(racer, sizeof(struct racer));

	strncpy(racer->who, who, MAXUSERNAME-1);
	strncpy(racer->group, group, MAXUSERNAME-1);
	racer->starttime = starttime;

	race_announce_newracer(race, racer, speed);

	racer->next = race->top_racer;
	race->top_racer = racer;

	consolef("** NEW RACER: %s@%s enters the race!\n", who, group);

	return racer;

}

void race_file_calc_speed(struct race_file *race_file)
{
	long timesec, timeusec;
	float speedthisfile;
	long hseconds;

	// How much time has lapsed?
	timesec = race_file->endtimesec - race_file->starttimesec;

	// If endtimeusec is less than starttime... what then?
	if (race_file->endtimeusec < race_file->starttimeusec) {
		timesec--;
		timeusec = (race_file->endtimeusec + 1000000) - race_file->starttimeusec;
	} else {
		timeusec = race_file->endtimeusec - race_file->starttimeusec;
	}

	// Go to the nearest 10th of a second
	hseconds = timesec * 100;
	hseconds += (timeusec / 10000);

	if (hseconds == 0)
		speedthisfile = 0;
	else {
		// Speed for this file in tenths of seconds, then up for per second.
		speedthisfile = (float)(race_file->size / (float)hseconds);
		speedthisfile *= 100;
	}

	race_file->speed = speedthisfile;

}

//-------------------------------------------------------------------------
// Announcements start here
//-------------------------------------------------------------------------
void race_announce_newracer(struct race *race, struct racer *racer, float speed)
{
#ifdef IRCBOT
	char *racers;

	racers = race_who(race);

	if (section_announce(race->path)) {
		if (racers) {
			
			irc_announce("RACERNEW|section=%s|user=%s|group=%s|racers=%s|dir=%s|speed=%0.2f\n",
						 section_name(race->path), racer->who, racer->group, racers,
						 section_rlsfind(race->path), speed / 1024.0);
			
			
			free(racers);
		} else {
			
			irc_announce("RACERSTART|section=%s|user=%s|group=%s|dir=%s|speed=%0.2f\n",
						 section_name(race->path), racer->who, racer->group,
						 section_rlsfind(race->path), speed / 1024.0);
			
		}
	}
#endif
}

void race_announce_half(struct race *race)
{
#ifdef IRCBOT
	int min, sec;
	long now;
	struct racer **winners;
	int count;

	race->announced |= RACE_HALF;

	now = global_time_now;
	min = (int)((now - race->starttime) / 60);
	sec = (now - race->starttime) % 60;
	count = race_count(race);

	winners = race_winners(race);

	if (section_announce(race->path))
		irc_announce("RACEHALF|section=%s|user=%s|group=%s|half=%d|files=%d|dir=%s|racercnt=%d|mins=%d|secs=%d\n",
					 section_name(race->path), winners[0]->who, winners[0]->group,
					 race->filesdone, race->filestotal, section_rlsfind(race->path),
					 count, min, sec);
	
	free(winners);

	race_state_save(race);

#endif
}

void race_announce_end(struct race *race)
{
	int hour, min, sec, i, racers;
	long now, elap;
	struct racer **winners;
	FILE *fd = NULL;
	char buf[MAX_PATH];

	// Attempt to open the .message file to write stuff into.

	// Open and truncate a new file (.racestats-PATHNAME)
	snprintf(buf, MAX_PATH-1, "%s/.message", race->path);
	fd = fopen(buf, "a");


	race->announced |= RACE_END;

	// Calculate the race duration.
	now = global_time_now;
	elap = now - race->starttime;
	hour = elap / 3600;
	elap %= 3600;
	min = elap / 60;
	elap %= 60;
	sec = elap;

	winners = race_winners(race);
	racers = race_count(race);

#ifdef IRCBOT
	if (section_announce(race->path))
		irc_announce("RACEEND|section=%s|dir=%s|racecnt=%d|hour=%d|min=%d|sec=%d\n",
					 section_name(race->path), section_rlsfind(race->path),
					 racers, hour, min, sec);
#endif

	if (fd) {
		fprintf(fd, "RACE COMPLETED:\r\n [%s] Completed: %s by %d uploader%s in %dh%dm%ds\r\n",
				section_name(race->path),
				section_rlsfind(race->path),
				racers, racers == 1 ? "" : "s", 
				hour, min, sec);
	}


	for (i = 0; i < 8; i++) {
		if (winners[i]) {
			long hseconds;
			float speed;
			int hours, mins, seconds;
			char racertime[32];

			// Calculate time the racer was involved.
			seconds = winners[i]->endtime - winners[i]->starttime;
			if ((hours = seconds / 3600))
				seconds %= 3600;
			if ((mins = seconds / 60))
				seconds %= 60;
			snprintf(racertime, 31, "%dh%dm%02ds", hours, mins, seconds);

			// Go to the nearest 100th of a second
			hseconds = winners[i]->totaltimesec * 100;
			hseconds += (winners[i]->totaltimeusec / 10000);

			if (hseconds == 0)
				speed = 0;
			else {
				// Speed for this file in tenths of seconds, then up for per second.
				speed = (float)(winners[i]->bytes / (float)hseconds);
				speed *= 100;
			}

#ifdef IRCBOT
			if (section_announce(race->path))
				irc_announce("RACEREND|section=%s|rank=%d|user=%s|group=%s|files=%d|total=%d|speed=%- 5.2f|megs=%- 3.2f|percent=%2.1f|time=%s\n",
							 section_name(race->path), i+1, winners[i]->who, winners[i]->group,
							 winners[i]->files, race->filestotal, speed / 1024.0,
							 (float)(winners[i]->bytes / (1024 * 1024)), 
							 (winners[i]->files / (float)race->filestotal) * 100,
							 racertime);
#endif


			if (fd) {

				fprintf(fd, " [%s]   [%d] %s@%s %d/%d %-5.2fKB/sec %-3.2fMB %2.1f%% %s\r\n",
						section_name(race->path), i+1, winners[i]->who, 
						winners[i]->group,
						winners[i]->files, race->filestotal, speed / 1024.0,
						(float)(winners[i]->bytes / (1024 * 1024)), 
						(winners[i]->files / (float)race->filestotal) * 100,
						racertime);
						

			}


		}
	}

	if (fd) fclose(fd);

	free(winners);
}

// Caller should save state.
void race_racer_remove(struct race *race, struct racer *racer)
{

	struct racer *run;

	if (race->top_racer == racer) {
		race->top_racer = racer->next;
		free(racer);
		return ;
	}

	for (run = race->top_racer; run->next; run = run->next) {
		if (run->next == racer) {
			run->next = racer->next;
			free(racer);
			return;
		}
	}

}
	

void race_delete(struct race *race)
{

	struct race *tmp;

	consolef("** RACE DELETE: Wiping race %s from memory\n", path2deepest(race->path, 2));

	if (races == race) {
		races = race->next;
	} else {

		for (tmp = races; tmp->next; tmp = tmp->next) {
			if (race == tmp->next) {
				tmp->next = race->next;
				break;
			}
		}
	}

	race_state_free(race);
	return;
}

void race_state_free(struct race *race)
{

	struct racer *tmp;
	struct racer *next = NULL;
	struct race_file *tmpf;
	struct race_file *nextf;

	for (tmp = race->top_racer; tmp; tmp = next) {
		next = tmp->next;
		free(tmp);
	}

	for (tmpf = race->top_race_file; tmpf; tmpf = nextf) {
		nextf = tmpf->next;
		free(tmpf);
	}

	free(race);
}

void race_file_start(char *path, char *file)
{
	struct race *race;
	struct race_file *race_file;
	struct timeval tv;

	// Strip any trailing from the path.
	stripslashes(path);

	if ((race = race_state_load(path, file)) == NULL)
		return;

	if ((race_file = race_file_find_inrace(race, file)) == NULL)
		return;

	gettimeofday(&tv, NULL);
	race_file->starttimesec = tv.tv_sec;
	race_file->starttimeusec = tv.tv_usec;
	race_state_save(race);
}

void race_state_clean(char *path)
{

	DIR *dh;
	struct dirent *de;
	struct race *race;
	char buf[MAX_PATH];


	// Strip any trailing from the path.
	stripslashes(path);

	if ((dh = opendir(path)) == NULL)
		return;

	while ((de = readdir(dh))) {
		if (!lfnmatch(".racestats-*", de->d_name, LFNM_CASEFOLD)) {
			if ((race = race_state_find(path, de->d_name)))
				race_delete(race);
			snprintf(buf, MAX_PATH-1, "%s/%s", path, de->d_name);
			log_xfer("TEST race deletebad %s\n", 
					 buf);
			remove(buf);
		}
	}
	closedir(dh);
}

void race_show(struct login_node *t, char *args)
{

	struct race *runner;
	int seperator = 0, racertot;
	struct racer *racer;
	struct login_node *l;
	struct data_node *dc;
	float up, down, raceavg;
	char sbuf[80], sbuf2[80];

	lion_printf(t->handle, "200-[ RACES BEGIN ]\r\n"
		".--[RACES]--------------------------------------------------------------------.\r\n");

	if (!races) {
		lion_printf(t->handle, "| %-75.75s |\r\n", "There are currently no races!");
	}

	// cycle each race in memory, print out files done, current files, who etc.
	for (runner = races; runner; runner = runner->next) {

		racertot = race_count(runner);
		raceavg = (runner->bytes / (time(NULL) - runner->starttime)) / 1024.0;

		if (seperator)
			lion_printf(t->handle,
				"|-----------------------------------------------------------------------------|\r\n");

		snprintf(sbuf, sizeof(sbuf), "%d/%d files complete by %d racers averaging %.2fKB/sec",
			runner->filesdone, runner->filestotal, racertot, raceavg);
		lion_printf(t->handle, 
			"| %-75.75s |\r\n"
			"|  %-74.74s |\r\n",
			section_rlsfind(runner->path), sbuf);

		// Go through each racer...
		for (racer = runner->top_racer; racer; racer = racer->next) {

			snprintf(sbuf, sizeof(sbuf),
				"%s %d/%d files avg %.2fKB/s.",
				racer->who, racer->files, runner->filestotal,
				(racer->bytes / (racer->endtime - racer->starttime)) / 1024.0);

			// Find their current node/speed
			if ((l = login_isloggedin(racer->who))) {

				// Are they doing a STOR in this race?
				for (dc = l->data; dc; dc = dc->next) {

					// STORing to this race?
					if (dc->type & DATA_STOR && race_file_find_inrace(runner, path2file(dc->name))) {
						lion_get_cps(dc->handle, &up, &down);
						snprintf(sbuf2, sizeof(sbuf2), "%s Current: %13.13s (%.2fKB/s)",
								sbuf, path2file(dc->name), up);
					} else {
						snprintf(sbuf2, sizeof(sbuf2), "%s Currently idle!", sbuf);
					}

				}

				if (!l->data)
					snprintf(sbuf2, sizeof(sbuf2), "%s Currently idle!", sbuf);

			} else {
				snprintf(sbuf2, sizeof(sbuf2), "%s Currently idle!", sbuf);
			}

			lion_printf(t->handle, "|  %-74.74s |\r\n", sbuf2);
		}

		//"| `-> %s @ %.2fKB/sec (%d files)                                              |\r\n"
		//"|-----------------------------------------------------------------------------|\r\n"

		seperator = 1;
	}

	lion_printf(t->handle,
		"`-----------------------------------------------------------------------------'\r\n"
		"200 [ RACES END ]\r\n");

}




//
// XDUPE is quite poorly thought out, but there is not much we can do
// about that. We will ignore mode 0, as it is far too broken (file names
// with spaces anyone?). Mode 2 and 3 could have merit, and mode 4 is
// pointless, since either your code handles mode 2 or 3 already. Also,
// does the Author of X-Dupe recommend that clients TRUST that the reply
// will not be too long? Buffer over-run anyone? We all remember various
// jpeg buffer over-runs in client code.
//
// Anyway. Sending the reply to 553 is also silly. If you are going to
// implement this, it would make more sense to send it in the 226-
// reply. Then the client need not be slowed down at all.
//
// The "RFC" for X-DUPE currently only talks about "completed/duped" files.
// Whereas, I would consider any file currently "transfering" to be XDUPE
// issued. True it is not complete, but if the sole motive for this is
// efficiency, you want clients to skip those already in progress.
//
// Additionally, I would suggest clients don't remove the items, but re-queue
// them last. That way, they will be checked on at the end of the transfer.
//
//
// LundFTPd. Alas, we do not store all the info I would like in the Race
// stats. However, we can assume that XDUPE is
//
// 1 Any file transfer completed AFTER the time when user's file 
//   transfer started (completed, and duped).
// 2 Any file transfer started AFTER the time when the user's file 
//   transfer started (in progress).
//
// However, "the start time of user's last transfer" is not stored. We will
// have to run through the race-files, to find the latest file by the user.
//
// Then, run through the race-files looking for any item started after.
//
// Either give race struct, or, filename.
void race_xdupe(struct race *race, struct login_node *t, int code, char *name)
{
	struct race_file *file, *newest = NULL;

	consolef("[race] XDUPE processing..\n");

	// no race? Find it..
	if (!race) {
		char *path, *last;

		path = strdup(name);
		if (!path) return;

		last = pathsplit(path);

		race = race_state_find(path, last);

		free(path);
	}

	if (!race)
		return;


	// Find start time of our last transfer.
	for (file = race->top_race_file;
		 file;
		 file = file->next) {

		// Is it users?
		if (!mystrccmp(t->user, file->who)) {

			// Assign "newest" if first time, or, newer file.
			if (!newest || (file->starttimesec > newest->starttimesec))
				newest = file;

		}
	} // all files.

	// Did we find a file previously uploaded by user? If not, there
	// is no XDUPE to send.
	if (!newest) return; 

	
	// Now, find all files completed after ours started. Or, any file
	// currently in-progress. How would we do this test?
	for (file = race->top_race_file;
		 file;
		 file = file->next) {

		// Skip our own file
		if (file == newest) continue;

		// Do we need to filter out anything? Files by this user? They
		// could race multiple times, so we want to send it.
		if (file->starttimesec >= newest->starttimesec) {

			if (t->options & UO_XDUPE_2)
				socket_print(t, "%d- X-DUPE: %*.*s\r\n",
							 code,
							 66, 66, // As per RFC
							 file->file);
			else
				socket_print(t, "%d- X-DUPE: %s\r\n",
							 code,
							 file->file);

		}

	}

}









#endif
