/*
 * Help Functions
 *
 * Jorgen Lundman
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>

#if HAVE_TIME_H
#include <time.h>
#endif
#include <stdio.h>
#include <fcntl.h>

#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif


#include "socket.h"
#include "help.h"
#include "userflag.h"
#include "misc.h"


void help_get4letter(char *s, char *r)
{
  /* Gets the first non-null,. non-space chars in s, or pads with space. */
  int t;


  /* Kill any leading Spaces */
  while(*s == ' ') s++;

  t = 0;
  while(*s && (t < 5)) {
    *r = toupper(*s);
    r++;
    s++;
    t++;
  }

  while(t<5) {
    *r++ = ' ';
    t++;
  }

  *r = (char) 0;

}



void help_commands(void *socket, char *command)
{
  char com[5];

  /* Kill any leading Spaces */


  help_get4letter(command, com);

  switch(TOINT(com[0], com[1], com[2], com[3])) {
  case TOINT('A','B','O','R'):
    socket_print(socket, "214 Syntax: ABOR (abort operation)\r\n");
    break;
  case TOINT('U','S','E','R'):
    socket_print(socket, "214 Syntax: USER <sp> username\r\n");
    break;
  case TOINT('P','A','S','S'):
    socket_print(socket, "214 Syntax: PASS <sp> password\r\n");
    break;
  case TOINT('Q','U','I','T'):
    socket_print(socket, "214 Syntax: QUIT (terminate service)\r\n");
    break;
  case TOINT('P','O','R','T'):
    socket_print(socket, "214 Syntax: PORT <sp> b0, b1, b2, b3, b4, b5\r\n");
    break;
  case TOINT('P','A','S','V'):
    socket_print(socket, "214 Syntax: PASV (set server in passive mode)\r\n");
    break;
  case TOINT('T','Y','P','E'):
    socket_print(socket, "214 Syntax: TYPE <sp> [ A | E | I | L ]\r\n");
    break;
  case TOINT('M','O','D','E'):
    socket_print(socket, "214 Syntax: MODE (specify transfer mode)\r\n");
    break;
  case TOINT('R','E','T','R'):
    socket_print(socket, "214 Syntax: RETR <sp> file-name\r\n");
    break;
  case TOINT('S','T','O','R'):
    socket_print(socket, "214 Syntax: STOR <sp> file-name\r\n");
    break;
  case TOINT('A','P','P','E'):
    socket_print(socket, "214 Syntax: APPE <sp> offset (restart command)\r\n");
    break;
  case TOINT('R','E','S','T'):
    socket_print(socket, "214 Syntax: REST <sp> offset (restart command)\r\n");
    break;
  case TOINT('R','N','F','R'):
    socket_print(socket, "214 Syntax: RNFR <sp> file-name\r\n");
    break;
  case TOINT('R','N','T','O'):
    socket_print(socket, "214 Syntax: RNTO <sp> file-name\r\n");
    break;
  case TOINT('D','E','L','E'):
    socket_print(socket, "214 Syntax: DELE <sp> file-name\r\n");
    break;
  case TOINT('C','W','D',' '):
    socket_print(socket, "214 Syntax: CWD [ <sp> directory-name ]\r\n");
    break;
  case TOINT('L','I','S','T'):
    socket_print(socket, "214-Syntax: LIST [ <sp> path-name ]\r\n");
    socket_print(socket, "214-   <sp> options are:\r\n");
    socket_print(socket,
"214-   <la1XWG> view options: long, show-dot, short, XML, dir-size, genre\r\n");
    socket_print(socket,
"214-   <tNSIDr> sort: time, name, size, case-insensitive, dirs-first, reverse\r\n");
    socket_print(socket,
"214-   <R> recursive\r\n");
    socket_print(socket, "214 Syntax: LIST [ <sp> path-name ]\r\n");
    break;
  case TOINT('N','L','S','T'):
    socket_print(socket, "214 Syntax: NLST [ <sp> path-name ]\r\n");
    break;
  case TOINT('S','I','T','E'):
    socket_print(socket, "214 Syntax: SITE site-cmd [ <sp> arguments ]\r\n");
    break;
  case TOINT('S','Y','S','T'):
    socket_print(socket, "214 Syntax: SYST (get type of operating system)\r\n");
    break;
  case TOINT('S','T','A','T'):
    socket_print(socket, "214 Syntax: STAT [ <sp> path-name ]\r\n");
    break;
  case TOINT('H','E','L','P'):
    socket_print(socket, "214 Syntax: HELP [ <sp> <string> ]\r\n");
    break;
  case TOINT('N','O','O','P'):
    socket_print(socket, "214 Syntax: NOOP \r\n");
    break;
  case TOINT('M','K','D',' '):
    socket_print(socket, "214 Syntax: MKD <sp> path-name\r\n");
    break;
  case TOINT('R','M','D',' '):
    socket_print(socket, "214 Syntax: RMD <sp> path-name\r\n");
    break;
  case TOINT('P','W','D',' '):
    socket_print(socket, "214 Syntax: PWD (return current directory)\r\n");
    break;
  case TOINT('W','H','O',' '):
    socket_print(socket, "214 Syntax: WHO (list active connections)\r\n");
    break;
  case TOINT('C','D','U','P'):
    socket_print(socket, "214 Syntax: CDUP (change to parent directory)\r\n");
    break;
  case TOINT('T','O','P','U'):
    socket_print(socket, "214 Syntax: TOPUP <day|week|month|all> TOP lists\r\n");
    break;
  case TOINT('T','O','P','D'):
    socket_print(socket, "214 Syntax: TOPDOWN <day|week|month|all> TOP lists\r\n");
    break;
  case TOINT('K','I','C','K'):
    socket_print(socket, "214 Syntax: KICK <user> (requires admin status)\r\n");
    break;
  case TOINT('N','U','K','E'):
    socket_print(socket, "214 Syntax: NUKE <directory> (Rename <dir> and remove upload credits)\r\n");
    break;
  case TOINT('T','C','P','S'):
    socket_print(socket, "214 Syntax: TCPSTAT (list TCP connections, admin)\r\n");
    break;
  case TOINT('M','S','G',' '):
    socket_print(socket, "214 Syntax: MSG [ <user> message ] (Check for, or send messages)\r\n");
    break;
  case TOINT('U','S','E','2'):
    socket_print(socket, "214 Syntax: USER [ <user> ] (Show stats about current login or <user>\r\n");
    break;
  case TOINT('C','H','G','R'):
    socket_print(socket, "214 Syntax: CHGRP [ <group> ] (list or set active group)\r\n");
    break;
  case TOINT('G','R','O','U'):
    socket_print(socket, "214 Syntax: GROUP [ <group> ] (list groups or members of a group)\r\n");
    break;
  case TOINT('S','E','T','G'):
    socket_print(socket, "214 Syntax: SETGROUP <+-user> <group> (Add or remove user from group)\r\n");
    break;
  case TOINT('G','T','O','P'):
    socket_print(socket, "214 Syntax: GTOP WEEK|ALL (Show amounts transfered by each group (unsorted))\r\n");
    break;
  case TOINT('N','E','W',' '):
    socket_print(socket, "214 Syntax: NEW last|yymmdd (Show new uploads since LAST or DATE)\r\n");
    break;
  case TOINT('C','H','O','W'):
    socket_print(socket, "214 Syntax: chown user path (Change owner of file or directory)\r\n");
    break;
  case TOINT('P','A','S','2'):
    socket_print(socket, "214 Syntax: PASSWD old new (Change password from old to new)\r\n");
    break;
  case TOINT('T','A','G','L'):
    socket_print(socket, "214 Syntax: TAGLINE [ <line> ] (Set your tag line)\r\n");
    break;
  case TOINT('W','A','L','L'):
    socket_print(socket, "214 Syntax: WALL [ <line> ] (Display or Add <line> to Message Wall)\r\n");
    break;
  case TOINT('R','E','H','A'):
    socket_print(socket, "214 Syntax: REHASH (Re-read configuration file)\r\n");
    break;
  case TOINT('D','U','P','E'):
    socket_print(socket, "214 Syntax: DUPE [+-]<file> (List, +add or -remove <file> from DUPE-DB)\r\n");
    break;
  case TOINT('S','E','T','C'):
    socket_print(socket, "214 Syntax: SETCRED <user> <+-amount> (Set, add or remove <amount>MB credits from <user>)\r\n");
    break;
  case TOINT('G','I','V','E'):
    socket_print(socket, "214 Syntax: GIVE <user> <amount> (Give <amount>MB of your credits to <user>)\r\n");
    break;
  case TOINT('A','D','D','U'):
    socket_print(socket, "214 Syntax: ADDUSER <user> <pass> [ <pattern> ... ] (Create a new user)\r\n");
    break;
  case TOINT('D','E','L','U'):
    socket_print(socket, "214 Syntax: DELUSER <user> (Delete a user)\r\n");
    break;
  case TOINT('R','E','N','U'):
    socket_print(socket, "214 Syntax: RENUSER <oldname> <newname> (Rename a user)\r\n");
    break;
  case TOINT('S','E','T','I'):
    socket_print(socket, "214 Syntax: SETIP <user> <entry> <pattern> (Set user'sIP pattern)\r\n");
    break;
  case TOINT('S','E','T','L'):
    socket_print(socket, "214 Syntax: SETLIMIT <user> <login|down|up> <limit> (Set user's limits)\r\n");
    break;
  case TOINT('S','E','T','F'):
    socket_print(socket, "214-Syntax: SETFLAGS <user> <flags> (Set user's userflag level)\r\n");
	socket_print(socket, "214-  SITE SETFLAGS ftp UF_PUNISH|UF_DUPE   - To set it.\r\n");
	socket_print(socket, "214-  SITE SETFLAGS ftp +UF_PUNISH|UF_DUPE  - To ADD flags\r\n");
	socket_print(socket, "214-  SITE SETFLAGS ftp -UF_PUNISH|UF_DUPE  - To DEL flags\r\n");
	socket_print(socket, "214-  SITE SETFLAGS ftp UF_ALL|~UF_ADDUSER  - To set to ALL but ADDUSER\r\n");
	socket_print(socket, "214- Currently available flags are:\r\n");
	socket_print(socket, "214- %s\r\n", flagstoa(UF_ALL-1));
	socket_print(socket, "214 HELP SETFLAGS.\r\n");
    break;
  case TOINT('S','E','T','P'):
    socket_print(socket, "214 Syntax: SETPASS <user> <newpass>\r\n");
    break;
  case TOINT('U','P','T','I'):
    socket_print(socket, "214 Syntax: UPTIME (Display server uptime and statistics)\r\n");
    break;
  case TOINT('S','I','Z','E'):
    socket_print(socket, "214 Syntax: SIZE <sp> (Display SIZE of file in bytes)\r\n");
    break;
  case TOINT('P','R','O','T'):
    socket_print(socket, "214 Syntax: PROT P|C (Set data channel privacy level, Clear|Private)\r\n");
    break;
  case TOINT('C','C','S','N'):
    socket_print(socket, "214 Syntax: CCSN ON|OFF (For secure site to site mode)\r\n");
    break;
  case TOINT('R','E','Q','U'):
	  socket_print(socket, "214 Syntax: REQUEST [directory] (Add a new request, or list current active)\r\n");
    break;
  case TOINT('R','E','Q','F'):
    socket_print(socket, "214 Syntax: REQFILLED <directory> (Announce completion of request)\r\n");
    break;
#if 0
  case TOINT('','','',''):
    socket_print(socket, "214 Syntax: \r\n");
    break;
#endif

  default:
    socket_print(socket, "502 Sorry, no help for '%s'.\r\n", command);
  }
}
