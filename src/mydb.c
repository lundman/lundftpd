#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/stat.h>

#include "global.h"
#include "login.h"
#include "mydb.h"
#include "data.h"
#include "check.h"
#include "quota.h"
#include "oldlogin.h"
#include "misc.h"
#include "log.h"
#include "groups.h"

int db_check_login(unsigned long size, int fd)
{
  /* Ok, we have an OLD .users. file, check all known old sizes. */
  if (!( size % sizeof(struct login_node_3_3))) {
    
    consolef("Version 3.3 detected...\n");
    
    db_upgrade_login(sizeof(struct login_node_3_3), fd);

  } else if (!( size % sizeof(struct login_node_2_4))) {
    
    consolef("Version 2.4 detected...\n");
    
    db_upgrade_login(sizeof(struct login_node_2_4), fd);
    
  } else if (!( size % sizeof(struct login_node_2_1))) {
    
    consolef("Version 2.1 detected...\n");
    
    db_upgrade_login(sizeof(struct login_node_2_1), fd);
    
  } else if (!( size % sizeof(struct login_node_1_8))) {
    
    consolef("Version 1.8 detected...\n");
    
    db_upgrade_login(sizeof(struct login_node_1_8), fd);
    
  } else if (!( size % sizeof(struct login_node_1_7))) {
    
    consolef("Version 1.7 detected...\n");

    db_upgrade_login(sizeof(struct login_node_1_7), fd);

  } else if (!( size % sizeof(struct login_node_1_4))) {
    
    consolef("Version 1.4 detected...\n");

    db_upgrade_login(sizeof(struct login_node_1_4), fd);

  } else {

    consolef("Unknown version.\n");
    consolef("Email lundman@argonaut.com for assistance.\n");

    exit(0);

  }

  return 0;

}



int db_check_quota(unsigned long size, int fd)
{
  /* Ok, we have an OLD .users. file, check all known old sizes. */

	if (!( size % sizeof(struct quota_node_3_3))) {
		
		consolef("Version 3.3 detected...\n");
		
		db_upgrade_quota(sizeof(struct quota_node_3_3), fd);

	} else if (!( size % sizeof(struct quota_node_2_1))) {
		
    consolef("Version 2.1 detected...\n");

    db_upgrade_quota(sizeof(struct quota_node_2_1), fd);

    } else if (!( size % sizeof(struct quota_node_1_8))) {
    
    consolef("Version 1.8 detected...\n");

    db_upgrade_quota(sizeof(struct quota_node_1_8), fd);

    } else if (!( size % sizeof(struct quota_node_1_7))) {
    
    consolef("Version 1.7 detected...\n");

    db_upgrade_quota(sizeof(struct quota_node_1_7), fd);

  } else if (!( size % sizeof(struct quota_node_1_4))) {
    
    consolef("Version 1.4 detected...\n");

    db_upgrade_quota(sizeof(struct quota_node_1_4), fd);

  } else {

    consolef("Unknown version.\n");
    consolef("Email lundman@lundman.net for assistance.\n");

    exit(0);

  }

  return 0;

}


//
// Checking group file is harder, you can't just go on struct size as there
// are multiple, but going from pre v3.3 we start with a 4 byte header to
// say the version.
//
int db_check_group(unsigned long size, int fd)
{
	unsigned char buf[4];
	unsigned long header;

	// Read 4 bytes. and check
	if (read(fd, buf, 4) != 4) {
		close(fd);
		exit(-1);
	}


	header =  ((unsigned long)buf[0])<<24;
	header += ((unsigned long)buf[1])<<16;
	header += ((unsigned long)buf[2])<<8;
	header += ((unsigned long)buf[3]);

	//	consolef("version %lx %lx %lx\n", 
	//	   ('G'<<24) | ('R'<<16) | ('0'<<8)|'1',
	//	   0x47513031,
	//		 header);

	switch (header) {

	case GROUPS_VERSION_VALUE:
		return 0; // perfect.

	case ('G'<<24) | ('R'<<16) | ('0'<<8)|'2':
	case ('G'<<24) | ('R'<<16) | ('0'<<8)|'3':
		consolef("[db] group DB version is FUTURE version -- STOP\n");
		exit(-1);
		break;


	default: // No know signature, assume old format

		lseek(fd, 0L, SEEK_SET);

		db_upgrade_group(sizeof(struct group_3_3), 
						 sizeof(struct group_member_3_3),
								fd);
		break;
		
		
	}

	return 0;

}





/* This function assumes new fields are added TO THE END! */
void db_upgrade_login(unsigned long old_size, int fd)
{
  int new_size = sizeof(struct login_node);
  struct login_node tmp;
  char tmpnam[18] = "/tmp/.ftpd.XXXXXX";
  int newfile, count = 0;
  unsigned long offset = 0;
  
  /* Ask the user! */
  consolef("Make sure you have made a backup of your userdb file before proceeding!!\nThis includes userdb, quotadb and groupdb files.\n\n");
  consolef("Kill any background lundftpd processes before proceeding.\n");

  consolef("Upgrade userdb? (Yes/No): ");
  fflush(stdout);

  if (!get_word(stdin, genbuf)) exit(1);

  if (mystrccmp(genbuf, "yes")) {
    consolef("Exiting on user request. (Didn't type 'yes')\n");
    exit(0);
  }

  consolef("\n");

  memset(&tmp, 0, sizeof(tmp));

  newfile = mkstemp(tmpnam);

  if (newfile < 0) {
    perror("Couldn't open tmp file");
    return;
  }

  consolef("Opening tmp file %s\n", tmpnam);

  while(read(fd, (char *)&tmp, old_size) == old_size) {

    count++;
    tmp.offset = offset;

    if (write(newfile, (char *)&tmp, new_size) != new_size)
      perror("Warning, unsuccessful write?!");

    offset += new_size;
  }

  consolef("Rewinding both files.\n");

  if (lseek(fd, 0L, SEEK_SET) ||
      lseek(newfile, 0L, SEEK_SET)) {
    consolef("Rewind failed?!? Aborting..\n");
    exit(1);
  }

  while(read(newfile, &tmp, new_size) == new_size) {

    if (write(fd, &tmp, new_size) != new_size)
      perror("Warning, unsuccessful write?!");

  }

  close(newfile);
  unlink(tmpnam);

  consolef("Done, converted %d users.\n\n", count);

}




/* This function assumes new fields are added TO THE END! */
void db_upgrade_quota(unsigned long old_size, int fd)
{
  int new_size = sizeof(struct quota_node);
  struct quota_node newtmp;
  struct quota_node oldtmp;  /* Enough space to fit the old */
  struct quota_node_1_4 *node14 = (struct quota_node_1_4 *)&oldtmp;
  struct quota_node_1_7 *node17 = (struct quota_node_1_7 *)&oldtmp;
  struct quota_node_1_8 *node18 = (struct quota_node_1_8 *)&oldtmp;
  struct quota_node_2_1 *node21 = (struct quota_node_2_1 *)&oldtmp;
  struct quota_node_3_3 *node33 = (struct quota_node_3_3 *)&oldtmp;
  char tmpnam[18] = "/tmp/.ftpd.XXXXXX";
  int newfile, count = 0;
  unsigned long offset = 0;

  /* rewind quota file */
  lseek(fd, 0L, SEEK_SET);

  memset(&newtmp, 0, sizeof(newtmp));
  memset(&oldtmp, 0, sizeof(oldtmp));

  newfile = mkstemp(tmpnam);

  if (newfile < 0) {
    perror("Couldn't open tmp (quota file still old) file");
    return;
  }

  consolef("Opening tmp file %s: new node %d, old %d\n", tmpnam,
	   new_size, old_size);


  while(read(fd, (char *)&oldtmp, old_size) == old_size) {

    count++;

    /* MAKE SURE OFFSET IS CORRECT THO */

    switch (old_size) {
      
    case sizeof(struct quota_node_3_3):
		memcpy(&newtmp, &oldtmp, old_size);
		break;

    case sizeof(struct quota_node_2_1):
      memcpy(&newtmp, &oldtmp, old_size);

#if 1
      /* Calculate credits variable */
      if (oldtmp.ratio > 0) 
	newtmp.credits = (oldtmp.bytes_up * oldtmp.ratio) - oldtmp.bytes_down;
      else
	newtmp.credits = 0;

#endif
      break;

    case sizeof(struct quota_node_1_8):
      memcpy(&newtmp, &oldtmp, old_size);
      break;

    case sizeof(struct quota_node_1_4):
#if 0
      newtmp.daily =      node14->daily;
      newtmp.bytes_down = node14->bytes_down;
      newtmp.bytes_up =   node14->bytes_up;
      newtmp.ratio =      node14->ratio;
      newtmp.time_limit = node14->time_limit;
#endif
      break;

    case sizeof(struct quota_node_1_7):
      newtmp.daily =      node17->daily;
      newtmp.bytes_down = node17->bytes_down;
      newtmp.bytes_up =   node17->bytes_up;
      newtmp.ratio =      node17->ratio;
      newtmp.time_limit = node17->time_limit;
      break;
    }


    /* DOH! Set offset AFTER we copy old structure over!!! */

    newtmp.offset = offset;

    if (write(newfile, (char *)&newtmp, new_size) != new_size)
      perror("Warning, unsuccessful write?!");

    offset += new_size;
  }

  consolef("Rewinding both files.\n");

  if (lseek(fd, 0L, SEEK_SET) ||
      lseek(newfile, 0L, SEEK_SET)) {
    consolef("Rewind failed?!? Aborting..\n");
    exit(1);
  }


  while(read(newfile, &newtmp, new_size) == new_size) {
    
    if (write(fd, &newtmp, new_size) != new_size)
      perror("Warning, unsuccessful write?!");

  }

  close(newfile);
  unlink(tmpnam);

  consolef("Done, converted %d quota nodes.\n\n", count);

  lseek(fd, 0L, SEEK_SET);


}






void db_upgrade_group(unsigned long old_group_size,
					  unsigned long old_member_size,
					  int fd)
{
	struct group newgroup;
	struct group oldgroup; // we assume it always only grows
	struct group_member newmember, oldmember;
	struct group_3_3 *oldgroup_3_3 = (struct group_3_3 *) &oldgroup;
	struct group_member_3_3 *oldmember_3_3 = (struct group_member_3_3 *) &oldmember;
	int groups = 0, members = 0, i;
	char tmpnam[18] = "/tmp/.ftpd.XXXXXX";
	int newfile;
	int new_group_size = sizeof(newgroup);
	int new_member_size = sizeof(newmember);
	int count;


	// We only know one old group size.. we need to fix this code
	// in future.
	
	memset(&newgroup, 0, sizeof(newgroup));
	memset(&oldgroup, 0, sizeof(oldgroup));
	
	newfile = mkstemp(tmpnam);
	
	if (newfile < 0) {
		perror("Couldn't open tmp (group file still old) file");
		return;
	}
	
	consolef("Opening tmp file %s: new node %d/%d, old %d/%d\n", tmpnam,
			 new_group_size, new_member_size,
			 old_group_size, old_member_size);
	


	// read old in...
	while(read(fd, (char *)&oldgroup, old_group_size) == old_group_size) {


		groups++;

		count = 0;

		// copy structs over based on versions...
		// How many members to read?
		switch (old_group_size) {

		case sizeof(*oldgroup_3_3): 
			count = oldgroup_3_3->usedslots;

			// copy the username over (two extra chars)
			memcpy(&newgroup.name, &oldgroup_3_3->name, 
				   sizeof(oldgroup_3_3->name));

			// copy everything after all name.
			memcpy(&newgroup.gid, &oldgroup_3_3->gid, 
				   old_group_size - sizeof(oldgroup_3_3->name));

			break;

		default:
			consolef("[db] unknown group size for upgrade -- stop\n");
			exit(1);
		}



		// write new out.
		
		if (write(newfile, (char *)&newgroup, new_group_size) != new_group_size)
			perror("Warning, unsuccessful write?!");
		




		// Read in count members.
		for (i = 0; i < count; i++) {
	
			if (read(fd, (char *)&oldmember, old_member_size) == old_member_size) {
				
				// alas, the name part is now 2 bytes longer.

				// copy structs over based on versions...
				// How many members to read?
				switch (old_member_size) {
					
				case sizeof(*oldmember_3_3): 
					
					// copy the username over (two extra chars)
					memcpy(&newmember.name, &oldmember_3_3->name, 
						   sizeof(oldmember_3_3->name));

					// copy the rest
					memcpy(&newgroup.flags, &oldmember_3_3->flags, 
						   old_member_size - sizeof(oldgroup_3_3->name));

					break;

				default:
					consolef("[db] warning - unhandled size\n");
					exit(1);

				}

				if (write(newfile, (char *)&newmember, new_member_size) != new_member_size)
					perror("Warning, unsuccessful write?!");
				
				members++;
				
			}

		} // for

	} // while read group

	consolef("[db] upgraded %d groups, %d members\n", groups, members);


	consolef("Rewinding both files.\n");
	
	if (lseek(fd, 0L, SEEK_SET) ||
		lseek(newfile, 0L, SEEK_SET)) {
		consolef("Rewind failed?!? Aborting..\n");
		exit(1);
	}

	
	// Write version header.
	
	if (write(fd, GROUPS_VERSION_STR, 4) != 4)
		perror("Warning, unsuccessful write?!");


	
	// read any size ok.
	while((i = read(newfile, &newgroup, new_group_size)) > 0) {
    
		if (write(fd, &newgroup, i) != i)
			perror("Warning, unsuccessful write?!");
		
	}
	
	close(newfile);
	unlink(tmpnam);
	
	consolef("Done\n\n", count);
	
	lseek(fd, 0L, SEEK_SET);

	
	consolef("Exiting... \n");
	exit(0);
}



