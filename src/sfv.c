#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#ifndef WIN32
#include <dirent.h>
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>

#include "lion.h"
#include "lfnmatch.h"

#include "global.h"
#include "data.h"
#include "login.h"
#include "log.h"
#include "file.h"
#include "socket.h"
#include "misc.h"
#include "section.h"
#include "user.h"
#include "sfv.h"
#include "check.h"
#ifdef RACESTATS
#include "race.h"
#endif
#ifdef IRCBOT
#include "irc.h"
#endif
#ifdef WITH_SSL
#include <openssl/md5.h>
#endif


struct sfv_state *sfv_state_head = NULL;

// Loads the specified SFV file into memory.
struct sfv_state *sfv_file_load(char *pathfile)
{

	struct sfv_state *sfv_state = NULL;
	char work[1024], *split, *ar, *filename, *crc, *tmp;
	FILE *fd;
	char buffer[1024];
	struct stat sstat;

	// Load the file and read the entries from it:
	// BUG: 2005/08/10. Static size 255 here, please use sizeof, and it
	// is far too small to hold the name.
	strncpy(work, pathfile, sizeof(work));
	split = path2file(work);
	*(split - 1) = 0;

	if (!(fd = fopen(pathfile, "rb"))) {
			consolef("[sfv] `-> Unable to open SFV file %s: %s\n", pathfile, strerror(errno));
			return NULL;
	}

	if ((sfv_state = malloc(sizeof(struct sfv_state))) == NULL) {
		consolef("[sfv] FATAL: Could not malloc memory: %s\n", strerror(errno));
		abort();
		_exit(1);
	}
	bzero(sfv_state, sizeof(struct sfv_state));

	strncpy(sfv_state->path, work, 255);
	strncpy(sfv_state->filename, split, 255);

	if (!lfnmatch("*.md5", pathfile, LFNM_CASEFOLD))
		sfv_state->type |= 1;

#ifdef RACESTATS
	if (section_racestats(sfv_state->path))
		race_new(sfv_state->path, sfv_state->filename);
#endif

	if (stat(pathfile, &sstat) != 0)
		strcpy(sfv_state->user, "nobody");
	else {
		char *name = user_get_name_by_uid(sstat.st_uid - server_useridstart);
		snprintf(sfv_state->user, MAXUSERNAME-1, "%s", name ? name : "unknown");
	}

	// Don't need to check strrchr return, we know it'll work because this
	// function is only called with *.sfv files.
	tmp = strrchr(split, '.');

	// Actually not true.. if string is vastly larger than work buffers can
	// hold.
	if (!tmp) return NULL;


	// SFV style file:
	// "01-u2-window_in_the_skies.mp3 a630b87a"
	// MD5 style file:
	// "07af6fa19606b2932e7260a5bf718396 *vx-die.hard.4.0.r03"



	*tmp = 0;
	snprintf(sfv_state->statefilename, 255, ".state-%s", split);
	*tmp = '.';

	while(fgets(buffer, 1024, fd)) {

		if (*buffer == ';')
			continue;
		if (*buffer == ' ')
			continue;

		ar = buffer;


		switch(sfv_state->type&0xff) {
		case 0: // Parse SFV
			{
				int end;
				end = strlen(buffer);

				while((end > 0) && !misc_ishex(buffer[end]))
					end--;
				while((end > 0) && buffer[end] != ' ')
					end--; //ThereMUST be a space

				buffer[end] = 0; // Kill the space
			filename = buffer;
			ar = &buffer[end+1];
			}

			crc = digtoken(&ar, " \r\n");
			if (!crc)
				continue;

			// Hardcoded ints should be defines
			if ((strlen(crc) != 8))
				continue;

			sfv_entry_add(sfv_state, filename, crc);
			break; // SFV


		case 1: // MD5
#ifdef WITH_SSL
			// read CRC
			crc = digtoken(&ar, " \r\n");
			if (!crc)
				continue;

			if (strlen(crc) != 32) continue;

			// It MUST have a space now.
			// The digtoken command above will eat the space. And
			// unfortunately, any spaces that "might" be part of the
			// start of a filename. Ie, " filename". Oh well.


			// It CAN optionally have a "*" if it was generated on Windows.
			if (*ar == '*')
				ar++;

			// The rest should be filename.
			filename = digtoken(&ar, "\r\n");
			if (!filename)
				continue;

			sfv_entry_add(sfv_state, filename, crc);
#endif
			break;

		} // Switch type


	}

	fclose(fd);

	if (!sfv_state->count) {
		sfv_state_free(sfv_state);
		sfv_state = NULL;
		return NULL;
	}

	sfv_state->next = sfv_state_head;
	sfv_state_head = sfv_state;

	sfv_state_save(sfv_state);

	consolef("[sfv] %s loaded with %d entries\n",
				sfv_state->filename, sfv_state->count);

	// Lets stop them from re-uploading this many times.
	file_goroot();
	chmod(pathfile, (mode_t) server_goodfile);
	file_gononroot();



	return sfv_state;

}

void sfv_entry_add(struct sfv_state *sfv_state, char *filename, char *crc)
{

	struct sfv_state_entry *sfve;


	// Here, we ensure we dont add "free files" like .nfo etc, since those
	// are not considered anyway.
	// Also, make sure it's not another .sfv file, since we can get into ugly
	// recursive situations.
	if (check_isfree(filename))
		return;

	if (!lfnmatch("*.sfv", filename, LFNM_CASEFOLD))
		return;

#ifdef WITH_SSL
	if (!lfnmatch("*.md5", filename, LFNM_CASEFOLD))
		return;
#endif


	// Allocate
	if ((sfve = malloc(sizeof(struct sfv_state_entry))) == NULL)
		abort();
	bzero(sfve, sizeof(struct sfv_state_entry));

	// Copy into struct
	strcpy(sfve->filename, filename);

	switch (sfv_state->type&0xff) {
	case 0:
		strncpy(sfve->crc, crc, 8);
		break;
	case 1:
		strncpy(sfve->crc, crc, 32);
		break;
	};

	// Insert into list
	sfve->next = sfv_state->entry;
	sfv_state->entry = sfve;

	// Increment count
	sfv_state->count++;

#ifdef RACESTATS
	{
		struct race *race;

		if (section_racestats(sfv_state->path)) {
			if ((race = race_state_load(sfv_state->path, sfv_state->filename)))
				race_file_new(race, filename);
		}
	}
#endif

}



void sfv_incomplete(char *state_path, int method)
{
	char buf[1024];
	section_t *rootsect;
	int k;

	consolef("[sfv] checking is section has incompletes..\n");

	rootsect = section_find(state_path);

	if (rootsect) {
		char *str;

		str = section_rlsfind(state_path);
		while (*str == '/') str++;
		//stripslashes(str);

        // If the very last char is "/", remove it.
        if (str && (k = strlen(str))) {
                while((k > 0) && str[k-1] == '/') {
                    str[k-1] = 0;
                    k--;
                }
        }

        snprintf(buf, sizeof(buf), "%s/%s-(INCOMPLETE)",
				 rootsect->path,
				 str);
		// we have section/release/CD1-(INCOMPLETE)
		// so we need to convert any "/" after "section/" to be
		// "_"
		k = strlen(rootsect->path) + 1; // plus slash
		for ( ; buf[k]; k++)
			if (buf[k] == '/')
				buf[k] = '_';

		if (file_pathchecklogin(NULL,buf)) {

			file_goroot();

			// Create the dir, or touch it if already there.
			switch(method) {
			case 0:
				if (access(buf, X_OK))
					mkdir(buf, (mode_t)server_permdir);
				else
					utimes(buf, NULL);
				consolef("[sfv] create/touch '%s'\n", buf);
				break;
			case 1:
				rmdir(buf);
				consolef("[sfv] removed incompletes dir '%s'\n", buf);
				break;
			} // switch

			file_gononroot();

		} // patchcheck

	} // rootsect

}


// Wrap OS remove function
int sfv_fileremove(const char *fname, void *arg1)
{
    remove(fname);
    return 1;
}


// Save the statefile to disk
// TODO: Could this whole process be a little less wasteful...
void sfv_state_save(struct sfv_state *sfv_state)
{

	struct sfv_state_entry *run;
	FILE *fp;
	char buf[512];
	int ok, missing, failed, fd;
	ok = missing = failed = 0;

	// Open and truncate a new file (.state-PATHNAME)
	snprintf(buf, sizeof(buf),
			 "%s/%s", sfv_state->path, sfv_state->statefilename);
	if ((fp = fopen(buf, "w+b")) == NULL) {
		consolef("WARNING: Cannot open %s for writing\n", sfv_state->statefilename);
		return;
	}

	// Write to statefile
	fwrite(sfv_state, sizeof(struct sfv_state), 1, fp);
	for (run = sfv_state->entry; run; run = run->next) {

		// Write entry to statefile
		fwrite(run, sizeof(struct sfv_state_entry), 1, fp);

		// Update state on disk for this file.
		switch (run->status) {
			// Create file on disk for missing/failed entries.
			case SFV_FAILED:
				failed++;
				sfv_state_remove(sfv_state->path, run->filename, "-missing");
				sfv_state_remove(sfv_state->path, run->filename, "-unknown");
				sfv_state_create(sfv_state->path, run->filename, "-failed", sfv_state->user);
				break;
			case SFV_MISSING:
				missing++;
				sfv_state_remove(sfv_state->path, run->filename, "-failed");
				sfv_state_remove(sfv_state->path, run->filename, "-unknown");
				sfv_state_create(sfv_state->path, run->filename, "-missing", sfv_state->user);
				break;
			case SFV_OK:
				ok++;
				sfv_state_remove(sfv_state->path, run->filename, "-failed");
				sfv_state_remove(sfv_state->path, run->filename, "-missing");
				sfv_state_remove(sfv_state->path, run->filename, "-unknown");
				break;
			default:
				;
		}
	}
	fclose(fp);


	// Remove old % complete files:
	fnmatch_func("-*COMPLETE_*_OF_*_FILES_*-", sfv_state->path, sfv_fileremove,
				 LFNM_CASEFOLD, NULL);

	// Write % complete file:
	// TODO: Make this configurable in .conf
	snprintf(buf, sizeof(buf), "%s/-[%s]_%sCOMPLETE_%d%%_%d_OF_%d_FILES_[%s]-",
		sfv_state->path, server_greeting ? server_greeting : localhost_a,
		(ok == sfv_state->count) ? "" : "IN",
		(int)(100 * (float)(ok / (float)sfv_state->count)),
		ok, sfv_state->count,
		server_greeting ? server_greeting : localhost_a);

	// Create [in]completes file
	fd = open(buf, O_CREAT|O_TRUNC|O_RDWR, (mode_t)server_permfile);
	if (fd > -1) {
		close(fd);
		file_chown_byname(sfv_state->user, buf);
	}


	// If we are now complete
	if (ok == sfv_state->count) {

#ifdef IRCBOT
		//TODO: store/find SIZE of release

        // Use highbit in type field to say if we have already announced.
        if (!(sfv_state->type & 0xff00)) {
            sfv_state->type |= 0x100;

            if (section_announce(sfv_state->path)) {
                irc_announce("COMPLETE|section=%s|user=%s|dir=%s|size=0\n",
                             section_name(sfv_state->path), sfv_state->user,
                             section_rlsfind(sfv_state->path));
            }

            // Execute autoextract
            if (server_autoextract_diz)
                {
                    consolef("** path '%s'\n", sfv_state->path);

                    snprintf(genbuf, sizeof(genbuf),
                             //"env DIZ_DIR=\"%s\" DIZ_FILE=\"%s\" %s \"%s\"",
                             "%s \"%s\" \"%s\"",
                             //  sfv_state->path, sfv_state->filename,
                             server_autoextract_diz,
                             sfv_state->path,
                             sfv_state->filename);

                    consolef("Executing '%s'\n", genbuf);

                    lion_system(genbuf, 1, LION_FLAG_NONE, NULL);

                }



        }
#endif

		// Find section root to create the directory there. Convert
		// "/" to "_" for the remainder.

		if (section_incompletes(sfv_state->path)) {

			sfv_incomplete(sfv_state->path, 1); // 1 = rmdir

		}


		// Get rid of this SFV out of memory if it's complete.
		sfv_state_free(sfv_state);

	} else { // if complete
		// if incomplete
		consolef("[sfv] checking is section has incompletes..\n");

		if (section_incompletes(sfv_state->path)) {

			sfv_incomplete(sfv_state->path, 0); // 0 = mkdir/touch
		}
	}

}

// Free the state from memory
void sfv_state_free(struct sfv_state *sfv_state)
{

	struct sfv_state_entry *run, *next;
	struct sfv_state *run2;

	// Free entries
	for (run = sfv_state->entry; run; run = next) {
		next = run->next;
		free(run);
	}

	// Hasn't been added yet.
	if (!sfv_state_head)
		return;

	// Find in head list
	if (sfv_state_head == sfv_state) {
		sfv_state_head = sfv_state->next;
		free(sfv_state);
		return;
	}

	for (run2 = sfv_state_head; run2->next; run2 = run2->next) {
		if (run2->next == sfv_state) {
			run2->next = sfv_state->next;
			free(sfv_state);
			break;
		}
	}

}


// Loads an existing statefile from disk.  Expects full path and sfv filename.
// Or full path and .state-xxx filename.
struct sfv_state *sfv_state_load(char *pathfile)
{

	FILE *fp;
	struct sfv_state *sfv_state;
	struct sfv_state_entry *se;
	char buf[256], *path, *sfilename, sfilepath[512], tmp[512];

	strncpy(tmp, pathfile, 511);
	sfilename = strrchr(tmp, '/');
	*sfilename++ = 0;
	path = tmp;

	if (!lfnmatch("*.sfv", pathfile, LFNM_CASEFOLD)
#ifdef WITH_SSL
		|| !lfnmatch("*.md5", pathfile, LFNM_CASEFOLD)
#endif
		) {

			char *work = strrchr(sfilename, '.');
		*work = 0;
		snprintf(buf, 255, ".state-%s", sfilename);
		sfilename = buf;
	}

	snprintf(sfilepath, 255, "%s/%s", path, sfilename);


	// If SFV state is already in memory, just return.
	if (sfv_state_find(sfilename, path))
		return NULL;




	// Open the file.
	if ((fp = fopen(sfilepath, "rb")) == NULL)
		return NULL;

	// Allocate new struct
	if (!(sfv_state = malloc(sizeof(struct sfv_state)))) {
		perror("malloc");
		abort();
		_exit(-1);
	}
	bzero(sfv_state, sizeof(struct sfv_state));

	// Read contents into SFV structure
	if (fread(sfv_state, sizeof(struct sfv_state), 1, fp) != 1) {
		consolef("** WARNING: Corrupt sfv statefile: %s\n", path2file(pathfile));
		fclose(fp); // leak fix?
		free(sfv_state);
		return NULL;
	}
	sfv_state->entry = NULL;
	// Copy the current path into the sfv_state's path.  Could have changed...
	strncpy(sfv_state->path, path, 255);

	// Read contents into SFV entry structures
	while (1) {
		if (!(se = malloc(sizeof(struct sfv_state_entry)))) {
			perror("malloc");
			abort();
			_exit(-1);
		}
		if (fread(se, sizeof(struct sfv_state_entry), 1, fp) != 1) {
			free(se);
			if (feof(fp) || ferror(fp))
				break;
			consolef("** WARNING: Corrupt sfv statefile entry: %s\n", path2file(pathfile));
			break;
		}

		se->next = sfv_state->entry;
		sfv_state->entry = se;

	}

	// Stick on SFV master list
	sfv_state->next = sfv_state_head;
	sfv_state_head = sfv_state;


	// Generally, it is recommended to free the file
	fclose(fp);


	return sfv_state;

}

// Finds sfv_state for a particular file.
struct sfv_state *sfv_state_find(char *filename, char *path)
{

	struct sfv_state *run;
	struct sfv_state_entry *se;
	int sfv = 0, state = 0;


	// Handle .sfv files differently.
	if (!lfnmatch("*.sfv", filename, LFNM_CASEFOLD))
		sfv = 1;
#ifdef WITH_SSL
	if (!lfnmatch("*.md5", filename, LFNM_CASEFOLD))
		sfv = 2;
#endif
	// Handle .state files differently.
	if (!lfnmatch(".state-*", filename, LFNM_CASEFOLD))
		state = 1;

	//===================================================================
	// Look for the filename in memory.
	for (run = sfv_state_head; run; run = run->next) {
		if (sfv) {
			if (strcasecmp(run->filename, filename) == 0)
				return run;
		} else if (state) {
			if (strcasecmp(run->statefilename, filename) == 0)
				return run;
		} else {
			for (se = run->entry; se; se = se->next) {
				if (strcasecmp(se->filename, filename) == 0)
					return run;
			}
		}
	}

	// Exit early when looking for .state files.
	if (state)
		return NULL;

	//====================================================================
	// OK, not in memory, perhaps it's on disk.  Try and load some states.
	if (path) {
		char buf[512];
		DIR *dir;
		struct dirent *dp;
		int cnt = 0;
		if (!(dir = opendir(path)))
			return NULL;

		while ((dp = readdir(dir)) != NULL) {
			if (!lfnmatch(".state-*", dp->d_name, LFNM_CASEFOLD)) {
				snprintf(buf, 511, "%s/%s", path, dp->d_name);
				if (sfv_state_load(buf))
					cnt++;
			}
		}
		closedir(dir);

		// If we loaded some new .state's, try the whole process again.
		if (cnt)
			return sfv_state_find(filename, path);
	}

	//====================================================================
	// Exit after trying .state files if we are looking for a .sfv
	if (sfv)
		return NULL;

	// We are looking for a file that isn't an sfv.  Start loading .sfv files in
	// the path this file comes from.
	// This code should not be run if path is NULL, ie, if it is called by
	// ourselves.. better stop recursion ne?

	if (path) {
		char buf[512];
		DIR *dir;
		struct dirent *dp;
		int cnt = 0;


		if (!(dir = opendir(path)))
			return NULL;

		while ((dp = readdir(dir)) != NULL) {
			if (!lfnmatch("*.sfv", dp->d_name, LFNM_CASEFOLD)) {
				if (!sfv_state_find(dp->d_name, NULL)) {
					snprintf(buf, 511, "%s/%s", path, dp->d_name);
					if (sfv_file_load(buf))
						cnt++;
				}
			}
#ifdef WITH_SSL
			if (!lfnmatch("*.md5", dp->d_name, LFNM_CASEFOLD)) {
				if (!sfv_state_find(dp->d_name, NULL)) {
					snprintf(buf, 511, "%s/%s", path, dp->d_name);
					if (sfv_file_load(buf))
						cnt++;
				}
			}
#endif
		}
		closedir(dir);

		// If we loaded some new .sfv's, try the whole process again.
		if (cnt)
			return sfv_state_find(filename, path);
	}

	// Absolutely wasn't found in memory, in .states or .sfvs.  Copout...
	return NULL;

}


void sfv_state_remove(char *path, char *filename, char *stub)
{
	char buff[1024];

	snprintf(buff, sizeof(buff), "%s/%s%s", path, filename, stub);
	strlower(path2file(buff));
	remove(buff);
}

void sfv_state_create(char *path, char *filename, char *stub, char *user)
{
	char buff[1024];
	int file;

	snprintf(buff, sizeof(buff), "%s/%s%s", path, filename, stub);
	strlower(path2file(buff));
	file = open(buff, O_CREAT|O_TRUNC|O_RDWR, (mode_t) server_permfile);
	if (file > -1)
		close(file);
	file_chown_byname(user, buff);
}


void sfv_state_clean(char *pathfile)
{
    char buf[512], *filename;
	struct sfv_state *sfv_state = NULL;
	struct sfv_state_entry *se;

	consolef("** Cleaning up SFV statefiles for %s\n", hide_path(pathfile));

	// Grab from global list in memory.
	strncpy(buf, pathfile, 511);
	if ((filename = strrchr(buf, '/')))
		*filename++ = 0;

	if ((sfv_state = sfv_state_find(filename, buf)) == NULL) {
		consolef("`-> Unable to find SFV file in global SFV list\n");
		return;
	}

	// Cycle through files from this SFV:
	for (se = sfv_state->entry; se; se = se->next) {
		sfv_state_remove(sfv_state->path, se->filename, "-failed");
		sfv_state_remove(sfv_state->path, se->filename, "-missing");
	}

	// Remove *-*COMPLETE_*_OF_*_FILES-* entries
	fnmatch_func("-*_*COMPLETE_*_OF_*_FILES_*-", buf,  sfv_fileremove,
                 LFNM_CASEFOLD,
				 NULL);

	// Remove the .state-xxx file.
	snprintf(buf, sizeof(buf),
			 "%s/%s", sfv_state->path, sfv_state->statefilename);
	remove(buf);

	// Remove .message
	snprintf(buf, sizeof(buf), "%s/.message", sfv_state->path);
	remove(buf);

	// Remove incomplete directory
	if (section_incompletes(sfv_state->path)) {

		sfv_incomplete(sfv_state->path, 1); // 1 = rmdir

	}

	// Free sfv_state
	sfv_state_free(sfv_state);

}


// The function that does the checksum and returns a status.
int sfv_test(char *pathfile, char *crc)
{
	int fdd = 0;
	unsigned long real_crc, exp_crc;

	consolef("`-> Testing %s %s\n", pathfile, crc);

	fdd = open(pathfile, O_RDONLY
#ifdef WIN32
		|O_BINARY
#endif
		);


	// NO SUCH FILE
	if (fdd < 0) {
		consolef("`-> SFV_child: Failed to open file %s for reading\n", pathfile);
		return SFV_UNKNOWN;
	}

	// Calculate CRC
	exp_crc = (unsigned long)strtoul(crc, NULL, 16);
	real_crc = SFV_GetFileCRC(fdd);
	close(fdd);

	// File is GOOD!
	if (real_crc == exp_crc) {
		consolef("`-> File is GOOD (%08lX)\n", real_crc);
		return SFV_OK;
	}

	// File is EEEEEVIL!@
	consolef("`-> File is EVIL (%08lX != %08lX)\n", real_crc, exp_crc);

	return SFV_FAILED;

}


// Someone elses CRC code, but whose?  Thanks!
unsigned long SFV_MakeCRC32(unsigned long iCRC, const char *buff, long count)
{

	unsigned long CRCtab[]= {
		0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
		0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
		0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
		0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
		0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
		0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
		0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
		0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
		0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
		0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
		0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
		0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
		0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
		0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
		0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
		0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
		0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
		0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
		0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
		0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
		0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
		0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
		0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
		0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
		0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
		0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
		0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
		0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
		0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
		0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
		0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
		0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
		0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
		0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
		0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
		0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
		0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
		0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
		0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
		0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
		0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
		0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
		0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
		0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
		0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
		0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
		0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
		0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
		0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
		0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
		0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
		0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
		0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
		0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
		0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
		0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
		0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
		0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
		0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
		0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
		0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
		0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
		0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
		0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
	};

	if(buff && count) {
		do {
			iCRC = ((iCRC >> 8) & 0xFFFFFF) ^ CRCtab[(unsigned char) ((iCRC & 0xff) ^ *buff++)];
		} while(--count);
	}

	return(iCRC);
}


#define SFV_CHUNK 65536

unsigned long SFV_GetFileCRC(int fd)
{
	unsigned long iCRC = 0xffffffff;
	long iRead;
	char buff[SFV_CHUNK];

	while((iRead = read(fd, buff, SFV_CHUNK)) > 0)
		iCRC = SFV_MakeCRC32(iCRC, buff, iRead);

	iCRC = (iCRC^0xffffffff);

	return(iCRC);
}



char *SFV_GetFileMD5(int fd)
{
#ifdef WITH_SSL
	static unsigned char result[MD5_DIGEST_LENGTH * 2 + 1];
	MD5_CTX md5;
	unsigned char hash[MD5_DIGEST_LENGTH];
	long iRead;
	int i;
	char buff[SFV_CHUNK];

	MD5_Init(&md5);

	while((iRead = read(fd, buff, SFV_CHUNK)) > 0)
		MD5_Update(&md5, buff, iRead);

	MD5_Final(hash, &md5);

	for (i = 0; i < MD5_DIGEST_LENGTH; i++)
		sprintf((char *)&result[i*2], "%02X", hash[i]);

	// Always null terminate the string
	result[MD5_DIGEST_LENGTH * 2] = 0;

	return (char *)result;
#endif
	return "";
}




struct sfv_state *sfv_state_update(char *filename, char *path, int status)
{

	struct sfv_state *ss;
	struct sfv_state_entry *sse;


	// Wasn't found anywhere... who cares then?  Little orphan annie.
	if (!(ss = sfv_state_find(filename, path)))
		return NULL;

	for (sse = ss->entry; sse; sse = sse->next) {
		// Update the status
		if (strcasecmp(sse->filename, filename) == 0) {
			sse->status = status;
			break;
		}
	}
	sfv_state_save(ss);

	return ss;

}


// Find the CRC field for this one.
char *sfv_state_crc(char *filename, char *path)
{

	struct sfv_state *ss;
	struct sfv_state_entry *sse;

	if (!(ss = sfv_state_find(filename, path)))
		return NULL;
	for (sse = ss->entry; sse; sse = sse->next) {
		if (strcasecmp(sse->filename, filename) == 0)
			return sse->crc;
	}

	return NULL;

}

int sfv_state_count(struct sfv_state *ss, int status)
{
	struct sfv_state_entry *sse;
	int cnt = 0;

	for (sse = ss->entry; sse; sse = sse->next) {
		if (sse->status == status)
			cnt++;
	}

	return cnt;
}

//-------------------------------------------------------------------
// SITE COMMANDS


void sfv_site_incompletes(struct login_node *t, char *args)
{

	struct sfv_state *ss;

	// Header
	socket_print(t,
			"200-[ INCOMPLETES BEGIN ]\r\n");
	socket_print(t,
			".-----------------------------------------------------------------------------.\r\n");
	socket_print(t,
			"| Release                                                      | Files |   %%  |\r\n");
	socket_print(t,
			"|-----------------------------------------------------------------------------|\r\n");

	// No releases
	if (!sfv_state_head) {
		socket_print(t,
			"| No incomplete releases!                                                     |\r\n");
		socket_print(t,
			"`-----------------------------------------------------------------------------'\r\n");
		socket_print(t,
			"200 [ INCOMPLETES END ]\r\n");
		return;
	}


	// Cycle through SFVs
	for (ss = sfv_state_head; ss; ss = ss->next) {

		if (!section_stealth(ss->path))
			socket_print(t,
					 "| %-60.60s | %2d/%02d | % 3.0f%% |\r\n",
					 path2deepest(ss->path, 2), sfv_state_count(ss, SFV_OK), ss->count,
					 (sfv_state_count(ss, SFV_OK) / (float)ss->count) * 100);
	}

	socket_print(t,
			"`-----------------------------------------------------------------------------'\r\n");
	socket_print(t,
			"200 [ INCOMPLETES END ]\r\n");
}
