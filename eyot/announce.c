#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#include "lion.h"
#include "misc.h"
#include "irc.h"
#include "announce.h"
#include "parser.h"
#include "arguments.h"



struct an_announce *an_announces = NULL;


// Add a new announcement to the global announces
struct an_announce *an_add(char *section, char *channel, char *name)
{

	struct an_announce *an;

	if (!(an = malloc(sizeof(*an)))) {
		perror("malloc");
		exit(-1);
	}
	bzero(an, sizeof(*an));

	an->section = strdup(section);
	an->name = strdup(name);

	an->channel = (channel) ? strdup(channel) : NULL;

	an->next = an_announces;
	an_announces = an;

	return an;

}

struct an_announce *an_find(char *section)
{

	struct an_announce *runner, *ret = NULL;

	for (runner = an_announces; runner; runner = runner->next) {

		if (!strcasecmp(runner->section, section))
			return runner;

		if (!ret && !strcasecmp(runner->section, "DEFAULT"))
			ret = runner;

	}

	// Will return DEFAULT announce structure if it exists.
	return ret;

}

struct an_event *an_add_event(char *section, char *type, char *format, char *name)
{

	struct an_announce *an;
	struct an_event *ev;

	if (!(an = an_find(section))) {
#ifdef DEBUG
		fprintf(stderr, "[an_add_event:%s:%d] Could not find section '%s'\n",
					__FILE__, __LINE__, section);
#endif
		return NULL;
	}

	if (!(ev = malloc(sizeof(*ev)))) {
		perror("malloc");
		exit(-1);
	}
	bzero(ev, sizeof(*ev));

	ev->type = strdup(type);
	ev->fmt = strdup(format);

	ev->next = an->events;
	an->events = ev;
	return ev;

}


struct an_event *an_find_event(char *section, char *type)
{

	struct an_announce *an;
	struct an_event *runner;

	if (!(an = an_find(section)))
		return NULL;

	for (runner = an->events; runner; runner = runner->next) {

		if (!strcasecmp(runner->type, type))
			return runner;

	}

	return NULL;

}



//
// search ONLY in the list for this channel.
//
// look for section first, then event, if either fail,
// look for default, then event.
//
struct an_event *an_find_event2(irc_channel_t *ic, char *section, char *type)
{

	struct an_announce *an;
	struct an_event *runner;
	int i;

	for (i = 0; i < ic->num_announce; i++) {

		an = (struct an_announce *)ic->announces[i];

		if (!strcasecmp(section, an->section)) {


			for (runner = an->events;
				 runner;
				 runner = runner->next) {

				if (!strcasecmp(runner->type, type))
					return runner;

			} // events

			return NULL; // Found section, but not event, fail. (don't loop)
		}

	}

	return NULL; // Not found section

}


struct userdata_struct {
    int finished;
    char *url;
    float rating;
    char *votes;
    char *genres;
    char *screens;
    char *host;
};

int imdb_handler( connection_t *handle,
                  void *user_data, int status, int size, char *line)
{
	struct userdata_struct *data = (struct userdata_struct *) user_data;

	switch(status) {

	case LION_CONNECTION_LOST:
		if (arguments_verbose)
			printf("[imdb] connection failed: %d:%s\n", size, line);

		/* fall-through */
	case LION_CONNECTION_CLOSED:
		if (arguments_verbose)
			printf("[imdb] connection closed.\n");

        if (data)
            data->finished = 1;
		break;

	case LION_CONNECTION_CONNECTED:
		if (arguments_verbose)
			printf("[imdb] connected\n");
        if (data) {

            lion_printf(handle, "GET %smaindetails HTTP/1.0\r\n"
                        "SERVER: %s\r\n"
                        "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.7; rv:8.0.1) Gecko/20111121 Firefox/8.0.1 SeaMonkey/2.5\r\n"
                        "\r\n",
                        data->url,
                        data->host);

            if (arguments_verbose)
                printf("GET %s HTTP/1.0\n", data->url);
        }
		break;

	case LION_INPUT:
        // 728 IMDb users have given an average vote of 7.3/10
        // /Sections/Genres/*
        // We should use IMDB API.
        if (line && data) {
            float rate;
            char *r, *p, *tmp;

            //if (arguments_verbose)
            //  printf(" :: %s\n", line);

            if ((r = strstr(line, " IMDb users have given an average vote of "))) {
                char nums[20];
                r--; // eeewwww
                while ((r >= line) && (isdigit(*r) || (*r == ','))) r--; //eeww
                r++;

                if (sscanf(r, "%20[0123456789,] IMDb users have given an average vote of %f",
                           (char *)&nums, &rate) == 2) {
                    if (arguments_verbose)
                        printf("matched on vote line: %s: '%s'\n\n", line, nums);
                    data->votes = strdup(nums);
                    data->rating = rate;
                }
            }

            while ((r = strstr(line, "\"/genre/"))) {

                if (arguments_verbose)
                    printf("matched on genre line: %s\n\n", r);

                // Change "genre" to something else, so we dont match it again
                r[3] = 'a';

                p = &r[8];
                r = p;
                while (isalpha(*r) || (*r == '-')) r++;
                *r = 0;

                line = &r[1]; // set line so we can iterate.

                //printf("Checking genre '%s' in '%s' \n", p, data->genres);

                if (!data->genres || !strstr(data->genres, p)) {
                    if (!data->genres) {
                        data->genres = strdup(p);
                    } else {
                        tmp = misc_strjoin(data->genres, p);
                        SAFE_FREE(data->genres);
                        data->genres = tmp;
                    }
                }
            } // genre
        } // line
        break;
    }
    return 0;
}




void announce_imdb(char **keys, char **values, int *items)
{
    static char extra_line[1024]; // Eww. static storage!
    char *url;
    char *r, *p = NULL;
    int port;
    struct userdata_struct userdata;
    lion_t *socket;

    if (arguments_verbose)
        printf("[imdb] attempting imdb... \n");

    url = parser_findkey(keys, values, *items, "url");
    if (!url || !*url) return;

    // url = http://www.imdb.com/title/tt1753549/

    if (strncasecmp("http://", url, 7)) return;

    r = strchr(&url[7], '/');
    if (!r) return;

    // Stop string at end of hostname.
    *r = 0;

    // Handle optional port
    port = 80;
    if ((p = strchr(&url[7], ':'))) {
        *p = 0;
        port = atoi(&p[1]);
    }

    if (arguments_verbose)
        printf("[imdb] connecting to %s:%d ... \n", &url[7], port);
    userdata.host = strdup(&url[7]);
    socket = lion_connect(&url[7], port,
                          0, 0, LION_FLAG_FULFILL, (void *)&userdata);

    // Restore the null terminated things.
    *r = '/';
    if (p) *p = ':';

    if (!socket) return;

    userdata.finished = 0;
    userdata.rating = 0.0;
    userdata.votes = NULL;
    userdata.genres = NULL;
    userdata.url = r; // Starting from "/", don't free.

    lion_set_handler(socket, imdb_handler);

    // loop until complete.
	while( !userdata.finished) {
		lion_poll(0, 1);
    }

    snprintf(extra_line, sizeof(extra_line),
             "RATING|%2.1f|GENRES|%s|VOTES|%s|",
             userdata.rating,
             userdata.genres  ? userdata.genres  : "",
             userdata.votes ? userdata.votes : "");
    SAFE_FREE(userdata.genres);
    SAFE_FREE(userdata.host);
    SAFE_FREE(userdata.votes);

    r = extra_line;

    while (r && *r) {
        keys[*items] = r;
        r = strchr(r, '|');
        if (!r) break;
        *r = 0;
        r++;
        values[*items] = r;
        r = strchr(r, '|');
        if (!r) break;
        *r = 0;
        r++;

        *items+=1;
    };

}




// Default action for stuff from the bot, passed into by parser.c
char *an_format(char **keys, char **values, int items, irc_channel_t *ic)
{

	static char an_format[512];
	char field[32];
	struct an_event *ev;
	char *section, *type, *value, *p;
	int i, j;

	// Wipe out any old cruft.
	bzero(an_format, 512);
	bzero(field, 32);

	// Extract the section value or use DEFAULT
	section = parser_findkey(keys, values, items, "section");


	// Extract the event type value or return nothing
	if (!(type = parser_findkey(keys, values, items, "type")))
		return NULL;

    // For some types, say IMDB, we actuually want to expand it a little and
    // add a few more.
    if (!strcasecmp("IMDB", type))
        announce_imdb(keys, values, &items);


	// Find the event within the section
	// look in the list of announces for the channel, try to match on
	// section, and if not, go for default.

	// Try to find based on "section" and if not, try DEFAULT
	if (!section || !(ev = an_find_event2(ic, section, type))) {

		if (!(ev = an_find_event2(ic, "DEFAULT", type))) {

			// Couldn't find a format...
			return NULL;

		}
	}

	// Build a string with the correct fields substituted
	for (p = ev->fmt, i = 0; *p; p++) {

		// Grab a string...
		if (*p == '%') {
			p++;

			// Just add a single % if there's %%
			if (*p == '%') {
				an_format[i++] = *p;
				an_format[i++] = *p;
				continue;
			}

			j=0;
			i+=2; // For the start/end %'s
			while (*p && *p != '%' && ((*p >= 65 && *p <= 90) || (*p >= 97 && *p <= 122)))
				field[j++] = *p++;
			field[j] = 0;
			//printf("DEBUG: found field %s\n", field);


			if ((value = parser_findkey(keys, values, items, field))) {
				//printf("DEBUG: found value %s\n", value);
				strncat(an_format, value, 512);
				i = strlen(an_format); // Reset i
			} else {
				// If we don't find the value
				i = strlen(an_format); // Reset i
			}

			// Add any spaces from the above while loop
			if (*p != '%' && ((*p < 65 || *p > 90) || (*p < 97 || *p > 122)))
				an_format[i++] = *p;

		} else {

			an_format[i++] = *p;

		}

	}

	return an_format;

}




void an_assigntochannel(irc_channel_t *ic)
{
	struct an_announce *runner;

	// For all announces
	for (runner = an_announces; runner; runner = runner->next) {

		// Check announce name vs channel's desired announce
		if (!strcasecmp(ic->announce, runner->name)) {

			// Add this announce to irc channel list.
			// Add another to the list.
			ic->announces = realloc(ic->announces,
									sizeof(runner) *
                                    (ic->num_announce + 1));

			if (!ic->announces) {
				perror("malloc: ");
				exit(1);
			}

			ic->announces[ ic->num_announce ] = (void *)runner;
			ic->num_announce++;

			//printf("[announce] connecting announce '%s' to channel '%s'\n",
			//	   runner->name,
			//	   ic->name);

		}

	}

	if (!ic->num_announce) {

		printf("[announce] warning - channel '%s' has no announce sections.\n",
			   ic->name);

	}


}

