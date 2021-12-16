#if HAVE_CONFIG_H
#include <config.h>
#endif


#ifndef WIN32
#include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <time.h>

#if defined (POSIX) || defined (__FreeBSD__) || defined (__linux__)
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include <string.h>

#include "lion.h"

#include "log.h"

#include "global.h"
#include "login.h"
#include "data.h"
#include "checkchild.h"
#include "check.h"
#include "fnmatch.h"
#include "sfv.h"

// We have a name space clash. Just misc would open lftpd misc, which 
// is no longer really in used, but we've not completely moved over so
// until that day, specify the path.
#include "../lion/src/misc.h"


//#define STAND_ALONE

#ifdef STAND_ALONE
#define consolef printf
#define checkchild_handler lion_userinput
struct check_node *check_node_head = NULL;
#define SFV_GetFileCRC(X) (unsigned long) (X)
#endif


//#define LOG_TO_FILE "checkchild.log"

static THREAD_SAFE int checkchild_quit = 0;
static THREAD_SAFE lion_t *parent_handle = NULL;


// Do we only fork one tester at a time, or no limit...
// No longer used!
//#define CHECK_ONLYONE


struct checkchild_struct {
	char *name;
	unsigned int id;
	int type;     // SFV_TYPE_SFV, SFV_TYPE_MD5, ...
};

typedef struct checkchild_struct checkchild_t;


// We need to build requests up in a queue like thingy here.
// We will have a circular list, the size of list is in
// _allocated. Current index start is _start, and of course _end.
// We re-allocate bigger array if needed.
static THREAD_SAFE checkchild_t *checkchild_circular_list = NULL;
static THREAD_SAFE int checkchild_allocated = 0;
static THREAD_SAFE int checkchild_start = 0;
static THREAD_SAFE int checkchild_end = 0;
#define CHECKCHILD_CHUNKSIZE 100

// This one is assigned the current node testing.
static THREAD_SAFE checkchild_t *checkchild_current = NULL;



checkchild_t *checkchild_addnode(unsigned int id, char *file, int type)
{
	checkchild_t *newd;

	// Do we need to allocate more space ?
	if (!checkchild_circular_list ||
		(checkchild_end + 1 == checkchild_start) ||
		((!checkchild_start) && (checkchild_end + 1 == checkchild_allocated))){
		
		// Yep

		printf("[checkchild] allocating space %d nodes\n",
			   checkchild_allocated);

		newd = (checkchild_t *) realloc(checkchild_circular_list,
										(checkchild_allocated + 
										 CHECKCHILD_CHUNKSIZE) * 
										sizeof(checkchild_t));

		if (!newd) return NULL;

		checkchild_circular_list = newd;


		// We also need to clean things up here...
		// if Start is > End (ie, the list wraps) we need to copy
		// the end chunk to the new end, and update Start.
		if (checkchild_start > checkchild_end) {

			printf("[checkchild] relocating..\n");

			memcpy( &checkchild_circular_list[ checkchild_start + 
											   CHECKCHILD_CHUNKSIZE ],
					&checkchild_circular_list[ checkchild_start ],
					sizeof(checkchild_t) * 
					(checkchild_allocated - checkchild_start));

			checkchild_start += CHECKCHILD_CHUNKSIZE;
			
			if (checkchild_current)
				checkchild_current = &checkchild_circular_list[ checkchild_start ];

		}
		
		// Update allocated size.
		checkchild_allocated += CHECKCHILD_CHUNKSIZE;


	}

	// Insert us at _end

	newd = &checkchild_circular_list[ checkchild_end ];

	newd->name = strdup(file);
	newd->id = id;
	newd->type = type;

	printf("[chechchild] adding %d -> %u : %u : %s\n",
		   checkchild_end,
		   id,
		   type,
		   file);


	checkchild_end++;
	if (checkchild_end >= checkchild_allocated)
		checkchild_end = 0;


	return newd;


}



// Return the node start points to (unless we are empty).
void checkchild_assign_current(void)
{
	checkchild_t *result;

	// Empty?
	if (checkchild_start == checkchild_end) {

		checkchild_current = NULL;
		return; 
	}

	result = &checkchild_circular_list[checkchild_start];

	printf("[checkchild] popped %d -> %u : %u : %s\n",
		   checkchild_start,
		   result->id,
		   result->type,
		   result->name);


	checkchild_current = result;

	checkchild_processnode();

}


// Release the node start points to (as we are done with it)
void checkchild_releasenode(void)
{
	checkchild_t *node;

	node = &checkchild_circular_list[checkchild_start];

	if (checkchild_current &&
		(node != checkchild_current))
		printf("[checkchild] internal error (node != current)!?\n");


	if (checkchild_start == checkchild_end) {
		printf("[checkchild] error, list is empty when we've called releasenode\n");
	}


	printf("[checkchild] Releasing %d -> %u : %u : %s\n",
		   checkchild_start,
		   node->id,
		   node->type,
		   node->name);

	SAFE_FREE(node->name);
	node->id = 0;



	checkchild_start++;

	if (checkchild_start >= checkchild_allocated)
		checkchild_start = 0;


	checkchild_current = NULL;

	printf("[checkchild] current NULL\n");

	// Attempt to re-assign it if we can...
	// AH! This calls assign, which calls process, which calls int/ext process
	// that does the task, that calls release, which calls assign, which...
	// So, lets make it NULL here, and sleep for one second. 
	// io_force_loop = 1; // No sleep.
	//	checkchild_assign_current();

}




void checkchild_process( char *line )
{
	char *token, *ar, *file;
	unsigned int id;
	int type;


	// file should be:
	// "id:type:/path/to/file name.txt"
	ar = line;

	token = misc_digtoken(&ar, ":\r\n");

	if (!token) {
		printf("[checkchild] parse error '%s'\n", line);
		return;
	}

	id = atoi( token );


	token = misc_digtoken(&ar, ":\r\n");

	if (!token) {
		printf("[checkchild] parse error '%s'\n", line);
		return;
	}

	type = atoi( token );


	file = ar;

	printf("[checkchild] parsed '%u:%u:%s'\n", id, type, file);




	// Add this to testing queue.
	if (!checkchild_addnode(id, file, type)) {

		// failed with unknown
		lion_printf(parent_handle, "%u!12/Cannot allocate memory\n", id);
		return;

	}


}





void checkchild_processnode( void )
{
	struct check_node *c;

	if (!checkchild_current)
		return;

	printf("[checkchild] current set to %u : %s\n",
		   checkchild_current->id,
		   checkchild_current->name);



	// Check if it is an internal test, or, external.
	// Since we've forked we should only ever read this list, it could
	// change from under our feet. But, we never re-arrange the check list
	// in lftpd. (And we'd better not)
	for (c = check_node_head; c; c=c->next) {
			   
		if (!fnmatch(c->ext, checkchild_current->name, FNM_CASEFOLD)) {
			
			if ((c->exe == (char *)CHECK_INTERNAL)) {

				checkchild_test_int( checkchild_current->id, 
									 checkchild_current->name,
									 checkchild_current->type);
				break;

			} else { // Do external...

				checkchild_test_ext( checkchild_current->id, 
									 checkchild_current->name,
									 c->exe);
				break;

			} // external

		} // casefold

	} // for

}





//
// handler for the main parent pipe.
//
int checkchild_handler( lion_t *handle,
						void *user_data, int status, int size, char *line)
{

	switch( status ) {

	case LION_PIPE_RUNNING:  // We get this, tells us parent is alive
		break;

	case LION_PIPE_EXIT:     // parent died, or quit. So shall we.
	case LION_PIPE_FAILED:
		checkchild_quit = 1;
		break;

	case LION_INPUT:
		// New request from parent.
		printf("[checkchild] request '%s'\n", line);

		checkchild_process( line );
		break;

	}

	return 0;

}



#if 0
			/* Build DIZDIR and DIZFILE for testing as well. */
			// Path to execute.
			snprintf(buf, sizeof(buf), "%s \"%s\"", c->exe, d->name);

			
#ifndef NOPUTENV
			setenv("DIZ_DIR", check_node->dir_name, 1);
			setenv("DIZ_FILE", check_node->file_name, 1);
			setenv("DIZ_USER", d->login->user, 1);
			setenv("DIZ_GROUP", d->login->current_group ?
				   ((struct group *)d->login->current_group)->name :
				   "Independent", 1);
			setenv("DIZ_SITE", 
				   server_greeting ? server_greeting : localhost_a, 1);
#else
			{
				static char user[80];
				static char group[80];
				static char site[80];
				
				snprintf(genbuf, sizeof(genbuf), "DIZ_DIR=%s", 
						 check_node->dir_name);
				putenv(genbuf);
				snprintf(path, sizeof(path), "DIZ_FILE=%s", 
						 check_node->file_name);
				putenv(path);
				snprintf(user, sizeof(user), "DIZ_USER=%s", d->login->user);
				putenv(user);
				snprintf(group, sizeof(group), "DIZ_GROUP=%s", 
						 d->login->current_group ?
						 ((struct group *)d->login->current_group)->name :
						 "Independent");
				putenv(group);
				snprintf(site, sizeof(site), "DIZ_SITE=%s", 
						 server_greeting ? server_greeting :
						 localhost_a);
				putenv(site);
				
			}	  
#endif
			
			
			// Call lion!
			// basically this then sorts things out and eventually get a return
			// code (good, bad, unknown/timeout) then it calls check_return 
			// that compares the results, which in turn calls _action that
			// acts on the return status.
			consolef("executing '%s' for %p\n", buf, check_node);

			lion_set_handler(
							 lion_system( buf, 1, LION_FLAG_FULFILL, 
										  check_node),
							 check_handler);
			// Unattractive that is! But fulfill guarantees us a node back
			// so it is perfectly valid to call set_handler on it.
#endif






int checkchild_init( lion_t *parent, void *user_data, void *arg )
{

#ifdef LOG_TO_FILE
	FILE *i;

	i = fopen(LOG_TO_FILE, "a");

	if (i) {

		dup2(fileno(i), 0);
		dup2(fileno(i), 1);
		dup2(fileno(i), 2);

		setvbuf(stdin,  (char *)NULL, _IOLBF, 0);
		setvbuf(stdout, (char *)NULL, _IOLBF, 0);
		setvbuf(stderr, (char *)NULL, _IOLBF, 0);

		fclose(i);
	}

#endif

	consolef("[checkchild] running...\n");

	parent_handle = parent;

	// We set our own handler for the child
	lion_set_handler(parent, checkchild_handler);

	// Lower our priority
	// FIXME! conf option
#ifdef POSIX
	consolef("Set priority 19: %d\n", 
			 setpriority(PRIO_PROCESS, 0, 10));
	setpriority(PRIO_PROCESS, 0, 10);
#endif


	

	while(!checkchild_quit) {

		lion_poll(0,1);

		if (!checkchild_current) {

			checkchild_assign_current();

		}

	}

	return 0;

}


//
// *********************************************************************
//
// E X T E R N A L
//
// *********************************************************************
//
// External testing. This is accomplished by spawning a new program that
// does the actual testing. We then take the return value from the program
// and return it to parent.
// We have one option here. Do we enforce that these testers are serial, ie
// only one at a time (like that of internal) or do we spawn however many
// as needed. Only if they have SMP would it make sense to spawn more than
// one. 3 CPUs, and you could run two etc.
//


//
// Handler for all spawned tester programs.
//
int checkchild_ext_handler( lion_t *handle,
							void *user_data, int status, int size, char *line)
{
	unsigned long id = (unsigned long) user_data;
	static time_t start, end;

	switch( status ) {
		
	case LION_PIPE_RUNNING:  // We get this, tells us parent is alive
		printf("[checkchild] %lu tester running. \n", id);
		time(&start);
		break;

	case LION_PIPE_FAILED:
		printf("[checkchild] %lu failed. \n", id);

		// Use "!" to signify failure.
		lion_printf(parent_handle, "%u!%d/%s\n", id, size, line);
#ifdef CHECK_ONLYONE
		// Turn parent reading back on.
		lion_enable_read(parent_handle);
#endif
		checkchild_releasenode();

		break;

	case LION_PIPE_EXIT:
		// make sure we have a received the return code.
		printf("[checkchild] %lu exit\n", id);

		if (size == -1) { 
			lion_want_returncode(handle);
			break;
		}

		printf("[checkchild] %lu exit -> %d. \n", id, size);

		// We have a return code.
		// return code
		// Use ":" to show success
		lion_printf(parent_handle, "%u:%d\n", id, size);

#ifdef CHECK_ONLYONE
		// Turn parent reading back on.
		lion_enable_read(parent_handle);
#endif
		time(&end);
		printf("[checkchild] test %lu, duration %lu.\n",
			   id, end - start);

		checkchild_releasenode();
		break;


	case LION_INPUT:
		// Log text from programs?
		printf("[checkchild] %lu -> '%s'\n", id, line);
		break;

	}

	return 0;

}



void checkchild_test_ext( unsigned int id, char *file, char *program )
{
	char buf[1024];

	// Build the command to execute.
	snprintf(buf, sizeof(buf), "%s \"%s\"", program, file);


	//lion_printf(parent_handle, "%u:not implemented\n", id);

	consolef("executing '%s'\n", buf);

	// We can do this because we have FULFILL. Never returns NULL.
	lion_set_handler(
					 lion_system( buf, 1, LION_FLAG_FULFILL, 
								  (void *)id),
					 checkchild_ext_handler);

#ifdef CHECK_ONLYONE
	// If we deal only with one at a time, pause parent reading.
	// Alas, it is a while(getline) that calls us, so we will get more...
	// but we need lion alive too...
	printf("[checkchild] pausing parent\n");

	lion_disable_read( parent_handle );

#endif

}



void checkchild_test_int( unsigned int id, char *file, int type )
{
	int fd;
	unsigned long crc;
	char *md5;
	time_t before, after;

	fd = open(file, O_RDONLY
#ifdef O_BINARY
			  |O_BINARY
#endif
			  , 0400);

	if (fd < 0) {
		// Return failure
		lion_printf(parent_handle, "%u!%d:%s\n", id,
					errno, strerror(errno));

		checkchild_releasenode();
		return;
	}

	time(&before);
	switch(type) {
	case SFV_TYPE_SFV:
		crc = SFV_GetFileCRC(fd);
		lion_printf(parent_handle, "%u:%08lX\n", id, crc);
		break;
	case SFV_TYPE_MD5:
		md5 = SFV_GetFileMD5(fd);
		lion_printf(parent_handle, "%u:%s\n", id, md5);
		break;
	}
	time(&after);

	close(fd);


	printf("[checkchild] test %u, duration %lu\n", id, after - before);

	checkchild_releasenode();

}























#ifdef STAND_ALONE

unsigned int rond()
{
	static int fd = -2;
	unsigned long poo;

	if (fd == -2) {
		fd = open("/dev/urandom", O_RDONLY);
	}

	if (fd >= 0)
		read(fd, &poo, sizeof(poo));

	return poo;
}


int main(int argc, char **argv)
{
	int i, j;

	printf("[checkchild] exercise\n");

	for (j = 0; j < 1000; j++) {

	for (i = 0; i < 300; i++) {
		
		if ((rond()&512) == 512)
			checkchild_addnode(i, "nomatter");
		
		if ((i > 50) && (i < 166))
			checkchild_addnode(i, "nomatter");

		if ((rond()&128) == 128) 
			checkchild_releasenode();

		if (checkchild_current &&
			(checkchild_current != 
			&checkchild_circular_list[ checkchild_start ]))
			printf("CURRENT IS NOT START\n");

		if (!checkchild_current &&
			(checkchild_start != checkchild_end))
			printf("Current NULL BUT LIST NOT EMPTY\n");
		
	}

	printf("Releasing...\n");

	while(checkchild_start != checkchild_end)
		checkchild_releasenode();

	printf("bigloop\n");

	}
}

#endif



