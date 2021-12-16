#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WIN32
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>

#include "lion.h"

#include "global.h"
#include "data.h"
#include "socket.h"
#include "log.h"
#include "misc.h"
#include "login.h"
#include "sitecust.h"
#include "groups.h"
#include "../lion/src/misc.h"
//#include "lion/misc.h"
#include "user.h"

#include "file.h"
#include "section.h"


struct sitecust *site_cust_head = NULL;





static struct sitecust_exec *sitecust_exec_new(void)
{
	static struct sitecust_exec *exec;

	exec = (struct sitecust_exec *) malloc(sizeof(*exec));

	if (!exec) return NULL;

	memset(exec, 0, sizeof(*exec));

	return exec;
}


static void sitecust_exec_free(struct sitecust_exec *exec)
{

	// Safety
	if (!exec) return;

	// Clear login_nodes reference to us.
	if (exec->t)
		exec->t->sitecust = NULL;

	exec->t = NULL;
	exec->sc = NULL;
	exec->handle = NULL;

	SAFE_FREE(exec);

}






sitecust_arg_t sitecust_typestr2type(char *str)
{

	if (!mystrccmp("DIR", str))
		return SITECUST_ARG_DIR ;
	else if (!mystrccmp("FILE", str))
		return SITECUST_ARG_FILE ;
	else if (!mystrccmp("DIRORFILE", str))
		return SITECUST_ARG_DIRORFILE ;
	else if (!mystrccmp("SECTION", str))
		return SITECUST_ARG_SECTION ;
	else if (!mystrccmp("USER", str))
		return SITECUST_ARG_USER ;
	else if (!mystrccmp("GROUP", str))
		return SITECUST_ARG_GROUP ;
	else if (!mystrccmp("INTEGER", str))
		return SITECUST_ARG_INTEGER ;

	// *, any, string etc
	return SITECUST_ARG_ANY;

}


char *sitecust_typetotypestr(sitecust_arg_t type)
{

	switch(type) {
	case SITECUST_ARG_DIR:
		return "directory";
	case SITECUST_ARG_FILE:
		return "file";
	case SITECUST_ARG_DIRORFILE:
		return "directory or file";
	case SITECUST_ARG_SECTION:
		return "section";
	case SITECUST_ARG_USER:
		return "user";
	case SITECUST_ARG_GROUP:
		return "group";
	case SITECUST_ARG_INTEGER:
		return "integer";
	case SITECUST_ARG_STRING:
		return "string";
	default:
		break;
	};

	return "any";

}



void sitecust_add(char *name, char *type, char *exec, char *desc, int flags,
				  char *args)
{
	struct sitecust *sc;

	if (!(sc = (struct sitecust *) malloc(sizeof(struct sitecust)))) {
		perror("Failed to get memory for a new entry");
		exit(-1);
	}
	memset(sc, 0, sizeof(struct sitecust));

	sc->name = name;
	sc->exec = exec;
	sc->desc = desc;
	sc->flags = flags;

	if (strcasecmp(type, "TEXT")==0) {
		sc->type = SC_TEXT;
	} else if (strcasecmp(type, "EXEC")==0) {
		sc->type = SC_EXEC;
	} else {
		free(sc->name);
		free(sc->exec);
		free(sc->desc);
		free(sc);
		return;
	}

	sc->next = site_cust_head;
	site_cust_head = sc;


	// Setup the argument stuff as well. The strings should be like:
	// "2" (only 2 args)
	// "2+" (2 or more arguments)
	// "2-" (2 or less)
	// "2,type1,type2" (2, of specific type)
	// "3+,type1,type2" (3 or more, where 1 and 2 are specific type)
	if (args && *args) {
		int num;
		char *token;

		// Attempt to read out the number of arguments.
		num = strtol(args, &args, 10);

		if ((num <= 0) ||
			(num > 100)) // just some rediculous maximum
			return;

		sc->args = num;

		// Check if it is exact, less or more.
		sc->exact = 0;

		if (*args == '+')
			sc->exact = 1;
		else if (*args == '-')
			sc->exact = -1;

		// Skip the + or -
		if (sc->exact)
			args++;



		// Set up the type array...
		while((token = misc_digtoken(&args, " ,\r\n"))) {

			// Make room for another type..
			sc->num_types++;

			sc->types = (sitecust_arg_t *)
				realloc(sc->types, sizeof(sitecust_arg_t *) * sc->num_types);

			if (!sc->types) {
				sc->num_types = 0;
				return;
			}

			// Match type-string to type
			sc->types[ sc->num_types-1 ] = sitecust_typestr2type(token);

			//			consolef("adding type '%s' %d\n", token, sc->types[sc->num_types-1]);


		} // while token


	}

}

void sitecust_free(void)
{
	struct sitecust *runner, *next;

	for (runner = site_cust_head; runner; runner = next) {
		next = runner->next;
		free(runner->name);
		free(runner->exec);
		free(runner->desc);
		free(runner);
	}

	// We should also NULL the start of the list, hmm?
	site_cust_head = NULL;

}

void sitecust_help(struct login_node *user)
{

	struct sitecust *sc;

	if (site_cust_head) {
		socket_print(user,
			"|-[ EXTRA SITE COMMANDS ]------------------------------------------------------|\r\n");
	}

	for (sc = site_cust_head; sc; sc=sc->next)
		socket_print(user, "| %-24s - %-49s |\r\n", sc->name, sc->desc);

	return;

}


struct sitecust *sitecust_find(char *name)
{
	struct sitecust *sc;

	for (sc = site_cust_head; sc; sc=sc->next) {
		if (strcasecmp(name, sc->name) == 0)
			return sc;
	}

	return NULL;
}

int sitecust_site_do(struct login_node *user, char *args)
{
	char *ar = args, *name, *rest;
	struct sitecust *sc;

	if (!(name = digtoken((char **)&ar, " ")))
		return -1;

	rest = ar;

	// Check they aren't already running a sitecust (clients should not let
	// you do this, but someone might be out to hurt us.
	if (user->sitecust) {
		lion_printf(user->handle, "500 SITECUST command already active.\r\n");
		return 0;
	}




	// Find command
	if (!(sc = sitecust_find(name)))
		return -1;

	// Check FLAGS
	if ((sc->flags) && !((user->level|server_defaultflags) & sc->flags)) {
		socket_print(user, "500 Permission denied for %s\r\n", sc->name);
		return 0;
	}


	consolef("[sitecust] Executing command %s args=%s\n", name, rest);

	if (sc->type == SC_TEXT) {

		sitecust_sendfile(user, sc, sc->exec);

	} else if (sc->type == SC_EXEC) {

		sitecust_sendexec(user, sc, rest, sc->name);

	}

	return 0;

}


void sitecust_sendfile(struct login_node *t, struct sitecust *sc, char *file)
{
	struct sitecust_exec *exec;

	exec = sitecust_exec_new();
	if (!exec) {
		return;
	}

	exec->sc = sc;
	exec->t = t;
	t->sitecust = exec;

	exec->handle=lion_open(file, O_RDONLY, 0600, LION_FLAG_FULFILL,
						 (void *)exec);

	lion_set_handler(exec->handle, sitecust_send_handler);

}







//
//  0 is failed
// !0 good.
//
int sitecust_checkargtype(struct login_node *t, struct sitecust *sc,
						  int argc, char **argv)
{
	int i;
	struct section_node *section;
	char *buf = genbuf; // buffer area to write new strings to, if needed.
	char *rep;
	int end = sizeof(genbuf);
	struct stat st;
	long pint;

	for (i = 1; i < argc; i++) { // skip the first, as its exe name.

		if (i > sc->num_types) break; // if we've parsed al lthe types...



		if (end <= 10) {

			lion_printf(t->handle,
						"500 Out of memory processing arguments\r\n");
			return 0;
		}


		switch(sc->types[ i - 1 ]) {


		case SITECUST_ARG_DIR:
			//			consolef("[sitecust] checking that '%s' is a directory\n",
			//	 argv[i]);

			// build path, using absolutes, or cwd, then reduce it, then
			// check for permissions.
			if (*argv[i] == '/')
				snprintf(buf, end, "%s/", argv[i]);
			else
				snprintf(buf, end, "%s/%s/", t->cwd, argv[i]);

			file_undot(buf);

			if (!file_isdir(t, buf))
				goto fail;

			// It's good!
			argv[i] = buf;

			end -= strlen(buf) + 1;  // this is how much space there is left.
			buf += strlen(buf) + 1;  // skip past this string.

			break;


		case SITECUST_ARG_FILE:
			//consolef("[sitecust] checking that '%s' is a file\n",
			//	 argv[i]);

			buf++; // want room for a slash.
			end--;

			snprintf(buf, end, "%s", argv[i]);

			argv[i] = buf; // leave room for a "/"

			// build path, using absolutes, or cwd, then reduce it, then
			// check for permissions.
			if (!file_pathchecklogin(t, buf))
				goto fail;

			if (stat(buf, &st))
				goto fail;

			if (!(st.st_mode & S_IFREG))
				goto fail;

			// pathcheck gives us full on-disk path, but we want
			// it from ftp root POV.

			end -= strlen(buf) + 1;  // this is how much space there is left.
			buf += strlen(buf) + 1;  // skip past this string.

			argv[i] = hide_path(argv[i]);
			argv[i]--; // we can do this as we did ++ above.
			*argv[i] = '/';

			break;


		case SITECUST_ARG_DIRORFILE:
			//consolef("[sitecust] checking that '%s' is a directory or file\n",
			//	 argv[i]);

			buf++; // want room for a slash.
			end--;

			snprintf(buf, end, "%s", argv[i]);

			argv[i] = buf; // leave room for a "/"

			// build path, using absolutes, or cwd, then reduce it, then
			// check for permissions.
			if (!file_pathchecklogin(t, buf))
				goto fail;

			if (stat(buf, &st))
				goto fail;

			if (!(st.st_mode & S_IFREG) &&
				!(st.st_mode & S_IFDIR))
				goto fail;

			// pathcheck gives us full on-disk path, but we want
			// it from ftp root POV.

			end -= strlen(buf) + 1;  // this is how much space there is left.
			buf += strlen(buf) + 1;  // skip past this string.

			argv[i] = hide_path(argv[i]);
			argv[i]--; // we can do this as we did ++ above.
			*argv[i] = '/';

			break;



		case SITECUST_ARG_SECTION:
			//consolef("[sitecust] checking that '%s' is a section\n",
			//		 argv[i]);
			section = section_find(argv[i]);

			// If we couldn't find a section, or it is the default one..
			if (!section ||
				!section->path ||
				!(strcmp(section->path, "/")))
				goto fail;

			argv[i] = section->path;

			break;


		case SITECUST_ARG_USER:
			//consolef("[sitecust] checking that '%s' is a user\n",
			//		 argv[i]);

			if (user_getuidbyname(argv[i]) == -1)
				goto fail;

			break;


		case SITECUST_ARG_GROUP:
			//consolef("[sitecust] checking that '%s' is a group\n",
			//		 argv[i]);

			if (!group_find_byname(argv[i]))
				goto fail;

			break;


		case SITECUST_ARG_INTEGER:
			//consolef("[sitecust] checking that '%s' is an integer\n",
			//		 argv[i]);

			pint = strtol(argv[i], &rep, 10);
			if ((pint == LONG_MAX) || (pint == LONG_MIN) ||
				(*rep))
				goto fail;

			break;


		default:
			break;

		} // switch type

	} // for

	return 1;

 fail:
	lion_printf(t->handle, "500 Argument %d must be of type '%s'\r\n",
				i, sitecust_typetotypestr(sc->types[ i - 1 ]));
	return 0;
}












#define MAX_ARG 100
#define MAX_ENV 100


void sitecust_sendexec(struct login_node *t, struct sitecust *sc,
					   char *args,char *name)
{
	char *argv[MAX_ARG];
	int argc;
	char *envp[MAX_ENV];
	char *ar, *arg;
	char env[8192];
	struct sitecust_exec *exec;
	char *cop;

	consolef("[sitecust] '%s' for '%s %s'\n", t->user, sc->exec, args);

	cop = strdup(args);
	//ar = args;  // Modifies args!!
	ar = cop;  // Modifies args!!
	argv[0] = sc->exec;
	argc = 1;

	// Sucks we have to duplicate the code here, already in lion.
	while ((arg = misc_digtoken(&ar, " \t\r\n")) && (argc < MAX_ARG-1)) {
		argv[ argc++ ] = arg;
		consolef("  arg '%s'\n", arg);
	}

	//consolef("setting %d to NULL\n", argc);
	argv[argc] = NULL;



	// Verify we have the right number of arguments.
	if ((!sc->exact && (sc->args != (argc - 1)))  ||
		(sc->exact  && ((argc - 1) * sc->exact < (sc->args * sc->exact)))) {

		lion_printf(t->handle,
					"500 Wrong number of arguments for %s (%u != %u%s)\r\n",
					sc->name, argc - 1, sc->args,
					(sc->exact == 0) ? "" :
					(sc->exact > 0) ? "+" : "-");

		SAFE_FREE(cop);
		return;

	}

	// Phew. Check types now.
	// For each argument we have passed, check the type it "should" be
	// then attempt to parse it as that.
	if (!sitecust_checkargtype(t, sc, argc, argv)) {
		SAFE_FREE(cop);
		return;
	}


	// This next code ASSUMES there are no "," in the variables set.
	// Ugh!
	snprintf(env, sizeof(env),
			 "L_USER=%s,L_UID=%lu,L_GROUP=%s,L_GID=%lu,L_CWD=%s,L_ROOT=%s,L_CMD=%s,L_EXE=%s",
			 t->user, (unsigned long)user_getuid(t),
			 (t->current_group) ? ((struct group *)t->current_group)->name :
			 server_nogroupname,
			 (unsigned long) 1234, // uhmm group id?
			 t->cwd,
			 server_usechroot,
			 name,
			 "./lundftpd");


	ar = env;
	argc = 0;
	while ((arg = misc_digtoken(&ar, ",\r\n")) && (argc < MAX_ENV-1)) {
		envp[ argc++ ] = arg;
	}
	envp[argc] = NULL;


	// Don't proxy stderr, that way scripts can log to lundftpd logs too.

	exec = sitecust_exec_new();
	if (!exec) {
		SAFE_FREE(cop);
		return;
	}

	exec->sc = sc;
	exec->t = t;
	t->sitecust = exec;

	exec->handle = lion_execve(argv[0], argv, envp, 0,
							   LION_FLAG_FULFILL, (void *)exec);

	lion_set_handler(exec->handle, sitecust_send_handler);
	SAFE_FREE(cop);
}

int sitecust_send_handler(lion_t *handle, void *user_data, int status, int size, char *line)
{
	struct sitecust_exec *exec = (struct sitecust_exec *)user_data;
	struct login_node *t;

	if (!exec) return 0;

	t = exec->t;

	switch (status) {

		case LION_PIPE_FAILED:
		case LION_FILE_FAILED:
			consolef("[sitecust] failed to open file/pipe: %s\r\n", line);

			if (t && t->handle)
				lion_printf(t->handle,
							"500 Failed to run command, sorry! Contact a siteop.\r\n");

			// Important we clear the reference to handle
			if (exec)
				sitecust_exec_free(exec);

			break;

		case LION_PIPE_EXIT:
		case LION_FILE_CLOSED:
			consolef("[sitecust] file/pipe closed\r\n");

			if (t && t->handle)
				lion_printf(t->handle, "200 [END]\r\n");

			if (exec)
				sitecust_exec_free(exec);
			break;

		case LION_PIPE_RUNNING:
		case LION_FILE_OPEN:
			consolef("[sitecust] file/pipe opened\r\n");

			if (t && t->handle)
				lion_printf(t->handle, "200-[ BEGIN ]\r\n");
			break;

		// Send file data to the user
			// I guess technically you want to make sure no line starts with
			// "200 " :)
        case LION_BINARY:
			//lion_output(t->handle, line, size);
			break;

        case LION_INPUT:
			//consolef("sending data %p:%p %d\n", t, t->handle, exec->lines);
			// I recommend using " %s\r\n" - the first space ensures you
			// could never get "220 " and removes need for parsing.
			if (t && t->handle)
				lion_printf(t->handle, " %s\r\n", line);

#ifdef SITECUST_MAX_LINES
			if (exec->lines++ > SITECUST_MAX_LINES) {

				if (t && t->handle)
					lion_printf(t->handle,
								"200-Max lines (%d) sent, terminating external process\r\n",
								SITECUST_MAX_LINES);

				if (exec)
					lion_close(exec->handle);

			}
#endif

			break;

			// Unless you want lion's buffer to just grow, you should do
			// the flow control thang.
			// Problem is this, you send output on login_node's handle, so
			// the buffering events are sent to IT. When you get that
			// event you need to disable_read on the reader, ie, the
			// site_cust handle. Which in this case you don't save.
			//
			// Two options. Save the sitecust handle in login_node (there is
			// room) and change login_node's handler to this one, then you
			// know to pause it here. BUT, there are repercussions with
			// changing handler! So, don't do it. Add handle to login_node and
			// add code to login_handler in login, to, if there is the buffer
			// events, and sitecust handler is set, pause it.
			//
			// But we may need flow control with login node in future so
			// calling it althandle.
			//
			// This is where lion_tie() would be nice.
#if 0
	case LION_BUFFER_USED:
		break;

	case LION_BUFFER_EMPTY:
		break;
#endif

        default:
			; // MS has to have a ";" here.

    }

	return 0;
}





//
// login_node flow control call. Pause reading.
//
void sitecust_disable_read(struct sitecust_exec *sc)
{

	lion_disable_read(sc->handle);

}



//
// login_node flow control call. Continue reading.
//
void sitecust_enable_read(struct sitecust_exec *sc)
{

	lion_enable_read(sc->handle);

}



//
// Our login node logged out, so we should clear our references so we
// dont call us with data for the login. Also close the sitecust, and it
// can detect and quit (or ignore and carry on).
//
void sitecust_exit(struct sitecust_exec *sc)
{

	// No more references to login node.
	sc->t = NULL;

	// Close child
	lion_close(sc->handle);

}
