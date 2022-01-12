#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/socket.h>
#endif
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>

#include <errno.h>

#include "global.h"
#include "misc.h"
#include "io.h"
#include "socket.h"
#include "login.h"
#include "data.h"
#include "exec.h"
#include "file.h"
#include "check.h"
#include "quota.h"
#include "log.h"
#include "ls.h"
#include "colour.h"
#include "sfv.h"
#include "section.h"
#include "race.h"


#include "lion.h"
#include "dirlist.h"



__RCSID("$LundFTPD: data.c,v 1.28 2003/04/23 08:32:03 lundman Exp $");



/*
 * data status works like this.
 *
 * When data is connected, it gets ST_ACTIVE set (data_connected is called)
 * If data_list is called, its type gets set (DATA_LIST etc) data_connected()
 * is called. If port mode, we also initiate connect() here.
 *
 * In data_connected, if both type, and Active, we act on the command.
 *
 * If data has sent 150, we set ST_SEND_226. That way data_close() can
 * send 226 if required.
 *
 */


int data_global_rate_cap_group = 0;


void data_connected(struct data_node *d);
int data_handler( lion_t *handle,
				  void *user_data, int status, int size, char *line);

//
// This is the lion handler for all file reading, socket sending. list & retr
//
int data_file_read_handler( lion_t *handle,
							void *user_data, int status, int size, char *line)
{
	struct data_node *d = (struct data_node *) user_data;

	switch( status ) {

	case LION_FILE_OPEN:
		consolef("[data] %p/%p file open.\n", handle, user_data);

		if (!d) return 0;

		// Switch to binary, chunked mode.
		lion_setbinary( handle );

		// If any caps are in effect, set those now.
		// reading from file, writing to socket, its rate_out
		data_set_rate_out( d );


		// Ok, if we need to sync in SSL, we had better pause file from
		// starting, or it will send in the middle of it (quite fun really).
		if (d->login && (d->login->status & ST_DATA_SECURE) &&
			!lion_ssl_enabled( d->handle ))
			lion_disable_read( handle );


		//		consolef("is connected %d\n", lion_isconnected( d->handle ));

		// Send the 150 now, since the file is open and we are ready to go
		// Ok sometimes this sends CCSN on plain ftp! FIXME
		if (d && d->login && d->login->handle) {

			if (d->bytes)
				lion_printf(d->login->handle,
		 "150 Opening %s%s mode data connection for %s (%"PRIu64" bytes).\r\n",
							(d->mode&MODE_TYPE_BINARY?"BINARY":"ASCII"),
							(d->mode&MODE_TYPE_CCSN)?"+CCSN":
							(d->mode&MODE_TYPE_SSCN)?"+SSCN":"",
							(d->type & DATA_LIST) ? "/bin/ls" :
							path2file(d->name),
							d->bytes);
			else

				lion_printf(d->login->handle,
		  "150 Opening %s%s mode data connection for %s.\r\n",
							(d->mode&MODE_TYPE_BINARY?"BINARY":"ASCII"),
							(d->mode&MODE_TYPE_CCSN)?"+CCSN":
							(d->mode&MODE_TYPE_SSCN)?"+SSCN":"",
							(d->type & DATA_LIST) ? "/bin/ls" :
							path2file(d->name));

			d->status |= ST_SEND_226;


			// Used to be the case that we zero the bytes counter (that
			// was the file size) so we could count how many bytes have been
			// transferred. But we now let lion do that (see get_bytes()).
			// so lets keep this so we can compute %-ages in site who
			//d->bytes = 0;

			// We should never have a read from socket, but we need it on
			// to detect connection lost etc
			lion_enable_read( d->handle );

		}


		break;






	case LION_FILE_CLOSED:

		consolef("[data] %p/%p file finished.\n", handle, user_data);
		if (d) {
			d->althandle = NULL;
			// Also close the connection.
 			lion_close( d->handle );
			//data_close(d);
		}
		break;

	case LION_FILE_FAILED:
		consolef("[data] %p/%p file lost? %d:%s %d\n", handle, user_data,
				size, line,
				 lion_fileno(handle));

		if (d) {

			d->althandle = NULL;

			if (d && d->login && d->login->handle) {
				lion_printf(d->login->handle,
							"500 TRANSFER FAILED: %d:%s\r\n",
							size, line);
				d->status &= ~ST_SEND_226;
			}

			lion_close( d->handle ); // or disconnect?
		}
		break;

	case LION_BINARY:
		if (d && d->handle) {

			//			if (d->type & DATA_LIST)
			//	consolef("[data] %p sending %d.\n", d->handle, size);
			//			printf("in%d ", size); fflush(stdout);

			// If we saw data, update idle
			if (d->login) d->login->idle = lion_global_time;

			// Update idle counter.
			d->time = lion_global_time;

			lion_output(d->handle, line, size);
			// Please be aware that "d" can be released after this call
			// to lion_output. Add no code here.


		}
		break;

	default:
		consolef("[data] unhandled event in data_file_read_handler %d\n",
				 status);

	}


	return 0;
}





//
// This is the lion handler for all file writing, STOR and APPE.
//
int data_file_write_handler( lion_t *handle,
							void *user_data, int status, int size, char *line)
{
	struct data_node *d = (struct data_node *) user_data;

	switch( status ) {

	case LION_FILE_OPEN:
		consolef("[data] %p/%p file open.\n", handle, user_data);

		if (!d) return 0;

#ifdef RACESTATS
		/* Start a new file off. */
		{
			char *filename;
			filename = pathsplit(d->name);
			race_file_start(d->name, filename);
			pathfixsplit(d->name, filename);
		}
#endif

		// Switch to binary, chunked mode.
		lion_setbinary( handle );

		// disable read, so it doesn't try to read, and say it is at EOF
		lion_disable_read( handle );

		// And enable reading from the network socket now that we are ready.
		lion_enable_read( d->handle );

		// Change socket into binary
		//lion_setbinary( d->handle );
		// ALL data nodes are now binary!!


		// If any caps are in effect, set those now.
		// writing to file, reading from socket, its rate_in
		data_set_rate_in( d );



		//		consolef("is connected %d\n", lion_isconnected( d->handle ));

		// Send the 150 now, since the file is open and we are ready to go
		if (d && d->login && d->login->handle) {

			if (d->bytes)
				lion_printf(d->login->handle,
		 "150 Opening %s%s mode data connection for %s (%"PRIu64" bytes).\r\n",
							(d->mode&MODE_TYPE_BINARY?"BINARY":"ASCII"),
							(d->mode&MODE_TYPE_CCSN)?"+CCSN":
							(d->mode&MODE_TYPE_SSCN)?"+SSCN":"",
							(d->type & DATA_LIST) ? "/bin/ls" :
							path2file(d->name),
							d->bytes);
			else

				lion_printf(d->login->handle,
		  "150 Opening %s%s mode data connection for %s.\r\n",
							(d->mode&MODE_TYPE_BINARY?"BINARY":"ASCII"),
							(d->mode&MODE_TYPE_CCSN)?"+CCSN":
							(d->mode&MODE_TYPE_SSCN)?"+SSCN":"",
							(d->type & DATA_LIST) ? "/bin/ls" :
							path2file(d->name));

			d->status |= ST_SEND_226;



			d->bytes = 0;

			lion_enable_read(d->handle);

		}


		break;






	case LION_FILE_CLOSED:

		consolef("[data] %p/%p file finished.\n", handle, user_data);
		if (d) {
			d->althandle = NULL;
			// Also close the connection.
 			lion_close( d->handle );
			//data_close(d);
		}
		break;

	case LION_FILE_FAILED:

		consolef("[data] %p/%p file lost? %d:%s\n", handle, user_data,
				size, line);

		if (d) {
			d->althandle = NULL;

			if (d && d->login && d->login->handle) {
				lion_printf(d->login->handle,
							"500 TRANSFER FAILED: %d:%s\r\n",
							size, line);
				d->status &= ~ST_SEND_226;
			}

			lion_close( d->handle ); // or disconnect?

		}
		break;

		// Most likely in file writing if we are here.
	case LION_BUFFER_USED:
		if (d && d->handle)
			lion_disable_read( d->handle );
		break;

	case LION_BUFFER_EMPTY:
		if (d && d->handle)
			lion_enable_read( d->handle );
		break;


	default:
		consolef("[data] unhandled event in data_file_write_handler %d\n",
				 status);

	}


	return 0;
}


//
// This is the lion handler for all data nodes awaiting list results.
//
int data_list_handler( lion_t *handle,
				  void *user_data, int status, int size, char *line)
{
	struct data_node *d = (struct data_node *) user_data;
	int reply;

	switch( status ) {

		// Should these two happen? Guess they could. Client is gone.
		// This actually needs fixing in libdirlist, it needs to take over
		// the handler completely, and if connection is lost, or closed,
		// dirlist needs to cancel the listing internally, then relay the
		// event to handle. Otherwise dirlist will send the result to an
		// already closed handle.
	case LION_CONNECTION_CLOSED:
	case LION_CONNECTION_LOST:
		consolef("[data] %p/%p Got close/lost even while dirlisting!\n",
				 handle, user_data);

		// We remember that this handle failed.
		dirlist_cancel(handle);


		// Call the real handler to deal with this.
		if (d) {
			data_handler(handle, user_data, status, size, line);
			return 0;
		}
		break;


		// If we get reply from dirlist, deal with it.
	case LION_INPUT:

		if (!d) return 0;

		// We should get ":0 /tmp/file" or ":errno reason"
		if (line[0] != ':') {
			consolef("[data] %p/%p dirlist reply didn't start with ':'?\n",
					 handle, user_data);
			return 0;
		}


		reply = atoi(&line[1]);

		consolef("[data] %p/%p waiting for dirlist got : '%s':%d\n",
				 handle, user_data, line, reply);

		// Change the socket back to normal handler.
		lion_set_handler(d->handle, data_handler);

		// Turn reading back on.
		lion_enable_read( d->handle );

		if (!reply) {


			// Success, we have a filename!
			d->althandle = lion_open( &line[ 3 ], O_RDONLY, 0444,
										 LION_FLAG_FULFILL,
										 (void *)d);

			lion_set_handler(d->althandle, data_file_read_handler);

			// If we need to get SSL talking, pause first, that is
			// if it isn't already on.

#if 1
			if (d->login && (d->login->status & ST_DATA_SECURE) &&
				!lion_ssl_enabled(d->handle)) {

				consolef("pausing file\n");
				lion_disable_read( d->althandle );
			}
#endif

			// We also delete the file here, yep. If the open was successful
			// it will be deleted when it is finally closed.
			remove( &line[3] );

			return 0;
		}

		consolef("[data] list_handler, return failure.\n");

		// We failed!
		// Send something to client, like an error.
		if (d && d->login && d->login->handle)
			lion_printf(d->login->handle,
						"5%02d %s\r\n",
						reply,
						&line[ 3 ]);

		data_close(d);
		break;




		// Any other events, deal with it in the real handler.
	default:
		consolef("[data_list_handler] default called %p/%p %d\n",
				 handle, user_data, status);
		data_handler(handle, user_data, status, size, line);
	}


	return 0;
}



//
// This is the lion handler for all data nodes.
//
int data_handler( lion_t *handle,
				  void *user_data, int status, int size, char *line)
{
	struct data_node *d;

	//	consolef("[data] handle %p/%p event %d\n", handle, user_data, status);

	d = (struct data_node *) user_data;


	// We can get called with NULL data, in particular if data_close() is
	// called first, then userdata is set to NULL so that we don't call
	// data_close() again, from here.
#if 0
	if (!d) {
		consolef("[data] Called with NULL data_node??\n");
		return -1;
	}
#endif



	switch(status) {

	case LION_CONNECTION_LOST:
		consolef("[data] connection lost: %p [%p] (%d:%s)\n",
				 d,
				 handle, size, line);
		if (d) {

			if (!(d->type & DATA_PASSIVE) &&
				!(d->status & ST_SEND_226)) {

				if (d->login && d->login->handle)
					lion_printf(d->login->handle, "500 DATA Connect: %d:%s\r\n",
								size, line);

			}


			// Remove the file if required for this section
			// BUG 2004:12:17 - ONLY delete the file if we were an
			// UPLOAD. Classic, wiping files randomly.
			if (d->name && *d->name &&
				(d->type & DATA_STOR)) {
					if (section_deletebad(d->name)) {

				char *p;

				// I doubt this is a good idea? Is the name always right here
				// even though "d->name" might not have been set yet (no retr
				// or stor sent)? Do we always wipe all files, or just non-
				// free?
				log_xfer("TEST %s (partial) debug_deletebad %s\n",
						 d->login ? d->login->user : "(null)",
						 d->name);

				remove(d->name);
#ifdef RACESTATS
				p = pathsplit(d->name);
				race_file_deleted(d->name, p);
				pathfixsplit(d->name, p);
#endif
			} else {
				// File failed, make it writable by others
				file_goroot();
				chmod(d->name, (mode_t) 0666);
				file_gononroot();
				}

			} else {

				// Update the bytes counter stats. Needs to be done while we
				// still have a reference to d->handle.
				data_update_stats(d);

			}

			d->handle = NULL; // it's closed, don't call close below.

			// We call the testing code here even though we know it failed.
			// So we remove that we were a STOR and it wont
			d->type &= ~DATA_STOR;

			data_close( d );
		}
		break;

	case LION_CONNECTION_CLOSED:
		consolef("[data] connection closed: %p [%p]\n",
				 d,
				 handle); // printing ->bytes here can core.
		if (d) {

			// Update the bytes counter stats. Needs to be done while we
			// still have a reference to d->handle.
			data_update_stats(d);

			// Old place to call file checking.

			d->handle = NULL; // it's closed, don't call close below.
			data_close( d );
		}
		break;

		// A PASV connection has just come in.
	case LION_CONNECTION_NEW:
		consolef("[data] New incoming connection: %p/%p\n", handle, d);

		if (!d) return 0;

		// Get us the new handle, close the old, always fulfill, and fill
		// in the host and port information.
		d->handle = lion_accept(handle, 1, LION_FLAG_FULFILL,
								d, &d->host, &d->port);
		// Set the handler to here.
		lion_set_handler(d->handle, data_handler);
		lion_setbinary(d->handle);
 		break;


#ifdef WITH_SSL
	case LION_CONNECTION_SECURE_ENABLED:
		consolef("[data] %p/%p SSL successful: type %04X.\n", handle, d,
                 d->type);

		// File sending was paused until SSL is done, so start now.
		// but only enable read if we AREN'T STOR/APPE.
#if 0
		if (d && d->althandle && !(d->type & DATA_STOR)) {
            // APPE sets STOR too
            consolef("[data] %p/%p starting file\n", handle, d);
            lion_enable_read( d->althandle );
		}
#endif

        // Make it call data_connected(), outside of event-context.
        d->status |= DATA_WANTCONNECTED;
        //data_connected(d);

		break;

	case LION_CONNECTION_SECURE_FAILED:
		consolef("[data] %p/%p SSL failed.\n", handle, d);

		if (d && d->handle) {

			consolef("[data] data close hack\n");

			// This should take care of decrementing active_retr etc.
			data_update_stats(d);

			// We remote STOR so we don't enter check_isbad code.
			// We remove STOR so we don't enter check_isbad code.
			if (d->type & DATA_STOR) {
				d->type &= ~DATA_STOR;
				if (d->name) {
					consolef("Deleting file: '%s'\n", d->name);
					remove(d->name);
				}
			}

			lion_disconnect(d->handle);

			// We used to always send this, but sometimes this can happen
			// BEFORE we've sent 150-
			if (d->status & ST_SEND_226) {
				d->status &= ~ST_SEND_226;
				lion_printf(d->login->handle, "500 Secure DATA channel failed.\r\n");
			}
			return 0;


		}

		break;
#endif



	case LION_CONNECTION_CONNECTED:
		consolef("[data] %p/%p connected %p/%s\n", handle, d,
				 d ? d->login : NULL,
				 (d && d->login) ? d->login->user : "(null)" );

		if (!d) return 0;

#if 0
		if (!lion_isconnected( d->handle )) {
			consolef("[data] %p/%p race condition occured\n", handle,
					 user_data);
			return 0;
		}
#endif

		d->status |= ST_ACTIVE;

		// Lets disable read until we are ready to deal with something
		//lion_disable_read( d->handle );

		// SSL call was here before... now it is not.
		// data_do_ssl(d);
		// Stop us from reading, especially since we have not yet
		// asked for SSL, if indeed it wants SSL.
		lion_disable_read(d->handle);

        d->status |= DATA_WANTCONNECTED;
        //		data_connected( d );
		break;



	case LION_INPUT:
		//consolef("[data] %p/%p input '%s'\n", handle, user_data, line);
		//abort(); // Really should NEVER happen.
		break;



		// Buffering, if we are in file sending, we should get these.
	case LION_BUFFER_USED:
		//		consolef("pause on %p\n", d);
		//		printf("U ");fflush(stdout);
		if (d && d->althandle) {
			lion_disable_read( d->althandle );
		}
		break;

	case LION_BUFFER_EMPTY:
		//consolef("pause off %p\n", d);
		//		printf("E ");fflush(stdout);
		if (d && d->althandle) {
			lion_enable_read( d->althandle );
		}
		break;



		// If we are here, it means we are in file writing.
	case LION_BINARY:
		if (d && d->althandle) {

			// If we saw data, update idle
			if (d->login) d->login->idle = lion_global_time;

			// Update idle counter.
			d->time = lion_global_time;

			// This call, if it fails, d is no longer valid!!
			lion_output(d->althandle, line, size);

			d = NULL; // just to make sure
			handle = NULL;

		}
		break;







	default:
		consolef("[data] %p/%p default called event %d\n", handle, user_data,
				 status);
		break;


	}


	return 0;

}











void data_connected(struct data_node *d)
{

	consolef("data_connected(%d:%d) %p/%s\n", d->status, d->type,
			 d,
			 d && d->login && d->login->user ? d->login->user : "(null)");





	// If both the connection is established,
	// and we've had data_list() called, do so.
	if (!(d->status & ST_ACTIVE) )
		return;


	// The new SSL place. After we've received the command
	if (d->login && (d->login->status & ST_DATA_SECURE)&&
		!(d->type & DATA_TLS)) {

		// If LIST or NLST, we shall always call ssl_accept irrelevant
		// of the CCSN/SSCN setting according to the specifications.
		// We also only want to call this once we know the command wanted.
		// So if type is strictly one of STOR/RETR/LIST, we set it.
		//
		// Now additionally, if we DID set it, we leave this function
		// only to return when SSL was negotiated.
		if ((d->type & DATA_LIST) || (d->type & DATA_NLST)) {
		  consolef("[data_handler] requesting SSL in LIST mode.\n");
		  d->type |= DATA_TLS;
		  lion_ssl_set(d->handle,
			       LION_SSL_SERVER);
		  // Since we ask for SSL, we can have it start that now
		  lion_enable_read(d->handle);
		}

		if ((d->type & DATA_RETR) || (d->type & DATA_STOR)) {
			if ( d->login->mode & (MODE_TYPE_CCSN|MODE_TYPE_SSCN)) {
				consolef("[data_handler] requesting SSL in client/CCSN/SSCN mode.\n");
				d->type |= DATA_TLS;
#ifdef WITH_SSL
				lion_ssl_set(d->handle,
							 LION_SSL_CLIENT);
#endif
				// Since we ask for SSL, we can have it start that now
				lion_enable_read(d->handle);
				//return;

			} else {
				consolef("[data_handler] requesting SSL in server/normal mode.\n");
				d->type |= DATA_TLS;
#ifdef WITH_SSL
				lion_ssl_set(d->handle,
							 LION_SSL_SERVER);
#endif
				// Since we ask for SSL, we can have it start that now
				lion_enable_read(d->handle);

			}
		} // STOR or RETR

	} // if SECURE


	// We get here when SSL is done, if SSL
	// or if no SSL, when we get the command.

	if (d->type & DATA_LIST) {

        // Stop nestling requests
        if (d->althandle) {

            consolef("[data] LIST ready, releasing handle\n");
            lion_enable_read(d->althandle);

            return;
        }

		consolef("[data] asking dirlist for help...2\n");

		// Issue a LIST request for us.
		// Change our handler first, so we get data there,
		// then ask for a list.
		lion_set_handler(d->handle, data_list_handler);
		dirlist_list(d->handle, d->name, d->list_precat,
					 d->sort_by | DIRLIST_FILE | DIRLIST_USE_CRNL,
					 d);
	}



	if (d->type & DATA_RETR) {

        if (d->althandle) {
            consolef("[data] RETR ready, releasing handle\n");
            lion_enable_read(d->althandle);
            return;
        }

        consolef("[data] DATA_RETR.\n");

        file_retr(d);

		if (!d->althandle) {
			lion_disconnect(d->handle);
			return; // It failed.
		}

		// Success. Ready open the load.
		lion_set_handler(d->althandle, data_file_read_handler);


	}



	if (d->type & DATA_STOR) {

        if (d->althandle) {
            consolef("[data] STOR ready, enable data read\n");
            if (d->handle) lion_enable_read(d->handle);
            return;
        }

        consolef("[data] No SSL, and STOR, enabling read\n");
		lion_enable_read(d->handle);


		if (section_convertstorspace(d->name)) {
            char *spcptr = d->name;
			while ( (spcptr=strchr(spcptr,' '))!=NULL )
				*spcptr='_';
		}

		if (d->type & DATA_APPE)
			file_stor(d, FILE_APPE);
		else
			file_stor(d, FILE_OLD);

		if (!d->althandle) {
			lion_disconnect(d->handle);
			return; // It failed.
		}

		// Success. Ready open the load.
		lion_set_handler(d->althandle, data_file_write_handler);

		lion_disable_read(d->althandle); // Was handle?!

	}

	/*
	if (d->type & DATA_STOR) {

		if (section_convertstorspace(d->name)) {
			while ( (spcptr=strchr(spcptr,' '))!=NULL )
				*spcptr='_';
		}

		if (d->type & DATA_APPE)
			file_stor(d, FILE_APPE);
		else
			file_stor(d, FILE_OLD);

		if (!d->althandle) {
			lion_disconnect(d->handle);
			return; // It failed.
		}

		// Success. Ready open the load.
		lion_set_handler(d->althandle, data_file_write_handler);

		lion_disable_read(d->handle);

	}
	*/

}




void data_update_stats(struct data_node *d)
{

	//consolef("[data] update_stats: %p\n", d->handle);

	/* If this was up or down-load, adjust counters */
	if (d && d->login && d->login->quota) {

		if ((d->type & DATA_RETR)) // could check its not LIST...?
			quota_retr_close(d);
		else if ((d->type & DATA_STOR))
			quota_stor_close(d);
	}


}






void data_close(struct data_node *d)
{
	struct data_node *curr, *last = NULL;
	struct login_node *send226 = NULL;
	lion_t *holder;


	consolef("data_close(%p): closing %d\n", d->handle, d->status);


	if (d->login) {

		d->login->idle = lion_global_time;

		d->login->port_port = 0;
		d->login->port_host = 0;

	}


    if (d->timer_handle) {
        timers_cancel(d->timer_handle);
        d->timer_handle = NULL;
    }


	if (d->type != DATA_RENAME) {

		// If handle is set, we unset the user_data, so we don't get call
		// yet again.

		if (d->handle) {

			// The handler will only call this if user_data is set
			// but since data_close was called first, we set it to
			// NULL so it wouldn't be called upstairs.
			data_update_stats(d);

			lion_set_userdata(d->handle, NULL);
			holder = d->handle;
			d->handle = NULL;
			lion_close(holder);
		}

		if (d->althandle) {
            consolef("[data] data_close with althandle, closing.\n");
			lion_set_userdata(d->althandle, NULL);
			holder = d->althandle;
			d->althandle = NULL;
			lion_close(holder);
		}

	}


	if (d->status & ST_SEND_226) {


		// Send 226 means that the transfer was actually started, and not
		// rejected due to file permissions, or dupe db existince.

		// If it was a stor, and since we cleanly closed, maybe check.
		if ((d->type & DATA_STOR) &&
			section_filecheck(d->name)) {

#ifdef RACESTATS
			if (d->login->options & (UO_XDUPE_2|UO_XDUPE_3))
				race_xdupe(NULL, d->login, 226, d->name);
#endif

			check_filebad( d ); // "d" is released after this call.
		}

		send226 = d->login;

		// If we send 226, it might see login is gone, which calls login_exit,
		// which calls data_close, which gets us here, with a data node that
		// has been freed.

		d->status &= ~ST_SEND_226;

	}


	// This code is never used.
#if 0
	if ((d->type == DATA_FILECHECK && !(d->status & ST_ACTIVE)))  {

		if (!server_backgroundtest) {
			socket_number(d->login, 226, NULL);

		}
	}
#endif

	// d->login MIGHT have logged out, and be 0-ed.

	d->login->data_active--;




	d->status = 0;


	if (d->name)
		free(d->name);

	if (d->list_precat)
		free(d->list_precat);


	for (curr = (struct data_node *)d->login->data;
		 curr; last = curr, curr=curr->next) {

		if (curr == d) {
			if (!last)
				d->login->data = (void *) curr->next;
			else
				last->next = curr->next;

#ifdef DEBUG
#endif
			memset(d, 0, sizeof(*d));

			free(d);
			break;

		}
	}

#if 0
	if (d->login->data) {
		consolef("WARNING!! data->next is NOT NULL %d %u\n",
				 d->login->socket, d->login->user);
	}
#endif

	if (send226)
		socket_number(send226, 226, NULL);

}



struct data_node *data_findtype(struct login_node *t, int type)
{
	struct data_node *nd;

	for (nd = t->data; nd; nd=nd->next) {

		if (nd->type == type) { /* Found it */

			return nd;

		}
	}
	return NULL;
}



void data_clearall(struct login_node *t)
{
	struct data_node *nd;
	struct data_node *keep_list = NULL;
	int count = 0;

	consolef("[data] releasing all %p/%s\n",
			 t, t->user ? t->user : "<null>");

	// We may want to keep some nodes still, so we will build a
	// keep list.

	t->waiting_for = 0;
	t->port_port = 0;
	t->port_host = 0;

	while(t->data) {

		nd = (struct data_node *)t->data;

		// If we have a RETR, or STOR, and it is not actually idle
		// yet, we don't close now. Some clients use anti-idle features
		// which sometimes include sending of "pasv".
		if (!(nd->status & ST_SEND_226) ||
			(lion_global_time - nd->time) > DATA_IDLE_SEC) {

			consolef("[data_clearall] releasing %p/%p as idle. (%d/%d)\n",
					 t, nd,
					 nd->status,
					 lion_global_time - nd->time);
			data_close(nd);

		} else {

			// Set login's data node to the next node, skipping this one.
			t->data = nd->next;

			// Set this node's 'next' field to keep_list next.
			nd->next = keep_list;

			// Assign this node to keep_list.
			keep_list = nd;

			count++;

		}
	}

	t->data = (void *) keep_list;

	if (count) consolef("[data_clearall] %p has %d data_nodes\n", t, count);

}



//
// If create_new is set, we just create a new node, regardless.
// Otherwise, in PORT mode, port_port and port_host should be set. Return new.
// Otherwise, if waiting_for = PASV, return previously made node.
//
struct data_node *data_init(struct login_node *t, int create_new)
{
	struct data_node *nd;

	/* NULL args means passive build */

	consolef("[data] init: %p %d\n", t, create_new);

	// We aren't to just create a node.
	if (!create_new) {

		if (t->waiting_for == DATA_PASSIVE) {

			t->waiting_for = 0;

			nd = data_findtype(t, DATA_PASSIVE);

			if (!nd) {
				socket_number(t, 425, "Issue PASV or PORT first");
				return NULL;
			}

			return nd; /* return this old node, instead of a new one */

		} // waiting_for


		// No previous PORT command?

		if ((!t->port_host || !t->port_port)) {
			socket_number(t, 425, "Issue PASV or PORT first");
			return NULL;
		}

	}

	// Create new node.

	consolef("data_init(): num_active is %d retr=%d stor=%d\n",
			 t->data_active,
			 ((struct quota_node *)t->quota)->num_active_retr,
			 ((struct quota_node *)t->quota)->num_active_stor);

	/* Check if we previously issued PASV, then, find the node instead. */

	nd = (struct data_node *) malloc(sizeof(struct data_node));
	if (!nd) {
		socket_number(t, 425, "Out of memory");
		return NULL;
	}

	memset(nd, 0, sizeof(struct data_node));

	/* 425 Can't build data connection: No route to host. */

	nd->name = NULL;
	nd->host = t->port_host;
	nd->port = t->port_port;
	t->port_host = 0;
	t->port_port = 0;
	nd->time = global_time_now - 1;  // Make it assume 1 second has already gone
	nd->bytes = 0;
	nd->mode = t->mode;
	nd->type = 0;
	nd->login = t;

	/* Incorrect, increases it for TESTER dummy nodes, pasv etc, but may
	   not get decreased properly. */
	nd->login->data_active++;


	nd->sort_by = 0;

	nd->next = (struct data_node *) t->data;
	t->data = (void *) nd;

	return nd;
}




void data_set_arguments(struct data_node *nd, char *args)
{
	int i;
	int name = 0;

	// If site extra is set on, we set -W on automatically as well.
	//if (nd->login->options & UO_FANCY)
	//	nd->sort_by ^= DIRLIST_SHOW_DIRSIZE;

	i = 0;

	while(args[i]) {
		switch(args[i]) {
		case 'l':
			nd->sort_by |= DIRLIST_LONG | (name ? DIRLIST_SORT_NAME : 0 );
			nd->sort_by &= ~(DIRLIST_SHORT);
			name++;
			break;
		case 'N':
			nd->sort_by |= DIRLIST_SORT_NAME;
			break;
		case 'r':
			nd->sort_by ^= DIRLIST_SORT_REVERSE;
			break;
		case 'R':
			// currently disabled until security can be fixed.
			nd->sort_by ^= DIRLIST_SHOW_RECURSIVE;
			break;
		case 't':
			nd->sort_by |= DIRLIST_SORT_DATE;
			break;
		case 'S':
			nd->sort_by |= DIRLIST_SORT_SIZE;
			break;
		case 'a':
			nd->sort_by |= DIRLIST_SHOW_DOT;
			break;
		case 'X':
			nd->sort_by |= DIRLIST_XML;
			break;
		case '1':
			nd->sort_by |= DIRLIST_SHORT;
			nd->sort_by &= ~(DIRLIST_LONG);
			break;
		case 'I':
			nd->sort_by ^= DIRLIST_SORT_CASE;
			break;
		case 'D':
			nd->sort_by ^= DIRLIST_SORT_DIRFIRST;
			break;
		case 'W':
			nd->sort_by ^= DIRLIST_SHOW_DIRSIZE;
			break;
		case 'G':
			nd->sort_by ^= DIRLIST_SHOW_GENRE;
			break;

		}
		i++;
	}

}



struct data_node *data_list(struct login_node *t, char *args, char *path2)
{
	struct data_node *nd;
	char path[8192], *r, *expanded;
	struct stat stbf;



	//
	// We split whatever we get from the user, argument "-l" etc and
	// argument. "../mp3/*.zip". The we pathchecklogin, which reduces the
	// path (deals with ".." etc.) and makes sure it is valid (within the
	// FTP root).
	// The we stat it, if this stat fails, we attempt to truncate the
	// path at the last "/", to assume that the remainder is wildcard argument.
	// We then stat it again, and should this fail too, we stop.
	//
	// Otherwise, first or 2nd stat is valid, we setup directory listing for
	// full path, with wildcard at end, plus the pre-cat string. "../mp3/" if
	// any.

	// When we are called it is like args = "-l" and path2 = "../mp3/*.zip"


	/* make sure the path is ok */
	if (*path2 == '/')
		snprintf(path, sizeof(path), "%s", path2);
	else
		snprintf(path, sizeof(path), "%s%s", t->cwd, path2);


	//file_undot(path);
	// Copy over since we want to leave this unaffected
	//	strncpy(buf, path, sizeof(buf));
	//	buf[sizeof(buf)-1] = 0;

	// This is old group check code
	if (!file_pathchecklogin(t, path)) {
		socket_number(t, 581, "");
		return NULL;
	}

	// this is new group check code.
	if (section_grouped(path) &&
		!section_group_ismember(path,t->user)) {
		socket_number(t, 581, "required for this section");
		return NULL;
	}



	//consolef("asdfadsf %s\n", path2);


	// We only need to expand paths, if we've passed something
	if (path2 && *path2) {

		// Attempt to expand
		if ((expanded = section_expand(path))) {

			// Ok, now re-write
			//consolef("asdfasdf %s => %s\n", path, expanded);

			// New stuff is "path" - strlen(path2);
			path2 =  &expanded[ strlen(path) - strlen(path2) ];

			//			consolef("New stuff should be: %s", path2);

			// Problem is "../" are reduced! and might therefor be lost...
			// so it wont work. but to hell with that. Nor with ".." after
			// work at all, and not be expanded.

		}

	}


	/* get us a new data node */
	if (!(nd = data_init(t, 0)))
		return NULL;

	nd->type |= DATA_LIST;

	/*
	 * Ok, we have "args" which are listing arguments -l -r -t etc.
	 * Then we have "server_usechroot" which is the real start to file area.
	 * and finally "path" which any dir-path or file pattern match they type.
	 *
	 */


	//	sprintf(buf, "%s/%s", server_usechroot, path);

	/* Set arguments */
	nd->sort_by = DIRLIST_LONG;   // default
	data_set_arguments(nd, args);



	// Attempt to stat it, if this fails, we attempt to remove the last
	// bit in "../mp3/*.zip" in the hopes that it is the "*.zip", or, wilcards.
	if (stat(path, &stbf)) {

		//		consolef("    list: stat failed, assuming argument: '%s'\n", path2);

		r = strrchr(path2, '/');
		if (r)
			r[1] = 0;  // Terminate after "/"
		else
			*path2 = 0;

	} else {

		//		consolef("    list: stat good\n");

		// if the full path ends with exactly a file, precat should be empty.
		// otherwise, should end with "/"
		if ((stbf.st_mode & S_IFMT) == S_IFREG) {

			// Might be dir/file, so go back to last "/" or just zero
			r = strrchr(path2, '/');
			if (r)
				r[1] = 0;  // Terminate after "/"
			else
				*path2 = 0;

		} else {

			// Ensure it ends with "/".
			if (path2 && *path2 && path2[ strlen( path2) -1 ] != '/') {
				strcat(path2, "/");
			}

		}

	}

#if 0
	consolef("[data] list '%s' -> '%s' '%s' with precat '%s'\n",
			 t->user,
			 args,
			 path,
			 path2);
#endif


	// Lets expand dirlistings too! In case it is wanted.

	nd->name = mystrcpy(path);


	// See if we need bother to remember the precat?
	if (path2 && *path2)
		nd->list_precat = mystrcpy(path2);


    // No more work needed for STATLIST.
    if (t->status&ST_STATLIST)
        return nd;


	if (!(nd->type & DATA_PASSIVE) && !(nd->type & DATA_QUEUE))
		data_process(nd);

	// If data is already connected, this was what we were waiting on
    nd->status |= DATA_WANTCONNECTED;

	return nd;
}








void data_process(struct data_node *d)
{

	// This is kinda scary, without FULFILL, lion_* could send us
	// events, which could mean we execute things elsewhere.
	if (server_data_bind_port)
		file_goroot();


	d->handle = lion_connect( lion_ntoa(d->host), d->port,
							  (server_bindif) ?
							  lion_addr(server_bindif) : 0,
							  server_data_bind_port,
							  LION_FLAG_FULFILL,
							  d);

	if (server_data_bind_port)
		file_gononroot();


	lion_set_handler( d->handle, data_handler );
	lion_setbinary(d->handle);

	// This status is no longer needed.
	//d->status = ST_PENDING;
	//	consolef("data_process(%p): connection pending\n", d->handle);


	return;
}





struct data_node *data_retr(struct login_node *t, char *args)
{
	struct data_node *nd;

	/* First, count the number of RETR already in session */

	if (t->quota) {


		//		consolef("data_retr(): num_active is %d\n",
		//	 ((struct quota_node *)t->quota)->num_active_retr);

		if ((int) ((struct quota_node *)t->quota)->num_active_retr < 0)
			((struct quota_node *)t->quota)->num_active_retr = 0;

		if (((struct quota_node *)t->quota)->num_active_retr >= t->num_downloads) {


			/* If we're in passive, remove the data node */
			if (t->waiting_for == DATA_PASSIVE) {

				t->waiting_for = 0;

				nd = data_findtype(t, DATA_PASSIVE);
				if (nd) data_close(nd);

			}

			socket_number(t, 425, "download limit reached");
			return NULL;

		}
	}


	if (!(nd = data_init(t, 0)))
		return NULL;


	nd->name = mystrcpy(args);

	if (file_isdir(t, nd->name)) {
		socket_number(t, 550, "That is a directory");
		data_close(nd);
		return NULL;
	}


	nd->type |= DATA_RETR;


	if (!(nd->type & DATA_PASSIVE) && !(nd->type & DATA_QUEUE))
		data_process(nd);

	if (nd->status & DATA_PASSIVE)
		log_xfer("RETR %s %s/%s\n", t->user, t->cwd, nd->name);
	else
		log_xfer("RETR %s %s\n", t->user, nd->name);

	if (t->quota) {
		((struct quota_node *)t->quota)->num_active_retr++;
	}


    nd->status |= DATA_WANTCONNECTED;

	return nd;

}




struct data_node *data_stor(struct login_node *t, char *args, int flags)
{
	struct data_node *nd;
	char *slash;

	/* First, count the number of STOR already in session */
	if (t->quota) {

		if ((int) ((struct quota_node *)t->quota)->num_active_stor < 0)
			((struct quota_node *)t->quota)->num_active_stor = 0;

		if (((struct quota_node *)t->quota)->num_active_stor >= t->num_uploads) {

			/* If we're in passive, remove the data node */
			if (t->waiting_for == DATA_PASSIVE) {

				t->waiting_for = 0;

				nd = data_findtype(t, DATA_PASSIVE);
				if (nd)
					data_close(nd);

			}

			socket_number(t, 425, "upload limit reached");
			return NULL;

		}
	}

	if (!(nd = data_init(t, 0)))
		return NULL;


	// Set flags.
	nd->type |= DATA_STOR|flags;

	nd->name = mystrcpy(args);

	if (section_lowercasefile(nd->name)) {

		slash = path2file(nd->name);

		if (slash)
			strlower(slash);

	}

	if (!(nd->type & DATA_PASSIVE) && !(nd->type & DATA_QUEUE))
		data_process(nd);


	// Open the file right here and now, even if the data connection
	// is not ready (since, with pasv and stor, it wont be until
	// retr is set, which wont be sent until 150 message is seen

#if 1  // EARLY STOR 150 MESSAGE
	if (nd->type & DATA_APPE)
	  file_stor(nd, FILE_APPE);
	else
	  file_stor(nd, FILE_OLD);

	if (!nd->althandle) {
	  lion_disconnect(nd->handle);
	  return NULL; // It failed.
	}

	// Success. Ready open the load.
	lion_set_handler(nd->althandle, data_file_write_handler);

	lion_disable_read(nd->althandle); // Was handle?!
#endif


	/* If it is passive, we need to send the CWD too. */
	if (nd->status & DATA_PASSIVE)
		log_xfer("STOR %s %s/%s\n", t->user, t->cwd, nd->name);
	else
		log_xfer("STOR %s %s\n", t->user, nd->name);


	if (t->quota) {
		((struct quota_node *)t->quota)->num_active_stor++;

		//consolef("[data] increasing active_stor %d\n",
		//		 ((struct quota_node *)t->quota)->num_active_stor);
	}


    nd->status |= DATA_WANTCONNECTED;

	return nd;

}





//
// Identical to list except default is short list.
struct data_node *data_nlst(struct login_node *t, char *args, char *path2)
{
	struct data_node *nd;
	char path[8192], *r;
	struct stat stbf;

	if (*path2 == '/')
		snprintf(path, sizeof(path), "%s", path2);
	else
		snprintf(path, sizeof(path), "%s%s", t->cwd, path2);

	if (!file_pathchecklogin(t, path)) {
		socket_number(t, 581, "");
		return NULL;
	}

	if (section_grouped(path) &&
		!section_group_ismember(path,t->user)) {
		socket_number(t, 581, "required for this section");
		return NULL;
	}

	/* get us a new data node */
	if (!(nd = data_init(t, 0)))
		return NULL;

	nd->type |= DATA_LIST;

	/* Set arguments */
	nd->sort_by = DIRLIST_SHORT;   // default
	data_set_arguments(nd, args);

	if (stat(path, &stbf)) {

		r = strrchr(path2, '/');
		if (r)
			r[1] = 0;  // Terminate after "/"
		else
			*path2 = 0;

	} else {

		if ((stbf.st_mode & S_IFMT) == S_IFREG) {

			// Might be dir/file, so go back to last "/" or just zero
			r = strrchr(path2, '/');
			if (r)
				r[1] = 0;  // Terminate after "/"
			else
				*path2 = 0;

		} else {

			// Ensure it ends with "/".
			if (path2 && *path2 && path2[ strlen( path2) -1 ] != '/') {
				strcat(path2, "/");
			}

		}

	}

#if 0
	consolef("[data] list '%s' -> '%s' '%s' with precat '%s'\n",
			 t->user,
			 args,
			 path,
			 path2);
#endif


	nd->name = mystrcpy(path);

	// See if we need bother to remember the precat?
	if (path2 && *path2)
		nd->list_precat = mystrcpy(path2);

	if (!(nd->type & DATA_PASSIVE) && !(nd->type & DATA_QUEUE))
		data_process(nd);

	// If data is already connected, this was what we were waiting on
	data_connected( nd );

	return nd;
}




struct data_node *data_pasv(struct login_node *t, lion_t *handle)
{
	struct data_node *nd;


	/* Check if they've already issued a PASV and now a second, if so
	   remove the first to avoid memory leakage */

	if (t->waiting_for == DATA_PASSIVE) {

		nd = data_findtype(t, DATA_PASSIVE);

		if (nd) data_close(nd);

		t->waiting_for = 0;
	}


	if (!(nd = data_init(t, 1))) /* signal passive */
		return NULL;


	// Set up our handler.
	lion_set_handler(handle, data_handler);
	//lion_setbinary(handle);

	// Assign our data node to the handle
	lion_set_userdata(handle, nd);

	//  nd->socket = fd;
	nd->handle = handle;
	nd->type = DATA_PASSIVE;
	nd->status = ST_NONE;
	nd->name = NULL;


	t->waiting_for = DATA_PASSIVE;

	/*if (!(nd->type & DATA_QUEUE))  data_process(nd);*/

	return nd;

}


/*
  ---> RNFR foo
  550 foo: No such file or directory.
  ftp> ren bah bah.txt
  ---> RNFR bah
  350 File exists, ready for destination name
  ---> RNTO bah.txt
  250 RNTO command successful.
  ftp> ---> QUIT
*/

struct data_node *data_rnfr(struct login_node *t, char *args)
{
	struct data_node *nd;
	struct stat st;

	/*
	  First - double check there isn't already a rename packet there.
    */

	nd = data_findtype(t, DATA_RENAME);

	if (nd) {
		socket_print(t, "550 RNFR must be followed with RNTO, received two RNFR's!\r\n");
		return NULL;
	}

	if (!(nd = data_init(t, 1)))
		return NULL;

	nd->name = mystrcpy(args);
	nd->type = DATA_RENAME;
	nd->status = ST_NONE;

	if (!file_pathcheck(nd)) {
		socket_number(t, 550, path2file(nd->name));
		data_close(nd);
		return NULL;
	}

	if (stat(nd->name, &st) < 0) {
		socket_number(t, 550, path2file(nd->name));
		data_close(nd);
		return NULL;
	}

	socket_number(t, 350, NULL);

	return nd;
}




struct data_node *data_rnto(struct login_node *t, char *args)
{
	struct data_node *nd;
	char data[256];

	/*
	  First - double check there isn't already a rename packet there.
    */

	nd = data_findtype(t, DATA_RENAME);

	if (nd) {

		strcpy(data, nd->name);
		free(nd->name);
		nd->name = mystrcpy(args);

		if (!file_pathcheck(nd)) {
			socket_number(t, 550, path2file(nd->name));
			data_close(nd);
			return NULL;
		}

		file_gouser(nd->login);

		if (rename(data, nd->name) < 0)
			socket_number(t, 550, path2file(nd->name));
		else
			socket_number(t, 250, path2file(nd->name));

		file_gononuser();

		data_close(nd);

		return NULL;
	}

	socket_print(t, "550 Received RNTO without a previous RNFR!\r\n");
	return NULL;

}










//
// These are called on data sending, and on data receiving. Optionally
// sent on list as well. They set any cap that is desired.
//
// WRITING to file, READING from socket, rate_in and UPLOAD
void data_set_rate_in( struct data_node *d )
{


	if (!server_capping)
		return;

	consolef("[data] data_set_rate_in: %p\n", d);


	// If we are not to cap, skip us.
	if (d->login && (d->login->status & ST_NOCAP)) {
		consolef("[data_incap] in NOCAP IP list -- skip\n");
		return;
	}

	// Set session cap?
	if (server_cap_upload) {

		lion_rate_in( d->handle, server_cap_upload );

	}


	// Set user (multiple-sessions) cap.
	if (server_cap_user_upload) {

		lion_group_add(d->handle, d->login->cap_group);
		//consolef("[data] rate_in assigning to group %d\n",
		//	 d->login->cap_group);

	}


	if (server_cap_total_upload && data_global_rate_cap_group) {

		lion_group_add(d->handle, data_global_rate_cap_group);
		consolef("[data] capped\n");

	}

}



//
// Set outgoing cap here.
// Note it is by far more efficient to set READ cap on the file, as
// opposed to the WRITE cap on socket. Either will do, but in terms of
// efficiency, read cap is better. I have both just to show that the
// write cap code actually works.
//
// READING file, WRITING to socket, rate_out, DOWNLOADING.
void data_set_rate_out( struct data_node *d )
{

	if (!server_capping)
		return;

	//consolef("[data] data_set_rate_out: %p\n", d);

	// If we are not to cap, skip us.
	if (d->login && (d->login->status & ST_NOCAP)) {
		consolef("[data_outcap] in NOCAP IP list -- skip\n");
		return;
	}

	// Set session cap?
	if (server_cap_download) {

		//consolef("[data] %p set to %d\n", d->handle, server_cap_download);

		// Either cap setting here works, but the rate_in call is much more
		// efficient
		lion_rate_out( d->handle, server_cap_download );
		//lion_rate_in( d->althandle, server_cap_download );


	}


	// Set user (multiple-sessions) cap.
	if (server_cap_user_download) {

		//lion_group_add(d->althandle, d->login->cap_group);
		lion_group_add(d->handle, d->login->cap_group);
		//consolef("[data] rate_in assigning to group %d\n",
		//	 d->login->cap_group);

	}

	if (server_cap_total_download && data_global_rate_cap_group) {

		//lion_group_add(d->althandle, data_global_rate_cap_group);
		lion_group_add(d->handle, data_global_rate_cap_group);

	}

}
