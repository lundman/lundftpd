#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "global.h"
#include "misc.h"
#include "login.h"
#include "log.h"
#include "lfnmatch.h"
#include "socket.h"
#include "section.h"
#include "../lion/src/misc.h"

#include "data.h"
#include "file.h"
#include "groups.h"

struct section_node *section_node_head = NULL;

// hide_path in disguise

char *section_rootfind(char *fullpath)
{

	char *r;

	if (server_usechroot && fullpath
		&& !strncmp(fullpath, server_usechroot, strlen(server_usechroot)))
	{

    	r = &fullpath[strlen(server_usechroot)];

		// Actually, we NEED the leading "/" in say "/incoming"
		if ((r != fullpath) &&  // check that we can go -1 if needed.
			(*r != '/')) {      // and that it already dont start with /

			r--; // go back a char, this had better be a /
			if (*r == '/')
				return r;

			r++;
		}

		// this code is left as it was before.

		while (*r == '/' && r[1] == '/')
			r++;

		//consolef("       ret  '%s'\n", r);
		return r;

	}

	//consolef("          fullpath '%s'\n", fullpath);
	return fullpath;

}


// Removes server_usechroot AND the section path
char *section_rlsfind(char *fullpath)
{

	char *path, *p;

	// Just return the section path and the rls name paths
	path = section_rootfind(fullpath);

	// Strip the section path from it
	if (*path == '/')
		path++;

	for (p = path; *p; p++) {

		if (*p == '/')
			return p + 1;

	}

	return path;

}

struct section_node *section_add(char *path)
{

	struct section_node *sn;
	int len;

	if (!(sn = malloc(sizeof(struct section_node)))) {
		perror("malloc");
		exit(-1);
	}

	bzero(sn, sizeof(struct section_node));

	// Remove any trailing / or /* from the path.

	// Check for trailing "/*" and strip it:
	len = strlen(path);

	if (len > 1) {
		if (path[len - 1] == '*') path[len - 1] = 0;
		len = strlen(path);
		if (path[len - 1] == '/') path[len - 1] = 0;
	}

	sn->path = strdup(path);
	sn->patts = NULL;
	sn->smartpath = NULL;
	sn->next = NULL;

	// Special satanic setting of defaults so that section_apply_defaults()
	// can work OK later.
    sn->dupecheck =
    sn->announce  = 
    sn->filecheck =
    sn->deletebad =
    sn->racestats =
    sn->ratio     =
    sn->convertmkdspace  =
    sn->convertstorspace =
    sn->extractmp3genre  =
    sn->extractimdb      =
    sn->uppercasedir     =
	sn->stealth          =
	sn->request          =
	sn->incompletes       =
    sn->lowercasefile    = -666;

	// No need to set to 0/NULL since it is bzero()ed above.

	sn->next = section_node_head;
	section_node_head = sn;

	return sn;

}

// Once all the sections have been read, find the default section
// and apply settings from that to any section with values of -666.
// Also add hidefile/denyfile from default to section.
void section_apply_defaults(void)
{

	struct section_node *defsect, *runner;
	struct misc_patt *pattrun;


	// Clear up all the -666 values for all sections
	// into some hardwired defaults.
	if (!(defsect = section_find("/"))) {

		for (runner = section_node_head; runner; runner = runner->next) {

			// Don't do anything by default:
			if (runner->dupecheck == -666) runner->dupecheck = 0;
			if (runner->announce == -666) runner->announce = 0;
			if (runner->filecheck == -666) runner->filecheck = 0;
			if (runner->deletebad == -666) runner->deletebad = 0;
			if (runner->racestats == -666) runner->racestats = 0;
			if (runner->convertmkdspace == -666) runner->convertmkdspace = 0;
			if (runner->convertstorspace == -666) runner->convertstorspace = 0;
			if (runner->extractmp3genre == -666) runner->extractmp3genre = 0;
			if (runner->extractimdb == -666) runner->extractimdb = 0;
			if (runner->uppercasedir == -666) runner->uppercasedir = 0;
			if (runner->lowercasefile == -666) runner->lowercasefile = 0;
			if (runner->stealth == -666) runner->stealth = 0;
			if (runner->request == -666) runner->request = 0;
			if (runner->incompletes == -666) runner->incompletes = 0;
			// Default ratio is 3
			if (runner->ratio == -666) runner->ratio = 3;

		}

		return;

	}

	// Clean up the default node
	// Don't do anything by default:
	if (defsect->dupecheck == -666) defsect->dupecheck = 0;
	if (defsect->announce == -666) defsect->announce = 0;
	if (defsect->filecheck == -666) defsect->filecheck = 0;
	if (defsect->deletebad == -666) defsect->deletebad = 0;
	if (defsect->racestats == -666) defsect->racestats = 0;
	if (defsect->convertmkdspace == -666) defsect->convertmkdspace = 0;
	if (defsect->convertstorspace == -666) defsect->convertstorspace = 0;
	if (defsect->extractmp3genre == -666) defsect->extractmp3genre = 0;
	if (defsect->extractimdb == -666) defsect->extractimdb = 0;
	if (defsect->uppercasedir == -666) defsect->uppercasedir = 0;
	if (defsect->lowercasefile == -666) defsect->lowercasefile = 0;
	if (defsect->stealth == -666) defsect->stealth = 0;
	if (defsect->request == -666) defsect->request = 0;
	if (defsect->incompletes == -666) defsect->incompletes = 0;
	// Default ratio is 3
	if (defsect->ratio == -666) defsect->ratio = 3;

	// Apply the default sections settings to those that need it.
	for (runner = section_node_head; runner; runner = runner->next) {

		// Skip ourselves
		if (runner == defsect)
			continue;

		if (runner->dupecheck == -666) runner->dupecheck = defsect->dupecheck;
		if (runner->announce == -666) runner->announce = defsect->announce;
		if (runner->filecheck == -666) runner->filecheck = defsect->filecheck;
		if (runner->deletebad == -666) runner->deletebad = defsect->deletebad;
		if (runner->racestats == -666) runner->racestats = defsect->racestats;
		if (runner->convertmkdspace == -666) runner->convertmkdspace = defsect->convertmkdspace;
		if (runner->convertstorspace == -666) runner->convertstorspace = defsect->convertstorspace;
		if (runner->extractmp3genre == -666) runner->extractmp3genre = defsect->extractmp3genre;
		if (runner->extractimdb == -666) runner->extractimdb = defsect->extractimdb;
		if (runner->uppercasedir == -666) runner->uppercasedir = defsect->uppercasedir;
		if (runner->lowercasefile == -666) runner->lowercasefile = defsect->lowercasefile;
		if (runner->stealth == -666) runner->stealth = defsect->stealth;
		if (runner->request == -666) runner->request = defsect->request;
		if (runner->incompletes == -666) runner->incompletes = defsect->incompletes;
		if (runner->ratio == -666) runner->ratio = defsect->ratio; 

		// Assign hidefile/denyfile patterns to each node
		for (pattrun = defsect->patts; pattrun; pattrun = pattrun->next)
			misc_patt_add(&runner->patts, pattrun->type, pattrun->patt);

	}

}

// Find the best section to match the given path, return the node.
struct section_node *section_find(char *path)
{

	struct section_node *run;
	char *matchpath;
	struct section_node *bestmatch = NULL;

	matchpath = section_rootfind(path);

	for (run = section_node_head; run; run = run->next) {

		if (!lfnmatch(run->path, matchpath, LFNM_LEADING_DIR|LFNM_CASEFOLD)) {
			if (!bestmatch || (strlen(bestmatch->path) < strlen(run->path)))
				bestmatch = run;
		}

		// Best match so far is the default section
		if (!bestmatch && !(strcmp(run->path, "/")))
			bestmatch = run;

	}

	if (bestmatch)
		return bestmatch;

	return NULL;

}

char *section_find_smartpath(char *smartpath)
{

	struct section_node *sn;

	if (smartpath[0] == '/')
		smartpath++;

	for (sn = section_node_head; sn; sn = sn->next) {
		if (sn->smartpath && !lfnmatch(sn->smartpath, smartpath,
				LFNM_CASEFOLD | LFNM_LEADING_DIR))
		{
			return sn->path;
		}
	}
	return NULL;

}


struct section_node *section_find_byname(char *name)
{
	struct section_node *run;

	for (run = section_node_head; run; run = run->next) {

		if (!mystrccmp(run->name, name))
			return run;
	}
	return NULL;
}

char *section_name(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return NULL;
	return sn->name;
}

int section_uppercasedir(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->uppercasedir;
}

int section_lowercasefile(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->lowercasefile;
}

int section_dupecheck(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->dupecheck;
}

int section_announce(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->announce;
}

int section_filecheck(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->filecheck;
}

int section_deletebad(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->deletebad;
}

int section_racestats(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->racestats;
}

int section_ratio(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->ratio;
}

int section_convertmkdspace(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->convertmkdspace;
}


int section_convertstorspace(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->convertstorspace;
}

int section_extractmp3genre(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->extractmp3genre;
}

int section_extractimdb(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->extractimdb;
}

int section_hidefile(char *path, char *file)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return misc_patt_match(sn->patts, PATT_HIDE, file);

}
int section_stealth(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->stealth;

}
int section_remote(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return (int)sn->remote_user;

}

int section_incompletes(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return (int)sn->incompletes;
}


char *section_request(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	if (sn->request) return path;

	// Attempt to locate ANY section with request on.
	for (sn = section_node_head; sn; sn = sn->next) 
		if (sn->request) return sn->path;

	return NULL;
}



int section_grouped(char *path)
{
	struct section_node *sn = section_find(path);
	if (!sn)
		return 0;
	return sn->num_groups;
}


// Check if section is a grouped section, and then if we are beloning to
// any groups in that section. If multiple groups it just returns first found.
char *section_group_ismember(char *path, char *user)
{
	int i;

	struct section_node *sn = section_find(path);
	if (!sn)
		return NULL;
	if (!sn->num_groups)
		return NULL;

	for (i = 0; i < sn->num_groups; i++) {
		if (group_hasuser_byname(sn->groups[i], user))
			return sn->groups[i];
	}

	return NULL;
}


int section_denyfile(char *file)
{
	struct section_node *sn = section_find(file);
	if (!sn)
		return 0;
	return misc_patt_match(sn->patts, PATT_DENY, file);

}

int section_denydir(char *dir)
{
	struct section_node *sn = section_find(dir);
	if (!sn)
		return 0;
	return misc_patt_match(sn->patts, PATT_DENYD, dir);

}

void section_site_section(struct login_node *t, char *args)
{

	// Grab the current section
	// Show configuration for section

	struct section_node *sn;
	char ratio[5], hidden[50] = "\0", denied[40] = "\0", *tmp;
	char groups[48], deniedd[40] = "\0";;
	int i, pos, len;

	// Can't find a corresponding section
	if (!(sn = section_find(t->cwd))) {
		socket_print(t, "200 Sorry, you are not in a section right now.\r\n");
		return;
	}

	bzero(hidden, 50);
	bzero(denied, 40);
	bzero(deniedd, 40);
	*groups = 0;

	snprintf(ratio, 5, "1:%d",
		(sn->ratio) ? sn->ratio : server_userquotaratio);
	
	if ((tmp = misc_patt_list(sn->patts, PATT_HIDE)))
		strncpy(hidden, tmp, 49);
	if ((tmp = misc_patt_list(sn->patts, PATT_DENY)))
		strncpy(denied, tmp, 39);
	if ((tmp = misc_patt_list(sn->patts, PATT_DENYD)))
		strncpy(deniedd, tmp, 39);

	for (i = 0, pos = 0; i < sn->num_groups; i++) {

		len = strlen(sn->groups[i]);
		if (len + 1 >= sizeof(groups)) break; // ran out of space

		len = snprintf(&groups[pos], sizeof(groups) - pos -1, 
					   "%s ", sn->groups[i]);

		if (len < 0) break;

		pos += len;

	}

	groups[pos] = 0; // null terminate it, no matter where we are.



	socket_print(t,
		"200-[ SECTION START ]\r\n"
		".------------------------------------------------------------------------------.\r\n"
		"| Name: %-10.10s | Path: %-29.29s | Smartpath: %-8.8s |\r\n"
		"|------------------------------------------------------------------------------|\r\n"
		"| Files added to dupe database? (dupecheck)                  : %-3.3s             |\r\n"
		"| Files checked for integrity? (filecheck)                   : %-3.3s             |\r\n"
		"| Delete files failing integrity? (deletebad)                : %-3.3s             |\r\n"
		"| Record racestats? (racestats)                              : %-3.3s             |\r\n"
		"| Announce information to IRC bot? (announce)                : %-3.3s             |\r\n"
		"| Convert spaces to _ when creating dir? (convertmkdspace)   : %-3.3s             |\r\n"
		"| Convert spaces to _ when creating files? (convertstorspace): %-3.3s             |\r\n"
		"| Convert all new directories to uppercase? (uppercasedir)   : %-3.3s             |\r\n"
        "| Convert all new files to lowercase? (lowercasefile)        : %-3.3s             |\r\n",
				 (sn->name) ? sn->name : "None", 
				 sn->path,
				 (sn->smartpath) ? sn->smartpath : "None",
				 (sn->dupecheck) ? "On" : "Off", 
				 (sn->filecheck) ? "On" : "Off", 
				 (sn->deletebad) ? "On" : "Off", 
				 (sn->racestats) ? "On" : "Off",
				 (sn->announce) ? "On" : "Off",
				 (sn->convertmkdspace) ? "On" : "Off", 
				 (sn->convertstorspace) ? "On" : "Off", 
				 (sn->uppercasedir) ? "On" : "Off",
				 (sn->lowercasefile) ? "On" : "Off");

	socket_print(t,
		"| Extract MP3 genre to file on disk? (extractmp3genre)       : %-3.3s             |\r\n"
		"| Ratio for this section. (ratio)                            : %-4.4s            |\r\n"
		"| Hidden area (stealth)                                      : %-4.4s            |\r\n"
		"| Request area (request)                                     : %-4.4s            |\r\n"
		"| Incompletes dirs (incompletes)                             : %-4.4s            |\r\n"
		"| Remote area (remote)                                       : %-4.4s            |\r\n"
		"| Hidden files/dirs (hidefile): %-46.46s |\r\n"
		"| Denied files/dirs files (denyfile): %-40.40s |\r\n"
		"| Denied files/dirs files (denydir) : %-40.40s |\r\n"
		"| Limited to groups (groups): %-48.48s |\r\n"
		"`------------------------------------------------------------------------------'\r\n"
		"200 [ SECTION END ]\r\n",
				 (sn->extractmp3genre) ? "On" : "Off",
				 ratio, 
				 (sn->stealth) ? "On" : "Off", 
				 (sn->request) ? "On" : "Off", 
				 (sn->incompletes) ? "On" : "Off", 
				 (sn->remote_user) ? "On" : "Off", 
				 (*hidden) ? hidden : "None", 
				 (*denied) ? denied : "None",
				 (*deniedd) ? deniedd : "None",
				 (*groups) ? groups : "None");

}






void section_free(void)
{
	struct section_node *node, *next;
	//	struct misc_patt *misc;

	for (node = section_node_head; node; node = next) {

		next = node->next;

		SAFE_FREE( node->path );
		SAFE_FREE( node->name );
		SAFE_FREE( node->smartpath );

		SAFE_FREE( node->remote_user );
		SAFE_FREE( node->remote_pass );

		// patts

		// This code core's, mainly cos these pattern lists all point to the
		// same node, so once they are free's, they would be free'd again.
#if 0
		while (node->patts) {

			misc = node->patts;
			node->patts = node->patts->next;
			SAFE_FREE(misc->patt);
			SAFE_FREE(misc);

		}
#endif

		node->patts = NULL;

		SAFE_FREE(node);

	}

	section_node_head = NULL;

}





//
// Add a group, this function allocates new space, so no need to strdup
// Currently takes "char *" groups, but this could be a group pointer to be
// faster compary (is_member_of test) It takes a string like "group1,group2"
//
void section_add_groups(section_t *section, char *groups)
{
	//	char buffer[8192];
	char *ar = groups, *group;

	if (!section->path) {
		consolef("[section] section's 'path' definition must be before any 'group' types\n");
		return;
	}


	while((group = misc_digtoken(&ar, ",\r\n"))) {
		
		// Allocate space
		section->num_groups++;
		section->groups = realloc(section->groups,
								  sizeof(char *) *section->num_groups);
		
		if (!section->groups) {
			section->num_groups = 0;
			perror("add_group:");
			return;
		}

		section->groups[ section->num_groups - 1 ] = strdup(group);
		
		//		consolef("[section] adding group '%s'\n", group);

	}
		
}




//
// Add an expand path, this function allocates new space, so no need to strdup
//
void section_add_expand(section_t *section, char *path)
{
	//	char buffer[8192];

	if (!section->path) {
		consolef("[section] section's 'path' definition must be before any 'epxand' types\n");
		return;
	}


	//snprintf(buffer, sizeof(buffer), "%s/%s", 
	//		 section->path,
	//		 path);

	// Allocate space
	section->num_expand_paths++;
	section->expand_paths = realloc(section->expand_paths,
									sizeof(char *) *section->num_expand_paths);
	
	if (!section->expand_paths) {
		section->num_expand_paths = 0;
		perror("expand_path:");
		return;
	}

	//file_undot(buffer);

	//section->expand_paths[ section->num_expand_paths - 1 ] = strdup(buffer);
	section->expand_paths[ section->num_expand_paths - 1 ] = strdup(path);

	//	consolef("[section] adding expand '%s'\n", path);
		
}



char *section_expand(char *path)
{
	struct section_node *sn = section_find(path);
	int i, ret;
	char *matchpath;
	char buffer[8192];
	static char result[8192];

	//consolef("[section] expand called...%s\n",
	//		 sn ? sn->name : "(null)");

	if (!sn)
		return NULL;

	matchpath = section_rootfind(path);


	//consolef("[section] has %d defines %p\n",
	//		 sn->num_expand_paths,
	//		 sn->expand_paths);


	// We are in a section, does it have expand_paths
	if (!sn->expand_paths || !sn->num_expand_paths)
		return NULL;

	// Check if we are indeed inside one:
	for (i = 0; i < sn->num_expand_paths; i++) {



		//		if (!lfnmatch(sn->expand_paths[i], path, 
		//			  LFNM_LEADING_DIR|LFNM_CASEFOLD)) {

		snprintf(buffer, sizeof(buffer), "%s/%s",
				 sn->path,
				 sn->expand_paths[i]);

		///consolef("comparing '%s' with '%s'\n",
		//		 buffer,
		//		 matchpath);

		if (!lfnmatch(buffer, matchpath,
					  LFNM_LEADING_DIR|LFNM_CASEFOLD)) {
			
			//consolef("[section] expand matched: '%s'\n", sn->expand_paths[i]);


			// Read the link's contents.
			snprintf(buffer, sizeof(buffer), "%s/%s/%s",
					 server_usechroot, 
					 sn->path,
					 sn->expand_paths[i]);
			

#ifdef WIN32
			return NULL;
#else

			if ((ret = readlink(buffer, buffer, sizeof(buffer)-1)) < 0) {
				perror("readlink");
				return NULL;
			}
#endif
			
			buffer[ret] = 0;
			
			//consolef("link read as '%s'\n", buffer);

			// We need to find "where" to write it back.
			// I HATE using strstr() but there is no better way here.
			matchpath = strstr(path, sn->expand_paths[i]);

			if (!matchpath) return NULL;

			//consolef("found at '%s' which is %d\n", matchpath,
			//					 matchpath - path);
			// found at 'today/' which is 10
			// result '/incoming/!/incoming!0827!/'


			snprintf(result, sizeof(result),
					 "%*.*s%s%s",
					 matchpath - path,
					 matchpath - path,
					 path,
					 //					 sn->path,
					 buffer,
					 &matchpath[strlen(sn->expand_paths[i])]);
			
			consolef("[section] expand result '%s'\n", result);

			file_undot(result);

			return result;
			
			break;


		}

	}


	return NULL;

}




//
// Add an expand path, this function parses the crap out of the string to save
// the data independently, so no need to strdup.
//
void section_add_remote(section_t *section, char *site)
{
	// Expect "site" to be in the form of: "mp3:mp3@1.2.3.4:21"
	// where only the port is optional. 
	// We set that the section is remote if "remote_user" is defined. We
	// don't use "remote_host" as it would involve INADDR_NONE.
	// TODO: expand this to allow keyword to allow sending of real userid
	//       and if slave supports auto-add...

	char *ar, *user, *pass, *host, *port;
	int ssl = 0;

	if (!section->path) {
		consolef("[section] section's 'path' definition must be before any 'remote' types\n");
		return;
	}

	// Parse me.
	ar = site;

	user = misc_digtoken(&ar, ":");
	if (!user) {
		consolef("[section] parse error reading 'user' field in 'remote' type.\n");
		return;
	}

	pass = misc_digtoken(&ar, "@:");
	if (!pass) {
		consolef("[section] parse error reading 'pass' field in 'remote' type.\n");
		return;
	}

	if (misc_digtoken_optchar == ':')
		ssl = 1;

	host = misc_digtoken(&ar, ":\r\n");
	if (!host) {
		consolef("[section] parse error reading 'host' field in 'remote' type.\n");
		return;
	}

	port = misc_digtoken(&ar, "\r\n");

	//	consolef("[section] remote parsed  '%s' '%s' '%s' and '%s'\n",
	//		 user, pass, host, port ? port : "(null)");

	// Remember these values, also look up the hostname (if needed) now so
	// we don't still later. FIXME
	section->remote_host = lion_addr(host);

	// LiON needs to handle rudamental host lookup functions!!
#ifndef INADDR_NONE 
#define INADDR_NONE -1
#endif
	if (section->remote_host == INADDR_NONE) {
		consolef("[section] 'host' field must be in IP dot-formation for 'remote' type, sorry\n");
		return;
	}

	if (port && (atoi(port) > 0)) 
		section->remote_port = atoi(port);
	else
		section->remote_port = 21;


	SAFE_DUPE(section->remote_pass, pass);
	section->remote_ssl = ssl; // Enforce SSL?
		
	// Set "remote_user" last since that triggers this to be remote section.
	SAFE_DUPE(section->remote_user, user);
}
