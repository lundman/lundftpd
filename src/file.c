
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SYS_FILE_H
#include <sys/file.h>
#endif


#if HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif
#if HAVE_SYS_STATFS_H
#include <sys/statfs.h>
#endif

#ifdef WIN32
#include <errno.h>
#include "win32.h"
#include "dirent.h" /* For what opendir() returns, see man opendir */
#else
#include <dirent.h> /* For what opendir() returns, see man opendir */
#endif // WIN32


#define MYDIR DIR  /* Posix has DIR *opendir(char *path) */

#include <sys/stat.h>

#include <time.h>

#if HAVE_SYS_VFS_H
#include <sys/vfs.h>
#endif

#if HAVE_SYS_STATVFS_H
#include <sys/statvfs.h>
#endif



#include "global.h"
#include "misc.h"
#include "socket.h"
#include "login.h"
#include "data.h"
#include "file.h"
#include "check.h"
#include "quota.h"
#include "fnmatch.h"
#include "user.h"
#include "userflag.h"
#include "log.h"
#include "sfv.h"
#include "global.h"
#include "dupe.h"
#include "global.h"
#include "groups.h"
#include "section.h"
#ifdef RACESTATS
#include "race.h"
#endif

#ifdef IRCBOT
#include "irc.h"
#endif

#if HAVE_ERRNO_H
#  include <errno.h>
#endif /*HAVE_ERRNO_H*/
#ifndef errno
/* Some systems #define this! */
extern int errno;
#endif


void file_retr(struct data_node *d)
{
	struct stat st;
	int p;

	consolef("file_retr(): %s\n", d->name);

	d->althandle = NULL;

	if (!file_pathcheck(d)) {
		d->login->data_restart = 0;
		socket_number(d->login, 550, path2file(d->name));
		return ;
	}

	if (stat(d->name, &st)) { /* Stat the file */
		d->login->data_restart = 0;
		socket_number(d->login, 550, path2file(d->name));
		return;
	}

	/* Check its a file? Later */

	/* For the transfer greeting, gets 0-ed again. */
	d->bytes = (lion64u_t) st.st_size;

	/* Check quota first */
	if (!(p = quota_retr(d, &st))) {
		d->login->data_restart = 0;
		consolef("quota_retr failed -- abort %d\n", p);
		// 550 is sent from quota.c
		//    socket_number(d->login, 550, path2file(d->name));
		return;
	}

	//	consolef("quota %d\n", p);

	file_gouser(d->login);

	d->althandle = lion_open(d->name, O_RDONLY, 0400,
							 0, d);

	file_gononuser();

	if (!d->althandle) {
		d->login->data_restart = 0;
		socket_number(d->login, 550, path2file(d->name));
		return;
	}

	// not needed, lion is all nonblocking
	//fcntl(d->altsocket,F_SETFL,(fcntl(d->altsocket,F_GETFL)|O_NONBLOCK));

	/* Check if we need to FF anything */
	if (lseek(lion_fileno(d->althandle),
			  (off_t) d->login->data_restart,
			  SEEK_SET) < 0)
		perror("file_retr(): lseek failed.");

	d->rest_value = d->login->data_restart;
	d->login->data_restart = 0;

	// No longer needed.
	//d->status |= ST_ACTIVE;

	/* here, bump it's download count, if wanted... */
	// This code has been retired as we now have real groups.
	//#ifdef GID_COUNT_HACK
	//file_bump_gid(d);
	//#endif
}


void file_stor(struct data_node *d, int type)
{
#ifdef DUPECHECK
	struct dupe_node *dupe;
#endif

	consolef("file_stor(): %s\n", d->name);

	d->althandle = NULL;

	// Can we write to this path?
	if (!file_pathcheck(d)) {
		d->login->data_restart = 0;
    	socket_number(d->login, 550, path2file(d->name));
		// data_close(d);
		return ;
	}

	// Is the file denied?
	if (section_denyfile(d->name)) {
		d->login->data_restart = 0;

		socket_print(d->login, "550 File '%s' is denied from being "
					 "uploaded.\r\n", path2file(d->name));
		consolef("Denied the uploading of file '%s' by %s\n",
				 path2file(d->name), d->login->user);
		return;

	}


	// LOGIC CHANGE! We No longer have UF_OVERWRITE!! We let the file
	// permissions decide if we can overwrite now. That way people can complete
	// things themselves.

	switch(type) {
	case FILE_OLD:
#if 0
		if (!access(d->name, F_OK) &&
			(!((d->login->level|server_defaultflags)&UF_OVERWRITE)) &&
			!file_isowner(d->login, d->name)) {

			d->login->data_restart = 0;
			socket_number(d->login, 599, path2file(d->name));
			d->status = ST_NONE;
			/*data_close(d); */
			consolef("file_stor(%p): Denied overwriting %s.\n", d->login->handle,
					 d->name);
			return;
		}
#else
		if (0) {

		}
#endif
		// This else comes from above.
		else {

			/*	consolef("Opening %s\n", d->name);*/

#ifdef DUPECHECK
			/*
			 * Check is dupecheck is on
			 * = yes, is it in the list of paths for dupe-checking?
			 * = yes, is it NOT a free-file?
			 * = yes, can we find an entry for this file in DUPE list?
			 * = yes, fail!!
			 */

			if (server_dupecheck
				&& section_dupecheck(d->name)
				&& !check_isfree(d->name)
				&& (dupe = dupe_find(path2file(d->name))))
				{

					d->status = ST_NONE;
					d->bytes = 0;
					d->login->data_restart = 0;

					log_xfer("STOR %s failed (DUPE %s %lu) %s\n",
							 d->login->user,
							 dupe->username, dupe->filesize,
							 hide_path(d->name));

#ifdef RACESTATS
					if (d->login->options & (UO_XDUPE_2|UO_XDUPE_3))
						race_xdupe(NULL, d->login, 553, d->name);
#endif

					socket_number(d->login, 1553, (char *) dupe);

					return;

				}
#endif

#ifdef RACESTATS
			/* Start a new file off.
			{
				char *filename;
				filename = pathsplit(d->name);
				race_file_start(d->name, filename);
				pathfixsplit(d->name, filename);
			}
			*/
#endif



			file_gouser(d->login);


			d->althandle = lion_open(d->name, O_WRONLY|O_CREAT,
									 (mode_t) server_permfile,
									 LION_FLAG_EXCLUSIVE,
									 d);

			file_gononuser();


			/* Here we should reset the download count */
			//#ifdef GID_COUNT_HACK

			//if (d->althandle) {
			//	file_goroot();
			//	chown(d->name, -1, GID_COUNT_OFFSET);
			//	file_gononroot();
			//}

			//#endif

		}
		break;


		// Whoopsie part 2, same for STOR as some resume is with REST&STOR
		// as opposed to APPE.


		/*
		 * Whoopsie.
		 *
		 * APPEND does not guarantee only 1 writer on the file, so
		 * we could get multiple resume uploads on the same file resulting
		 * in a corrupt file.
		 *
		 * We now attempt to flock the file too (sufficient as we are all one
		 * process)
		 *
		 * NB: I don't unlock it anywhere, just free it.
		 *
		 */

	case FILE_APPE:
		//		consolef("file_appe(): appending\n");

		file_gouser(d->login);


		d->althandle = lion_open(d->name, O_WRONLY|O_CREAT|O_APPEND,
								 (mode_t) server_permfile,
								 LION_FLAG_EXCLUSIVE,
								 d);

		file_gononuser();

		break;

	default:
		d->althandle = NULL;
	}

	if (!d->althandle) {

		perror("file_stor(x): ");
		d->login->data_restart = 0;

#ifdef RACESTATS
		if (d->login->options & (UO_XDUPE_2|UO_XDUPE_3))
			race_xdupe(NULL, d->login, 553, d->name);
#endif


		d->status = ST_NONE;
		socket_number(d->login, 553, path2file(d->name));
		/*    data_close(d); */
		consolef("file_stor(%p): Failed to open for write?: %s\n",
				 d->login->handle, path2file(d->name));
		return;
	}

	//	consolef("file_stor(%p): opening %p\n", d->handle, d->althandle);


	/* Check if we need to FF anything */
	// This was broken as it would always lseek, even when REST was 0, and
	// we are in APPE. Oh dear.

	if (d->login->data_restart &&
		(lseek(lion_fileno(d->althandle),
			   (off_t) d->login->data_restart, SEEK_SET) < 0))
		perror("file_retr(): lseek failed.");

	d->rest_value = d->login->data_restart;
	d->login->data_restart = 0;

	file_chown(d->login, d->name);
}



void file_load()
{

	file_goroot();

	if (server_usechroot) {

		if (!server_usesoftchroot) {
#ifndef WIN32
			if (geteuid() || chroot(server_usechroot) || chdir("/") ) {
				if (!geteuid()) perror("chroot()");
				consolef("Enabling software chroot(%s)\n", server_usechroot);

			} else {

				server_hasroot = 1;
				server_usechroot = 0; /* it was sucessful, turn off all checks */
				strcpy(server_usechroot, "/");

				/* Nicety */

			}
#endif // WIN32

		} else {
			if (!geteuid()) server_hasroot = 1;
		}
	}

	file_gononroot();

}

#if 1
int file_pathcheck(struct data_node *d)
{
	static char fullpath[1024]; /* The soon to be new path. */

	*fullpath = (char) 0;

	if (!server_usechroot) { /* BUG fix 30/11/95
							  * this was executed in non-root execution :(
							  * it's mean to be run when chroot is not used or
							  * root's chroot() was executed correctly */
		if (*d->name != '/') {
			strcpy(fullpath, d->login->cwd);
		}
		strcat(fullpath, d->name);

		if (strcmp(fullpath, d->name)) {
			free(d->name);
			d->name = mystrcpy(fullpath);
		}

		return 1;
	}

	/* If usechroot */

	if (strncmp(server_usechroot, d->name, strlen(server_usechroot))) {

		strcpy(fullpath, server_usechroot); /* the real root */

		if (*d->name != '/') {
			strcat(fullpath, d->login->cwd);
		}

		strcat(fullpath, d->name);

	} else strcpy(fullpath, d->name);

	file_undot(fullpath);

	if (strncmp(server_usechroot, fullpath, strlen(server_usechroot)))
		return 0;

	if (strcmp(fullpath, d->name)) {
		free(d->name);
		d->name = mystrcpy(fullpath);
	}


#if 1
	// Check if it's group dirs
	if (server_grouppath) {
		int len, end;
		unsigned char remain;

		len = strlen(server_grouppath);

		if (!strncasecmp(d->name, server_grouppath, len)) {
			/* Ok, we are now looking inside group directory
			 * we need to verify they are allowed to look in here
			 */

			// Make sure we are onto the next directory bit.
			while( d->name[len] == '/' ) len++;

			/* If d->name stops here, all is well */

			if (d->name[len]) {  /* there is more */

				end = len;

				while( d->name[end] && d->name[end] != '/' ) end++;

				remain = d->name[end];

				/* temporarily terminate string */
				d->name[end] = 0;

				//				printf("file: checking group perm %s and %s\n",
				//   d->login->user, &d->name[len]);

				if (!group_hasuser_byname(&d->name[len], d->login->user)) {

					d->name[end] = remain;
					return 0;

				}

				d->name[end] = remain;

			}  // d->name[len]

		} // strncase

	} // grouppath
#endif



	consolef("file_pathcheck(%p): %s - %s\n", d->handle, d->name, fullpath);

	return 1;

}

int file_pathchecklogin(struct login_node *l, char *path)
{ /* There are TWO versions of this function ! */

	static char fullpath[1024]; /* The soon to be new path. */

	*fullpath = (char) 0;

	if (strncmp(server_usechroot, path, strlen(server_usechroot))) {
		/* If it doesn't start with the server_usechroot */

		strcpy(fullpath, server_usechroot); /* the real root */

		if (l && *path != '/') { /* If they didn't reference with a leading / */
			strcat(fullpath, l->cwd);  /* add the current dir */
		}

		strcat(fullpath, path);    /* Now add the path */

	} else strcpy(fullpath, path);

	file_undot(fullpath); /* Remove and cleanup strangeness */

	/* If the cleanup made in not start with server_usechroot, deny access! */
	if (strncmp(server_usechroot, fullpath, strlen(server_usechroot)))
		return 0;

	if (strcmp(fullpath, path)) { /* If the path's differ.. */
		strcpy(path, fullpath); /* Copy over the new path, is this legal? */
	}


#if 1
	// Check if it's group dirs
	if (server_grouppath) {
		int len, end;
		unsigned char remain;

		len = strlen(server_grouppath);

		if (!strncasecmp(path, server_grouppath, len)) {
			/* Ok, we are now looking inside group directory
			 * we need to verify they are allowed to look in here
			 */

			// Make sure we are onto the next directory bit.
			while( path[len] == '/' ) len++;

			/* If path stops here, all is well */

			if (path[len]) {  /* there is more */

				end = len;

				while( path[end] && path[end] != '/' ) end++;

				remain = path[end];

				/* temporarily terminate string */
				path[end] = 0;

				//				printf("file: checking group perm %s and %s\n", l->user, &path[len]);

				if (l && !group_hasuser_byname(&path[len], l->user)) {

					path[end] = remain;
					return 0;

				}

				path[end] = remain;

			}  // path[len]

		} // strncase

	} // grouppath
#endif


	consolef("file_pathchecklogin(%p): '%s' %s - %s\n",
			 l ? l->handle : 0,
			 server_usechroot, path, fullpath);

	return 1;

}

#else

int file_pathcheck(struct data_node *d)
{
	return file_pathchecklogin(d->login, d->name);
}


int file_pathchecklogin(struct login_node *l, char *path)

	 int file_pathchecker(struct login_node *l, char *path)
{
	static char fullpath[1024]; /* The soon to be new path. */

	*fullpath = (char) 0;

	if (!server_usechroot) { /* BUG fix 30/11/95
							  * this was executed in non-root execution :(
							  * it's mean to be run when chroot is not used or
							  * root's chroot() was executed correctly */
		if (*d->name != '/') {
			strcpy(fullpath, l->cwd);
		}
		strcat(fullpath, path);

		if (strcmp(fullpath, path)) {
			free(d->name);
			d->name = mystrcpy(fullpath);
		}

		return 1;
	}

	/* If usechroot */

	if (strncmp(server_usechroot, d->name, strlen(server_usechroot))) {

		strcpy(fullpath, server_usechroot); /* the real root */

		if (*d->name != '/') {
			strcat(fullpath, d->login->cwd);
		}

		strcat(fullpath, d->name);

	} else strcpy(fullpath, d->name);

	file_undot(fullpath);

	if (strncmp(server_usechroot, fullpath, strlen(server_usechroot)))
		return 0;

	if (strcmp(fullpath, d->name)) {
		free(d->name);
		d->name = mystrcpy(fullpath);
	}

	consolef("file_pathcheck(%p): %s - %s\n", d->handle, d->name, fullpath);

	return 1;

}
#endif


void file_undot(char *s)
{
	/*
	 * remove /./
	 *        /../
	 *        /..
	 *     and now also remove // because it looks nicer.
	 *     and finally, things ending with /.
	 */

	char *r, *e;

	//	consolef("file_undot() from '%s'\n", s);

    /* In Win32 we say //c/dir/file.ext */
#ifdef WIN32
	while((r = (char *) strstr(&s[1], "//")))
#else
	while((r = (char *) strstr(s, "//")))
#endif
		{

#if 0 // over-lapping strcpy() is NOT allowed
			strcpy(&r[1], &r[2]);
#else
			bcopy(&r[2], &r[1], strlen(&r[1]));
#endif

		}

	while((r = (char *) strstr(s, "/./"))) {

#if 0
		strcpy(&r[1], &r[3]);
#else
        bcopy(&r[3], &r[1], strlen(&r[1]));
#endif

	}

	while((r = (char *) strstr(s, "/../"))) {
		*r = (char) 0;
		if (!(e = (char *) strrchr(s, '/'))) {
			/* Tried to CD too far */
#if 0
			strcpy(&r[1], &r[4]);
#else
			bcopy(&r[4], &r[1], strlen(&r[1]));
#endif
			continue;
		}

#if 0
		strcpy(e, &r[3]);
#else
        bcopy(&r[3], e, strlen(&r[3])+1);
#endif

	}

	if (cmptail(s, "/..")) {
		s[strlen(s)-3] = (char) 0;
		if ((e = (char *) strrchr(s, '/')))
			e[1] = (char) 0;
	}

	if (cmptail(s, "/.")) {
		s[strlen(s)-1] = (char) 0;
	}

	if (!*s) { /* Make sure it's always "/" and not "" */
		s[0] = '/';
		s[1] = (char) 0;
	}

	//	consolef("file_undot() to '%s'\n", s);

}


int file_isdir(struct login_node *t, char *s)
{
	char path[1024];
	struct stat pstat;

	/* This function is strange? */

	if (strncmp(s, server_usechroot, strlen(server_usechroot)))
		sprintf(path, "%s/%s/.", server_usechroot, s);
	else
		strcpy(path, s);

	//consolef("Checking for dir '%s'\n", s);

	if (access(path, F_OK) || stat(path, &pstat)) {
		return 0;
	}

	if (!(pstat.st_mode & S_IFDIR)) {
		return 0;
	}

	return 1;
}

void file_mkdir(struct login_node *l, char *path)
{
	char path2[1024];
	int fd,i;
	char *spcptr=path;

	if (section_convertmkdspace(path)) {
		while ( (spcptr=strchr(spcptr,' '))!=NULL )
			*spcptr='_';
	}

	consolef("file_mkdir(%p): creating '%s'\n", l->handle, path);

	if (server_usechroot && !file_pathchecklogin(l, path)) {
		socket_number(l, 550, path2file(path));
		return ;
	}


	file_gouser(l);

	if (*path && !mkdir(path, (mode_t) server_permdir)) { /* UMASK! */
		/*250 MKD command successful.*/
		socket_print(l, "250 MKD command successful.\r\n");

		/*    chmod(path, 0777);*/

		// Create 0 sized file in new directory
		if (server_mkdirstring) {

			sprintf(genbuf, "%s/%s", path, server_mkdirstring);
			sprintf(path2, genbuf, l->user);
			//			consolef("DEBUG: making new file %s\n", path2);

			fd = open(path2, O_RDWR | O_CREAT | O_TRUNC
#ifdef WIN32
					  |O_BINARY
#endif
					  ,
					  (mode_t) server_permfile);

			if (fd >= 0)
				close(fd);

		}

		file_gononuser();

		file_chown(l, path);

		log_xfer("MKD %s %s\n", l->user, hide_path(path));


#ifdef IRCBOT
		// this if always fail...
		if (section_announce(path)) {
			irc_announce("MKD|section=%s|user=%s|dir=%s|time=screwyou\n",
						 section_name(path), l->user, section_rlsfind(path));
		}
#endif

#ifdef DUPECHECK


		// Add this dir to the dupedb so we can search for it later.
		// The issue here is we only want to add top-level dirs, and not those
		// CD2 and COMPLETE ones.

		// Both the new code here and the dupe stuff below just wants top level
		// dirs.

		if (server_dupecheck && section_dupecheck(path)) {

			consolef("file_mkd(): within DUPE area - considering..\n");


			// Don't add to NEW if it is a stealth section.
			if (!section_stealth(path)) {

				// Add to site new?
				if (global_newn[NUM_NEW - 1]) {

					free(global_newn[0]);

					for (i = 1; i < NUM_NEW; i++) {
						//strlcpy(global_newu[i - 1], global_newu[i],sizeof(global_newu[0]));
						strncpy((char *)global_newu[i - 1], (char *)global_newu[i],MAXUSERNAME);
						global_newu[i - 1][MAXUSERNAME-1] = 0;

						global_newd[i - 1] = global_newd[i];
						global_newn[i - 1] = global_newn[i];
					}

					//strlcpy(global_newu[NUM_NEW - 1],l->user,sizeof(global_newu[0]));
					strncpy((char *)global_newu[NUM_NEW - 1],l->user,MAXUSERNAME);
					global_newu[NUM_NEW - 1][MAXUSERNAME-1] = 0;

					global_newn[i - 1] = (unsigned char *)mystrcpy(hide_path(path));
					global_newd[NUM_NEW - 1] = global_time_now;

				} else {

					for (i = 0; i < NUM_NEW; i++) {
						if (global_newn[i] == NULL) {
							//strlcpy(global_newu[i],l->user,sizeof(global_newu[0]));
							strncpy((char *)global_newu[i],l->user,MAXUSERNAME);
							global_newu[i][MAXUSERNAME-1] = 0;

							global_newn[i]=(unsigned char *)mystrcpy(hide_path(path));
							global_newd[i] = global_time_now;
							break;
						}
					}
				}
			}

			// Add to dupedb
			dupe_add(hide_path(path), l->user, DUPE_IS_DIR, global_time_now);

		}
#endif

		return;
	}


	/* 550 Permission denied.*/
	socket_print(l, "550 %s.\r\n", strerror(errno));
	perror("mkdir()");


	file_gononuser();
}



void file_rmdir(struct login_node *l, char *path)
{

	consolef("file_rmdir(%p): deleting '%s'\n", l->handle, path);

	if (server_usechroot && !file_pathchecklogin(l, path)) {
		socket_number(l, 550, path2file(path));
		return ;
	}

	if (!file_isowner(l, path) && (!((l->level|server_defaultflags)&UF_DELETE)))
		{
			socket_print(l, "550 Permission denied.\r\n");
			return;
		}


	if (!((l->level|server_defaultflags)&UF_DELETE))
		file_gouser(l);
	else
		file_goroot();

#ifdef RACESTATS
	// Quick test here for our statefiles.  If all that remains in the dir are
	// .racestats-* frag those then rmdir.
	{

		DIR *dh;
		int removestate = 1;
		struct dirent *de;

		if ((dh = opendir(path))) {
			while ((de = readdir(dh))) {
				if (strcmp(".", de->d_name)
						&& strcmp("..", de->d_name)
						&& fnmatch(".racestats-*", de->d_name, FNM_CASEFOLD)
				) {
					removestate = 0;
					break;
				}
			}

			if (removestate)
				race_state_clean(path);

			closedir(dh);
		}

	}
#endif



	if (*path && !rmdir(path)) { /* UMASK! */

		/*250 RMD command successful.*/
		socket_print(l, "250 RMD command successful.\r\n");

#ifdef IRCBOT
		if (section_announce(path)) {
			irc_announce("RMD|section=%s|user=%s|dir=%s|time=screwyou\n",
						 section_name(path), l->user, section_rlsfind(path));
		}
#endif

		log_xfer("RMD %s %s\n",
				 l->user, hide_path(path));


		if (!((l->level|server_defaultflags)&UF_DELETE))
			file_gononuser();
		else
			file_gononroot();

		return;
	}

	/* 550 Permission denied.*/
		socket_print(l, "550 FAILED: %s\r\n", strerror(errno));
	perror("rmdir()");

	if (!((l->level|server_defaultflags)&UF_DELETE))
		file_gononuser();
	else
		file_gononroot();


}



void file_checkmessage(struct login_node *l, int num)
{
	char data[1024];
	FILE *ff;

	sprintf(data, "%s/%s/.message", server_usechroot, l->cwd);

	if (access(data, R_OK)) {

		// Alas, file_parsedir will attempt to copy string back to
		// "path" which in this case is a static string. This is bad.
#if 0
		ret = file_parsedir(l, "file_id.diz",
							0, FILE_FINDFIRST, FNM_CASEFOLD);
		if (!ret) return;

		sprintf(data, "%s/%s/%s", server_usechroot, l->cwd, ret);
		if (access(data, R_OK)) return;
#endif
		return;
	}


	if (!(ff = fopen(data, "r"))) return;
	while(fgets(data,256,ff)) {
		chop2(data);
		socket_print(l, "%d-%s\r\n", num, data);
	}

	fclose(ff);
}





//
// This function is obsolete and should probably not be called anymore?
char *file_parsedir(struct login_node *l, char *args, unsigned int sock,
					int flags,
					int matchflag)
{
	char path[1024];
	static char ret[1024];
	char *r, match[1024];
	MYDIR *dirr;
	struct dirent *dp;
	struct stat sb;
	time_t newscan = 0;

	//consolef("file: checking for match '%s'.\n", args);

	if (flags & FILE_NEWSCAN) {
		newscan = (time_t) sock;
	}


	// I changed these lines so that SFV check could fine a file when
	// ->cwd was already defined with the rootdir.
	//
	*path = 0;
	strcpy(path, l->cwd);
#if 0
	if (server_usechroot)
		{
			strcpy(path, server_usechroot);
			strcat(path, l->cwd);
		} else
			strcpy(path, l->cwd);
#endif


	if ((r = strrchr(args, '/'))) { /* args contains a directory path */
		strcpy(match, r+1);
		r[1] = (char) 0;

		strcat(path, args);
	} else strcpy(match, args);


	/* Check the path */
	if (!file_pathchecklogin(l, path)) {
		consolef("file: pathcheck doesn't like it\n");
		return 0;
	}

	/* Now, try to open dir path... */
	if (!(dirr = opendir(path))) {
		perror("file: opendir");
		return 0; /* no such directory */
	}


	/* Special case, if they ask for "." then list all in current dir. */
	if (!strcmp(match, ".")) *match = 0;

	while ((dp = readdir(dirr)) != NULL)
		{

			if (!strcmp(dp->d_name, "."))
				continue;

			if (!strcmp(dp->d_name, ".."))
				continue;

			if (!strcmp(dp->d_name, ".message"))
				continue;


			if ((flags & FILE_NEWSCAN)) {

				sprintf(genbuf, "%s%s", path, dp->d_name);

				if (!stat(genbuf, &sb) && (newscan <= sb.st_mtime)) {


					/* It sucks to have the print line here! */

					socket_print(l,
								 "200-| %-8s | %12s | %3d | %03d.%1d | %-31s |\r\n",
								 user_get_username(sb.st_uid),
								 misc_file_date(sb.st_mtime),
								 file_num_dir_entries(path, dp->d_name, NULL),
								 0, 0,
								 dp->d_name);

				}

				continue;
			}


			if (!*match || !(fnmatch(match, dp->d_name, matchflag))) {
				/* This is our file!! */

				if (flags & FILE_FINDFIRST) {

					if (r)
						sprintf(ret, "%s%s/", args, dp->d_name);
					else
						sprintf(ret, "/%s", dp->d_name);

					//					consolef("Match, returning '%s'\n", ret);

					closedir(dirr);
					return ret;

				} else {

#if 0 // LION uses libdirlist
					if (flags & FILE_NLST) {
						int foo;

						foo = l->socket;
						l->socket = sock;

						if (r)
							socket_print(l, "%s%s\r\n", args, dp->d_name);
						else
							socket_print(l, "%s\r\n", dp->d_name);

						l->socket = foo;


					}
#endif

				}
			}
		}
	closedir(dirr);
	//	consolef("file: all done\n");

	return 0;
}


void file_gononroot(void)
{
	if (server_usesetuid && server_hasroot)
		{
			if (geteuid())
				{
					consolef("usesetuid is set, but we aren't running as root? chown disabled.\n");
					server_usesetuid = 0;
					return ;
				}

			if (seteuid(getuid())) perror("seteuid");
		}
}

void file_goroot(void)
{
	if (server_usesetuid && server_hasroot)
		{
			if (seteuid(0)) perror("seteuid");
		}
}


void file_gouser(struct login_node *l)
{
	if (!server_hasroot)
		return;

	file_goroot();


	if (seteuid(user_getuid(l))) perror("seteuid");

	if (user_getuid(l) != geteuid())
		consolef("WARNING! user_getuid %d != geteuid %d\n", user_getuid(l),
				 geteuid());

}


void file_gononuser()
{
	if (!server_hasroot)
		return ;

	file_goroot();
	file_gononroot();

}



void file_chown(struct login_node *l, char *fname)
{

	if (server_hasroot) {
		file_goroot();
		chown(fname, user_getuid(l), user_getgid(l));
		file_gononroot();
	}
}

void file_chown_byname(char *user, char *fname)
{

	if (server_hasroot) {
		file_goroot();
		chown(fname, user_getuidbyname(user), user_getgidbyname(user));
		file_gononroot();
	}

}

void file_delete(struct login_node *l, char *path)
{
	struct stat sb;
	int owner;

	consolef("file_delete(%p): deleting '%s'\n", l->handle, path);

	if ((server_usechroot && !file_pathchecklogin(l, path)) || stat(path,&sb)) {
		socket_number(l, 550, path2file(path));
		return;
	}


	owner = file_isowner(l, path);

	if (!owner && (!((l->level|server_defaultflags)&UF_DELETE))) {
		socket_print(l, "550 Permission denied.\r\n");
		return;
	}


	// SET USER UID
	if (!((l->level|server_defaultflags)&UF_DELETE))
		file_gouser(l);
	else
		file_goroot();


	// Try to delete
	if (*path && !unlink(path)) { /* UMASK! */


		// RESET USER UID
		if (!((l->level|server_defaultflags)&UF_DELETE))
			file_gononuser();
		else
			file_gononroot();



		socket_print(l, "257 DELE command successful.\r\n");

		log_xfer("DELE %s %s\n", l->user, hide_path(path));

		// REMOVE SFV STATE
		if (!fnmatch("*.sfv", path, FNM_CASEFOLD))
			sfv_state_clean(path);

		// UPDATE SFV STATE FOR CHECK_INTERNAL FILES
		if ((void *)check_find_exe(path) == (void *)CHECK_INTERNAL) {
			char *filename, buf[MAX_PATHLEN];
			strncpy(buf, path, MAX_PATHLEN-1);
			filename = strrchr(buf, '/');
			*filename++ = 0;
			sfv_state_update(filename, buf, SFV_MISSING);
		}

		// TODO - announce incomplete rls...

		// Remove DUPE/CREDITS
		if (!check_isfree(path)) {

#ifdef RACESTATS
			{
				char *filename;
				filename = pathsplit(path);
				race_file_deleted(path, filename);
				pathfixsplit(path, filename);
			}
#endif

			/* Not free, remove from dupe */
#ifdef DUPECHECK
			if (server_dupecheck && section_dupecheck(path))
				dupe_del(path2file(path));
#endif

			/* If we owned it, remove credits */
			if (owner && l->quota) {

				if (((struct quota_node *)l->quota)->bytes_up >
					(lion64u_t) sb.st_size)
					{
						((struct quota_node *)l->quota)->bytes_up -=
							(lion64u_t) sb.st_size;
					} else {
						((struct quota_node *)l->quota)->bytes_up = 0;
					}
			}

		} // DUPE/CREDITS

	} else {


		// RESET USER UID
		if (!((l->level|server_defaultflags)&UF_DELETE))
			file_gononuser();
		else
			file_gononroot();

		/* 550 Permission denied.*/
		socket_print(l, "550 %s.\r\n", strerror(errno));
		perror("unlink()");
	}

}

int file_isowner(struct login_node *l, char *path)
{
	/* true is we own path, and false if not */
	struct stat st;

	if (stat(path, &st) < 0)
		return 0;

	if (st.st_uid == user_getuid(l))
		return 1;

	return 0;

}


/*
 * This sucks, is there really no better way to count number of entries?
 */
int file_num_dir_entries(char *path, char *dir, unsigned long *bytes)
{
	int items = 0;
	static char realpath[1024], *r;
	DIR *dirr;
	struct dirent *dp;
	struct stat sb;


	if (bytes) *bytes = 0;


	snprintf(realpath, sizeof(realpath), "%s/%s/", path, dir);
	r = &realpath[strlen(realpath)];


	if (!(dirr = opendir(realpath))) {
		return 0;
	}

	while ((dp = readdir(dirr)) != NULL) {

		if (!strcmp(dp->d_name, "."))
			continue;

		if (!strcmp(dp->d_name, ".."))
			continue;

		items++;


		// Only tally up bytes if we actually WANT bytes...
		if (bytes) {

#ifdef DT_REG   /* Speed up */
			if (dp->d_type & DT_REG)
#endif
				{
					strcpy(r, dp->d_name);

					if (!stat(realpath, &sb) && (sb.st_mode & S_IFREG)) {

						// FIXME - unsigned long is woefully inadequate
						*bytes += (unsigned long) sb.st_size;

					}

					*r = 0;

				}

		}

	}

	closedir(dirr);


	return items;

}

int file_wipe_recursive(char *fullpath, struct wipedata **wdata, int del)
{

	DIR *dirp;
	struct dirent *dp;
	struct stat sb;
	char path[1024], *user, *group;
	struct wipedata *run;

    // Recurse through dir, add up bytes for each user encountered
	// opendir()
	if ((dirp=opendir(fullpath))==NULL)
		return 0;

	// while (readdir())
	while ((dp = readdir(dirp)) != NULL) {
		snprintf(path, sizeof(path), "%s/%s", fullpath, dp->d_name);
		if (lstat(path, &sb))
			continue;
        // If dir, drop back into it and wipe...
		if (S_ISDIR(sb.st_mode) && strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
			file_wipe_recursive(path, wdata, del);

		// Otherwise, find the size, store it in the wipedata array and frag if
		// necessary
		} else if (!S_ISDIR(sb.st_mode)) {

			if (stat(path, &sb))
				continue;

			// Find the user who owns this file
			user = user_get_name_by_uid(sb.st_uid - server_useridstart);
			if (!user) user = "Unknown";
			// Find the group who owns this file
			group = group_findname_bygid(sb.st_gid - server_useridstart);
			if (!group) group = "Unknown";

			// Move through wipedata and find the user
			for (run = *wdata; run; run = run->next) {

				// Find the user ...
				if (run->user[0] && !strcmp(run->user, user)) {

					run->bytes += sb.st_size;
					run->files++;
					break;

				}

			}

			// User wasn't found above:
			if (!run) {

				run = malloc(sizeof(struct wipedata));
				bzero(run, sizeof(struct wipedata));
				strcpy(run->user, user);
				strcpy(run->group, group);
				run->bytes = sb.st_size;
				run->files = 1;
				run->next = *wdata;
				*wdata = run;

			}

			consolef("[FILE_WIPE] wiping %s for %s/%s - %ld bytes\n",
					path, user, group, sb.st_size);

			// Really delete the file if necessary
			if (del) {
				file_goroot();
				remove(path);
				consolef("[FILE_WIPE] Removing file: %s\n", path);
				file_gononroot();
			}

		}

	}

	closedir(dirp);

	// Delete the directory if required
	if (del) {
		file_goroot();
		consolef("[FILE_WIPE] Removing dir: %s\n", fullpath);
		rmdir(fullpath);
		file_gononroot();
	}

	return 1;

}

/*
 * Parse all files in dir, remove bytes * mul from owner, delete..
 */
/*
int file_nuke(struct login_node *l, char *dir, int mul)
{
	DIR *dirr;
	struct dirent *dp;
	struct stat sb;
	struct login_node *t;
	struct quota_node *quota;
	struct quota_node *fquota;
	static char path[1024];

	fquota = (struct quota_node *) l->quota;

	if (!(dirr = opendir(dir))) {
		return 0;
	}

	while ((dp = readdir(dirr)) != NULL) {

		if (!strcmp(dp->d_name, "."))
			continue;

		if (!strcmp(dp->d_name, ".."))
			continue;

		sprintf(path, "%s/%s", dir, dp->d_name);

		if (stat(path, &sb)) continue;

		// whatever it is, we want to try to delete it...

		// right, is it owned by a lftpd user?
		if (sb.st_uid >= server_useridstart) {

			// would this file have awarded bytes?
			if (!check_isfree(dp->d_name)) {

				// Can we find user?
				if ((t = user_findbyuid(sb.st_uid))) {

					quota_new(t);

					// can we find user's quota ?
					if ((quota = quota_getquota(t))) {

						consolef("Removing %d bytes\n", (int)sb.st_size * mul);

						// Do they have bytes to be removed ?
						if (sb.st_size * mul > quota->bytes_up)
							quota->bytes_up = 0;
						else {
							if (quota->bytes_up > (lion64u_t)sb.st_size * mul)
								quota->bytes_up -= (lion64u_t)sb.st_size * mul;
							else
								quota->bytes_up = 0;
						}

						// Update nukee's nuked-bytes as well
						quota->got_nuked_bytes += (lion64u_t) sb.st_size;
						quota->got_nuked_files++;

						// And the Nuker's stats
						if (fquota) {
							fquota->nuked_bytes += (lion64u_t) sb.st_size;
							fquota->nuked_files++;
							fquota->dirty = 1;
						}


						quota->dirty = 1;

					} // getquota

					quota_exit(t);

				} // find_user

			} // isfree

		} // userid

		// quota has been dealt with, delete us:

#if 1
		//    consolef("Deleting %s\n", path);
		file_goroot();
		if (unlink(path))
			perror("unlink");
		file_gononroot();
#endif

	} // while

	closedir(dirr);
	return 1;
}
*/

#ifdef GID_COUNT_HACK_DELETE_ME
void file_bump_gid(struct data_node *d)
{
	struct stat sb;
	gid_t gid;

	//	consolef("bumping\n");

	if (stat(d->name, &sb)) return;

	/* If it's an directory, skip it :) */
	if (!(sb.st_mode & S_IFREG)) return;

	if (sb.st_gid < GID_COUNT_OFFSET)
		gid = GID_COUNT_OFFSET + 1;
	else
		gid = sb.st_gid + 1;

	if ((gid - GID_COUNT_OFFSET) > 999)
		gid = GID_COUNT_OFFSET + 999;

	file_goroot();
	chown(d->name, -1, gid);
	file_gononroot();

	//consolef("Bumping %s to %d\n", d->name, gid);
}
#endif


void file_size(struct login_node *t, char *file)
{
	struct stat st;

	if (!file_pathchecklogin(t, file)) {
		socket_number(t, 550, path2file(file));
		return ;
	}

	if (stat(file, &st)) { /* Stat the file */
		socket_number(t, 550, path2file(file));
		return;
	}

	if (!(st.st_mode & S_IFREG)) {
		socket_print(t, "550 Not a plain file.\r\n");
		return;
	}


	// we cast it to 64 here for those OS with stat still in 32 bit.
	socket_print(t, "213 %"PRIu64"\r\n", (lion64_t) st.st_size);

}

int file_hasmessages(struct login_node *t)
{
	struct stat st;

	if (!server_msgpath)
		return 0;

	sprintf(genbuf, "%s/%s", server_msgpath, t->user);

	file_goroot();

	if (!stat(genbuf, &st)) {
		file_gononroot();

		socket_number(t, 1230, t->user);

		return 1;
	}

	file_gononroot();

	return 0;
}


void file_readwall(void)
{
	/* Possible security hole here, for setuid versions */
	FILE *fd;
	int line = 0;
	char *ar, *user;

	file_goroot();
	fd = fopen(server_wallpath, "r");
	file_gononroot();

	if (!fd)
		return;

	while(fgets(genbuf, 256, fd)) {

		ar = genbuf;

		if (!(user = digtoken((char **)&ar, " ")))
			continue;

		if ((!*user) || !*ar) continue;

		chop(ar);

		strncpy((char *)global_mwu[line], user, MAXUSERNAME);
		global_mwu[line][MAXUSERNAME+1] = 0;

		global_mwm[line] = (unsigned char *)mystrcpy(ar);

		line++;

		if (line == NUM_WALL) break;

	}

	fclose(fd);

#ifdef IRCBOT

	/* Insert code to send WALL to bot. See for-loop above! */
	irc_wall();

#endif

}



void file_writewall(void)
{
	/* Possible security hole here, for setuid versions */
	FILE *fd;
	int i = 0;

	file_goroot();
	fd = fopen(server_wallpath, "w");
	file_gononroot();

	if (!fd) {
		perror("");
		consolef("Failed to create message wall file %s\r\n", server_wallpath);
		return;
	}

	for (i = 0; i < NUM_WALL; i++) {
		if (global_mwm[i])
			fprintf(fd, "%s %s\n", global_mwu[i], global_mwm[i]);
	}

	fclose(fd);

#ifdef IRCBOT

	/* Insert code to send WALL to bot. See for-loop above! */
	irc_wall();

#endif

}




/* This is virtually a copy of isdir(). */
void file_free(struct login_node *t, char *s, int num)
{
#ifndef WIN32
	char path[1024];
	struct stat pstat;
#if HAVE_SYS_STATVFS_H
	struct statvfs pstatfs;
#else
	struct statfs pstatfs;
#endif

	/* This function is strange? */

	if (strncmp(s, server_usechroot, strlen(server_usechroot)))
		sprintf(path, "%s/%s/.", server_usechroot, s);
	else
		strcpy(path, s);

	//consolef("file_free() Checking for dir '%s'\n", s);

	if (access(path, F_OK) || stat(path, &pstat)) {
		return ;
	}

	if (!pstat.st_mode & S_IFDIR) {
		return ;
	}

	/* It's a dir alright! */

	if (t->cwd_devid != pstat.st_dev) {  /* New device! Print free disk */

		//consolef("New device, fetching diskfree stats\n");

#ifdef IRIX

		if (statfs(path, &pstatfs, sizeof(pstatfs), 0)) return;

#elif defined ( DEC )

		if (statfs(path, &pstatfs, sizeof(pstatfs))) return;

#elif HAVE_SYS_STATVFS_H

		if (statvfs(path, &pstatfs)) return;

#else

		if (statfs(path, &pstatfs)) return;

#endif

		t->cwd_devid = pstat.st_dev;

#ifdef DEC
		socket_print(t, "%3d-Available space: %.2f MB.\r\n", num,
					 (float)pstatfs.f_bavail / 1024.0);
#else
		socket_print(t, "%3d-Available space: %.2f MB.\r\n", num,
					 (float)pstatfs.f_bfree * (float)pstatfs.f_bsize / 1048576.0);
#endif

	}
#endif // WIN32
}
