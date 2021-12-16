#if HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#ifdef POSIX
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif


#ifdef DEC_ES
#include <sys/security.h>
#include <prot.h>
#endif

#include <string.h>

#include "lion.h"
#include "dirlist.h"

#include "global.h"
#include "conf.h"
#include "io.h"
#include "socket.h"
#include "user.h"
#include "login.h"
#include "exec.h"
#include "data.h"
#include "file.h"
#include "log.h"
#include "check.h"
#include "quota.h"
#include "misc.h"
#include "groups.h"
#include "ident.h"
#include "dupe.h"
#include "irc.h"
#include "export.h"
#include "section.h"
#include "../lion/src/misc.h"


extern int getopt();
extern char *optarg;

static int main_adduser = 0, main_deleteuser = 0, main_edituser = 0,
	main_listuser=0, main_export=0, main_import=0,main_dupelist = 0,
	main_toplists=0;

#ifdef WIN32
static int windows_service = 0;
#endif

static char *irc_send = NULL;

static char *dupelist_pattern = NULL;
static unsigned long dupelist_age = 0;

static int main_daemon = 1;


/* If we are behind dynamic IP situations and the user can pass the
   real external IP for incomming connections (passive) let them do
   so here */

unsigned long main_passiveIP = 0;


time_t server_uptime = 0;


void main_rehash_handler(void);



#define ONOFF(x) ((x)?("on"):("off"))

void display_settings()
{

	consolef("%s by Jorgen Lundman, 1995-2009.\n\n", VERSION_STR);

#if 1
	if (server_bindif)
		consolef(" - Listening on     : %s:%d\n", server_bindif, server_port);
	else
		consolef(" - Listening on     : *:%d\n", server_port);

	consolef(" - User file        : %s\n", server_userfile);
	consolef(" - Quota file       : %s\n", server_quotafile);

	if (server_groupfile)
		consolef(" - Group file       : %s\n", server_groupfile);

	if (server_dupedbfile)
		consolef(" - Dupedb file      : %s\n", server_dupedbfile);
	if (server_loguser)
		consolef(" - USER log file    : %s\n", server_loguser);
	if (server_logxfer)
		consolef(" - XFER log file    : %s\n", server_logxfer);

	consolef(" - Root directory   : %s\n", server_usechroot);

	consolef(" - User record size : %d bytes\n", sizeof(struct login_node));
	consolef(" - Quota record size: %d bytes\n", sizeof(struct quota_node));
#endif

}


void display_usage()
{
	fprintf(stderr,"Usage:\r\n");
	fprintf(stderr,"\t -h        - Display this text \r\n");
	fprintf(stderr,"\t -a        - Adduser \r\n");
	fprintf(stderr,"\t -d        - Deleteuser \r\n");
	fprintf(stderr,"\t -e        - Edituser \r\n");
	fprintf(stderr,"\t -f <file> - Load alternative config file \r\n");
	fprintf(stderr,"\t -l        - Listuser(s) \r\n");
	fprintf(stderr,"\t -ll       - Listuser(s) long format \r\n");
	fprintf(stderr,"\t -F        - Top files top lists\r\n");
#if 1
	fprintf(stderr,"\t -x        - Export lftpd database to text file\r\n");
	fprintf(stderr,"\t -i        - Import text file to lftpd database\r\n");
#endif
	fprintf(stderr,"\t -v        - Verbose (don't go daemon) \r\n");
	fprintf(stderr,"\t -r <ip>   - Specify real external IP for passive \r\n");
#ifdef DUPECHECK
	fprintf(stderr,"\t -p <glob> - List entries in DUPEDB\r\n");
	fprintf(stderr,"\t -P <age>  - Purge entries in DUPEDB older than <age> in seconds\r\n");
#endif
#ifdef IRCBOT
	fprintf(stderr,"\t -b <str>  - Send IRCBOT string\r\n");
#endif
	fprintf(stderr,"\t -c <path> - Pass path to checker logic (path is FTP root)\r\n");
#ifdef WIN32
	fprintf(stderr,"\t -I        - Install as Win32 Service\r\n");
	fprintf(stderr,"\t -U        - Uninstall Win32 Service\r\n");
	fprintf(stderr,"\t(-S pwd    - Start as Win32 Service in pwd)\r\n");
	fprintf(stderr,"\tWarning, if you use -r <ip> with -I, it needs to be specified first\r\n");
#endif
	exit(1);
}




void args_load(int argc, char **argv)
{
	int opt;

	while ((opt=getopt(argc, argv, "hvdaeilxFf:p:r:P:b:c:"
#ifdef WIN32
		"IUS:"  // Service install/uninstall
#endif
		)) != -1) {



		switch(opt) {
		case 'h':
			display_usage();
		case 'a':
			main_adduser = 1;
			main_daemon = 0;
			break;
		case 'd':
			main_deleteuser = 1;
			main_daemon = 0;
			break;
		case 'e':
			main_edituser = 1;
			main_daemon = 0;
			break;
		case 'f':
			SAFE_COPY(server_configfile, optarg);
			break;
#ifdef DUPECHECK
		case 'p':
			main_daemon = 0;
			main_dupelist = 1;
			SAFE_COPY(dupelist_pattern, optarg);
			break;
		case 'P':
			main_daemon = 0;
			main_dupelist = 2;
			dupelist_age = atoi( optarg );
			break;
#endif
		case 'x':
			main_adduser = main_export = 1;
			main_daemon = 0;
			break;
		case 'i':
			main_adduser = main_import = 1;
			main_daemon = 0;
			break;
		case 'l':
			main_adduser = main_listuser += 1; /* add user to skip various things*/
			main_daemon = 0;
			break;

		case 'F': // Top lists
			main_toplists = 1;
			main_daemon = 0;
			break;

		case 'v':
			main_daemon = 0;
			break;
		case 'r':
			main_passiveIP = lion_addr(optarg);
			if (main_passiveIP == -1) main_passiveIP = 0;
			break;
#ifdef IRCBOT
		case 'b':
			SAFE_COPY(irc_send, optarg);
			break;
#endif
		case 'c':
			check_manual_scan(optarg);
			break;


#ifdef WIN32
		case 'I':
			InstallService(main_passiveIP);
			exit(0);
			break;
		case 'U':
			UninstallService();
			exit(0);
			break;
		case 'S':
			main_daemon = 1;
			windows_service = 1;
			chdir(optarg); // change to the workingdir. Services
			//should really let you specify this
			break;
#endif


		}
	}
}


void sig_int()
{
	signal(SIGINT, sig_int);
	io_exit = 1;
}



//
// This is the default lion_userinput, and isn't use anywhere. It is here
// for linkage, and it will highlight if any lion_t is called wrong, and we
// get called.
//
int lion_userinput( lion_t *handle,
					void *user_data, int status, int size, char *line)
{

	consolef("lion_userinput called for %p status %d (size %d) with user_data %p\n",
			 handle, status, size, user_data);


	return 0;
}


void main_dirlist_start(void)
{
	struct section_node *defsect;

	if ((defsect = section_find("/"))) {

		struct misc_patt *patt;

		for (patt = defsect->patts; patt; patt = patt->next) {
			if (patt->type & PATT_HIDE)
				dirlist_hide_file(patt->patt);
		}

	}

	// Luckily our get_username is compatible with dirlist already.
	dirlist_set_uidlookup( user_get_username );
	dirlist_set_gidlookup( login_gid_lookup );


	if (dirlist_init(server_num_list_helpers)) {

		consolef("[main] libdirlist initialization failed...\n");
		exit(-2);
	}

}


//
// Windows needs a fake main so that when it is started by service
// we can go off and create thread for it, and then come back here.
//

#ifdef WIN32
int main(int argc, char **argv)
{

	args_load(argc, argv);

	if (windows_service) {
		StartedByService();
	} else {
		realmain(argc, argv);
	}
	exit(0);
}

int realmain(int argc, char **argv)

#else

int main(int argc, char **argv)

#endif
{

#ifdef DEC_ES
	set_auth_parameters(argc, argv);
#endif

//	_asm{ int 3}

	log_console(1); //1

	global_set_default();

	args_load(argc, argv);

#ifdef DEBUG
	main_daemon = 0;
#endif

	conf_init(server_configfile);

	display_settings();

	// Open logs before chroot()
	log_load();

	// Remove the console output
	if (main_daemon)
		log_console(0);

	// Do setuid root checks.
	file_gononroot();

	user_load();
	quota_load();

#if 0
	if (main_daemon && (main_adduser | main_deleteuser | main_edituser))
		exit(-5);
#endif

	// Invoked with -l
	if (main_listuser) {
		if (main_listuser == 1)
			user_listuser();
		else
			user_listuser_long();
		user_close();
		quota_quit();
		exit(0);
	}

	if (main_toplists) {

		user_list_top();
		user_close();
		quota_quit();
		exit(0);

	}


	// Invoked with -x
	if (main_export) {
		export_users();
		user_close();
		quota_quit();
		exit(0);
	}

	// Invoked with -i
	if (main_import) {
		import_users();
		user_close();
		quota_quit();
		exit(0);
	}

	// Invoked with -a
	if (main_adduser) {
		user_adduser();
		user_close();
		quota_quit();
		exit(0);
	}

	// Invoked with -d
	if (main_deleteuser) {
		user_deleteuser();
		user_close();
		quota_quit();
		exit(0);
	}

	// Invoked with -e
	if (main_edituser) {
		user_edituser();
		user_close();
		quota_quit();
		exit(0);
	}

#ifdef DUPECHECK
	// Invoked with -p
	if (main_dupelist) {
		struct login_node *user;
		struct dupe_node *dupe;

		dupe_init();
		user = login_console_user();

		if (main_dupelist == 2) {

			dupe_purge(dupelist_age);

		} else {

			for (dupe = dupe_fnmatch(dupelist_pattern)
					 ; dupe
					 ; dupe = dupe_fnmatch(NULL))
				{

					socket_number(user, 6200, (char *)dupe);

				}

			socket_number(user, 200, "DUPE");

		}

		dupe_quit();
		free(dupelist_pattern);
		exit(0);

	}
#endif // DUPECHECK


	// SSL cert. setup
	if (server_tls_rsafile)
		lion_ssl_rsafile(server_tls_rsafile);

	// SSL cipher setup
	if (server_tls_ciphers)
		lion_ssl_ciphers(server_tls_ciphers);

	// Entropy Gathering Daemon setup
	if (server_egdsocket)
		lion_ssl_egdfile(server_egdsocket);


	lion_buffersize(server_data_buffer_size);


	// Initialise lion
	if (lion_init()) {
		consolef("Failed to initialise LiON -- exit\n");
		exit(-1);
	}

	check_init();


	ident_init();

	socket_load();

	file_load();

	group_load();

#ifdef DUPECHECK
	dupe_init();
#endif

#ifdef IRCBOT


	if (irc_send) {

		server_irc_src_port = 0;
		irc_init();

		irc_announce("%s\n", irc_send);
		SAFE_FREE(irc_send);

		// Clean up, well, not full, and not really needed.
		irc_free();
		lion_free();

		exit(0);

	}

	irc_init();


#endif

	login_listen();


	// this stuff is no longer handled
	max_open_files_safe = getdtablesize() - 4; /* Fudge factor */

	consolef("Max open fd's is: %d\n", max_open_files_safe);


	server_uptime = lion_global_time;

	global_update_all();


	// Load up the dirlist library, PLEASE MAKE ME .conf VARIABLE

	// Load the hide path stuff before the init() call.. although it _probably_
	// will work to load after, it is technically not the right thing to do.
	//dirlist_hide_file(".state-*");

	main_dirlist_start();



	consolef("[main] libdirlist initialised. %d helpers.\n",
			 server_num_list_helpers);


	// THESE SHOULD COME FROM CONFIG? And they do!
	if (server_capping) {

		// Ok, we can use lion's "global" capping here, but the problem is
		// that it caps _everything_. Including control sessions, ident
		// lookups and so on. Which is somewhat undesirable. We want to
		// only cap data nodes. So, we create a "global group" here, that
		// we add data nodes to. Be aware, that currently lion only lets
		// you be in one group at a time, so setting global cap will over
		// ride that of user, and session caps.

		//lion_global_rate_in(server_cap_total_upload);
		//lion_global_rate_out(server_cap_total_download);
#if 1
		data_global_rate_cap_group = lion_group_new();

		consolef("[main] New total capping group is: %d\n",
				 data_global_rate_cap_group);

		lion_group_rate_in (data_global_rate_cap_group,
							server_cap_total_upload);
		lion_group_rate_out(data_global_rate_cap_group,
							server_cap_total_download);
#endif

	}

	// Check the data_pasv variables make sense.
	if ((server_data_pasv_start <= 0) ||
		(server_data_pasv_end <= 0) ||
		(server_data_pasv_end <= server_data_pasv_start) ||
		(server_data_pasv_end > 65535)) {

		if (server_data_pasv_start || server_data_pasv_end)
			consolef("[conf] silly data_pasv_start and end values. Clearing.\n");

	} else {

		consolef("[conf] PASV port range set to [%d-%d] (%d connections)\n",
				 server_data_pasv_start,
				 server_data_pasv_end,
				 server_data_pasv_end - server_data_pasv_start + 1);

	}


	signal(SIGINT, sig_int);
#ifndef WIN32
	signal(SIGHUP, main_rehash_handler);
#endif

	/*
	 *
	 */


	loop();


	/*
	 *
	 */

#ifndef WIN32
	signal(SIGHUP, SIG_DFL);
#endif

	login_listen_free();


	// loop() returns, exit the daemon gracefully
	user_close();
	quota_quit();
	group_save();

#ifdef DUPECHECK
	dupe_quit();
#endif


#ifdef IRCBOT
	irc_free();
#endif

	check_free();

	conf_free();

	dirlist_free();

	lion_free();

	log_free();

	exit(0);
}







#ifndef WIN32
void main_rehash_handler()
{

	signal(SIGHUP, main_rehash_handler);
	server_rehash = 1;

#ifdef IRCBOT
	irc_announce("REHASH|user=SIGHUP\n");
#endif

}
#endif





void main_rehash(void)
{

	if (server_usechroot && !server_usesoftchroot) {
		consolef("[main] rehashing not possible without softchroot\n");
		return;
	}


	consolef("[main] server rehash! pray on..\n");


	// We need to remember some settings so we can determine if they
	// have changed. So we know exactly how much we need to reload.

	file_goroot();



	// Stop things.
	consolef("conf_free\n");
	conf_free();

#ifdef IRCBOT
	consolef("irc_free\n");
	irc_free();
#endif

	check_free();

	dirlist_free();

	group_save();
	user_close();
	quota_quit();
#ifdef DUPECHECK
	dupe_quit();
#endif

	// Release listening port.
	login_listen_free();

	//	file_gononroot();

	consolef("[main] rehash, pausing before reload...\n");
	sleep(2);

	//	file_goroot();

	global_set_default();

	// Load things.
	conf_init(server_configfile);

	main_dirlist_start();

	user_load();
	quota_load();
	//	group_load();  // groups doubles if we call this.
	check_init();
#ifdef DUPECHECK
	dupe_init();
#endif


#ifdef IRCBOT
	irc_init();
#endif


	if (server_capping) {
		lion_global_rate_in(server_cap_total_upload);
		lion_global_rate_out(server_cap_total_download);
	}

	if (server_tls_ciphers)
		lion_ssl_ciphers(server_tls_ciphers);

	// login_free sets io_exit to one, since the listening port had closed
	// so we need to reset it, so that we don't just exit. But, we do so
	// before we start the new listening port, so that we exit if THAT failed
	// to start.

	io_exit = 0;

	// start listening again.
    login_listen();


	sleep(2);

	consolef("[main] server rehash! pray off..\n");


	//	file_gononroot();

}
