#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>


#include "lion.h"

#include "misc.h"
#include "log.h"
#include "global.h"
#include "conf.h"
#include "data.h"
#include "check.h"
#include "irc.h"
#include "section.h"
#include "sitecust.h"
#include "../lion/src/misc.h"

#include "dirlist.h"


struct conf_item *conf_item_head = NULL;
void conf_print(struct conf_item *head);

void conf_init(char *configfile)
{

	struct conf_item *ci;

	if (conf_load(configfile))
		exit(-1);

	// Go through entire config list and add items,
	// complain about shite entries.
	for (ci = conf_item_head; ci; ci = ci->next) {

		// Changing these to use SAFE_COPY as it releases any value
		// potentially already set.

		//-----------------------------------------------
		// NETWORK SETTINGS
		if (!strcasecmp(ci->name, "port")) {
			server_port = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "bindif")) {
			server_bindif = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "useident")) {
			server_useident = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "data_buffer_size")) {
			server_data_buffer_size = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "data_bind_port")) {
			server_data_bind_port = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "capping")) {
			server_capping = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "cap_download")) {
			server_cap_download = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "cap_upload")) {
			server_cap_upload = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "cap_user_download")) {
			server_cap_user_download = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "cap_user_upload")) {
			server_cap_user_upload = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "cap_total_download")) {
			server_cap_total_download = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "cap_total_upload")) {
			server_cap_total_upload = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "nocapip")) {
			misc_patt_add_gbl(PATT_NOCAP, strdup(conf_getstring_from(ci)));
		} else if (!strcasecmp(ci->name, "data_pasv_start")) {
			server_data_pasv_start = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "data_pasv_end")) {
			server_data_pasv_end = conf_getint_from(ci);

		//-----------------------------------------------
		// FILE LOCATIONS/FILESYSTEM SETUP
		} else if (!strcasecmp(ci->name, "userfile")) {
			server_userfile = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "msgpath")) {
			server_msgpath = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "wallpath")) {
			server_wallpath = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "quotafile")) {
			server_quotafile = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "dupedbfile")) {
			server_dupedbfile = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "logxfer")) {
			server_logxfer = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "loguser")) {
			server_loguser = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "logadmin")) {
			server_logadmin = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "usechroot")) {
			server_usechroot = strdup(conf_getstring_from(ci));
			/*if (server_usechroot[strlen(server_usechroot)-1] != '/') {
				server_usechroot = realloc(server_usechroot, strlen(server_usechroot)+1);
				strcpy(server_usechroot, conf_getstring_from(ci));
				server_usechroot[strlen(server_usechroot)-1] = '/';
			}*/
		} else if (!strcasecmp(ci->name, "logrotate")) {
			server_logrotate = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "dupemaxdays")) {
			server_dupemaxdays = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "usesoftchroot")) {
			server_usesoftchroot = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "permfile")) {
			server_permfile = conf_getint_from(ci);
			umask(0000);
		} else if (!strcasecmp(ci->name, "goodfile")) {
			server_goodfile = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "permdir")) {
			server_permdir = conf_getint_from(ci);
			umask(0000);
		} else if (!strcasecmp(ci->name, "mkdirstring")) {
			server_mkdirstring = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "norecurse")) {

			dirlist_no_recursion(conf_getstring_from(ci));

		} else if (!strcasecmp(ci->name, "num_list_helpers")) {
			server_num_list_helpers = conf_getint_from(ci);


		// SECTION BIT--------------------------------
		} else if (!strcasecmp(ci->name, "section")) {

			struct conf_item *block, *runner;
			struct section_node *this; // using "this" is a really bad idea.
			char *path;

			// GRAB CONFIG BLOCK
			if (!(block = conf_getblock_from(ci))) {
				consolef("[conf] Warning: bad section entry line %d, "
							"please rectify.\n", ci->line);
				continue;
			}

			// GRAB THE PATH AT THE VERY LEAST
			if (!(path = conf_getstring(block, "path"))) {
				consolef("[conf] Warning: section entry at line %d "
							"needs 'path' field.\n", ci->line);
				continue;
			}

			// CREATE A NEW SECTION NODE
			if (!(this = section_add(path)))  {
				consolef("[conf] Warning: unable to create now section "
							" entry at line %d ", ci->line);
				continue;
			}

			// CYCLE THROUGH THE AVAILABLE FIELDS
			for (runner = block; runner; runner = runner->next) {

				if (!strcasecmp(runner->name, "path")) {
					// Ignore the path, we have it already.
				} else if (!strcasecmp(runner->name, "smartpath")) {
					this->smartpath = strdup(conf_getstring_from(runner));
				} else if (!strcasecmp(runner->name, "name")) {
					this->name = strdup(conf_getstring_from(runner));
				} else if (!strcasecmp(runner->name, "dupecheck")) {
					this->dupecheck = conf_getbool_from(runner);

					if (this->dupecheck)
						server_dupecheck = 1;

				} else if (!strcasecmp(runner->name, "announce")) {
					this->announce = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "filecheck")) {
					this->filecheck = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "stealth")) {
					this->stealth = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "request")) {
					this->request = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "incompletes")) {
					this->incompletes = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "deletebad")) {
					this->deletebad = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "racestats")) {
					this->racestats = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "ratio")) {
					this->ratio = conf_getint_from(runner);
				} else if (!strcasecmp(runner->name, "groups")) {
					section_add_groups(this, conf_getstring_from(runner));
				} else if (!strcasecmp(runner->name, "expand")) {
					section_add_expand(this, conf_getstring_from(runner));
				} else if (!strcasecmp(runner->name, "remote")) {
					section_add_remote(this, conf_getstring_from(runner));
				} else if (!strcasecmp(runner->name, "hidefile")) {

					char *patt = strdup(conf_getstring_from(runner));

					if (!patt) {
						consolef("[conf]: Warning: bad hidefile pattern "
										"at line %d\n", runner->line);
						continue;
					}

					misc_patt_add(&this->patts, PATT_HIDE, patt);

				} else if (!strcasecmp(runner->name, "denyfile")) {

					char *patt = strdup(conf_getstring_from(runner));

					if (!patt) {
						consolef("[conf]: Warning: bad denyfile pattern "
										"at line %d\n", runner->line);
						continue;
					}

					misc_patt_add(&this->patts, PATT_DENY, patt);
				} else if (!strcasecmp(runner->name, "denydir")) {

					char *patt = strdup(conf_getstring_from(runner));

					if (!patt) {
						consolef("[conf]: Warning: bad denydir pattern "
										"at line %d\n", runner->line);
						continue;
					}

					misc_patt_add(&this->patts, PATT_DENYD, patt);

				} else if (!strcasecmp(runner->name, "convertmkdspace")) {
					this->convertmkdspace = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "convertstorspace")) {
					this->convertstorspace = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "extractmp3genre")) {
					this->extractmp3genre = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "extractimdb")) {
					this->extractimdb = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "uppercasedir")) {
					this->uppercasedir = conf_getbool_from(runner);
				} else if (!strcasecmp(runner->name, "lowercasefile")) {
					this->lowercasefile = conf_getbool_from(runner);
				} else {
					consolef("[conf]: Warning: unknown field %s at line %d\n",
							runner->name, runner->line);
				}
			}

			//			consolef("[conf]: new section added %s (%s)\n", this->name, this->path);

		//-----------------------------------------------
		// SERVER SETTINGS
		} else if (!strcasecmp(ci->name, "greeting")) {
			server_greeting = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "rawgreeting")) {
			server_rawgreeting = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "email")) {
			server_email = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "walllogin")) {
			server_walllogin = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "maximumlogins")) {
			server_maximumlogins = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "userquotadaily")) {
			server_userquotadaily = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "userquotaratio")) {
			server_userquotaratio = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "userquotatime")) {
			server_userquotatime = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "anonymous")) {
			server_anonymous = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "usesetuid")) {
			server_usesetuid = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "useridstart")) {
			server_useridstart = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "lookupuid")) {
			server_lookupuid = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "nobody")) {
			server_nobody = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "useridle")) {
			server_useridle = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "usermatch")) {
			server_usermatch = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "globalmatch")) {
			misc_patt_add_gbl(PATT_GLOBAL, strdup(conf_getstring_from(ci)));
		} else if (!strcasecmp(ci->name, "forceglobalonly")) {
			server_forceglobalonly = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "showlastlogin")) {
			server_showlastlogin = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "forcetagline")) {
			server_forcetagline = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "extractmp3format")) {
			server_extractmp3format = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "unnuke")) {
			server_unnuke = conf_getbool_from(ci);

		//-----------------------------------------------
		// ADDUSER DEFAULTS
		} else if (!strcasecmp(ci->name, "loginlimit")) {
			server_loginlimit = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "uploadlimit")) {
			server_uploadlimit = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "downloadlimit")) {
			server_downloadlimit = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "usercredits")) {
			server_usercredits = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "defaulttagline")) {
			server_defaulttagline = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "defaultflags")) {
			server_defaultflags = atoflags(conf_getstring_from(ci));

		//-----------------------------------------------
		// GROUP DEFAULTS
		} else if (!strcasecmp(ci->name, "groupfile")) {
			server_groupfile = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "grouppath")) {
			server_grouppath = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "groupmaxlogins")) {
			server_groupmaxlogins = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "groupmaxidle")) {
			server_groupmaxidle = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "groupmaxratio")) {
			server_groupmaxratio = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "grouptotalslots")) {
			server_grouptotalslots = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "groupleechslots")) {
			server_groupleechslots = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "nogroupname")) {
			server_nogroupname = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "group_pattern_limit")) {
			server_group_pattern_limit = conf_getint_from(ci);

		//-----------------------------------------------
		// FILECHECK
		} else if (!strcasecmp(ci->name, "filecheck")) {

			struct conf_item *block = NULL;

			if ((block = conf_getblock_from(ci))) {

				char *ext, *prog;
				int ret;

				ext = strdup(conf_getstring(block, "ext"));
				prog = strdup(conf_getstring(block, "prog"));
				ret = conf_getint(block, "ret");

				// If one, and not the other is defined, we have memory leak.
				if (!ext || !prog)
					consolef("[conf] Warning: bad filecheck entry line %d, please rectify.\n", block->line);
				else
					check_add(ext, prog, ret);
			}

		} else if (!strcasecmp(ci->name, "maxsizecheck")) {
			server_maxsizecheck = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "backgroundtest")) {
			server_backgroundtest = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "irccmds")) {
			server_irccmds = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "irc_src_port")) {
			server_irc_src_port = conf_getint_from(ci);
		} else if (!strcasecmp(ci->name, "autoextract_diz")) {
			server_autoextract_diz = strdup(conf_getstring_from(ci));

		//-----------------------------------------------
		// IRCBOT
		} else if (!strcasecmp(ci->name, "ircbot")) {

			struct conf_item *block;

			if ((block = conf_getblock_from(ci))) {

				char *host, *pass;
				int port;

				host = conf_getstring(block, "host");
				port = conf_getint(block, "port");
				pass = conf_getstring(block, "pass");

				if (!host || !port || !pass)
					consolef("[conf] Warning: bad %s entry line %d, please rectify.\n", block->name, block->line);
#ifdef IRCBOT
				else
					irc_addbot(host, port, pass); // we strdup in there.
#endif

			}

		//-----------------------------------------------
		// ENCRYPTION
		} else if (!strcasecmp(ci->name, "allow_only_secure")) {
			server_allow_only_secure = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "allow_only_secure_data")) {
			server_allow_only_secure_data = conf_getbool_from(ci);
		} else if (!strcasecmp(ci->name, "tls_rsafile")) {
			server_tls_rsafile = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "tls_ciphers")) {
			server_tls_ciphers = strdup(conf_getstring_from(ci));
		} else if (!strcasecmp(ci->name, "egdsocket")) {
			server_egdsocket = strdup(conf_getstring_from(ci));

		//-----------------------------------------------
		// SITECUST
		} else if (!strcasecmp(ci->name, "sitecust")) {

			struct conf_item *block;

			if ((block = conf_getblock_from(ci))) {

				char *name, *type, *fileexec = NULL, *desc, *textflags, *args;
				int flags = 0;

				name = conf_getstring(block, "name");
				desc = conf_getstring(block, "desc");
				type = conf_getstring(block, "type");
				args = conf_getstring(block, "args");
				textflags = conf_getstring(block, "flags");

				if (textflags)
					flags = atoflags(textflags);

				// Find out what human readable text to get from conf file.
				if (!strcasecmp(type, "TEXT")) {

					if (!(fileexec = conf_getstring(block, "file"))) {
						consolef("[conf] Warning: missing 'FILE' inside 'SITECUST' block line %d\n",
								ci->line);
						continue;
					}

				} else if (!strcasecmp(type, "EXEC")) {

					if (!(fileexec = conf_getstring(block, "exec"))) {
						consolef("[conf] Warning: missing 'EXEC' inside 'SITECUST' block line %d\n",
								ci->line);
						continue;
					}

				} else {

					consolef("[conf] Warning: bad type entry line %d\n",
						conf_getline(block, type));

				}

				if (!name || !type || !desc || !fileexec) {
					consolef("[conf] Warning: bad %s entry line %d, "
							" please rectify.\n", ci->name, ci->line);
				} else {
					sitecust_add(strdup(name), type,
							strdup(fileexec), strdup(desc), flags, args);
				}

			}

		} else {
			switch (ci->type) {
				case CONF_BLOCK:
					consolef("[conf] Warning: Unknown block %s at line %d\n",
										ci->name, ci->line);
					break;
				default:
					consolef("[conf] Warning: Unknown field %s at line %d\n",
										ci->name, ci->line);
			}
		}

	}

	// Check mandatory fields.
	conf_mandatory();
	// Sanitize section configs
	section_apply_defaults();

}



// Return 0 for no errors, non-zero for errors.
int conf_load(char *configfile)
{

	FILE *fp;
	int linecnt = 1;

	if (!(fp = fopen(configfile, "r"))) {
		consolef("[conf] CRITICAL: failed to load %s: %s\n",
						configfile, strerror(errno));
		return -1;
	}


	conf_item_head = conf_parseblock(&linecnt, fp);
	fclose(fp);

	if (!conf_item_head) {
		consolef("[conf] CRITICAL: empty config file %s\n", configfile);
		return -1;
	}

	return 0;
}


// Check mandatory fields
void conf_mandatory(void)
{

	//-----------------------------------------------
	// CHECK FOR MANDATORY FIELDS
	if (!server_userfile) {
		consolef("[conf] You must specify an 'userfile' file!\n");
		exit(-1);
	}




	// Fill in defaults.
	if (!server_email)
		server_email = strdup("root@localhost");

	if (!server_nogroupname)
		server_nogroupname = strdup("NoGroup");

}


// Parses until it finds a "};", returns top of the list.
struct conf_item *conf_parseblock(int *linecnt, FILE *fp)
{

	int c;
	char field[CONF_MAXFIELD], value[CONF_MAXVALUE];
	int fieldcnt, valuecnt;
	struct conf_item *top = NULL;


	while ((c = fgetc(fp)) != EOF) {

		//-----------------------------------------------------------
		// END OF BLOCK
		if (c == '}')
			break;

		//-----------------------------------------------------------
		// WHITESPACE
		if (c == ' ' || c == '\t')
			continue;
		if (c == '\n') {
			(*linecnt)++;
			continue;
		}

		//-----------------------------------------------------------
		// COMMENTS

		// SHELL STYLE COMMENT
		if (c == '#') {
			while ((c = fgetc(fp)) != EOF) {
				if (c == '\n') {
					(*linecnt)++;
					break;
				}
			}
			continue;
		}

		// C/C++ STYLE COMMENTS
		if (c == '/') {
			char c2 = fgetc(fp);
			// C++ STYLE COMMENT
			if (c2 == '/') {
				while ((c = fgetc(fp)) != EOF) {
					if (c == '\n') {
						(*linecnt)++;
						break;
					}
				}
				continue;

			// C STYLE COMMENT
			} else if (c2 == '*') {


				while ((c = fgetc(fp)) != EOF) {
					if (c == '\n')
						(*linecnt)++;
					else if (c == '*') {
						c = fgetc(fp);
						if (c == '/')
							break;
					}
				}
				continue;

			} else {
				ungetc(c2, fp);
			}
		}

		//-----------------------------------------------------------
		// Get rid of all junky characters not allowed in fields
		// ie anything not A-Za-z0-9_-
		do {
			if (c == '\n')
				(*linecnt)++;
			if ((c >= 48 && c <= 57) || (c >=65 && c <= 122) || c == '-' || c == '_')
				break;
		} while ((c = fgetc(fp)) != EOF);

		//-----------------------------------------------------------
		// FIELD - Terminates on whitespace or '=' sign.
		bzero(field, CONF_MAXFIELD);
		fieldcnt = 0;
		do {
			if (c == '\n') {
				(*linecnt)++;
				continue;
			}

			if (fieldcnt >= CONF_MAXFIELD)
				continue;

			if (c == ' ' || c == '\t')
				continue;

			if (c == '=' || c == '{')
				break;

			// Assign to field name
			field[fieldcnt++] = c;

		} while ((c = fgetc(fp)) != EOF);


		// SKIP WHITESPACE if '='
		if (c == '=') {
			while ((c = fgetc(fp)) != EOF) {
				if (c == '\n')
					(*linecnt)++;
				if (c == ' ' || c == '\t' || c == '\n')
					continue;
				break;
			}
		}

		// Bad/missing field - skip junk
		if (field[0] == 0) {

			fprintf(stderr, "%d: garbage field\n", *linecnt);

			while ((c = fgetc(fp)) != EOF) {
				if (c == '\n')
					(*linecnt)++;
				if (c == ';')
					break;
			}

			continue;
		}

		//-----------------------------------------------------------
		// VALUE - Terminates on ';'
		bzero(value, CONF_MAXVALUE);
		valuecnt = 0;

		if (c == '\n')
			(*linecnt)++;

		// BLOCK
		if (c == '{') {
			conf_add(&top, field, conf_parseblock(linecnt, fp), CONF_BLOCK, *linecnt);
			// Trick printing of GARBAGE LINE
			value[0] = 1;
		}

		// STRING
		else if (c == '"') {

			while ((c = fgetc(fp)) != EOF) {

				if (c == '\n')
					(*linecnt)++;
				// END OF STRING
				if (c == '"')
					break;

				// OVERSIZED
				if (valuecnt >= CONF_MAXVALUE)
					continue;

				value[valuecnt++] = c;
			}

			conf_add(&top, field, (void *)value, CONF_STRING, *linecnt);

		}

		// Boolean ON/OFF
		else if (c == 'o' || c == 'O') {

			char c2 = fgetc(fp);
			if (c2 == 'n' || c2 == 'N') {

				value[0] = '1';
				conf_add(&top, field, (void *)atoi(value), CONF_BOOL, *linecnt);
			} else if (c2 == 'f' || c2 == 'F') {
				char c3 = fgetc(fp);
				if (c3 == 'f' || c3 == 'F') {

					conf_add(&top, field, (void *)atoi(value), CONF_BOOL, *linecnt);
					value[0] = '1';

				} else {
					ungetc(c3, fp);
				}
			} else {
				ungetc(c2, fp);
			}

		}

		// Boolean YES
		else if (c == 'y' || c == 'Y') {

			char c2 = fgetc(fp);
			if (c2 == 'e' || c2 == 'E') {
				char c3 = fgetc(fp);
				if (c3 == 's' || c3 == 'S') {
					value[0] = '1';
					conf_add(&top, field, (void *)atoi(value), CONF_BOOL, *linecnt);
				} else {
					ungetc(c3, fp);
				}
			} else {
				ungetc(c2, fp);
			}

		// Boolean NO
		} else if (c == 'n' || c == 'N') {
			char c2 = fgetc(fp);
			if (c2 == 'o' || c2 == 'O') {
				conf_add(&top, field, (void *)atoi(value), CONF_BOOL, *linecnt);
				value[0] = '1';
			} else {
				ungetc(c2, fp);
			}
		}

		// NUMBER
		else if ((c >= 48 && c <= 57) || c == '-') {

			value[valuecnt++] = c;

			while ((c = fgetc(fp)) != EOF) {

				if (c == '\n')
					(*linecnt)++;

				// END OF NUMBER
				if (c == ';')
					break;

				// NOT A NUMBER ANYMORE?
				if (c < 48 || c > 57)
					break;

				// OVERSIZED
				if (valuecnt >= CONF_MAXVALUE)
					continue;

				value[valuecnt++] = c;
			}

			// Try and read numbers beginning with 0 as octal.
			if (value[0] == 48)
				conf_add(&top, field, (void *)strtol(value, NULL, 8), CONF_INT, *linecnt);
			else
				conf_add(&top, field, (void *)atoi(value), CONF_INT, *linecnt);


		}
		// SKIP ANY JUNK
		do {
			if (c == '\n')
				(*linecnt)++;
			if (c == ';')
				break;
		} while ((c = fgetc(fp)) != EOF);

		// Didn't find a value...
		if (value[0] == 0)
			fprintf(stderr, "%d: garbage value for field %s\n", *linecnt, field);


	}

	return top;
}


void conf_add(struct conf_item **top, char *field, void *value, int type, int linecnt)
{

	struct conf_item *ci, *run;

#ifdef DEBUG_CONF
	if (type & CONF_STRING) {
		consolef("[conf] debug: conf_add(f=%s, v=%s, t=%d, l=%d)\n",
					field, value, type, linecnt);
	}
#endif

	if ((ci = malloc(sizeof(struct conf_item))) == NULL) {
		perror("malloc");
		exit(-1);
	}
	bzero(ci, sizeof(struct conf_item));

	strncpy(ci->name, field, CONF_MAXFIELD-1);
	ci->type = type;
	switch (ci->type) {

		case CONF_STRING:
			ci->value = strdup((char *)value);
			break;

		case CONF_INT:
		case CONF_BOOL:
			ci->value = value;
			break;

		case CONF_BLOCK:
			ci->value = value;
			break;

		default:
			;
	}

	ci->line = linecnt;

	// Kludge to add to end
	if (*top == NULL)
		*top = ci;
	else {
		for (run = *top; run->next; run = run->next)
			;// Do nothing
		run->next = ci;
	}

}

char *conf_getstring(struct conf_item *ci, char *field)
{

	struct conf_item *run;

	for (run = ci; run; run = run->next) {
		if (!strcasecmp(run->name, field)) {
			if (run->type & CONF_STRING)
				return (char *)run->value;
			consolef("[conf] Warning: Field %s at line %d is not a string"
						" and should be!\n", run->name, run->line);
		}
	}

	return NULL;

}

int conf_getint(struct conf_item *ci, char *field)
{

	struct conf_item *run;

	for (run = ci; run; run = run->next) {
		if (!strcasecmp(run->name, field)) {
			if (run->type & CONF_INT)
				return (int)run->value;
			consolef("[conf] Warning: Field %s at line %d is not an integer"
						" and should be!\n", run->name, run->line);
		}
	}

	return 0;

}

int conf_getbool(struct conf_item *ci, char *field)
{

	struct conf_item *run;

	for (run = ci; run; run = run->next) {
		if (!strcasecmp(run->name, field)) {
			if (run->type & CONF_BOOL)
				return (int)run->value;
			consolef("[conf] Warning: Field %s at line %d is not a boolean"
						" and should be!\n", run->name, run->line);
		}
	}

	return 0;

}

char *conf_getstring_from(struct conf_item *ci)
{

	if (ci->type & CONF_STRING)
		return (char *)ci->value;

	consolef("[conf] Warning: Field %s at line %d is not a string and should be!\n",
				ci->name, ci->line);
	return NULL;

}

int conf_getint_from(struct conf_item *ci)
{

	if (ci->type & CONF_INT)
		return (int)ci->value;

	consolef("[conf] Warning: Field %s at line %d is not an integer and should be!\n",
				ci->name, ci->line);
	return 0;

}

int conf_getbool_from(struct conf_item *ci)
{

	if (ci->type & CONF_BOOL)
		return (int)ci->value;

	consolef("[conf] Warning: Field %s at line %d is not a boolean and should be!\n",
				ci->name, ci->line);
	return -2;

}

// Finds the next named item starting from the conf_item given to it.
struct conf_item *conf_getblock_from(struct conf_item *ci)
{

	if (ci->type & CONF_BLOCK)
		return (struct conf_item *)ci->value;

	consolef("[conf] Warning: Field %s at line %d is not a block and should be!\n",
				ci->name, ci->line);

	return NULL;
}

int conf_getline(struct conf_item *ci, char *field)
{

	struct conf_item *run;

	for (run = ci; run; run = run->next) {
		if (!strcasecmp(run->name, field))
			return (int)run->line;
	}

	return 0;

}





//
// This function releases all variable potentially set by a load, so that we
// can HUP the daemon.
//
void conf_free(void)
{

	SAFE_FREE(server_bindif);

	SAFE_FREE(server_userfile);
	SAFE_FREE(server_msgpath);
	SAFE_FREE(server_wallpath);
	SAFE_FREE(server_quotafile);
	SAFE_FREE(server_dupedbfile);
	SAFE_FREE(server_logxfer);
	SAFE_FREE(server_loguser);
	SAFE_FREE(server_logadmin);
	SAFE_FREE(server_usechroot);
	SAFE_FREE(server_mkdirstring);

	SAFE_FREE(server_greeting);
	SAFE_FREE(server_email);
	SAFE_FREE(server_nobody);

	SAFE_FREE(server_extractmp3format);
	SAFE_FREE(server_defaulttagline);
	SAFE_FREE(server_groupfile);
	SAFE_FREE(server_grouppath);
	SAFE_FREE(server_nogroupname);

	SAFE_FREE(server_autoextract_diz);

	SAFE_FREE(server_tls_rsafile);

	SAFE_FREE(server_tls_ciphers);
	SAFE_FREE(server_egdsocket);

	// The pattern "nocapip"
	//check_free();

	// Clear out dirlist settings? Just release the children?
	// * dirlist_free, should release the children. Then reload the conf
	// * and call the functions, and re-start the children.

	section_free();

	// Release globalmatch
	// done in sections

	// Release filecheckers
	// done in check_free

	// Free sitecust
	sitecust_free();

}



