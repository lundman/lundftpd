#if HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "global.h"
#include "socket.h"
#include "misc.h"

#include "login.h"
#include "data.h"
#include "check.h"
#include "quota.h"
#include "groups.h"
#include "file.h"
#include "global.h"
#include "userflag.h"
#include "tops.h"
#include "dupe.h"
#include "log.h"
#include "sitecust.h"
#include "section.h"

#ifdef IRCBOT
#include "irc.h"
#endif

#include "lion.h"
#include "version.h"


void socket_number(struct login_node *user, int number, char *opt)
{
  struct login_node *remote;
  struct dupe_node *dupe = (struct dupe_node *) opt;
  static char msg[DATA_BUFSZ_STATIC];
  int i;
  time_t tt;

  time(&tt);

  if (!user->handle) return;


  if (!(user->options & UO_COLOUR)) { /* if no colour */

    switch(number) {

    case 200:
      if (opt)
	sprintf(msg, "200 %s command successful.\r\n", opt);
      else
	sprintf(msg, "200 command successful.\r\n");
      break;

    case 1200:  /* NUKE COMMAND */
      sprintf(msg, "200-Deleting files: %s.\r\n", opt);
      break;

    case 2200:  /* NUKE COMMAND */
      sprintf(msg, "200-Renaming directory: %s.\r\n", opt);
      break;


    case 3200:
      local_user_top_up_day_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3210:
      local_user_top_down_day_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3220:
      local_user_top_up_week_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3230:
      local_user_top_down_week_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3240:
      local_user_top_up_month_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3250:
      local_user_top_down_month_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3260:
      local_user_top_up_alltime_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3270:
      local_user_top_down_alltime_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3300:
      local_group_top_up_day_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3310:
      local_group_top_down_day_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3320:
      local_group_top_up_week_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3330:
      local_group_top_down_week_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3340:
      local_group_top_up_month_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3350:
      local_group_top_down_month_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3360:
      local_group_top_up_alltime_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3370:
      local_group_top_down_alltime_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3280:
      local_user_top_up_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3290:
      local_user_top_down_mono(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 4200:  /* MSG */
      sprintf(msg, "200 You have no messages.\r\n");
      break;

    case 6200:  /* DUPE BODY */
		if (dupe->filesize == DUPE_IS_DIR)
			sprintf(msg,
					"| %-47.47s | %5s | %-7.7s | %-8.8s |\r\n",
					dupe->filename,
					"<dir>",
					idletime2( global_time_now - dupe->time),
					dupe->username );
		else
			sprintf(msg,
					"| %-47.47s |%6.1f | %-7.7s | %-8.8s |\r\n",
					dupe->filename,
					dupe->filesize / 1048576.0,
					idletime2( global_time_now - dupe->time),
					dupe->username );
		break;


    case 6300:  /* DUPE TOP */
lion_printf(user->handle, "200- [ DUPE ]\r\n");
lion_printf(user->handle, "+-------------------------------------------------+-------+---------+----------+\r\n");
lion_printf(user->handle, "| FILENAME                                        | MBYTE | AGE     | UPLOADER |\r\n");
              sprintf(msg, "+-------------------------------------------------+-------+---------+----------+\r\n");
    break;


    case 6400:  /* DUPE BOTTOM */
lion_printf(user->handle, "+-------------------------------------------------+-------+---------+----------+\r\n");
              sprintf(msg, "200 DUPE command successful\r\n");
    break;





	case 6500: /* SITE NEW */
		lion_printf(user->handle, "200- [ NEW ]\r\n");
		lion_printf(user->handle, "+--------------+--------+------------------------------------------------------+\r\n");
		lion_printf(user->handle, "| %-12.12s | %6.6s | %-52.52s |\r\n",
					 "USERNAME", "AGE", "NAME");
		lion_printf(user->handle, "+--------------+--------+------------------------------------------------------+\r\n");

		for(i = NUM_NEW - 1 ; i >= 0; i--) {
			if (global_newn[i] != NULL) {

				// No disrespect at all - just showing APIs
				// (not sure I can call this mess and API)
				lion_printf(user->handle, "| %-12.12s | %6.6s | %-52.52s |\r\n",
							 global_newu[i],
							 idletime(global_time_now - global_newd[i]),
							 global_newn[i]);

			}
		}

		lion_printf(user->handle, "+--------------+--------+------------------------------------------------------+\r\n");
		sprintf(msg, "200 NEW command successful.\r\n");
		break;







    case 214: /* SITE HELP */

lion_printf(user->handle, "200- [ HELP ]\r\n");

#ifdef ASCII_LOGO
lion_printf(user->handle,
"                       ____________________     __________\r\n");
lion_printf(user->handle,
"                      /    /    /  ___\\    \\   /_______   \\\r\n");
lion_printf(user->handle,
"                     /   __    /   ____\\    \\_/__\\    _____\\\r\n");
lion_printf(user->handle,
"                    /    /    /    |    \\    \\    \\    \\  /\r\n");
lion_printf(user->handle,
"                   /____/___ /     `     \\ ________\\____\\/\r\n");
lion_printf(user->handle,
"                   \\    \\  \\/_____________\\/       /    /\r\n");
lion_printf(user->handle,
"                    \\____\\__\\             /_______/____/\r\n");
lion_printf(user->handle,
"                             \\___________/ \r\n");
#endif

lion_printf(user->handle, ".------------------------------------------------------------------------------.\r\n");
lion_printf(user->handle, "| SPECIAL SITE COMMANDS   (+ =>'s gadmin command, * =>'s admin command)        |\r\n");
lion_printf(user->handle, "|------------------------------------------------------------------------------|\r\n");
lion_printf(user->handle, "|-[ GENERAL COMMANDS ]---------------------------------------------------------|\r\n");
lion_printf(user->handle, "| COLOUR <ON/OFF>          - Turn colours on/off                               |\r\n");
lion_printf(user->handle, "| EXTRA <ON/OFF>           - Turn extra messages and info on/off               |\r\n");
lion_printf(user->handle, "| GIVE <USER> <AMOUNT>     - Give <AMOUNT> MB of your credits to <USER>        |\r\n");
lion_printf(user->handle, "| HELP <COMMAND>           - Show this screen, show more help about <COMMAND>  |\r\n");
lion_printf(user->handle, "| INCOMPLETES              - Show currently incomplete releases                |\r\n");
/*lion_printf(user->handle, "| LAST <USER>              - Show last <X> users who logged in                 |\r\n");*/ // FIXME!!
lion_printf(user->handle, "| MSG <USER> <MESSAGE>     - Read message(s), send <MESSAGE> to <USER>         |\r\n");
lion_printf(user->handle, "| NEW <LAST/YYMMDD/##>     - Show new uploads                                  |\r\n");
lion_printf(user->handle, "| PASSWD <OLD> <NEW>       - Change password from <OLD> to <NEW>               |\r\n");
lion_printf(user->handle, "| RACES                    - Show current races                                |\r\n");
lion_printf(user->handle, "| SECTION                  - Show current section and settings.                |\r\n");
lion_printf(user->handle, "| TAGLINE <TAGLINE>        - Show tagline; set tagline to <TAGLINE>            |\r\n");
lion_printf(user->handle, "| TOPUP <D/W/M/A>          - Show usertop upload <DAY/WEEK/MONTH/ALLTIME>      |\r\n");
lion_printf(user->handle, "| TOPDN <D/W/M/A>          - Show usertop download <DAY/WEEK/MONTH/ALLTIME>    |\r\n");
lion_printf(user->handle, "| WALL <MESSAGE>           - Show messagewall, write <MESSAGE> on messagewall  |\r\n");
lion_printf(user->handle, "| WHO                      - Show online users                                 |\r\n");
lion_printf(user->handle, "| REQUEST <DIR>            - Request <DIR> or list requests                    |\r\n");
lion_printf(user->handle, "| REQFILLED <DIR>          - Announce completed request                        |\r\n");
lion_printf(user->handle, "|-[ ADMIN COMMANDS ]-----------------------------------------------------------|\r\n");
lion_printf(user->handle, "|*CHOWN <USER> <PATH>      - Change owner of file or directory                 |\r\n");
lion_printf(user->handle, "|*NUKE <DIR> <MULTI> <RSN> - Nuke a release                                    |\r\n");
lion_printf(user->handle, "|*UNNUKE <DIR> <MUL> <RSN> - UnNuke a release                                  |\r\n");
lion_printf(user->handle, "|*REHASH                   - Re-read configuration file                        |\r\n");
lion_printf(user->handle, "|*TCPSTAT                  - Show active TCPIP connections                     |\r\n");
lion_printf(user->handle, "|*DUPE [+/-]<FILE>         - List, +add or -remove <FILE> from dupe-database   |\r\n");
lion_printf(user->handle, "|-[ USER RELATED COMMANDS ]----------------------------------------------------|\r\n");
lion_printf(user->handle, "| USER [USER]              - Show all users, show info about <USER>            |\r\n");
lion_printf(user->handle, "|+ADDUSER <USR> <PW> <IPS> - Create a new user                                 |\r\n");
lion_printf(user->handle, "|+DELUSER <USR>            - Delete an existing user                           |\r\n");
lion_printf(user->handle, "|+RENUSER <USR> <NEW>      - Rename an existing user                           |\r\n");
lion_printf(user->handle, "|+SETLIMIT <USR> <LDU> <#> - Set user's <login|down|up> limit                  |\r\n");
lion_printf(user->handle, "|*SETFLAGS <USR> <UF>      - Set user's userflag level                         |\r\n");
lion_printf(user->handle, "|+SETPASS <USER> <NEWPASS> - Set user's password                               |\r\n");
lion_printf(user->handle, "|+SETRATIO <USER> <RATIO>  - Set user's ul/dl ratio, -1 = no ratio             |\r\n");
lion_printf(user->handle, "|+SETIP <USER> <#> <ID@IP> - Set user's ident@ip pattern on entry <#>          |\r\n");
lion_printf(user->handle, "|*SETCRED <USER> +/-<AMNT> - Set, +add or -remove <AMNT>MB credits from <USER> |\r\n");
lion_printf(user->handle, "|*KICK <USER>              - Kick user off the server                          |\r\n");
lion_printf(user->handle, "|-[ GROUP RELATED COMMANDS ]---------------------------------------------------|\r\n");
lion_printf(user->handle, "| CHGRP <GROUP>            - Show available groups, set active group           |\r\n");
lion_printf(user->handle, "|+GROUPINFO <GROUP>        - Show info about group                             |\r\n");
lion_printf(user->handle, "|*GROUPLIST                - Show groups                                       |\r\n");
lion_printf(user->handle, "|*GROUPADD <GROUP> [...]   - Add a new group                                   |\r\n");
lion_printf(user->handle, "|*GROUPDEL <GROUP>         - Delete a group                                    |\r\n");
lion_printf(user->handle, "|*GROUPUSER <GRP> +-<USER> - Add/remove a user from a group                    |\r\n");
lion_printf(user->handle, "|*GTAGLINE <TAGLINE>       - Display & set active group's tagline to <TAGLINE> |\r\n");
lion_printf(user->handle, "|*GTOPUP <D/W/M/A>         - Show grouptop uploads <DAY/WEEK/MONTH/ALLTIME>    |\r\n");
lion_printf(user->handle, "|*GTOPDN <D/W/M/A>         - Show grouptop downloads <DAY/WEEK/MONTH/ALLTIME>  |\r\n");
// Display SITECUST commands, if any.
sitecust_help(user);
lion_printf(user->handle, "|------------------------------------------------------------------------------|\r\n");
lion_printf(user->handle, "| Type SITE HELP <COMMAND> for more info about <COMMAND>                       |\r\n");
lion_printf(user->handle, "`------------------------------------------------------------------------------'\r\n");

   sprintf(msg, "200 Direct comments to %s.\r\n", server_email);
   break;


    case 215:
	  sprintf(msg, "215 UNIX Type: L8 Version: hard2get\r\n");
      break;

    case 220: /* Initial greeting, man ctime() is annoying. */
		if (server_rawgreeting) {
			strcpy(msg, server_rawgreeting);
			strcat(msg, "\r\n");
		} else {

			sprintf(msg, "220 %s FTP server (%s build %u %s %s",
					server_greeting ? server_greeting : lion_ntoa(localhost),
					VERSION_STR,
					VERSION_BUILD,
#ifdef WITH_SSL
					"+TLS"
#endif
					" +LiON",
					ctime(&tt));

			chop(msg);
			strcat(msg, ") ready.\r\n");
		}
      break;

    case 221:

      if (user->status & ST_LOGIN) {


	lion_printf(user->handle, "221- [ LOGOUT ]\r\n");
#ifdef ACC
	lion_printf(user->handle, "   /\\__________/\\______/\\______/\\______/\\______/\\______/\\__________/\\______ :  \r\n");
	lion_printf(user->handle, "  / /\\_________\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_\\___\\___\\/\\_____\\|__\r\n");
	lion_printf(user->handle, "  \\/________ _ /___    /___    /   ____/   ____/   ____/__ /   /   /   ____/|  \r\n");
	lion_printf(user->handle, "  : / /      //.  /___/.  /___/.  ____/___    /___    /   /   /   /.  ____/ :  \r\n");
	lion_printf(user->handle, "__|/ /   /   //  /   //  /   //  /   /   /   /   /   /   /_  /   //  /   /     \r\n");
	lion_printf(user->handle, "  |\\/___/   /\\_____ /\\_____ /\\_____ /______ /______ /___/\\/____/~\\_____ /      \r\n");
	lion_printf(user->handle, "  .:::\\/____ /::::\\/::::::\\/::::::\\/::::::\\/::::::\\/::::::::::::::::::\\/       \r\n");
	lion_printf(user->handle, "           \\/                                                                  \r\n");
	lion_printf(user->handle, "+ ---------------------[ Thanks For Visiting Accessive ]---------------------- +\r\n");

#elif defined ENIGMA
        lion_printf(user->handle, "                              ___/\\___                                          \r\n");
        lion_printf(user->handle, "      ___________/\\______  ___\\      /__________________  __/\\_________/\\__     \r\n");
        lion_printf(user->handle, "      \\__    _______     \\|  __\\____/__    ________     \\/    ___   _   __/     \r\n");
        lion_printf(user->handle, "  >>>> /   ____/  \\       \\   \\       \\    \\|    \\/            \\    |    \\>>>>  \r\n");
        lion_printf(user->handle, "      /      /     \\_  \\       \\_      \\_   |     \\_  \\    /    \\_        \\_    \r\n");
        lion_printf(user->handle, "      \\_____________/___\\___  __/_______/______  __/___\\  /|_____/__|______/    \r\n");
        lion_printf(user->handle, "                            \\/                 \\/       \\/                      \r\n");
	lion_printf(user->handle, "+ -----------------------[ Thanks For Visiting Enigma ]----------------------- +\r\n");
#endif



	lion_printf(user->handle, "   Hey %-8s\r\n",user->user);

	/*
	  lion_printf(user->handle, "   You Uploaded:   1234.5 Mb in 1234 file(s) at  123.0  Average k/sec. \r\n");
	  lion_printf(user->handle, "   You Downloaded: 1234.5 Mb in 1234 file(s) at  123.0  Average k/sec. \r\n");
	  lion_printf(user->handle, "   You Nuked:      1234.5 Mb in 1234 file(s).                          \r\n");
        */



/*
	lion_printf(user->handle, "   You have been online for %s.\r\n",
	idletime(global_time_now - remote->time) );
*/
	strcpy(genbuf, ctime(&tt));
	chop(genbuf);
	lion_printf(user->handle, "   You Logged Out On: %s.\r\n", genbuf);

	lion_printf(user->handle, "+ ============================================================================ +\r\n");
	lion_printf(user->handle, "   %s is waiting to see you soon %-8s\r\n",server_greeting ? server_greeting : localhost_a, user->user);
	lion_printf(user->handle, "+ ---------------------------------------------------------------------------- +\r\n");

      }

      sprintf(msg,"221 Goodbye.\r\n");
      break;

    case 226:
    {
     struct quota_node *quota;


     if ((quota = quota_getquota(user)) && user->options & UO_FANCY)
     {

      if (user->status & ST_MESSAGE) lion_printf(user->handle, "226- You have new messages! Type SITE MSG to read.\r\n");

      if (USER_RATIO < 0)

      sprintf(msg, "226 [ UP: %s ]=[ DN: %s ]=[ RATIO: NONE ]=[ CREDITS: UNLIMITED ]\r\n",
			  misc_bytestr(USER_BU),
			  misc_bytestr(USER_BD));

      else

      sprintf(msg, "226 [ UP: %s ]=[ DN: %s ]=[ RATIO: 1:%d ]=[ CREDITS: %s ]\r\n",
			  misc_bytestr(USER_BU),
			  misc_bytestr(USER_BD),
			  USER_RATIO,
			  misc_bytestr(USER_CRD));

     } else {

     sprintf(msg, "226 Transfer complete.\r\n");

     }
    }
    break;

		case 230:  /* Login */
			if (server_showlastlogin) {

				local_login(user);

				file_checkmessage(user, 230);

				if (server_wallpath && server_walllogin)
					socket_number(user, 240, "230");

				file_hasmessages(user);

			}

      sprintf(msg, "230 User %s logged in.\r\n", opt);
      break;

    case 1230:
      sprintf(msg, "230-%s has messages waiting. (SITE MSG)\r\n",
	      opt);
      break;


    case 240:
                            lion_printf(user->handle, "200- [ WALL ]\r\n");

                            #ifdef ASCII_LOGO
                            lion_printf(user->handle, "                   ________ _______________     _____\r\n");
                            lion_printf(user->handle, "                  /___    //    /  _  \\    \\   /\\    \\\r\n");
                            lion_printf(user->handle, "                  \\ /    //    /   _   \\    \\_/__\\    \\_____\r\n");
                            lion_printf(user->handle, "                   /    //    /    |    \\\\   \\    \\\\   \\    \\\r\n");
                            lion_printf(user->handle, "                  /____/\\___ //    |     \\.________\\.________\\\r\n");
                            lion_printf(user->handle, "                  \\    \\ \\ \\/______|______\\/       //        /\r\n");
                            lion_printf(user->handle, "                   \\____\\/\\_\\      |      /_______//________/\r\n");
                            lion_printf(user->handle, "                             \\_____|_____/ \r\n");
                            #endif

                            lion_printf(user->handle, "+------------------------------------------------------------------------------+\r\n");
        if (global_mwm[0])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[0], global_mwm[0]);
        if (global_mwm[1])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[1], global_mwm[1]);
        if (global_mwm[2])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[2], global_mwm[2]);
        if (global_mwm[3])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[3], global_mwm[3]);
        if (global_mwm[4])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[4], global_mwm[4]);
        if (global_mwm[5])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[5], global_mwm[5]);
        if (global_mwm[6])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[6], global_mwm[6]);
        if (global_mwm[7])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[7], global_mwm[7]);
        if (global_mwm[8])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[8], global_mwm[8]);
        if (global_mwm[9])  lion_printf(user->handle, " <%s> %s\r\n", global_mwu[9], global_mwm[9]);
        if (global_mwm[10]) lion_printf(user->handle, " <%s> %s\r\n", global_mwu[10], global_mwm[10]);
        if (global_mwm[11]) lion_printf(user->handle, " <%s> %s\r\n", global_mwu[11], global_mwm[11]);
                            lion_printf(user->handle, "+------------------------------------------------------------------------------+\r\n");
                            lion_printf(user->handle, " Type SITE WALL <MESSAGE> to post a message.\r\n");
    sprintf(msg, "200-\r\n");
      break;

    case 250:
      if (opt)
	sprintf(msg, "250 RNTO command successful.\r\n");
      else
	sprintf(msg, "250 CWD command successful.\r\n");
      break;

    case 257:
      if (opt)
	sprintf(msg, "257 \"%s\" is current directory.\r\n", opt);
      break;

    case 269:
      sprintf(msg, "269 KICK of user %s complete.\r\n", opt);
      break;

    case 280:
       if (opt)
        sprintf(msg, "280 Your current active group is '%s'\r\n", opt);
       else
	sprintf(msg, "280 You are not in any groups.\r\n");
      break;

    case 290:
      sprintf(msg, "290 Added member to group %s.\r\n", opt);
      break;

    case 1290:
      sprintf(msg, "290 Removed member from group %s.\r\n", opt);
      break;

    case 331:
      if (!opt)
	sprintf(msg, "331 Guest login ok, send your complete e-mail address as password.\r\n");
      else
	sprintf(msg, "331 Password required for %s.\r\n", opt);
      break;

    case 350:
      sprintf(msg, "350 File exists, ready for destination name\r\n");
      break;

    case 421:
      sprintf(msg, "421 Service not available, remote server has closed connection\r\n");
      break;

    case 425:
      if (!opt)
	sprintf(msg, "425 Can't build data connection: Connection refused.\r\n");
      else
	sprintf(msg, "425 Can't build data connection: %s\r\n", opt);
      break;

    case 500: /* UNrecognise command */
      sprintf(msg, "500 '%s': command not understood.\r\n", (opt?opt:""));
      break;

    case 502:
      sprintf(msg, "502 Sorry, no help for '%s'.\r\n", opt);
      break;

    case 503:
      sprintf(msg, "503 Login with USER first.\r\n");
      break;

    case 1503:
      sprintf(msg, "503 PRET failed: %s\r\n", opt ? opt : "");
      break;

    case 530:
      if (opt)
	sprintf(msg, "530 User %s unknown.\r\n", opt);
      else
	sprintf(msg, "530 Login incorrect.\r\n");
      break;

    case 5301: /* Bah lamers keep using same number */
      sprintf(msg, "530 Please login with USER and PASS.\r\n");
      break;

    case 550:
      sprintf(msg, "550 %s: No such file OR directory.\r\n",
	      (opt?opt:""));
      break;

    case 553:
      sprintf(msg, "553 %s: Can't open for writing\r\n", opt);
      break;

    case 1553:
	sprintf(msg, "553 Dupe check failed on %s (%s,%lu).\r\n",
		dupe->filename, dupe->username, dupe->filesize);
      break;

    case 560:
      sprintf(msg, "560 Message System Disabled.\r\n");
      break;

    case 561:
      sprintf(msg, "561 %s exists in DB, duplicate entries denied.\r\n", opt);
      break;

    case 562:
      sprintf(msg, "562 %s is not present in DB\r\n", opt);
      break;

    case 567:
      sprintf(msg, "567 User %s already present.\r\n", opt);
      break;

    case 568:
      sprintf(msg, "568 User %s not found.\r\n", opt);
      break;

    case 569:
      sprintf(msg, "569 You have been kicked off the server by admin %s.\r\n",
	      opt);
      break;

    case 580:
      sprintf(msg, "580 No such group %s.\r\n", opt);
      break;

    case 581:
      sprintf(msg, "581 You are not a member of group %s\r\n",
	      opt);
      break;

    case 582:
      sprintf(msg, "582 No groups defined on this site.\r\n");
      break;

    case 583:
      sprintf(msg, "583 No more %s available in group.\r\n", opt);
      break;

    case 598: /* Non-standard */
      sprintf(msg, "598 Internal FTPD occured: %s.\r\n", opt);
      break;

    case 599: /* Non-standard */
      sprintf(msg, "599 Special privilege level too low for %s.\r\n", opt);
      break;

    default:
      sprintf(msg, "%03d Cool, code asked for message #%d which I don't know.\r\n",
	      number, number);
    }



    /* C O L O U R */
    /* C O L O U R */
    /* C O L O U R */
    /* C O L O U R */
  } else { /* colour */
    /* C O L O U R */
    /* C O L O U R */
    /* C O L O U R */
    /* C O L O U R */



    switch(number) {

    case 200: /* Colour version */
      if (opt)
	sprintf(msg, "200 %s command successful.\r\n", opt);
      else
	sprintf(msg, "200 command successful.\r\n");
      break;

    case 1200:  /* NUKE COMMAND */
      sprintf(msg, "200-Deleting files: %s.\r\n", opt);
      break;

    case 2200:  /* NUKE COMMAND */
      sprintf(msg, "200-Renaming directory: %s.\r\n", opt);
      break;

    case 3200:
      local_user_top_up_day_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;




    case 3210:
      local_user_top_down_day_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3220:
      local_user_top_up_week_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3230:
      local_user_top_down_week_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3240:
      local_user_top_up_month_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3250:
      local_user_top_down_month_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3260:
      local_user_top_up_alltime_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3270:
      local_user_top_down_alltime_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3300:
      local_group_top_up_day_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3310:
      local_group_top_down_day_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3320:
      local_group_top_up_week_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3330:
      local_group_top_down_week_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3340:
      local_group_top_up_month_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3350:
      local_group_top_down_month_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3360:
      local_group_top_up_alltime_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3370:
      local_group_top_down_alltime_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3280:
      local_user_top_up_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;

    case 3290:
      local_user_top_down_colour(user, number, opt);
      sprintf(msg, "200-\r\n");
      break;






    case 4200:  /* MSG */
      sprintf(msg, "200 You have no messages.\r\n");
      break;


    case 6200:  /* DUPE BODY */
		if (dupe->filesize == DUPE_IS_DIR)
			sprintf(msg,
					"[1;36;44m| %-47.47s | [0;36;44m%5s [1;36;44m| [0;36;44m%-7.7s [1;36;44m| [0;36;44m%-8.8s |[0;40m\r\n",
					dupe->filename,
					"<dir>",
					idletime2( global_time_now - dupe->time),
					dupe->username );
		else
			sprintf(msg,
					"[1;36;44m| %-47.47s |[0;36;44m%6.1f [1;36;44m| [0;36;44m%-7.7s [1;36;44m| [0;36;44m%-8.8s |[0;40m\r\n",
					dupe->filename,
					dupe->filesize / 1048576.0,
					idletime2( global_time_now - dupe->time),
					dupe->username );
		break;


    case 6300:  /* DUPE TOP */
      lion_printf(user->handle, "200- [ DUPE ]\r\n");
      lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
      lion_printf(user->handle, "[1;36;44m| [1;37;44mFILENAME                                        [1;36;44m| [1;37;44mMBYTE [1;36;44m| [1;37;44mAGE     [1;36;44m| [1;37;44mUPLOADER [1;36;44m|[0;40m\r\n");
                    sprintf(msg, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
    break;


    case 6400:  /* DUPE BOTTOM */
      lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
              sprintf(msg, "200 DUPE command successful\r\n");
    break;






	case 6500: /* SITE NEW */
		lion_printf(user->handle, "200- [ NEW ]\r\n");
		lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
		lion_printf(user->handle, "[1;36;44m| [1;37;44m%-12.12s [1;36;44m| [1;37;44m%6.6s [1;36;44m| [1;37;44m%-52.52s [1;36;44m|[0;40m\r\n",
					 "USERNAME", "AGE", "NAME");
		lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");


		for(i = NUM_NEW - 1 ; i >= 0; i--) {
			if (global_newn[i] != NULL) {

				// No disrespect at all - just showing APIs
				// (not sure I can call this mess and API)
				lion_printf(user->handle,
							 "[1;36;44m| %-12.12s | [0;36;44m%6.6s [1;36;44m| [0;36;44m%-52.52s |[0;40m\r\n",
							 global_newu[i],
							 idletime(global_time_now - global_newd[i]),
							 global_newn[i]);

			}
		}

		lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
		sprintf(msg, "200 NEW command successful.\r\n");
		break;




    case 211:
      remote = (struct login_node *)opt;

      if ((remote == user) || (user->level & UF_STAT)) {
	/* It's ourselves, or an admin user doing the stats */


 lion_printf(user->handle, "200- [ USER ]\r\n");

 #ifdef ASCII_LOGO
 lion_printf(user->handle, "[1;37;44m                      ___________________________________                       [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                     /    /    /  ___\\     ____\\______   \\                      [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                    /    /    /____   \\     ____\\       __\\                     [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                   /    /    /    |    \\\\   \\    \\\\   \\    \\                    [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                  /________ //    '     \\.________\\____\\____\\                   [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                  \\       \\/_____________\\/       /    /    /                   [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                   \\_______\\             /_______/____/____/                    [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                            \\___________/                                       [0;40m\r\n");
 #endif

 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
 lion_printf(user->handle, "[1;36;44m| [0;36;44mUsername: [1;36;44m%-8s |[0;36;44m Tagline: [1;36;44m%-46s |[0;40m\r\n", remote->user, remote->tagline);
 lion_printf(user->handle, "[1;36;44m| [0;36;44mActv Grp: [1;36;44m%-8s |[0;36;44m  Groups: [1;36;44m%-46s |[0;40m\r\n", remote->current_group ? ((struct group *)remote->current_group)->name : "None", group_listgroups(remote->user));
 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");


 if (remote->quota) { /* If it has a quota node, do some quota stats */

   /* Make it easier */
   struct quota_node *quota = (struct quota_node *)remote->quota;

   lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m[ [0;36;44mTODAY [1;34;44m][0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m[ [0;36;44mWEEKLY [1;34;44m][0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m[ [0;36;44mMONTHLY [1;34;44m][0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m[ [0;36;44mALLTIME [1;34;44m][0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
   lion_printf(user->handle, "[1;36;44m|[0;36;44m       Uploaded  [1;36;44m|  %6.1f Mb  |  %7.1f Mb  |  %8.1f Mb  |  %8.1f Mb  |[0;40m\r\n", REMOTE_BUD / 1048576.0, REMOTE_BUW / 1048576.0, REMOTE_BUM / 1048576.0, REMOTE_BU / 1048576.0);
   lion_printf(user->handle, "[0;36;44m|     Downloaded  |[1;36;44m  %6.1f Mb  [0;36;44m|[1;36;44m  %7.1f Mb  [0;36;44m|[1;36;44m  %8.1f Mb  [0;36;44m|[1;36;44m  %8.1f Mb  [0;36;44m|[0;40m\r\n", REMOTE_BDD / 1048576.0, REMOTE_BDW / 1048576.0, REMOTE_BDM / 1048576.0, REMOTE_BD / 1048576.0);
   lion_printf(user->handle, "[1;36;44m|[0;36;44m      Got Nuked  [1;36;44m|     0.0 Mb  |      0.0 Mb  |       0.0 Mb  |       0.0 Mb  |[0;40m\r\n");
   lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");


   lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");

   if (quota->ratio < 0) {
   lion_printf(user->handle, "[1;36;44m| Ratio disabled[0;36;44m, [1;36;44m%4d Times Logged In                                         |[0;40m\r\n", quota->login_times);

  } else {

   lion_printf(user->handle, "[1;36;44m| Ratio 1:%d[0;36;44m, [1;36;44mCredits %9.1f Mb[0;36;44m, [1;36;44m%4.1f Mb Free/Day[0;36;44m, [1;36;44m%4d Times Logged In      |[0;40m\r\n",

    REMOTE_RATIO,
    REMOTE_CRD / 1048576.0,
    (float) quota->daily / 1048576.0,
    REMOTE_LT);

          }

 } /* Has quota */



 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
 sprintf(genbuf, "[1;36;44m%d %s[0;36;44m, [1;36;44m%d %s [0;36;44mand [1;36;44m%d %s", remote->num_logins,  remote->num_logins  > 1 ? "logins" : "login", remote->num_uploads, remote->num_uploads > 1 ? "uploads" : "upload", remote->num_downloads,remote->num_downloads>1?"downloads" : "download");
 lion_printf(user->handle, "[1;36;44m| [0;36;44mSession Limits: %-105.105s      |[0;40m\r\n", genbuf);
 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");

	if (server_usermatch) {
	  for (i=0; i < IPMATCHMAX; i++) {
	    if (!remote->matchip[i][0] || remote->matchip[i][0] == '\r' ||
		remote->matchip[i][0] == '\n') continue;
	    lion_printf(user->handle, "[1;36;44m| [0;36;44mIP Pattern %d: [1;36;44m%-62s |[0;40m\r\n",
			 i + 1, remote->matchip[i]);
	  }
	}


 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");

      if (!remote->idle) {
 lion_printf(user->handle, "[1;36;44m| User has never logged in.                                                    |[0;40m\r\n");

      } else {


	strcpy(genbuf, ctime(&remote->time));
	chop(genbuf);

 lion_printf(user->handle, "[1;36;44m| [0;36;44mLast Login: [1;36;44m%-29s", genbuf);


 sprintf(genbuf, "%s@%s",remote->ident, lion_ntoa(remote->host));
 lion_printf(user->handle, " [0;36;44mfrom [1;36;44m%-29.29s |[0;40m\r\n", genbuf);



	if (login_isloggedin(remote->user)) {

	  lion_printf(user->handle, "[1;36;44m| User is logged in.                                                           |[0;40m\r\n");

	} else {



	  strcpy(genbuf, ctime(&remote->logoff_time));
	  chop(genbuf);

	  lion_printf(user->handle, "[1;36;44m| [0;36;44mLast Logout: [1;36;44m%s", genbuf);

 sprintf(genbuf, "%s ago", idletime(time(NULL) - remote->idle));
 lion_printf(user->handle, " [0;36;44mwhich was [1;36;44m%-23.23s      |[0;40m\r\n", genbuf);



	}
      }

 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");




      } else {


/* WHOIS not ourselves, not admin */

 lion_printf(user->handle, "200- [ USER ]\r\n");

 #ifdef ASCII_LOGO
 lion_printf(user->handle, "[1;37;44m                      ___________________________________                       [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                     /    /    /  ___\\     ____\\______   \\                      [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                    /    /    /____   \\     ____\\       __\\                     [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                   /    /    /    |    \\\\   \\    \\\\   \\    \\                    [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                  /________ //    '     \\.________\\____\\____\\                   [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                  \\       \\/_____________\\/       /    /    /                   [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                   \\_______\\             /_______/____/____/                    [0;40m\r\n");
 lion_printf(user->handle, "[1;37;44m                            \\___________/                                       [0;40m\r\n");
 #endif

 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
 lion_printf(user->handle, "[1;36;44m| [0;36;44mUsername: [1;36;44m%-8s |[0;36;44m Tagline: [1;36;44m%-46s |[0;40m\r\n", remote->user, remote->tagline);
 lion_printf(user->handle, "[1;36;44m| [0;36;44mActv Grp: [1;36;44m%-8s |[0;36;44m  Groups: [1;36;44m%-46s |[0;40m\r\n", remote->current_group ? ((struct group *)remote->current_group)->name : "None", group_listgroups(remote->user));
 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");


 /* We have to look up quota here! */
 if (remote->quota) { /* If it has a quota node, do some quota stats */

   lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m[ [0;36;44mTODAY [1;34;44m][0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m[ [0;36;44mWEEKLY [1;34;44m][0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m[ [0;36;44mMONTHLY [1;34;44m][0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m[ [0;36;44mALLTIME [1;34;44m][0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
   lion_printf(user->handle, "[1;36;44m|[0;36;44m       Uploaded  [1;36;44m|  %6.1f Mb  |  %7.1f Mb  |  %8.1f Mb  |  %8.1f Mb  |[0;40m\r\n", REMOTE_BUD / 1048576.0, REMOTE_BUW / 1048576.0, REMOTE_BUM / 1048576.0, REMOTE_BU / 1048576.0);
   lion_printf(user->handle, "[0;36;44m|     Downloaded  |[1;36;44m  %6.1f Mb  [0;36;44m|[1;36;44m  %7.1f Mb  [0;36;44m|[1;36;44m  %8.1f Mb  [0;36;44m|[1;36;44m  %8.1f Mb  [0;36;44m|[0;40m\r\n", REMOTE_BDD / 1048576.0, REMOTE_BDW / 1048576.0, REMOTE_BDM / 1048576.0, REMOTE_BD / 1048576.0);
   lion_printf(user->handle, "[1;36;44m|[0;36;44m      Got Nuked  [1;36;44m|     0.0 Mb  |      0.0 Mb  |       0.0 Mb  |       0.0 Mb  |[0;40m\r\n");
   lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");

 }

 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");

      if (!remote->idle) {
 lion_printf(user->handle, "[1;36;44m| User has never logged in.                                                    |[0;40m\r\n");

      } else {


	strcpy(genbuf, ctime(&remote->time));
	chop(genbuf);

 lion_printf(user->handle, "[1;36;44m| [0;36;44mLast Login: [1;36;44m%-29s", genbuf);

	if (login_isloggedin(remote->user)) {

	  lion_printf(user->handle, "[0;36;44m, [1;36;44muser is logged in.                |[0;40m\r\n",
		       remote->user);

	} else {

 sprintf(genbuf, "%s ago", idletime(time(NULL) - remote->time));
 lion_printf(user->handle, " [0;36;44mwhich was [1;36;44m%-24.24s |[0;40m\r\n", genbuf);



	}
      }

 lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");


}


      /* This should be the last one */

      sprintf(msg, "200 WHOIS command successful\r\n");

      break;

    case 214: /* SITE HELP */

lion_printf(user->handle, "200- [ HELP ]\r\n");

#ifdef ASCII_LOGO
lion_printf(user->handle, "[1;37;44m                       ____________________     __________                      [0;40m\r\n");
lion_printf(user->handle, "[1;37;44m                      /    /    /  ___\\    \\   /_______   \\                     [0;40m\r\n");
lion_printf(user->handle, "[1;37;44m                     /   __    /   ____\\    \\_/__\\    _____\\                    [0;40m\r\n");
lion_printf(user->handle, "[1;37;44m                    /    /    /    |    \\    \\    \\    \\  /                     [0;40m\r\n");
lion_printf(user->handle, "[1;37;44m                   /____/___ /     `     \\ ________\\____\\/                      [0;40m\r\n");
lion_printf(user->handle, "[1;37;44m                   \\    \\  \\/_____________\\/       /    /                       [0;40m\r\n");
lion_printf(user->handle, "[1;37;44m                    \\____\\__\\             /_______/____/                        [0;40m\r\n");
lion_printf(user->handle, "[1;37;44m                             \\___________/                                      [0;40m\r\n");
#endif

lion_printf(user->handle, ".------------------------------------------------------------------------------.\r\n");
lion_printf(user->handle, "| SPECIAL SITE COMMANDS   (+ =>'s gadmin command, * =>'s admin command)        |\r\n");
lion_printf(user->handle, "|------------------------------------------------------------------------------|\r\n");
lion_printf(user->handle, "|-[ GENERAL COMMANDS ]---------------------------------------------------------|\r\n");
lion_printf(user->handle, "| COLOUR <ON/OFF>          - Turn colours on/off                               |\r\n");
lion_printf(user->handle, "| EXTRA <ON/OFF>           - Turn extra messages and info on/off               |\r\n");
lion_printf(user->handle, "| GIVE <USER> <AMOUNT>     - Give <AMOUNT> MB of your credits to <USER>        |\r\n");
lion_printf(user->handle, "| HELP <COMMAND>           - Show this screen, show more help about <COMMAND>  |\r\n");
lion_printf(user->handle, "| INCOMPLETES              - Show currently incomplete releases                |\r\n");
lion_printf(user->handle, "| LAST <USER>              - Show last <X> users who logged in                 |\r\n");
lion_printf(user->handle, "| MSG <USER> <MESSAGE>     - Read message(s), send <MESSAGE> to <USER>         |\r\n");
lion_printf(user->handle, "| NEW <LAST/YYMMDD/##>     - Show new uploads                                  |\r\n");
lion_printf(user->handle, "| PASSWD <OLD> <NEW>       - Change password from <OLD> to <NEW>               |\r\n");
lion_printf(user->handle, "| RACES                    - Show current races                                |\r\n");
lion_printf(user->handle, "| SECTION                  - Show current section and settings.                |\r\n");
lion_printf(user->handle, "| TAGLINE <TAGLINE>        - Show tagline; set tagline to <TAGLINE>            |\r\n");
lion_printf(user->handle, "| TOPUP <D/W/M/A>          - Show usertop upload <DAY/WEEK/MONTH/ALLTIME>      |\r\n");
lion_printf(user->handle, "| TOPDN <D/W/M/A>          - Show usertop download <DAY/WEEK/MONTH/ALLTIME>    |\r\n");
lion_printf(user->handle, "| WALL <MESSAGE>           - Show messagewall, write <MESSAGE> on messagewall  |\r\n");
lion_printf(user->handle, "| WHO                      - Show online users                                 |\r\n");
lion_printf(user->handle, "| REQUEST <DIR>            - Request <DIR> or list requests                    |\r\n");
lion_printf(user->handle, "| REQFILLED <DIR>          - Announce completed request                        |\r\n");
lion_printf(user->handle, "|-[ ADMIN COMMANDS ]-----------------------------------------------------------|\r\n");
lion_printf(user->handle, "|*CHOWN <USER> <PATH>      - Change owner of file or directory                 |\r\n");
lion_printf(user->handle, "|*NUKE <DIR> <MULTI> <RSN> - Nuke a release                                    |\r\n");
lion_printf(user->handle, "|*UNNUKE <DIR> <MUL> <RSN> - UnNuke a release                                  |\r\n");
lion_printf(user->handle, "|*REHASH                   - Re-read configuration file                        |\r\n");
lion_printf(user->handle, "|*TCPSTAT                  - Show active TCPIP connections                     |\r\n");
lion_printf(user->handle, "|*DUPE [+/-]<FILE>         - List, +add or -remove <FILE> from dupe-database   |\r\n");
lion_printf(user->handle, "|-[ USER RELATED COMMANDS ]----------------------------------------------------|\r\n");
lion_printf(user->handle, "| USER [USER]              - Show all users, show info about <USER>            |\r\n");
lion_printf(user->handle, "|+ADDUSER <USR> <PW> <IPS> - Create a new user                                 |\r\n");
lion_printf(user->handle, "|+DELUSER <USR>            - Delete an existing user                           |\r\n");
lion_printf(user->handle, "|+RENUSER <USR> <NEW>      - Rename an existing user                           |\r\n");
lion_printf(user->handle, "|+SETLIMIT <USR> <LDU> <#> - Set users <login|down|up> limit                   |\r\n");
lion_printf(user->handle, "|+SETPASS <USER> <NEWPASS> - Set users password                                |\r\n");
lion_printf(user->handle, "|+SETRATIO <USER> <RATIO>  - Set users ul/dl ratio, -1 = no ratio              |\r\n");
lion_printf(user->handle, "|+SETIP <USER> <#> <ID@IP> - Set users ident@ip pattern on entry <#>           |\r\n");
lion_printf(user->handle, "|*SETCRED <USER> +/-<AMNT> - Set, +add or -remove <AMNT>MB credits from <USER> |\r\n");
lion_printf(user->handle, "|*KICK <USER>              - Kick user off the server                          |\r\n");
lion_printf(user->handle, "|-[ GROUP RELATED COMMANDS ]---------------------------------------------------|\r\n");
lion_printf(user->handle, "| CHGRP <GROUP>            - Show available groups, set active group           |\r\n");
lion_printf(user->handle, "|+GROUPINFO <GROUP>        - Show info about group                             |\r\n");
lion_printf(user->handle, "|*GROUPLIST                - Show groups                                       |\r\n");
lion_printf(user->handle, "|*GROUPADD <GROUP> [...]   - Add a new group                                   |\r\n");
lion_printf(user->handle, "|*GROUPDEL <GROUP>         - Delete a group                                    |\r\n");
lion_printf(user->handle, "|*GROUPUSER <GRP> +-<USER> - Add/remove a user from a group                    |\r\n");
lion_printf(user->handle, "|*GTAGLINE <TAGLINE>       - Display & set active group's tagline to <TAGLINE> |\r\n");
lion_printf(user->handle, "|*GTOPUP <D/W/M/A>         - Show grouptop uploads <DAY/WEEK/MONTH/ALLTIME>    |\r\n");
lion_printf(user->handle, "|*GTOPDN <D/W/M/A>         - Show grouptop downloads <DAY/WEEK/MONTH/ALLTIME>  |\r\n");
// Display SITECUST commands, if any.
sitecust_help(user);
lion_printf(user->handle, "|------------------------------------------------------------------------------|\r\n");
lion_printf(user->handle, "| Type SITE HELP <COMMAND> for more info about <COMMAND>                       |\r\n");
lion_printf(user->handle, "`------------------------------------------------------------------------------'\r\n");

// TODO FIXME!! colourise help

#if 0
lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------------------------------------------------------------------[1;34;44m-[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
lion_printf(user->handle, "[1;36;44m| [1;37;44mSPECIAL SITE COMMANDS   (* =>'s unimplemented, + =>'s admin command)         [1;36;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------------------------------------------------------------------[1;34;44m-[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+ADDUSER <USR> <PW> <IPS> [1;37;44m- [0;36;44mCreate a new user                                 [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m CHGRP <GROUP>            [1;37;44m- [0;36;44mShow available groups, set active group           [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+CHOWN <USER> <PATH>      [1;37;44m- [0;36;44mChange owner of file or directory                 [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m COLOUR <ON/OFF>          [1;37;44m- [0;36;44mTurn colours on/off                               [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+DUPE [+/-]<FILE>         [1;37;44m- [0;36;44mList, +add or -remove <FILE> from dupe-database   [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m EXTRA <ON/OFF>           [1;37;44m- [0;36;44mTurn extra messages and info on/off               [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m GIVE <USER> <AMOUNT>     [1;37;44m- [0;36;44mGive <AMOUNT> MB of your credits to <USER>        [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m GROUP <GROUP>            [1;37;44m- [0;36;44mShow all groups on site; more info about <GROUP>  [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m*GTAGLINE <TAGLINE>       [1;37;44m- [0;36;44mDisplay & set active group's tagline to <TAGLINE> [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m*GTOPUP <D/W/M/A>         [1;37;44m- [0;36;44mShow grouptop uploads <DAY/WEEK/MONTH/ALLTIME>    [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m*GTOPDN <D/W/M/A>         [1;37;44m- [0;36;44mShow grouptop downloads <DAY/WEEK/MONTH/ALLTIME>  [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m HELP <COMMAND>           [1;37;44m- [0;36;44mShow this screen, show more help about <COMMAND>  [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+KICK <USER>              [1;37;44m- [0;36;44mKick user off the server                          [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m*LAST <USER>              [1;37;44m- [0;36;44mShow last <X> users who logged in                 [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m MSG <USER> <MESSAGE>     [1;37;44m- [0;36;44mRead message(s), send <MESSAGE> to <USER>         [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m WALL <MESSAGE>           [1;37;44m- [0;36;44mShow messagewall, write <MESSAGE> on messagewall  [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m*NEW <LAST/YYMMDD/##>     [1;37;44m- [0;36;44mShow new uploads                                  [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+NUKE <DIR>               [1;37;44m- [0;36;44mRemove contents of <DIR>                          [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m PASSWD <OLD> <NEW>       [1;37;44m- [0;36;44mChange password from <OLD> to <NEW>               [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+REHASH                   [1;37;44m- [0;36;44mRe-read configuration file                        [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+SETCRED <USER> +/-<AMNT> [1;37;44m- [0;36;44mSet, +add or -remove <AMNT>MB credits from <USER> [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+SETGROUP +/-<USER> <GRP> [1;37;44m- [0;36;44mAdd or remove <USER> from group <GRP>             [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+SETPASS <USER> <NEWPASS> [1;37;44m- [0;36;44mSet users password                                [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+SETLIMIT <USR> <LDU> <#> [1;37;44m- [0;36;44mSet users <login|down|up> limit                   [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+SETIP <USER> <#> <ID@IP> [1;37;44m- [0;36;44mSet users ident@ip pattern on entry <#>           [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+SETRATIO <USER> <RATIO>  [1;37;44m- [0;36;44mSet users ul/dl ratio, -1 = no ratio              [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m+TCPSTAT                  [1;37;44m- [0;36;44mShow active TCPIP connections                     [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m TAGLINE <TAGLINE>        [1;37;44m- [0;36;44mShow tagline; set tagline to <TAGLINE>            [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m TOPUP <D/W/M/A>          [1;37;44m- [0;36;44mShow usertop upload <DAY/WEEK/MONTH/ALLTIME>      [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m TOPDN <D/W/M/A>          [1;37;44m- [0;36;44mShow usertop download <DAY/WEEK/MONTH/ALLTIME>    [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m USER <USER>              [1;37;44m- [0;36;44mShow all users, show info about <USER>            [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;34;44m|[1;36;44m WHO                      [1;37;44m- [0;36;44mShow online users                                 [1;34;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------------------------------------------------------------------[1;34;44m-[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
lion_printf(user->handle, "[1;36;44m| [0;36;44mType [1;36;44mSITE HELP <COMMAND> [0;36;44mfor more info about <COMMAND>                       [1;36;44m|[0;40m\r\n");
lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------------------------------------------------------------------[1;34;44m-[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
#endif

   sprintf(msg, "200 Direct comments to %s.\r\n", server_email);

    break;



    case 215:
	sprintf(msg, "215 UNIX Type: L8 Version: hard2get\r\n");
      break;

    case 220: /* Initial greeting, man ctime() is annoying. */
		if (server_rawgreeting) {
			strcpy(msg, server_rawgreeting);
			strcat(msg, "\r\n");
		} else {

			sprintf(msg, "220 %s FTP server (%s build %u %s %s",
					server_greeting ? server_greeting : lion_ntoa(localhost),
					VERSION_STR,
					VERSION_BUILD,
#ifdef WITH_SSL
					"+TLS"
#endif
					" +LiON",
					ctime(&tt));

			chop(msg);
			strcat(msg, ") ready.\r\n");
		}
      break;
      break;

    case 221:
      if (user->status & ST_LOGIN) {


	lion_printf(user->handle, "221- [ LOGOUT ]\r\n");

#ifdef ACC
	lion_printf(user->handle, "[1;37;44m   /\\__________/\\______/\\______/\\______/\\______/\\______/\\__________/\\______ :   [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  / /\\_________\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_\\___\\___\\/\\_____\\|__ [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  \\/________ _ /___    /___    /   ____/   ____/   ____/__ /   /   /   ____/|   [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  : / /      //.  /___/.  /___/.  ____/___    /___    /   /   /   /.  ____/ :   [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m__|/ /   /   //  /   //  /   //  /   /   /   /   /   /   /_  /   //  /   /      [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  |\\/___/   /\\_____ /\\_____ /\\_____ /______ /______ /___/\\/____/~\\_____ /       [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  .:::\\/____ /::::\\/::::::\\/::::::\\/::::::\\/::::::\\/::::::::::::::::::\\/        [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m           \\/                                                                   [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m                                                                                [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m+ [1;36;44m-[0;36;44m-[1;34;44m------------------[0;36;44m-[1;36;44m[ [0;36;44mThanks For Visiting [1;37;44mAccessive [1;36;44m][0;36;44m-[1;34;44m-------------------[0;36;44m-[1;36;44m- [1;37;44m+[0;40m\r\n");
#elif defined ENIGMA
	lion_printf(user->handle, "[1;37;44m                              ___/\\___                                          [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m      ___________/\\______  ___\\      /__________________  __/\\_________/\\__     [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m      \\__    _______     \\|  __\\____/__    ________     \\/    ___   _   __/     [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  >>>> /   ____/  \\       \\   \\       \\    \\|    \\/            \\    |    \\>>>>  [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m      /      /     \\_  \\       \\_      \\_   |     \\_  \\    /    \\_        \\_    [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m      \\_____________/___\\___  __/_______/______  __/___\\  /|_____/__|______/    [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m                            \\/                 \\/       \\/                      [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m+ [1;36;44m-[0;36;44m-[1;34;44m--------------------[0;36;44m-[1;36;44m[ [0;36;44mThanks For Visiting [1;37;44mEnigma [1;36;44m][0;36;44m-[1;34;44m--------------------[0;36;44m-[1;36;44m- [1;37;44m+[0;40m\r\n");
#endif

	lion_printf(user->handle, "[0;36;44m   Hey [1;36;44m%-8s                                                                 [0;40m\r\n",user->user);

	/*
	  lion_printf(user->handle, "   You Uploaded:   1234.5 Mb in 1234 file(s) at  123.0  Average k/sec. \r\n");
	  lion_printf(user->handle, "   You Downloaded: 1234.5 Mb in 1234 file(s) at  123.0  Average k/sec. \r\n");
	  lion_printf(user->handle, "   You Nuked:      1234.5 Mb in 1234 file(s).                          \r\n");
	  lion_printf(user->handle, "   You have been online for <ONLINETIME>.\r\n");
	  */

	strcpy(genbuf, ctime(&tt));
	chop(genbuf);

	lion_printf(user->handle, "[0;36;44m   You Logged Out On: [1;36;44m%-24.24s[0;36;44m.                                 [0;40m\r\n", genbuf);
	lion_printf(user->handle, "[1;37;44m+ [1;36;44m=[0;36;44m=[1;34;44m========================================================================[0;36;44m=[1;36;44m= [1;37;44m+[0;40m\r\n");

        sprintf(genbuf, "%s [0;36;44mis waiting to see you soon [1;36;44m%-8s", server_greeting ? server_greeting : localhost_a, user->user);
	lion_printf(user->handle, "[1;37;44m   %-96.96s [0;40m\r\n", genbuf);

	lion_printf(user->handle, "[1;37;44m+ [1;36;44m-[0;36;44m-[1;34;44m------------------------------------------------------------------------[0;36;44m-[1;36;44m- [1;37;44m+[0;40m\r\n");

      }

      sprintf(msg,"221 Goodbye.\r\n");
      break;

    case 226:
    {
     struct quota_node *quota;

     if ((quota = quota_getquota(user)) && user->options & UO_FANCY)
     {

      if (user->status & ST_MESSAGE) lion_printf(user->handle, "226- You have new messages! Type SITE MSG to read.\r\n");

      if (USER_RATIO < 0)

      sprintf(msg, "226 [1;34;44m[ [1;37;44mUP: [1;36;44m%s [1;34;44m][1;37;44m=[1;34;44m[ [1;37;44mDN: [1;36;44m%s [1;34;44m][1;37;44m=[1;34;44m[ [1;37;44mRATIO: [1;36;44mNONE [1;34;44m][1;37;44m=[1;34;44m[ [1;37;44mCREDITS: [1;36;44mUNLIMITED [1;34;44m][0;40m\r\n",
			  misc_bytestr(USER_BU),
			  misc_bytestr(USER_BD));
      else

      sprintf(msg, "226 [1;34;44m[ [1;37;44mUP: [1;36;44m%s [1;34;44m][1;37;44m=[1;34;44m[ [1;37;44mDN: [1;36;44m%s [1;34;44m][1;37;44m=[1;34;44m[ [1;37;44mRATIO: [1;36;44m1:%d [1;34;44m][1;37;44m=[1;34;44m[ [1;37;44mCREDITS: [1;36;44m%s [1;34;44m][0;40m\r\n",
			  misc_bytestr(USER_BU),
			  misc_bytestr(USER_BD),
			  USER_RATIO,
			  misc_bytestr(USER_CRD));

     } else {

     sprintf(msg, "226 Transfer complete.\r\n");

     }
    }
    break;

    case 230:  /* Login */
      if (server_showlastlogin) {

	local_login(user);

	file_checkmessage(user, 230);

	if (server_wallpath && server_walllogin)
	  socket_number(user, 240, "230");

	file_hasmessages(user);

      }

      sprintf(msg, "230 User %s logged in.\r\n", opt);
      break;

    case 1230:
      sprintf(msg, "230-%s has messages waiting. (SITE MSG)\r\n",
	      opt);
      break;

    case 240:
                            lion_printf(user->handle, "200- [ WALL ]\r\n");

                            #ifdef ASCII_LOGO
                            lion_printf(user->handle, "[1;37;44m                   ________ _______________     _____                           [0;40m\r\n");
                            lion_printf(user->handle, "[1;37;44m                  /___    //    /  _  \\    \\   /\\    \\                          [0;40m\r\n");
                            lion_printf(user->handle, "[1;37;44m                  \\ /    //    /   _   \\    \\_/__\\    \\_____                    [0;40m\r\n");
                            lion_printf(user->handle, "[1;37;44m                   /    //    /    |    \\\\   \\    \\\\   \\    \\                   [0;40m\r\n");
                            lion_printf(user->handle, "[1;37;44m                  /____/\\___ //    |     \\.________\\.________\\                  [0;40m\r\n");
                            lion_printf(user->handle, "[1;37;44m                  \\    \\ \\ \\/______|______\\/       //        /                  [0;40m\r\n");
                            lion_printf(user->handle, "[1;37;44m                   \\____\\/\\_\\      |      /_______//________/                   [0;40m\r\n");
                            lion_printf(user->handle, "[1;37;44m                             \\_____|_____/                                      [0;40m\r\n");
                            #endif

                            lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
        if (global_mwm[0]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[0], global_mwm[0]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[1]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[1], global_mwm[1]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[2]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[2], global_mwm[2]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[3]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[3], global_mwm[3]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[4]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[4], global_mwm[4]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[5]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[5], global_mwm[5]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[6]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[6], global_mwm[6]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[7]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[7], global_mwm[7]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[8]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[8], global_mwm[8]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[9]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[9], global_mwm[9]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[10]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[10], global_mwm[10]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
        if (global_mwm[11]) { sprintf(genbuf, "%s[1;34;44m> [0;36;44m%s", global_mwu[11], global_mwm[11]);
                             lion_printf(user->handle, "[1;34;44m <[1;36;44m%-97.97s [0;40m\r\n", genbuf); }
                            lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------------------------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                            lion_printf(user->handle, "[0;36;44m Type [1;36;44mSITE WALL <MESSAGE> [0;36;44mto post a message.                                    [0;40m\r\n");
      sprintf(msg, "200-\r\n");
      break;

    case 250:
      if (opt)
	sprintf(msg, "250 RNTO command successful.\r\n");
      else
	sprintf(msg, "250 CWD command successful.\r\n");
      break;

    case 257:
      if (opt)
	sprintf(msg, "257 \"%s\" is current directory.\r\n", opt);
      break;

    case 269:
      sprintf(msg, "269 KICK of user %s complete.\r\n", opt);
      break;

    case 280:
      if (opt)
	sprintf(msg, "280 Your current active group is '%s'\r\n", opt);
      else
	sprintf(msg, "280 You are not in any groups.\r\n");
      break;

    case 290:
      sprintf(msg, "290 Added member to group %s.\r\n", opt);
      break;

    case 1290:
      sprintf(msg, "290 Removed member from group %s.\r\n", opt);
      break;

    case 331:
      if (!opt)
	sprintf(msg, "331 Guest login ok, send your complete e-mail address as password.\r\n");
      else
	sprintf(msg, "331 Password required for %s.\r\n", opt);
      break;

    case 350:
      sprintf(msg, "350 File exists, ready for destination name\r\n");
      break;

    case 421:
      sprintf(msg, "421 Service not available, remote server has closed connection\r\n");
      break;

    case 425:
      if (!opt)
	sprintf(msg, "425 Can't build data connection: Connection refused.\r\n");
      else
	sprintf(msg, "425 Can't build data connection: %s\r\n", opt);
      break;

    case 500: /* UNrecognise command */
      sprintf(msg, "500 '%s': command not understood.\r\n", (opt?opt:""));
      break;

    case 502:
      sprintf(msg, "502 Sorry, no help for '%s'.\r\n", opt);
      break;

    case 503:
      sprintf(msg, "503 Login with USER first.\r\n");
      break;

    case 1503:
      sprintf(msg, "503 PRET failed: %s\r\n", opt ? opt : "");
      break;

    case 530:
      if (opt)
	sprintf(msg, "530 User %s unknown.\r\n", opt);
      else
	sprintf(msg, "530 Login incorrect.\r\n");
      break;

    case 5301: /* Bah lamers keep using same number */
      sprintf(msg, "530 Please login with USER and PASS.\r\n");
      break;

    case 550:
      sprintf(msg, "550 %s: No such file OR directory.\r\n",
	      (opt?opt:""));
      break;

    case 553:
      sprintf(msg, "553 %s: Can't open for writing\r\n", opt);
      break;

    case 1553:
	sprintf(msg, "553 Dupe check failed on %s (%s,%lu).\r\n",
		dupe->filename, dupe->username, dupe->filesize);
      break;

    case 560:
      sprintf(msg, "560 Message System Disabled.\r\n");
      break;

    case 561:
      sprintf(msg, "561 %s exists in DB, duplicate entries denied.\r\n", opt);
      break;

    case 562:
      sprintf(msg, "562 %s is not present in DB\r\n", opt);
      break;

    case 567:
      sprintf(msg, "567 User %s already present.\r\n", opt);
      break;

    case 568:
      sprintf(msg, "568 User %s not found.\r\n", opt);
      break;

    case 569:
      sprintf(msg, "569 You have been kicked off the server by admin %s.\r\n",
	      opt);
      break;

    case 580:
      sprintf(msg, "580 No such group %s.\r\n", opt);
      break;

    case 581:
      sprintf(msg, "581 You are not a member of group %s\r\n",
	      opt);
      break;

    case 582:
      sprintf(msg, "582 No groups defined on this site.\r\n");
      break;

    case 583:
      sprintf(msg, "583 No more %s available in group.\r\n", opt);
      break;

    case 598: /* Non-standard */
      sprintf(msg, "598 Internal FTPD occured: %s.\r\n", opt);
      break;

    case 599: /* Non-standard */
      sprintf(msg, "599 Special priviledge level too low for %s.\r\n", opt);
      break;

    default:
      sprintf(msg, "%03d Cool, code asked for message #%d which I don't know.\r\n",
	      number, number);
    }


  }



	  //write(user->socket, msg, strlen(msg));
	  lion_output(user->handle, msg, strlen(msg));


  *msg = 0;  /* safety */

}












/*  ================  */
/*  = LOGIN SCREEN =  */
/*  ================  */


void local_login(struct login_node *user)
{


if (USER_LT < 2) {
/* 1ST LOGIN SCREEN, ALWAYS MONO */



lion_printf(user->handle, "230- [ 1ST LOGIN ]\r\n");
#ifdef ACC
	lion_printf(user->handle, "   /\\__________/\\______/\\______/\\______/\\______/\\______/\\__________/\\______ :  \r\n");
	lion_printf(user->handle, "  / /\\_________\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_\\___\\___\\/\\_____\\|__\r\n");
	lion_printf(user->handle, "  \\/________ _ /___    /___    /   ____/   ____/   ____/__ /   /   /   ____/|  \r\n");
	lion_printf(user->handle, "  : / /      //.  /___/.  /___/.  ____/___    /___    /   /   /   /.  ____/ :  \r\n");
	lion_printf(user->handle, "__|/ /   /   //  /   //  /   //  /   /   /   /   /   /   /_  /   //  /   /     \r\n");
	lion_printf(user->handle, "  |\\/___/   /\\_____ /\\_____ /\\_____ /______ /______ /___/\\/____/~\\_____ /      \r\n");
	lion_printf(user->handle, "  .:::\\/____ /::::\\/::::::\\/::::::\\/::::::\\/::::::\\/::::::::::::::::::\\/       \r\n");
	lion_printf(user->handle, "           \\/                                                                  \r\n");
#elif defined ENIGMA
	lion_printf(user->handle, "                              ___/\\___                                          \r\n");
	lion_printf(user->handle, "      ___________/\\______  ___\\      /__________________  __/\\_________/\\__     \r\n");
	lion_printf(user->handle, "      \\__    _______     \\|  __\\____/__    ________     \\/    ___   _   __/     \r\n");
	lion_printf(user->handle, "  >>>> /   ____/  \\       \\   \\       \\    \\|    \\/            \\    |    \\>>>>  \r\n");
	lion_printf(user->handle, "      /      /     \\_  \\       \\_      \\_   |     \\_  \\    /    \\_        \\_    \r\n");
	lion_printf(user->handle, "      \\_____________/___\\___  __/_______/______  __/___\\  /|_____/__|______/    \r\n");
	lion_printf(user->handle, "                            \\/                 \\/       \\/                      \r\n");
#endif

lion_printf(user->handle, "\r\n");
lion_printf(user->handle, "Welcome, this is your first login!\r\n");
lion_printf(user->handle, "\r\n");
lion_printf(user->handle, "Do a SITE HELP to get info about the special site commands.\r\n");
lion_printf(user->handle, "\r\n");
lion_printf(user->handle, "Be sure to check SITE COLOUR and SITE EXTRA\r\n");
lion_printf(user->handle, "\r\n");
lion_printf(user->handle, "Have fun and upload lots :)\r\n");
lion_printf(user->handle, "\r\n");


} else {



  if (!(user->options & UO_COLOUR)) {
/* MONO LOGIN SCREEN */


	lion_printf(user->handle, "230- [ LOGIN ]\r\n");
#ifdef ACC
	lion_printf(user->handle, "   /\\__________/\\______/\\______/\\______/\\______/\\______/\\__________/\\______ :  \r\n");
	lion_printf(user->handle, "  / /\\_________\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_\\___\\___\\/\\_____\\|__\r\n");
	lion_printf(user->handle, "  \\/________ _ /___    /___    /   ____/   ____/   ____/__ /   /   /   ____/|  \r\n");
	lion_printf(user->handle, "  : / /      //.  /___/.  /___/.  ____/___    /___    /   /   /   /.  ____/ :  \r\n");
	lion_printf(user->handle, "__|/ /   /   //  /   //  /   //  /   /   /   /   /   /   /_  /   //  /   /     \r\n");
	lion_printf(user->handle, "  |\\/___/   /\\_____ /\\_____ /\\_____ /______ /______ /___/\\/____/~\\_____ /      \r\n");
	lion_printf(user->handle, "  .:::\\/____ /::::\\/::::::\\/::::::\\/::::::\\/::::::\\/::::::::::::::::::\\/       \r\n");
	lion_printf(user->handle, "           \\/                                                                  \r\n");
#elif defined ENIGMA
	lion_printf(user->handle, "                              ___/\\___                                          \r\n");
	lion_printf(user->handle, "      ___________/\\______  ___\\      /__________________  __/\\_________/\\__     \r\n");
	lion_printf(user->handle, "      \\__    _______     \\|  __\\____/__    ________     \\/    ___   _   __/     \r\n");
	lion_printf(user->handle, "  >>>> /   ____/  \\       \\   \\       \\    \\|    \\/            \\    |    \\>>>>  \r\n");
	lion_printf(user->handle, "      /      /     \\_  \\       \\_      \\_   |     \\_  \\    /    \\_        \\_    \r\n");
	lion_printf(user->handle, "      \\_____________/___\\___  __/_______/______  __/___\\  /|_____/__|______/    \r\n");
	lion_printf(user->handle, "                            \\/                 \\/       \\/                      \r\n");
#endif

	lion_printf(user->handle, "\r\n");

sprintf(genbuf, "%s! This is your %d%s visit.", user->user, USER_LT,
	postnumber(USER_LT));
lion_printf(user->handle, "Welcome to %s, %-40.40s\r\n",
	     server_greeting ? server_greeting : localhost_a, genbuf);

strcpy(genbuf, ctime(&user->last_login));
chop(genbuf);
lion_printf(user->handle, "You last logged in at %s ", genbuf);
sprintf(genbuf, "%s@%s", user->last_ident, lion_ntoa(user->last_host));
lion_printf(user->handle, "from %s\r\n", genbuf);

lion_printf(user->handle, "Your Tagline is: %-32.32s\r\n", user->tagline);


sprintf(genbuf, "%s with active group %s", group_listgroups(user->user), user->current_group ? ((struct group *)user->current_group)->name : "None");
lion_printf(user->handle, "You are member of %-50.50s\r\n", genbuf);

sprintf(genbuf, "%d %s, %d %s and %d %s", user->num_logins,  user->num_logins  > 1 ? "logins" : "login", user->num_uploads, user->num_uploads > 1 ? "uploads" : "upload", user->num_downloads, user->num_downloads>1?"downloads" : "download");
lion_printf(user->handle, "You are allowed %-35.35s simultaneusly.\r\n", genbuf);

if (user->level) lion_printf(user->handle, "You have special access %-20.20s\r\n", flagstoa(user->level));

if ( USER_SU ) {
  sprintf(genbuf, "%.1f Mbytes in %d Files at %.1f K/s",
	  USER_BU / 1048576.0, USER_FU, USER_BU / USER_SU / 1024.0);
  lion_printf(user->handle, "You Uploaded:   %-40.40s\r\n", genbuf);

} else {

  lion_printf(user->handle, "No Uploads until now.\r\n");
}

if ( USER_SD ) { sprintf(genbuf, "%.1f Mbytes in %d Files at %.1f K/s", USER_BD / 1048576.0, USER_FD, USER_BD / USER_SD / 1024.0);
                 lion_printf(user->handle, "You Downloaded: %-40.40s\r\n", genbuf); } else {
                 lion_printf(user->handle, "No Downloads until now.\r\n"); }


lion_printf(user->handle, "\r\n");

lion_printf(user->handle, "Server has had %s uploaded.\r\n",
	     byte_str(global_total_bytes_up));
lion_printf(user->handle, "Server has had %s downloaded.\r\n",
	     byte_str(global_total_bytes_down));

 if (global_yubd)
	 lion_printf(user->handle,
				 " Yesterday's top   up was %12.12s %7.2f GB in %5u files.\r\n",
				 global_yuud, (float)global_yubd / 1073741824.0, global_yufd);

 if (global_ydbd)
	 lion_printf(user->handle,
				 " Yesterday's top down was %12.12s %7.2f GB in %5u files.\r\n",
				 global_ydud, (float)global_ydbd / 1073741824.0, global_ydfd);

lion_printf(user->handle, "\r\n");


} else {
/* COLOUR LOGIN SCREEN */


	lion_printf(user->handle, "230- [ LOGIN ]\r\n");
#ifdef ACC
	lion_printf(user->handle, "[1;37;44m   /\\__________/\\______/\\______/\\______/\\______/\\______/\\__________/\\______ :   [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  / /\\_________\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_____\\/\\_\\___\\___\\/\\_____\\|__ [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  \\/________ _ /___    /___    /   ____/   ____/   ____/__ /   /   /   ____/|   [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  : / /      //.  /___/.  /___/.  ____/___    /___    /   /   /   /.  ____/ :   [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m__|/ /   /   //  /   //  /   //  /   /   /   /   /   /   /_  /   //  /   /      [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  |\\/___/   /\\_____ /\\_____ /\\_____ /______ /______ /___/\\/____/~\\_____ /       [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  .:::\\/____ /::::\\/::::::\\/::::::\\/::::::\\/::::::\\/::::::::::::::::::\\/        [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m           \\/                                                                   [0;40m\r\n");
#elif defined ENIGMA
	lion_printf(user->handle, "[1;37;44m                              ___/\\___                                          [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m      ___________/\\______  ___\\      /__________________  __/\\_________/\\__     [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m      \\__    _______     \\|  __\\____/__    ________     \\/    ___   _   __/     [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m  >>>> /   ____/  \\       \\   \\       \\    \\|    \\/            \\    |    \\>>>>  [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m      /      /     \\_  \\       \\_      \\_   |     \\_  \\    /    \\_        \\_    [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m      \\_____________/___\\___  __/_______/______  __/___\\  /|_____/__|______/    [0;40m\r\n");
	lion_printf(user->handle, "[1;37;44m                            \\/                 \\/       \\/                      [0;40m\r\n");
#endif

	lion_printf(user->handle, "[1;37;44m                                                                                [0;40m\r\n");

sprintf(genbuf, "%s[0;36;44m, [1;36;44m%s[0;36;44m! This is your [1;36;44m%d%s [0;36;44mvisit.", server_greeting ? server_greeting : localhost_a, user->user, USER_LT, postnumber(USER_LT));
lion_printf(user->handle, "[0;36;44m Welcome to [1;37;44m%-117.117s [0;40m\r\n", genbuf);

strcpy(genbuf, ctime(&user->last_login));
chop(genbuf);
lion_printf(user->handle, "[0;36;44m You last logged in at [1;36;44m%s ", genbuf);
sprintf(genbuf, "%s@%s", user->last_ident, lion_ntoa(user->last_host));
lion_printf(user->handle, "[0;36;44mfrom [1;36;44m%-27.27s[0;40m\r\n", genbuf);

lion_printf(user->handle, "[0;36;44m Your Tagline is: [1;36;44m%-45.45s                 [0;40m\r\n", user->tagline);

sprintf(genbuf, "[1;36;44m%s [0;36;44mwith active group [1;36;44m%s", group_listgroups(user->user), user->current_group ? ((struct group *)user->current_group)->name : "None");
lion_printf(user->handle, "[0;36;44m You are member of %-91.91s[0;40m\r\n", genbuf);

sprintf(genbuf, "[1;36;44m%d %s[0;36;44m, [1;36;44m%d %s [0;36;44mand [1;36;44m%d %s [0;36;44msimultaneusly.", user->num_logins,  user->num_logins  > 1 ? "logins" : "login", user->num_uploads, user->num_uploads > 1 ? "uploads" : "upload", user->num_downloads, user->num_downloads>1?"downloads" : "download");
lion_printf(user->handle, "[0;36;44m You are allowed %-122.122s [0;40m\r\n", genbuf);

if (user->level) lion_printf(user->handle, "[0;36;44m You have special access [1;36;44m%-55.55s[0;40m\r\n", flagstoa(user->level));


if ( USER_SU ) {
  sprintf(genbuf, "[1;36;44m%.1f Mbytes [0;36;44min [1;36;44m%d Files [0;36;44mat [1;36;44m%.1f K/s",
	  USER_BU / 1048576.0, USER_FU, USER_BU / USER_SU / 1024.0);

  lion_printf(user->handle,
	       "[0;36;44m You Uploaded:   %-113.113s[0;40m\r\n",
	       genbuf);
} else {
  lion_printf(user->handle,
	       "[0;36;44m No Uploads until now.                                                          [0;40m\r\n");
}

if ( USER_SD ) {
  sprintf(genbuf, "[1;36;44m%.1f Mbytes [0;36;44min [1;36;44m%d Files [0;36;44mat [1;36;44m%.1f K/s",
	  USER_BD / 1048576.0, USER_FD, USER_BD / USER_SD / 1024.0);

  lion_printf(user->handle,
	       "[0;36;44m You Downloaded: %-113.113s[0;40m\r\n",
	       genbuf);
} else {
  lion_printf(user->handle,
	       "[0;36;44m No Downloads until now.                                                        [0;40m\r\n");
}

lion_printf(user->handle, "[1;37;44m                                                                                [0;40m\r\n");


sprintf(genbuf, "%s uploaded[0;36;44m.", byte_str(global_total_bytes_up));
lion_printf(user->handle, "[0;36;44m Server has had [1;36;44m%-65.65s         [0;40m\r\n", genbuf
	     );
sprintf(genbuf, "%s downloaded[0;36;44m.", byte_str(global_total_bytes_down));
lion_printf(user->handle, "[0;36;44m Server has had [1;36;44m%-65.65s         [0;40m\r\n", genbuf);

 if (global_yubd)
	 lion_printf(user->handle,
				 "[0;36;44m Yesterday's top   up was %12.12s %7.2f GB in %5u files.               [0;40m\r\n",
				 global_yuud, (float)global_yubd / 1073741824.0, global_yufd);

 if (global_ydbd)
	 lion_printf(user->handle,
				 "[0;36;44m Yesterday's top down was %12.12s %7.2f GB in %5u files.               [0;40m\r\n",
				 global_ydud, (float)global_ydbd / 1073741824.0, global_ydfd);





lion_printf(user->handle, "[1;37;44m                                                                                [0;40m\r\n");
lion_printf(user->handle, "230-\r\n");

}
}
}







/*  =======  */
/*  = WHO =  */
/*  =======  */


//
// Small little structure passed along to WHO iterations so we can total up
// some of the data.
//
struct who_stats {
	float total_cps_in;
	float total_cps_out;
	int num_users;
	int num_data;
};


int local_site_who_sub( lion_t *handle, void *arg1, void *arg2 )
{
	struct login_node *user = (struct login_node *) arg1;
	struct login_node *p = NULL;
	struct data_node *data = NULL;
	struct who_stats *stats = (struct who_stats *) arg2;
	lion_t *dh;
	unsigned int percentage = 0;
	float cps, in, out;
	char *usecwd = "/";
	lion64u_t bin, bout;

	if (!handle) // Shouldn't happen!
		return 1;

	if (lion_get_handler(handle) != login_handler)
		return 1;

	if (stats)
		stats->num_users++;


	p = (struct login_node *) lion_get_userdata( handle );

	if (!p) return 1;

	dh = NULL;
	cps = 0.0;
	in = 0.0;
	out = 0.0;


	data = (struct data_node *)p->data;

	if (data && data->handle) {

		if (stats)
			stats->num_data++;

		dh = data->handle;
		lion_get_cps( dh, &in, &out );
		if (stats) {
			stats->total_cps_in += in;
			stats->total_cps_out += out;
		}

		cps = in + out;

		if (cps > 9999) cps = 9999;


		// Work out the percentage, if we can.
		//		bin = bout = 0;

		if (data->bytes) {

			lion_get_bytes(dh, &bin, &bout);
			if (bin)
				percentage = (unsigned int)
					(bin  + (lion64_t)data->rest_value) * 100 / data->bytes;
			else
				percentage = (unsigned int)
					(bout + (lion64_t)data->rest_value) * 100 / data->bytes;

			if (percentage > 999)
			percentage = 999;

		} // if bytes

	} // if data->handle


	if (!strcmp(p->cwd, "/") && data && data->name) {
		char *r;

		strcpy(genbuf, hide_path(data->name));
		r = path2file(genbuf);
		*r = 0;
		usecwd = genbuf;

	} else
		usecwd = p->cwd;

	// Are they in hidden path? unless we have permission of course
	// Are users allowed to see themselves?
	if (!((user->level|server_defaultflags)&UF_SEEALL) &&
		section_stealth(usecwd))
		return 1;






	// Print this user

	lion_printf(user->handle,
				!(user->options & UO_COLOUR) ?
				"|%c%-8.8s |%c%5.0f %-3u| %-4.4s %-12.12s | %-34.34s |\r\n" :
				"[1;36;44m|%c[0;36;44m%-8.8s [1;36;44m|%c[0;36;44m%5.0f %-3u[1;36;44m| [0;36;44m%-4.4s %-12.12s [1;36;44m| [0;36;44m%-34.34s [1;36;44m|[0;40m\r\n",
				lion_ssl_enabled( p->handle) ? '*' : ' ',
				p->user,
				dh && lion_ssl_enabled(dh) ? '*' : ' ',
				cps,
				percentage,
				(p->last_command & 1024) ?
				commands2[(p->last_command&~1024)].name :
				commands[p->last_command].name,
				(p->data && ((struct data_node *)p->data)->name) ?
				path2file(((struct data_node *)p->data)->name) : "",
				usecwd
				);


	return 1; // keep iterating.
}





void local_site_who(struct login_node *user) {
  struct who_stats totals;

  memset(&totals, 0, sizeof(totals));


  // Start
  lion_printf(user->handle, "200- [ WHO ]\r\n");






  // Print header
  if (!(user->options & UO_COLOUR)) {

	  lion_printf(user->handle, "+----------+----------+-------------------+------------------------------------+\r\n");
	  lion_printf(user->handle, "| USER     | CPS   %%  | ACTION            | CWD                                |\r\n");
	  lion_printf(user->handle, "+----------+----------+-------------------+------------------------------------+\r\n");

  } else {
	  lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
	  lion_printf(user->handle, "[1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mCPS   %%  [1;36;44m| [1;37;44mACTION            [1;36;44m| [1;37;44mCWD                                [1;36;44m|[0;40m\r\n");
	  lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
  }





  // Start listing.
  lion_find( local_site_who_sub, (void *)user, (void *)&totals);







  // Print foot

  if (!(user->options & UO_COLOUR)) {

	  lion_printf(user->handle, "+----------+----------+-------------------+------------------------------------+\r\n");

	  lion_printf(user->handle, "Total k/s Upload: %7.2f    Download: %7.2f   Total: %7.2f\r\n",
				  totals.total_cps_in, totals.total_cps_out,
				  totals.total_cps_in + totals.total_cps_out);

	  lion_printf(user->handle, "Total users online: %3u              Total active data: %3u     \r\n",
				  totals.num_users, totals.num_data);

  } else {

	  lion_printf(user->handle, "[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");

	  lion_printf(user->handle, "Total k/s Upload: %7.2f    Download: %7.2f   Total: %7.2f\r\n",
				  totals.total_cps_in, totals.total_cps_out,
				  totals.total_cps_in + totals.total_cps_out);

	  lion_printf(user->handle, "Total users online:   %3u            Total active data:     %3u\r\n",
				  totals.num_users, totals.num_data);

  }



  // Finish.
  socket_number(user, 200, "WHO");

}






/*  ================  */
/*  = CWD SCREEN 1 =  */
/*  ================  */

/*


  if (!(user->options & UO_COLOUR))
  {

  lion_printf(user->handle, "200- [ WEEKTOP 10 ]                                                             \r\n");
  lion_printf(user->handle, "   _______ ___________________  __________________________    ____  ______      \r\n");
  lion_printf(user->handle, "  /__    //  /   ____/   ____/ |  |_ _____\\ ___   \\_____  \\  /    \\/   _  \\     \r\n");
  lion_printf(user->handle, "  \\ /.  //  //  ____/.  ____/    __/  \\   /\\   \\  \\\\   ____\\/\\/\\   \\   \\\\  \\    \r\n");
  lion_printf(user->handle, "   //  //  /.  /   //  /   /   _ ' \\   \\_/  \\   \\  .\\   \\  / /  \\   \\   \\\\  \\   \r\n");
  lion_printf(user->handle, "  /___/\\__/_______/______ /    |    \\___\\\\   \\_______\\___\\/\\/\\   \\___\\_______\\  \r\n");
  lion_printf(user->handle, "  \\    \\  \\       \\     \\/_____|_____\\  / \\  /       /   /    \\  /   /       /  \r\n");
  lion_printf(user->handle, "   \\___/\\__\\_______\\_____\\     |     /_/   \\/_______/___/      \\/___/_______/   \r\n");
  lion_printf(user->handle, " U P L O A D E R          \\____|____/                       D O W N L O A D E R \r\n");
  lion_printf(user->handle, " +----+----------+----------+---------+  +----+----------+----------+---------+ \r\n");
  lion_printf(user->handle, " | ## | USER     | GROUP    | MBYTES  |  | ## | USER     | GROUP    | MBYTES  | \r\n");
  lion_printf(user->handle, " +----+----------+----------+---------+  +----+----------+----------+---------+ \r\n");

  if (global_tubw[0] || global_tdbw[0]) lion_printf(user->handle, " | 01 | %-8s | %-8s | %7.1f |  | 01 | %-8s | %-8s | %7.1f | \r\n", global_tubw[0] ? global_tuuw[0] : (unsigned char *)"-", global_tubw[0] ? global_tugw[0] : (unsigned char *)"-", global_tubw[0] / 1048576.0, global_tdbw[0] ? global_tduw[0] : (unsigned char *)"-", global_tdbw[0] ? global_tdgw[0] : (unsigned char *)"-", global_tdbw[0] / 1048576.0);
  if (global_tubw[1] || global_tdbw[1]) lion_printf(user->handle, " | 02 | %-8s | %-8s | %7.1f |  | 02 | %-8s | %-8s | %7.1f | \r\n", global_tubw[1] ? global_tuuw[1] : (unsigned char *)"-", global_tubw[1] ? global_tugw[1] : (unsigned char *)"-", global_tubw[1] / 1048576.0, global_tdbw[1] ? global_tduw[1] : (unsigned char *)"-", global_tdbw[1] ? global_tdgw[1] : (unsigned char *)"-", global_tdbw[1] / 1048576.0);
  if (global_tubw[2] || global_tdbw[2]) lion_printf(user->handle, " | 03 | %-8s | %-8s | %7.1f |  | 03 | %-8s | %-8s | %7.1f | \r\n", global_tubw[2] ? global_tuuw[2] : (unsigned char *)"-", global_tubw[2] ? global_tugw[2] : (unsigned char *)"-", global_tubw[2] / 1048576.0, global_tdbw[2] ? global_tduw[2] : (unsigned char *)"-", global_tdbw[2] ? global_tdgw[2] : (unsigned char *)"-", global_tdbw[2] / 1048576.0);
  if (global_tubw[3] || global_tdbw[3]) lion_printf(user->handle, " | 04 | %-8s | %-8s | %7.1f |  | 04 | %-8s | %-8s | %7.1f | \r\n", global_tubw[3] ? global_tuuw[3] : (unsigned char *)"-", global_tubw[3] ? global_tugw[3] : (unsigned char *)"-", global_tubw[3] / 1048576.0, global_tdbw[3] ? global_tduw[3] : (unsigned char *)"-", global_tdbw[3] ? global_tdgw[3] : (unsigned char *)"-", global_tdbw[3] / 1048576.0);
  if (global_tubw[4] || global_tdbw[4]) lion_printf(user->handle, " | 05 | %-8s | %-8s | %7.1f |  | 05 | %-8s | %-8s | %7.1f | \r\n", global_tubw[4] ? global_tuuw[4] : (unsigned char *)"-", global_tubw[4] ? global_tugw[4] : (unsigned char *)"-", global_tubw[4] / 1048576.0, global_tdbw[4] ? global_tduw[4] : (unsigned char *)"-", global_tdbw[4] ? global_tdgw[4] : (unsigned char *)"-", global_tdbw[4] / 1048576.0);
  if (global_tubw[5] || global_tdbw[5]) lion_printf(user->handle, " | 06 | %-8s | %-8s | %7.1f |  | 06 | %-8s | %-8s | %7.1f | \r\n", global_tubw[5] ? global_tuuw[5] : (unsigned char *)"-", global_tubw[5] ? global_tugw[5] : (unsigned char *)"-", global_tubw[5] / 1048576.0, global_tdbw[5] ? global_tduw[5] : (unsigned char *)"-", global_tdbw[5] ? global_tdgw[5] : (unsigned char *)"-", global_tdbw[5] / 1048576.0);
  if (global_tubw[6] || global_tdbw[6]) lion_printf(user->handle, " | 07 | %-8s | %-8s | %7.1f |  | 07 | %-8s | %-8s | %7.1f | \r\n", global_tubw[6] ? global_tuuw[6] : (unsigned char *)"-", global_tubw[6] ? global_tugw[6] : (unsigned char *)"-", global_tubw[6] / 1048576.0, global_tdbw[6] ? global_tduw[6] : (unsigned char *)"-", global_tdbw[6] ? global_tdgw[6] : (unsigned char *)"-", global_tdbw[6] / 1048576.0);
  if (global_tubw[7] || global_tdbw[7]) lion_printf(user->handle, " | 08 | %-8s | %-8s | %7.1f |  | 08 | %-8s | %-8s | %7.1f | \r\n", global_tubw[7] ? global_tuuw[7] : (unsigned char *)"-", global_tubw[7] ? global_tugw[7] : (unsigned char *)"-", global_tubw[7] / 1048576.0, global_tdbw[7] ? global_tduw[7] : (unsigned char *)"-", global_tdbw[7] ? global_tdgw[7] : (unsigned char *)"-", global_tdbw[7] / 1048576.0);
  if (global_tubw[8] || global_tdbw[8]) lion_printf(user->handle, " | 09 | %-8s | %-8s | %7.1f |  | 09 | %-8s | %-8s | %7.1f | \r\n", global_tubw[8] ? global_tuuw[8] : (unsigned char *)"-", global_tubw[8] ? global_tugw[8] : (unsigned char *)"-", global_tubw[8] / 1048576.0, global_tdbw[8] ? global_tduw[8] : (unsigned char *)"-", global_tdbw[8] ? global_tdgw[8] : (unsigned char *)"-", global_tdbw[8] / 1048576.0);
  if (global_tubw[9] || global_tdbw[9]) lion_printf(user->handle, " | 10 | %-8s | %-8s | %7.1f |  | 10 | %-8s | %-8s | %7.1f | \r\n", global_tubw[9] ? global_tuuw[9] : (unsigned char *)"-", global_tubw[9] ? global_tugw[9] : (unsigned char *)"-", global_tubw[9] / 1048576.0, global_tdbw[9] ? global_tduw[9] : (unsigned char *)"-", global_tdbw[9] ? global_tdgw[9] : (unsigned char *)"-", global_tdbw[9] / 1048576.0);

  lion_printf(user->handle, " +----+----------+----------+---------+  +----+----------+----------+---------+ \r\n");


  } else {


  lion_printf(user->handle, "200- [ WEEKTOP 10 ]\r\n");
  lion_printf(user->handle, "[1;37;44m   _______ ___________________  __________________________    ____  ______      [0;40m\r\n");
  lion_printf(user->handle, "[1;37;44m  /__    //  /   ____/   ____/ |  |_ _____\\ ___   \\_____  \\  /    \\/   _  \\     [0;40m\r\n");
  lion_printf(user->handle, "[1;37;44m  \\ /.  //  //  ____/.  ____/    __/  \\   /\\   \\  \\\\   ____\\/\\/\\   \\   \\\\  \\    [0;40m\r\n");
  lion_printf(user->handle, "[1;37;44m   //  //  /.  /   //  /   /   _ ' \\   \\_/  \\   \\  .\\   \\  / /  \\   \\   \\\\  \\   [0;40m\r\n");
  lion_printf(user->handle, "[1;37;44m  /___/\\__/_______/______ /    |    \\___\\\\   \\_______\\___\\/\\/\\   \\___\\_______\\  [0;40m\r\n");
  lion_printf(user->handle, "[1;37;44m  \\    \\  \\       \\     \\/_____|_____\\  / \\  /       /   /    \\  /   /       /  [0;40m\r\n");
  lion_printf(user->handle, "[1;37;44m   \\___/\\__\\_______\\_____\\     |     /_/   \\/_______/___/      \\/___/_______/   [0;40m\r\n");
  lion_printf(user->handle, "[1;37;44m U P L O A D E R          \\____|____/                       D O W N L O A D E R [0;40m\r\n");
  lion_printf(user->handle, "[1;37;44m +[1;36;44m-[0;36;44m-[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+  +[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+ [0;40m\r\n");
  lion_printf(user->handle, "[1;36;44m | [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mMBYTES  [1;36;44m|  | [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mMBYTES  [1;36;44m| [0;40m\r\n");
  lion_printf(user->handle, "[1;37;44m +[1;36;44m-[0;36;44m-[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+  +[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+ [0;40m\r\n");

  lion_printf(user->handle, "[1;37;44m +[1;36;44m-[0;36;44m-[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+  +[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+ [0;40m\r\n");

  }


*/
