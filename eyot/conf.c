#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "lion.h"

#include "conf.h"
#include "irc.h"
#include "announce.h"
#include "udp.h"


//#define DEBUG_CONF

struct conf_item *conf_item_head = NULL;
char *conf_ircnick = NULL;
char *conf_ircname = NULL;
int conf_port = 0;
char *conf_passwd = NULL;
struct irc_server *conf_servers = NULL;
struct irc_channel *conf_chans = NULL;


void conf_print(struct conf_item *head);

void conf_init(char *configfile)
{

	struct conf_item *block, *cblock;
	struct irc_server *ts;
	struct irc_channel *tc;


	// Read the config file
	if (conf_load(configfile) == -1) {
		fprintf(stderr,"Bailing out!\n");
		exit(1);
	}



	// ******************************************************************
	// ******************************************************************
	// ******************************************************************
	// ******************************************************************

	// ******************************************************************
	// ******************************************************************
	// ******************************************************************
	// ******************************************************************


	// FTPSERVERS
	block = conf_item_head;
	while ((block = conf_getblock(block, "ftpserver"))) {

		// Takes
		// name, port, passwd

		char *servname, *servpass, *servip = NULL;
		int servport, remote_port = 0;
		struct conf_item *data = (struct conf_item *)block->value;

		// NAME
		if (!(servname = conf_getstring(data, "name"))) {
			fprintf(stderr,
					"Warning: ftpserver entry at line %d has no 'name' "
					"field, skipping.\n", data->line);
			continue;
		}

		// IP/HOSTNAME
		servip = conf_getstring(data, "ip");

		// PORT
		if (!(servport = conf_getint(data, "port"))) {
			fprintf(stderr,
					"Warning: ftpserver entry at line %d has no 'port' "
					"field, skipping.\n", data->line);
			continue;
		}

		// REMOTE_PORT
		remote_port = conf_getint(data, "remote_port");

		// PASSWD
		if (!(servpass = conf_getstring(data, "passwd"))) {
			fprintf(stderr,
					"Warning: ftpserver entry at line %d has no 'passwd' "
					"field, skipping.\n", data->line);
			continue;
		}


		udp_init(servname, lion_addr(servip), servport, servpass, remote_port);

	}


	// SERVERS
	block = conf_item_head;
	while ((block = conf_getblock(block, "ircserver"))) {

		char *servname, *servip, *servpass, *servmode, *servnick, *servuser,
			*servowner;
		int servport, servssl;
		struct conf_item *data = (struct conf_item *)block->value;


		// NAME
		if (!(servname = conf_getstring(data, "name"))) {
			fprintf(stderr, "Warning: server entry at line %d has no 'name' "
					"field, skipping.\n", data->line);
			continue;
		}

		// IP/HOSTNAME
		if (!(servip = conf_getstring(data, "ip"))) {
			fprintf(stderr, "Warning: server entry at line %d has no 'ip' "
						"field, skipping.\n", data->line);
			continue;
		}

		// PORT
		if (!(servport = conf_getint(data, "port"))) {
			fprintf(stderr, "Warning: server entry at line %d has no 'port' "
						"field, skipping.\n", data->line);
			continue;
		}

		// NICK
		if (!(servnick = conf_getstring(data, "nick"))) {
			fprintf(stderr, "Warning: server entry at line %d has no 'nick' "
					"field, skipping.\n", data->line);
			continue;
		}

		// USER
		if (!(servuser = conf_getstring(data, "user"))) {
			fprintf(stderr, "Warning: server entry at line %d has no 'user' "
					"field, skipping.\n", data->line);
			continue;
		}

		// OWNER
		if (!(servowner = conf_getstring(data, "owner"))) {
			fprintf(stderr, "Warning: server entry at line %d has no 'owner' "
					"field, skipping.\n", data->line);
			continue;
		}


		// PASS/MODE/USE SSL?
		servpass = conf_getstring(data, "passwd");
		servmode = conf_getstring(data, "mode");
		servssl = conf_getbool(data, "ssl");

		// ADD TO GLOBAL SERVER LIST
		if (!(ts = malloc(sizeof(struct irc_server)))) {
			perror("malloc:");
			exit(-1);
		}
		bzero(ts, sizeof(struct irc_server));

		ts->name = strdup(servname);
		ts->host = strdup(servip);
		ts->port = servport;
		ts->nick = strdup(servnick);
		ts->user = strdup(servuser);
		ts->owner = strdup(servowner);

		ts->passwd = (servpass && *servpass) ? strdup(servpass) : NULL;
		ts->mode = (servmode) ? strdup(servmode) : NULL;
		if (servssl)
			ts->flags |= IRC_FLAG_TLS;

		ts->next = conf_servers;
		conf_servers = ts;

		//	}

		// grab channel entries
		cblock = data;

		while ((cblock = conf_getblock(cblock, "channel"))) {

			char *channame, *chankey, *chaninvite, *chanfish,
				*ftpname, *announcename;
			struct conf_item *data = (struct conf_item *)cblock->value;

#if 0
			if (!(ircserv = conf_getstring(data, "ircserver"))) {
				fprintf(stderr, "Warning: channel entry at line %d has no 'name' "
						"field, skipping.\n", data->line);
				continue;
			}
#endif

			if (!(channame = conf_getstring(data, "name"))) {
				fprintf(stderr, "Warning: channel entry at line %d has no 'name' "
						"field, skipping.\n", data->line);
				continue;
			}

			if (!(ftpname = conf_getstring(data, "ftpserver"))) {
				fprintf(stderr,
						"Warning: channel entry at line %d has no 'ftpserver' "
						"field, skipping.\n", data->line);
				continue;
			}

			if (!(announcename = conf_getstring(data, "announce"))) {
				fprintf(stderr,
						"Warning: channel entry at line %d has no 'announce' "
						"field, skipping.\n", data->line);
				continue;
			}

			chankey = conf_getstring(data, "key");
			chaninvite = conf_getstring(data, "invite");
			chanfish = conf_getstring(data, "fish");

			// ADD TO GLOBAL CHANNEL LIST
			if (!(tc = malloc(sizeof(struct irc_channel)))) {
				perror("malloc:");
				exit(-1);
			}
			bzero(tc, sizeof(struct irc_channel));

			tc->name = strdup(channame);
			tc->ftpname = strdup(ftpname);
			tc->announce = strdup(announcename);

			tc->key = (chankey) ? strdup(chankey) : NULL;
			tc->invite = (chaninvite) ? strdup(chaninvite) : NULL;
			tc->fish = (chanfish) ? strdup(chanfish) : NULL;

			// find the irc server.
#if 0
			for (ts = conf_servers; ts; ts=ts->next) {

				if (!strcasecmp(ts->name, ircserv)) break;

			}

			if (!ts) {
				fprintf(stderr,
						"Warning: channel asks for ircserver '%s' at line %d which I can't find\n",
						ircserv, data->line);
				continue;
			}
#endif

			tc->ircserver = ts;

			tc->next = ts->channels;
			ts->channels = tc;

		} // channels


} // ircservers

	// ******************************************************************
	// ******************************************************************
	// ******************************************************************
	// ******************************************************************
	// ******************************************************************
	// ******************************************************************
	// ******************************************************************
	// ******************************************************************


	// ANNOUNCE
	block = conf_item_head;
	while ((block = conf_getblock(block, "announce"))) {

		char *section, *channel, *name;
		struct conf_item *data = (struct conf_item *)block->value;
		struct conf_item *an_block;

		// NAME
		if (!(name = conf_getstring(data, "name"))) {
			fprintf(stderr, "Warning: announce entry at line %d has no 'name' "
					"field, skipping.\n", data->line);
			continue;
		}

		// SECTION
		if (!(section = conf_getstring(data, "section"))) {
			fprintf(stderr, "Warning: announce entry at line %d has no 'section' "
						"field, skipping.\n", data->line);
			continue;
		}

		// CHANNEL
		if (!(channel = conf_getstring(data, "channel"))) {
			fprintf(stderr, "Warning: announce entry at line %d has no 'channel' "
						"field, skipping.\n", data->line);
			continue;
		}

		if (!(an_block = data)) {
			fprintf(stderr, "Warning: announce entry at line %d has no 'events' "
						"block, skipping.\n", data->line);
			continue;
		}



		// Add the announce block to the global list:
		an_add(section, channel, name);

		while ((an_block = conf_getblock(an_block, "event"))) {

			char *type, *fmt;
			struct conf_item *an_data = (struct conf_item *)an_block->value;

			// TYPE
			if (!(type = conf_getstring(an_data, "type"))) {
				fprintf(stderr, "Warning: event entry at line %d has no 'type' "
							"field, skipping.\n", an_data->line);
				continue;
			}

			// fmt
			if (!(fmt = conf_getstring(an_data, "format"))) {
				fprintf(stderr, "Warning: event entry at line %d has no 'format' "
							"field, skipping.\n", an_data->line);
				continue;
			}

			// Add this event:
			an_add_event(section, type, fmt, name);

		}

	}


	irc_init(conf_servers);
	conf_servers = NULL;

}



// Return 0 for no errors, non-zero for errors.
int conf_load(char *configfile)
{

	FILE *fp;
	int linecnt = 1;

	if (!(fp = fopen(configfile, "r"))) {
		fprintf(stderr,"[conf] CRITICAL: failed to load %s: %s\n",
						configfile, strerror(errno));
		return -1;
	}


	// Insert a dummy first node.
	conf_add(&conf_item_head, "", "", CONF_BLOCK, 999999);

	conf_item_head->next = conf_parseblock(&linecnt, fp);

	fclose(fp);

	if (!conf_item_head) {
		fprintf(stderr,"[conf] CRITICAL: empty config file %s\n", configfile);
		return -1;
	}


	return 0;
}


// Check mandatory fields
void conf_mandatory(void)
{

	//-----------------------------------------------
	// CHECK FOR MANDATORY FIELDS
	//if (!server_userfile) {
		//fprintf(stderr,"[conf] You must specify an 'userfile' file!\n");
		//exit(-1);
	//}

}


// Parses until it finds a "};", returns top of the list.
struct conf_item *conf_parseblock(int *linecnt, FILE *fp)
{

	char c, field[CONF_MAXFIELD], value[CONF_MAXVALUE];
	int fieldcnt, valuecnt;
	struct conf_item *top = NULL;
	FILE *saved_fd;

	saved_fd = NULL;

 keep_reading:
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

			// We need to check if it started with
			// #include
			// get the whole line
			strcpy(field, "include");
			fieldcnt = 0;

			// At the end of this, if value/valuecnt was set we want to
			// include another file.
			bzero(value, CONF_MAXVALUE);
			valuecnt = 0;

			while ((c = fgetc(fp)) != EOF) {

				if (c != field[fieldcnt++]) break;

				if (fieldcnt == strlen(field)) {

					// Found "#include" now look for quotes
					while ((c = fgetc(fp)) != EOF) {

						if (c == '\n')
							(*linecnt)++;

						if (c == '"') {

							while ((c = fgetc(fp)) != EOF) {

								if (c == '\n')
									(*linecnt)++;
								// END OF STRING
								if (c == '"')
									break;

								// OVERSIZED
								if (valuecnt >= CONF_MAXVALUE)
									break;

								value[valuecnt++] = c;

							} // while not '"'

							break;
						} // if '"'

					} // waiting for open '"'

				}

			}

			// Ignore the remainer of the line.
			while ((c = fgetc(fp)) != EOF) {
				if (c == '\n') {
					(*linecnt)++;
					break;
				}
			}

			if (valuecnt && *value) {
				printf("[conf] including '%s'\n", value);

				saved_fd = fp;
				fp = fopen(value, "r");
				if (!fp) {
					fp = saved_fd;
					saved_fd = NULL;
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

	if (saved_fd) {

		printf("[conf] finished included.\n");

		fclose(fp);

		fp = saved_fd;
		saved_fd = NULL;

		goto keep_reading;
	}



	return top;
}


void conf_add(struct conf_item **top, char *field, void *value, int type, int linecnt)
{

	struct conf_item *ci, *run;

#ifdef DEBUG_CONF
	if (type & CONF_STRING) {
		fprintf(stderr,"[conf] debug: conf_add(f=%s, v=%s, t=%d, l=%d)\n",
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
			if ((ci->value = malloc(strlen((char *)value))) == NULL) {
				perror("malloc");
				exit(-1);
			}
			ci->value = strdup((char *)value);
			break;

		case CONF_INT:
		case CONF_BOOL:
			if ((ci->value = malloc(sizeof(int))) == NULL) {
				perror("malloc");
				exit(-1);
			}
			ci->value = (int)value;
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
			fprintf(stderr,"[conf] Warning: Field %s at line %d is not a string"
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
			fprintf(stderr,"[conf] Warning: Field %s at line %d is not an integer"
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
			fprintf(stderr,"[conf] Warning: Field %s at line %d is not a boolean"
						" and should be!\n", run->name, run->line);
		}
	}

	return 0;

}

char *conf_getstring_from(struct conf_item *ci)
{

	if (ci->type & CONF_STRING)
		return (char *)ci->value;

	fprintf(stderr,"[conf] Warning: Field %s at line %d is not a string and should be!\n",
				ci->name, ci->line);
	return NULL;

}

int conf_getint_from(struct conf_item *ci)
{

	if (ci->type & CONF_INT)
		return (int)ci->value;

	fprintf(stderr,"[conf] Warning: Field %s at line %d is not an integer and should be!\n",
				ci->name, ci->line);
	return 0;

}

int conf_getbool_from(struct conf_item *ci)
{

	if (ci->type & CONF_BOOL)
		return (int)ci->value;

	fprintf(stderr,"[conf] Warning: Field %s at line %d is not a boolean and should be!\n",
				ci->name, ci->line);
	return -2;

}

struct conf_item *conf_getblock(struct conf_item *ci, char *name)
{

	struct conf_item *run;

	if (!ci || !ci->next)
		return NULL;
	//if (!ci) return NULL;

	for (run = ci->next; run; run = run->next) {

		if (run->type & CONF_BLOCK && !strcasecmp(run->name, name))
			return (struct conf_item *)run;
			//return (struct conf_item *)run->value;
	}

	return NULL;

}

// Finds the next named item starting from the conf_item given to it.
struct conf_item *conf_getblock_from(struct conf_item *ci)
{

	if (ci->type & CONF_BLOCK)
		return (struct conf_item *)ci->value;

	fprintf(stderr,"[conf] Warning: Field %s at line %d is not a block and should be!\n",
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



