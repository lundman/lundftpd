#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <time.h>
#include <sys/types.h>

#include "global.h"
#include "socket.h"
#include "login.h"
#include "global.h"
#include "tops.h"



void local_user_top_up_mono(struct login_node *user, int number, char *opt)
{
socket_print(user, "200- [ TOP UPLOADER COMBINED ]\r\n");
socket_print(user, "   ________________________________________________________________________\r\n");
socket_print(user, "  /______      _____________    \\_________     \\ \\       \\____________     \\\r\n");
socket_print(user, "    \\ /       / \\ /       /       /    __________\\ \\       \\     ____________\\\r\n");
socket_print(user, "    /       /   /       /       / .        \\         \\       \\       \\     /\r\n");
socket_print(user, "  /_______/   /____________   / /            \\   ______________\\_______\\ /\r\n");
socket_print(user, "  \\       \\   \\           \\ /__________________\\ /             /       /\r\n");
socket_print(user, "    \\_______\\ / \\___________\\                  /_____________/_______/\r\n");
socket_print(user, "                              \\______________/\r\n");
socket_print(user, "[##][ DAY        GB ] [ WEEK        GB ] [ MONTH       GB ] [ ALLTIME      GB ]\r\n");
socket_print(user, "+--+-----------------+------------------+------------------+-------------------+\r\n");
socket_print(user, "|01| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[0] ? global_tuud[0] : (unsigned char *)"-", global_tubd[0] / 1073741824.0, global_tubw[0] ? global_tuuw[0] : (unsigned char *)"-", global_tubw[0] / 1073741824.0, global_tubm[0] ? global_tuum[0] : (unsigned char *)"-", global_tubm[0] / 1073741824.0, global_tub[0] ? global_tuu[0] : (unsigned char *)"-", global_tub[0] / 1073741824.0);
socket_print(user, "|02| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[1] ? global_tuud[1] : (unsigned char *)"-", global_tubd[1] / 1073741824.0, global_tubw[1] ? global_tuuw[1] : (unsigned char *)"-", global_tubw[1] / 1073741824.0, global_tubm[1] ? global_tuum[1] : (unsigned char *)"-", global_tubm[1] / 1073741824.0, global_tub[1] ? global_tuu[1] : (unsigned char *)"-", global_tub[1] / 1073741824.0);
socket_print(user, "|03| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[2] ? global_tuud[2] : (unsigned char *)"-", global_tubd[2] / 1073741824.0, global_tubw[2] ? global_tuuw[2] : (unsigned char *)"-", global_tubw[2] / 1073741824.0, global_tubm[2] ? global_tuum[2] : (unsigned char *)"-", global_tubm[2] / 1073741824.0, global_tub[2] ? global_tuu[2] : (unsigned char *)"-", global_tub[2] / 1073741824.0);
socket_print(user, "|04| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[3] ? global_tuud[3] : (unsigned char *)"-", global_tubd[3] / 1073741824.0, global_tubw[3] ? global_tuuw[3] : (unsigned char *)"-", global_tubw[3] / 1073741824.0, global_tubm[3] ? global_tuum[3] : (unsigned char *)"-", global_tubm[3] / 1073741824.0, global_tub[3] ? global_tuu[3] : (unsigned char *)"-", global_tub[3] / 1073741824.0);
socket_print(user, "|05| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[4] ? global_tuud[4] : (unsigned char *)"-", global_tubd[4] / 1073741824.0, global_tubw[4] ? global_tuuw[4] : (unsigned char *)"-", global_tubw[4] / 1073741824.0, global_tubm[4] ? global_tuum[4] : (unsigned char *)"-", global_tubm[4] / 1073741824.0, global_tub[4] ? global_tuu[4] : (unsigned char *)"-", global_tub[4] / 1073741824.0);
socket_print(user, "|06| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[5] ? global_tuud[5] : (unsigned char *)"-", global_tubd[5] / 1073741824.0, global_tubw[5] ? global_tuuw[5] : (unsigned char *)"-", global_tubw[5] / 1073741824.0, global_tubm[5] ? global_tuum[5] : (unsigned char *)"-", global_tubm[5] / 1073741824.0, global_tub[5] ? global_tuu[5] : (unsigned char *)"-", global_tub[5] / 1073741824.0);
socket_print(user, "|07| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[6] ? global_tuud[6] : (unsigned char *)"-", global_tubd[6] / 1073741824.0, global_tubw[6] ? global_tuuw[6] : (unsigned char *)"-", global_tubw[6] / 1073741824.0, global_tubm[6] ? global_tuum[6] : (unsigned char *)"-", global_tubm[6] / 1073741824.0, global_tub[6] ? global_tuu[6] : (unsigned char *)"-", global_tub[6] / 1073741824.0);
socket_print(user, "|08| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[7] ? global_tuud[7] : (unsigned char *)"-", global_tubd[7] / 1073741824.0, global_tubw[7] ? global_tuuw[7] : (unsigned char *)"-", global_tubw[7] / 1073741824.0, global_tubm[7] ? global_tuum[7] : (unsigned char *)"-", global_tubm[7] / 1073741824.0, global_tub[7] ? global_tuu[7] : (unsigned char *)"-", global_tub[7] / 1073741824.0);
socket_print(user, "|09| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[8] ? global_tuud[8] : (unsigned char *)"-", global_tubd[8] / 1073741824.0, global_tubw[8] ? global_tuuw[8] : (unsigned char *)"-", global_tubw[8] / 1073741824.0, global_tubm[8] ? global_tuum[8] : (unsigned char *)"-", global_tubm[8] / 1073741824.0, global_tub[8] ? global_tuu[8] : (unsigned char *)"-", global_tub[8] / 1073741824.0);
socket_print(user, "|10| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tubd[9] ? global_tuud[9] : (unsigned char *)"-", global_tubd[9] / 1073741824.0, global_tubw[9] ? global_tuuw[9] : (unsigned char *)"-", global_tubw[9] / 1073741824.0, global_tubm[9] ? global_tuum[9] : (unsigned char *)"-", global_tubm[9] / 1073741824.0, global_tub[9] ? global_tuu[9] : (unsigned char *)"-", global_tub[9] / 1073741824.0);
socket_print(user, "+--+-----------------+------------------+------------------+-------------------+\r\n");
socket_print(user, " Type SITE TOPUP <D/W/M/A> to get detailed information.\r\n");
}


void local_user_top_up_colour(struct login_node *user, int number, char *opt)
{
socket_print(user, "200- [ TOP UPLOADER COMBINED ]\r\n");
socket_print(user, "[1;37;44m   ________________________________________________________________________     [0;40m\r\n");
socket_print(user, "[1;37;44m  /______      _____________    \\_________     \\ \\       \\____________     \\    [0;40m\r\n");
socket_print(user, "[1;37;44m    \\ /       / \\ /       /       /    __________\\ \\       \\     ____________\\  [0;40m\r\n");
socket_print(user, "[1;37;44m    /       /   /       /       / .        \\         \\       \\       \\     /    [0;40m\r\n");
socket_print(user, "[1;37;44m  /_______/   /____________   / /            \\   ______________\\_______\\ /      [0;40m\r\n");
socket_print(user, "[1;37;44m  \\       \\   \\           \\ /__________________\\ /             /       /        [0;40m\r\n");
socket_print(user, "[1;37;44m    \\_______\\ / \\___________\\                  /_____________/_______/          [0;40m\r\n");
socket_print(user, "[1;37;44m                              \\______________/                                  [0;40m\r\n");
socket_print(user, "[1;37;44m[##][ DAY        GB ] [ WEEK        GB ] [ MONTH       GB ] [ ALLTIME      GB ] [0;40m\r\n");
socket_print(user, "[1;37;44m+[1;36;44m--[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
 socket_print(user, "[1;36;44m|[1;37;44m01[1;36;44m| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |[0;40m\r\n", global_tubd[0] ? global_tuud[0] : (unsigned char *)"-", global_tubd[0] / 1073741824.0, global_tubw[0] ? global_tuuw[0] : (unsigned char *)"-", global_tubw[0] / 1073741824.0, global_tubm[0] ? global_tuum[0] : (unsigned char *)"-", global_tubm[0] / 1073741824.0, global_tub[0] ? global_tuu[0] : (unsigned char *)"-", global_tub[0] / 1073741824.0);
socket_print(user, "[0;36;44m|[1;37;44m02[0;36;44m| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.1f |[0;40m\r\n", global_tubd[1] ? global_tuud[1] : (unsigned char *)"-", global_tubd[1] / 1073741824.0, global_tubw[1] ? global_tuuw[1] : (unsigned char *)"-", global_tubw[1] / 1073741824.0, global_tubm[1] ? global_tuum[1] : (unsigned char *)"-", global_tubm[1] / 1073741824.0, global_tub[1] ? global_tuu[1] : (unsigned char *)"-", global_tub[1] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m03[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.1f [1;34;44m|[0;40m\r\n", global_tubd[2] ? global_tuud[2] : (unsigned char *)"-", global_tubd[2] / 1073741824.0, global_tubw[2] ? global_tuuw[2] : (unsigned char *)"-", global_tubw[2] / 1073741824.0, global_tubm[2] ? global_tuum[2] : (unsigned char *)"-", global_tubm[2] / 1073741824.0, global_tub[2] ? global_tuu[2] : (unsigned char *)"-", global_tub[2] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m04[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.1f [1;34;44m|[0;40m\r\n", global_tubd[3] ? global_tuud[3] : (unsigned char *)"-", global_tubd[3] / 1073741824.0, global_tubw[3] ? global_tuuw[3] : (unsigned char *)"-", global_tubw[3] / 1073741824.0, global_tubm[3] ? global_tuum[3] : (unsigned char *)"-", global_tubm[3] / 1073741824.0, global_tub[3] ? global_tuu[3] : (unsigned char *)"-", global_tub[3] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m05[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.1f [1;34;44m|[0;40m\r\n", global_tubd[4] ? global_tuud[4] : (unsigned char *)"-", global_tubd[4] / 1073741824.0, global_tubw[4] ? global_tuuw[4] : (unsigned char *)"-", global_tubw[4] / 1073741824.0, global_tubm[4] ? global_tuum[4] : (unsigned char *)"-", global_tubm[4] / 1073741824.0, global_tub[4] ? global_tuu[4] : (unsigned char *)"-", global_tub[4] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m06[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.1f [1;34;44m|[0;40m\r\n", global_tubd[5] ? global_tuud[5] : (unsigned char *)"-", global_tubd[5] / 1073741824.0, global_tubw[5] ? global_tuuw[5] : (unsigned char *)"-", global_tubw[5] / 1073741824.0, global_tubm[5] ? global_tuum[5] : (unsigned char *)"-", global_tubm[5] / 1073741824.0, global_tub[5] ? global_tuu[5] : (unsigned char *)"-", global_tub[5] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m07[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.1f [1;34;44m|[0;40m\r\n", global_tubd[6] ? global_tuud[6] : (unsigned char *)"-", global_tubd[6] / 1073741824.0, global_tubw[6] ? global_tuuw[6] : (unsigned char *)"-", global_tubw[6] / 1073741824.0, global_tubm[6] ? global_tuum[6] : (unsigned char *)"-", global_tubm[6] / 1073741824.0, global_tub[6] ? global_tuu[6] : (unsigned char *)"-", global_tub[6] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m08[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.1f [1;34;44m|[0;40m\r\n", global_tubd[7] ? global_tuud[7] : (unsigned char *)"-", global_tubd[7] / 1073741824.0, global_tubw[7] ? global_tuuw[7] : (unsigned char *)"-", global_tubw[7] / 1073741824.0, global_tubm[7] ? global_tuum[7] : (unsigned char *)"-", global_tubm[7] / 1073741824.0, global_tub[7] ? global_tuu[7] : (unsigned char *)"-", global_tub[7] / 1073741824.0);
socket_print(user, "[0;36;44m|[1;37;44m09[0;36;44m| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.1f |[0;40m\r\n", global_tubd[8] ? global_tuud[8] : (unsigned char *)"-", global_tubd[8] / 1073741824.0, global_tubw[8] ? global_tuuw[8] : (unsigned char *)"-", global_tubw[8] / 1073741824.0, global_tubm[8] ? global_tuum[8] : (unsigned char *)"-", global_tubm[8] / 1073741824.0, global_tub[8] ? global_tuu[8] : (unsigned char *)"-", global_tub[8] / 1073741824.0);
socket_print(user, "[1;36;44m|[1;37;44m10[1;36;44m|[0;36;44m %-8.8s %6.2f [1;36;44m|[0;36;44m %-8.8s %7.2f [1;36;44m|[0;36;44m %-8.8s %7.2f [1;36;44m|[0;36;44m %-8.8s %8.1f [1;36;44m|[0;40m\r\n", global_tubd[9] ? global_tuud[9] : (unsigned char *)"-", global_tubd[9] / 1073741824.0, global_tubw[9] ? global_tuuw[9] : (unsigned char *)"-", global_tubw[9] / 1073741824.0, global_tubm[9] ? global_tuum[9] : (unsigned char *)"-", global_tubm[9] / 1073741824.0, global_tub[9] ? global_tuu[9] : (unsigned char *)"-", global_tub[9] / 1073741824.0);
socket_print(user, "[1;37;44m+[1;36;44m--[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
socket_print(user, "[0;36;44m Type [1;36;44mSITE TOPUP <D/W/M/A>[0;36;44m to get detailed information.                         [0;40m\r\n");
}





void local_user_top_up_day_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP UPLOADER DAY ]\r\n");
                          socket_print(user, "        ____________________________________  _______ ________________\r\n");
                          socket_print(user, "       /___   ______  \\_____  \\\\   \\______  \\/\\____  \\_\\  _   \\   \\   \\\r\n");
                          socket_print(user, "        \\/   /\\/   /   /  _____\\\\   \\  ______\\ \\\\  \\   \\   _   \\____  .\\\r\n");
                          socket_print(user, "        /   / /   /   /.    \\    \\   \\   \\  /   \\.  \\   \\   \\   \\   \\  \\\\\r\n");
                          socket_print(user, "       /___/ /______ //      \\ _______\\___\\/ \\   \\______/\\___\\___\\_______\\\r\n");
                          socket_print(user, "       \\   \\ \\     \\/_________\\/      /   /   \\  /     / /   /   /       /\r\n");
                          socket_print(user, "        \\___\\/\\_____\\         /______/___/     \\/_____/\\/___/___/_______/\r\n");
                          socket_print(user, "+----+----------+----\\_______/-------+-------+---------------------------------+\r\n");
                          socket_print(user, "| ## | USER     | GROUP    | GBYTES  |AVG K/S| TAGLINE                         |\r\n");
                          socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
      if (global_tubd[0]) socket_print(user, "| 01 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[0], global_tugd[0], global_tubd[0] / 1073741824.0, global_tubd[0] / global_tusd[0] / 1024.0, global_tutd[0]);
      if (global_tubd[1]) socket_print(user, "| 02 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[1], global_tugd[1], global_tubd[1] / 1073741824.0, global_tubd[1] / global_tusd[1] / 1024.0, global_tutd[1]);
      if (global_tubd[2]) socket_print(user, "| 03 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[2], global_tugd[2], global_tubd[2] / 1073741824.0, global_tubd[2] / global_tusd[2] / 1024.0, global_tutd[2]);
      if (global_tubd[3]) socket_print(user, "| 04 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[3], global_tugd[3], global_tubd[3] / 1073741824.0, global_tubd[3] / global_tusd[3] / 1024.0, global_tutd[3]);
      if (global_tubd[4]) socket_print(user, "| 05 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[4], global_tugd[4], global_tubd[4] / 1073741824.0, global_tubd[4] / global_tusd[4] / 1024.0, global_tutd[4]);
      if (global_tubd[5]) socket_print(user, "| 06 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[5], global_tugd[5], global_tubd[5] / 1073741824.0, global_tubd[5] / global_tusd[5] / 1024.0, global_tutd[5]);
      if (global_tubd[6]) socket_print(user, "| 07 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[6], global_tugd[6], global_tubd[6] / 1073741824.0, global_tubd[6] / global_tusd[6] / 1024.0, global_tutd[6]);
      if (global_tubd[7]) socket_print(user, "| 08 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[7], global_tugd[7], global_tubd[7] / 1073741824.0, global_tubd[7] / global_tusd[7] / 1024.0, global_tutd[7]);
      if (global_tubd[8]) socket_print(user, "| 09 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[8], global_tugd[8], global_tubd[8] / 1073741824.0, global_tubd[8] / global_tusd[8] / 1024.0, global_tutd[8]);
      if (global_tubd[9]) socket_print(user, "| 10 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuud[9], global_tugd[9], global_tubd[9] / 1073741824.0, global_tubd[9] / global_tusd[9] / 1024.0, global_tutd[9]);
                          socket_print(user, "+----+----------+----------+---------+-------+--------------------[Hour %2.2d/24]-+\r\n", global_hourofday);
        if (global_tulbd) socket_print(user, "| YESTERDAYS #1:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tulud, global_tulbd / 1073741824.0, global_tulfd, global_tulbd / global_tulsd / 1024.0);
        if (global_tumbd) socket_print(user, ": DAYTOP RECORD:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tumud, global_tumbd / 1073741824.0 , global_tumfd, global_tumbd / global_tumsd / 1024.0);
        if (global_ubd)   socket_print(user, ".    DAY TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ubw / 1073741824.0, global_ufw);
}

void local_user_top_up_day_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP UPLOADER DAY ]\r\n");
                          socket_print(user, "[1;37;44m        ____________________________________  _______ ________________          [0;40m\r\n");
                          socket_print(user, "[1;37;44m       /___   ______  \\_____  \\\\   \\______  \\/\\____  \\_\\  _   \\   \\   \\         [0;40m\r\n");
                          socket_print(user, "[1;37;44m        \\/   /\\/   /   /  _____\\\\   \\  ______\\ \\\\  \\   \\   _   \\____  .\\        [0;40m\r\n");
                          socket_print(user, "[1;37;44m        /   / /   /   /.    \\    \\   \\   \\  /   \\.  \\   \\   \\   \\   \\  \\\\       [0;40m\r\n");
                          socket_print(user, "[1;37;44m       /___/ /______ //      \\ _______\\___\\/ \\   \\______/\\___\\___\\_______\\      [0;40m\r\n");
                          socket_print(user, "[1;37;44m       \\   \\ \\     \\/_________\\/      /   /   \\  /     / /   /   /       /      [0;40m\r\n");
                          socket_print(user, "[1;37;44m        \\___\\/\\_____\\         /______/___/     \\/_____/\\/___/___/_______/       [0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--[1;37;44m\\_______/[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                          socket_print(user, "[1;36;44m| [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mGBYTES  [1;36;44m|[1;37;44mAVG K/S[1;36;44m| [1;37;44mTAGLINE                         [1;37;44m|[0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
      if (global_tubd[0]) socket_print(user, "[1;36;44m| [1;37;44m01 [1;36;44m| %-8.8s | %-8.8s |%8.2f |%6.2f| %-31.31s |[0;40m\r\n", global_tuud[0], global_tugd[0], global_tubd[0] / 1073741824.0, global_tubd[0] / global_tusd[0] / 1024.0, global_tutd[0]);
      if (global_tubd[1]) socket_print(user, "[0;36;44m| [1;37;44m02 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuud[1], global_tugd[1], global_tubd[1] / 1073741824.0, global_tubd[1] / global_tusd[1] / 1024.0, global_tutd[1]);
      if (global_tubd[2]) socket_print(user, "[1;34;44m| [1;37;44m03 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuud[2], global_tugd[2], global_tubd[2] / 1073741824.0, global_tubd[2] / global_tusd[2] / 1024.0, global_tutd[2]);
      if (global_tubd[3]) socket_print(user, "[1;34;44m| [1;37;44m04 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuud[3], global_tugd[3], global_tubd[3] / 1073741824.0, global_tubd[3] / global_tusd[3] / 1024.0, global_tutd[3]);
      if (global_tubd[4]) socket_print(user, "[1;34;44m| [1;37;44m05 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuud[4], global_tugd[4], global_tubd[4] / 1073741824.0, global_tubd[4] / global_tusd[4] / 1024.0, global_tutd[4]);
      if (global_tubd[5]) socket_print(user, "[1;34;44m| [1;37;44m06 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuud[5], global_tugd[5], global_tubd[5] / 1073741824.0, global_tubd[5] / global_tusd[5] / 1024.0, global_tutd[5]);
      if (global_tubd[6]) socket_print(user, "[1;34;44m| [1;37;44m07 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuud[6], global_tugd[6], global_tubd[6] / 1073741824.0, global_tubd[6] / global_tusd[6] / 1024.0, global_tutd[6]);
      if (global_tubd[7]) socket_print(user, "[1;34;44m| [1;37;44m08 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuud[7], global_tugd[7], global_tubd[7] / 1073741824.0, global_tubd[7] / global_tusd[7] / 1024.0, global_tutd[7]);
      if (global_tubd[8]) socket_print(user, "[0;36;44m| [1;37;44m09 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuud[8], global_tugd[8], global_tubd[8] / 1073741824.0, global_tubd[8] / global_tusd[8] / 1024.0, global_tutd[8]);
      if (global_tubd[9]) socket_print(user, "[1;36;44m| [1;37;44m10 [1;36;44m| [0;36;44m%-8.8s [1;36;44m| [0;36;44m%-8.8s [1;36;44m|[0;36;44m%8.2f [1;36;44m|[0;36;44m%6.0f [1;36;44m| [0;36;44m%-31.31s [1;36;44m|[0;40m\r\n", global_tuud[9], global_tugd[9], global_tubd[9] / 1073741824.0, global_tubd[9] / global_tusd[9] / 1024.0, global_tutd[9]);
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------------------[[0;36;44mHour [1;36;44m%2.2d[0;36;44m/24[1;34;44m][1;36;44m-[1;37;44m+[0;40m\r\n", global_hourofday);
        if (global_tulbd) socket_print(user, "[1;36;44m|[0;36;44m YESTERDAYS #1:  [1;36;44m%s [0;36;44m%with [1;36;44m%%.2f Gbytes [0;36;44m%in [1;36;44m%%.0f Files [0;36;44m%at [1;36;44m%%.2f K/s[0;40m\r\n", global_tulud, global_tulbd / 1073741824.0, global_tulfd, global_tulbd / global_tulsd / 1024.0);
        if (global_tumbd) socket_print(user, "[0;36;44m:[0;36;44m DAYTOP RECORD:  [1;36;44m%s [0;36;44m%with [1;36;44m%%.2f Gbytes [0;36;44m%in [1;36;44m%%.0f Files [0;36;44m%at [1;36;44m%%.2f K/s[0;40m\r\n", global_tumud, global_tumbd / 1073741824.0 , global_tumfd, global_tumbd / global_tumsd / 1024.0);
        if (global_ubd)   socket_print(user, "[1;34;44m.[0;36;44m    DAY TOTALS:  [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files[0;40m\r\n", global_ubw / 1073741824.0, global_ufw);
}



void local_user_top_up_week_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP UPLOADER WEEK ]\r\n");
                          socket_print(user, "   ____________________________________  ____ ____________________________\r\n");
                          socket_print(user, "  /___   ______  \\_____  \\\\   \\______  \\/\\   \\\\   \\    ___\\    ___\\   \\   \\\r\n");
                          socket_print(user, "   \\/   /\\/   /   /  _____\\\\   \\   _____\\ \\   \\\\   \\\\   ___\\\\   ___\\     -'_\r\n");
                          socket_print(user, "   /   / /   /   //    \\    \\   \\   \\  /   \\   \\\\   \\.  \\   \\.  \\   \\   \\   \\\r\n");
                          socket_print(user, "  /___/ /______ /.      \\ _______\\___\\/ \\   \\___/\\___\\_______\\_______\\___\\___\\\r\n");
                          socket_print(user, "  \\   \\ \\     \\/_________\\/      /   /   \\  /   //   /       /       /   /   /\r\n");
                          socket_print(user, "   \\___\\/\\_____\\         /______/___/     \\/___/\\___/_______/_______/___/___/\r\n");
                          socket_print(user, "+----+----------\\_______/--+---------+-------+---------------------------------+\r\n");
                          socket_print(user, "| ## | USER     | GROUP    | GBYTES  |AVG K/S| TAGLINE                         |\r\n");
                          socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
      if (global_tubw[0]) socket_print(user, "| 01 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[0], global_tugw[0], global_tubw[0] / 1073741824.0, global_tubw[0] / global_tusw[0] / 1024.0, global_tutw[0]);
      if (global_tubw[1]) socket_print(user, "| 02 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[1], global_tugw[1], global_tubw[1] / 1073741824.0, global_tubw[1] / global_tusw[1] / 1024.0, global_tutw[1]);
      if (global_tubw[2]) socket_print(user, "| 03 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[2], global_tugw[2], global_tubw[2] / 1073741824.0, global_tubw[2] / global_tusw[2] / 1024.0, global_tutw[2]);
      if (global_tubw[3]) socket_print(user, "| 04 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[3], global_tugw[3], global_tubw[3] / 1073741824.0, global_tubw[3] / global_tusw[3] / 1024.0, global_tutw[3]);
      if (global_tubw[4]) socket_print(user, "| 05 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[4], global_tugw[4], global_tubw[4] / 1073741824.0, global_tubw[4] / global_tusw[4] / 1024.0, global_tutw[4]);
      if (global_tubw[5]) socket_print(user, "| 06 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[5], global_tugw[5], global_tubw[5] / 1073741824.0, global_tubw[5] / global_tusw[5] / 1024.0, global_tutw[5]);
      if (global_tubw[6]) socket_print(user, "| 07 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[6], global_tugw[6], global_tubw[6] / 1073741824.0, global_tubw[6] / global_tusw[6] / 1024.0, global_tutw[6]);
      if (global_tubw[7]) socket_print(user, "| 08 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[7], global_tugw[7], global_tubw[7] / 1073741824.0, global_tubw[7] / global_tusw[7] / 1024.0, global_tutw[7]);
      if (global_tubw[8]) socket_print(user, "| 09 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[8], global_tugw[8], global_tubw[8] / 1073741824.0, global_tubw[8] / global_tusw[8] / 1024.0, global_tutw[8]);
      if (global_tubw[9]) socket_print(user, "| 10 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuuw[9], global_tugw[9], global_tubw[9] / 1073741824.0, global_tubw[9] / global_tusw[9] / 1024.0, global_tutw[9]);
                          socket_print(user, "+----+----------+----------+---------+-------+-----------------------[Day %d/7]-+\r\n", global_dayofweek);
        if (global_tulbw) socket_print(user, "|  LAST WEEKS #1:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tuluw, global_tulbw / 1073741824.0, global_tulfw, global_tulbw / global_tulsw / 1024.0);
        if (global_tumbw) socket_print(user, ": WEEKTOP RECORD:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tumuw, global_tumbw / 1073741824.0 ,global_tumfw,global_tumbw /global_tumsw / 1024.0);
        if (global_ubw)   socket_print(user, ".    WEEK TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ubw / 1073741824.0, global_ufw);
}

void local_user_top_up_week_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP UPLOADER WEEK ]\r\n");
                          socket_print(user, "[1;37;44m   ____________________________________  ____ ____________________________      [0;40m\r\n");
                          socket_print(user, "[1;37;44m  /___   ______  \\_____  \\\\   \\______  \\/\\   \\\\   \\    ___\\    ___\\   \\   \\     [0;40m\r\n");
                          socket_print(user, "[1;37;44m   \\/   /\\/   /   /  _____\\\\   \\   _____\\ \\   \\\\   \\\\   ___\\\\   ___\\     -'_    [0;40m\r\n");
                          socket_print(user, "[1;37;44m   /   / /   /   //    \\    \\   \\   \\  /   \\   \\\\   \\.  \\   \\.  \\   \\   \\   \\   [0;40m\r\n");
                          socket_print(user, "[1;37;44m  /___/ /______ /.      \\ _______\\___\\/ \\   \\___/\\___\\_______\\_______\\___\\___\\  [0;40m\r\n");
                          socket_print(user, "[1;37;44m  \\   \\ \\     \\/_________\\/      /   /   \\  /   //   /       /       /   /   /  [0;40m\r\n");
                          socket_print(user, "[1;37;44m   \\___\\/\\_____\\         /______/___/     \\/___/\\___/_______/_______/___/___/   [0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------[1;37;44m\\_______/[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                          socket_print(user, "[1;36;44m| [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mGBYTES  [1;36;44m|[1;37;44mAVG K/S[1;36;44m| [1;37;44mTAGLINE                         [1;36;44m|[0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
      if (global_tubw[0]) socket_print(user, "[1;36;44m| [1;37;44m01 [1;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuuw[0], global_tugw[0], global_tubw[0] / 1073741824.0, global_tubw[0] / global_tusw[0] / 1024.0, global_tutw[0]);
      if (global_tubw[1]) socket_print(user, "[0;36;44m| [1;37;44m02 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuuw[1], global_tugw[1], global_tubw[1] / 1073741824.0, global_tubw[1] / global_tusw[1] / 1024.0, global_tutw[1]);
      if (global_tubw[2]) socket_print(user, "[1;34;44m| [1;37;44m03 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuuw[2], global_tugw[2], global_tubw[2] / 1073741824.0, global_tubw[2] / global_tusw[2] / 1024.0, global_tutw[2]);
      if (global_tubw[3]) socket_print(user, "[1;34;44m| [1;37;44m04 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuuw[3], global_tugw[3], global_tubw[3] / 1073741824.0, global_tubw[3] / global_tusw[3] / 1024.0, global_tutw[3]);
      if (global_tubw[4]) socket_print(user, "[1;34;44m| [1;37;44m05 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuuw[4], global_tugw[4], global_tubw[4] / 1073741824.0, global_tubw[4] / global_tusw[4] / 1024.0, global_tutw[4]);
      if (global_tubw[5]) socket_print(user, "[1;34;44m| [1;37;44m06 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuuw[5], global_tugw[5], global_tubw[5] / 1073741824.0, global_tubw[5] / global_tusw[5] / 1024.0, global_tutw[5]);
      if (global_tubw[6]) socket_print(user, "[1;34;44m| [1;37;44m07 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuuw[6], global_tugw[6], global_tubw[6] / 1073741824.0, global_tubw[6] / global_tusw[6] / 1024.0, global_tutw[6]);
      if (global_tubw[7]) socket_print(user, "[1;34;44m| [1;37;44m08 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuuw[7], global_tugw[7], global_tubw[7] / 1073741824.0, global_tubw[7] / global_tusw[7] / 1024.0, global_tutw[7]);
      if (global_tubw[8]) socket_print(user, "[0;36;44m| [1;37;44m09 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuuw[8], global_tugw[8], global_tubw[8] / 1073741824.0, global_tubw[8] / global_tusw[8] / 1024.0, global_tutw[8]);
      if (global_tubw[9]) socket_print(user, "[1;36;44m| [1;37;44m10 [1;36;44m| [0;36;44m%-8.8s [1;36;44m| [0;36;44m%-8.8s [1;36;44m|[0;36;44m%8.2f [1;36;44m|[0;36;44m%6.0f [1;36;44m| [0;36;44m%-31.31s [1;36;44m|[0;40m\r\n", global_tuuw[9], global_tugw[9], global_tubw[9] / 1073741824.0, global_tubw[9] / global_tusw[9] / 1024.0, global_tutw[9]);
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------------[[0;36;44mDay [1;36;44m%d[0;36;44m/7[1;34;44m][1;36;44m-[1;37;44m+[0;40m\r\n", global_dayofweek);
        if (global_tulbw) socket_print(user, "[1;36;44m|[0;36;44m  LAST WEEKS #1:  [1;36;44m%s [0;36;44mwith [1;36;44m%.2f Gbytes [1;37;44min %.0f Files [0;36;44mat [1;36;44m%.2f [0;36;44mK/s\r\n", global_tuluw, global_tulbw / 1073741824.0, global_tulfw, global_tulbw / global_tulsw / 1024.0);
        if (global_tumbw) socket_print(user, "[0;36;44m:[0;36;44m WEEKTOP RECORD:  [1;36;44m%s [0;36;44mwith [1;36;44m%.2f Gbytes [1;37;44min %.0f Files [0;36;44mat [1;36;44m%.2f [0;36;44mK/s\r\n", global_tumuw, global_tumbw / 1073741824.0 ,global_tumfw,global_tumbw /global_tumsw / 1024.0);
        if (global_ubw)   socket_print(user, "[1;34;44m.[0;36;44m    WEEK TOTALS:  [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files\r\n", global_ubw / 1073741824.0, global_ufw);
}



void local_user_top_up_month_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP UPLOADER MONTH ]\r\n");
                          socket_print(user, "   _______________________________  _____________________________________\r\n");
                          socket_print(user, "  /___  ______ \\____  \\\\  \\_____  \\/\\_______ \\____  \\____ _\\  ____\\   \\  \\\r\n");
                          socket_print(user, "   \\/  /\\/   /  / _____\\\\  \\  _____\\/ \\  \\  \\ \\   \\  \\   \\ \\\\  \\  /\\   _  \\\r\n");
                          socket_print(user, "   /  / /   /  //   \\\\   \\  \\  \\  //   \\  \\  \\ \\   \\  \\   \\ .\\  \\/  \\   \\  \\\r\n");
                          socket_print(user, "  /__/ /_____ /.     \\.______\\__\\/  \\   \\__\\__\\_\\______\\___\\__\\__\\   \\___\\__\\\r\n");
                          socket_print(user, "  \\  \\ \\    \\/________\\/     /  /    \\  /  /  / /      /   /  /  /\\  /   /  /\r\n");
                          socket_print(user, "   \\__\\/\\____\\        /_____/__/      \\/__/__/_/______/___/__/__/  \\/___/__/\r\n");
                          socket_print(user, "+----+--------\\______/-----+---------+-------+---------------------------------+\r\n");
                          socket_print(user, "| ## | USER     | GROUP    | GBYTES  |AVG K/S| TAGLINE                         |\r\n");
                          socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
      if (global_tubm[0]) socket_print(user, "| 01 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[0], global_tugm[0], global_tubm[0] / 1073741824.0, global_tubm[0] / global_tusm[0] / 1024.0, global_tutm[0]);
      if (global_tubm[1]) socket_print(user, "| 02 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[1], global_tugm[1], global_tubm[1] / 1073741824.0, global_tubm[1] / global_tusm[1] / 1024.0, global_tutm[1]);
      if (global_tubm[2]) socket_print(user, "| 03 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[2], global_tugm[2], global_tubm[2] / 1073741824.0, global_tubm[2] / global_tusm[2] / 1024.0, global_tutm[2]);
      if (global_tubm[3]) socket_print(user, "| 04 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[3], global_tugm[3], global_tubm[3] / 1073741824.0, global_tubm[3] / global_tusm[3] / 1024.0, global_tutm[3]);
      if (global_tubm[4]) socket_print(user, "| 05 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[4], global_tugm[4], global_tubm[4] / 1073741824.0, global_tubm[4] / global_tusm[4] / 1024.0, global_tutm[4]);
      if (global_tubm[5]) socket_print(user, "| 06 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[5], global_tugm[5], global_tubm[5] / 1073741824.0, global_tubm[5] / global_tusm[5] / 1024.0, global_tutm[5]);
      if (global_tubm[6]) socket_print(user, "| 07 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[6], global_tugm[6], global_tubm[6] / 1073741824.0, global_tubm[6] / global_tusm[6] / 1024.0, global_tutm[6]);
      if (global_tubm[7]) socket_print(user, "| 08 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[7], global_tugm[7], global_tubm[7] / 1073741824.0, global_tubm[7] / global_tusm[7] / 1024.0, global_tutm[7]);
      if (global_tubm[8]) socket_print(user, "| 09 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[8], global_tugm[8], global_tubm[8] / 1073741824.0, global_tubm[8] / global_tusm[8] / 1024.0, global_tutm[8]);
      if (global_tubm[9]) socket_print(user, "| 10 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuum[9], global_tugm[9], global_tubm[9] / 1073741824.0, global_tubm[9] / global_tusm[9] / 1024.0, global_tutm[9]);
                          socket_print(user, "+----+----------+----------+---------+-------+---------------------[Day %2.2d/%2.2d]-+\r\n", global_dayofmonth, global_daysinmonth);
        if (global_tulbm) socket_print(user, "|  LAST MONTHS #1:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tulum, global_tulbm / 1073741824.0, global_tulfm, global_tulbm / global_tulsm / 1024.0);
        if (global_tumbm) socket_print(user, ": MONTHTOP RECORD:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tumum, global_tumbm / 1073741824.0 ,global_tumfm,global_tumbm /global_tumsm / 1024.0);
        if (global_ubm)   socket_print(user, ".    MONTH TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ubm / 1073741824.0, global_ufm);
}

void local_user_top_up_month_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP UPLOADER MONTH ]\r\n");
                          socket_print(user, "[1;37;44m   _______________________________  _____________________________________       [0;40m\r\n");
                          socket_print(user, "[1;37;44m  /___  ______ \\____  \\\\  \\_____  \\/\\_______ \\____  \\____ _\\  ____\\   \\  \\      [0;40m\r\n");
                          socket_print(user, "[1;37;44m   \\/  /\\/   /  / _____\\\\  \\  _____\\/ \\  \\  \\ \\   \\  \\   \\ \\\\  \\  /\\   _  \\     [0;40m\r\n");
                          socket_print(user, "[1;37;44m   /  / /   /  //   \\\\   \\  \\  \\  //   \\  \\  \\ \\   \\  \\   \\ .\\  \\/  \\   \\  \\    [0;40m\r\n");
                          socket_print(user, "[1;37;44m  /__/ /_____ /.     \\.______\\__\\/  \\   \\__\\__\\_\\______\\___\\__\\__\\   \\___\\__\\   [0;40m\r\n");
                          socket_print(user, "[1;37;44m  \\  \\ \\    \\/________\\/     /  /    \\  /  /  / /      /   /  /  /\\  /   /  /   [0;40m\r\n");
                          socket_print(user, "[1;37;44m   \\__\\/\\____\\        /_____/__/      \\/__/__/_/______/___/__/__/  \\/___/__/    [0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[1;37;44m\\______/[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                          socket_print(user, "[1;36;44m| [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mGBYTES  [1;36;44m|[1;37;44mAVG K/S[1;36;44m| [1;37;44mTAGLINE                         [1;37;44m|[0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
      if (global_tubm[0]) socket_print(user, "[1;36;44m| [1;37;44m01 [1;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuum[0], global_tugm[0], global_tubm[0] / 1073741824.0, global_tubm[0] / global_tusm[0] / 1024.0, global_tutm[0]);
      if (global_tubm[1]) socket_print(user, "[0;36;44m| [1;37;44m02 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuum[1], global_tugm[1], global_tubm[1] / 1073741824.0, global_tubm[1] / global_tusm[1] / 1024.0, global_tutm[1]);
      if (global_tubm[2]) socket_print(user, "[1;34;44m| [1;37;44m03 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuum[2], global_tugm[2], global_tubm[2] / 1073741824.0, global_tubm[2] / global_tusm[2] / 1024.0, global_tutm[2]);
      if (global_tubm[3]) socket_print(user, "[1;34;44m| [1;37;44m04 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuum[3], global_tugm[3], global_tubm[3] / 1073741824.0, global_tubm[3] / global_tusm[3] / 1024.0, global_tutm[3]);
      if (global_tubm[4]) socket_print(user, "[1;34;44m| [1;37;44m05 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuum[4], global_tugm[4], global_tubm[4] / 1073741824.0, global_tubm[4] / global_tusm[4] / 1024.0, global_tutm[4]);
      if (global_tubm[5]) socket_print(user, "[1;34;44m| [1;37;44m06 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuum[5], global_tugm[5], global_tubm[5] / 1073741824.0, global_tubm[5] / global_tusm[5] / 1024.0, global_tutm[5]);
      if (global_tubm[6]) socket_print(user, "[1;34;44m| [1;37;44m07 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuum[6], global_tugm[6], global_tubm[6] / 1073741824.0, global_tubm[6] / global_tusm[6] / 1024.0, global_tutm[6]);
      if (global_tubm[7]) socket_print(user, "[1;34;44m| [1;37;44m08 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuum[7], global_tugm[7], global_tubm[7] / 1073741824.0, global_tubm[7] / global_tusm[7] / 1024.0, global_tutm[7]);
      if (global_tubm[8]) socket_print(user, "[0;36;44m| [1;37;44m09 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuum[8], global_tugm[8], global_tubm[8] / 1073741824.0, global_tubm[8] / global_tusm[8] / 1024.0, global_tutm[8]);
      if (global_tubm[9]) socket_print(user, "[1;36;44m| [1;37;44m10 [1;36;44m| [0;36;44m%-8.8s [1;36;44m| [0;36;44m%-8.8s [1;36;44m|[0;36;44m%8.2f [1;36;44m|[0;36;44m%6.0f [1;36;44m| [0;36;44m%-31.31s [1;36;44m|[0;40m\r\n", global_tuum[9], global_tugm[9], global_tubm[9] / 1073741824.0, global_tubm[9] / global_tusm[9] / 1024.0, global_tutm[9]);
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------------[[0;36;44mDay [1;36;44m%2.2d[0;36;44m/%2.2d[1;34;44m][1;36;44m-[1;37;44m+[0;40m\r\n", global_dayofmonth, global_daysinmonth);
        if (global_tulbm) socket_print(user, "[1;36;44m|[0;36;44m  LAST MONTHS #1:  [1;36;44m%s [0;36;44mwith [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files [0;36;44mat [1;36;44m%.2f K/s[0;40m\r\n", global_tulum, global_tulbm / 1073741824.0, global_tulfm, global_tulbm / global_tulsm / 1024.0);
        if (global_tumbm) socket_print(user, "[0;36;44m:[0;36;44m MONTHTOP RECORD:  [1;36;44m%s [0;36;44mwith [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files [0;36;44mat [1;36;44m%.2f K/s[0;40m\r\n", global_tumum, global_tumbm / 1073741824.0 ,global_tumfm,global_tumbm /global_tumsm / 1024.0);
        if (global_ubm)   socket_print(user, "[1;34;44m.[0;36;44m    MONTH TOTALS:  [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files[0;40m\r\n", global_ubm / 1073741824.0, global_ufm);
}


void local_user_top_up_alltime_mono(struct login_node *user, int number, char *opt)
{
                         socket_print(user, "200- [ TOP UPLOADER ALLTIME ]\r\n");
                         socket_print(user, "     _______________________________  ___________   ____ ________________\r\n");
                         socket_print(user, "    /___  ______ \\____  \\\\  \\_____  \\/_\\  _  \\   \\ /\\   \\\\__   __\\______ \\\r\n");
                         socket_print(user, "     \\/  /\\/   /  / _____\\\\  \\  _____\\ \\   _  \\   \\__\\   \\_/\\  \\ /\\  \\  \\ \\\r\n");
                         socket_print(user, "     /  / /   /  /    \\\\   \\  \\  \\  /   \\   \\  \\\\  \\  \\\\  \\  \\  \\  \\  \\  \\ \\\r\n");
                         socket_print(user, "    /__/ /_____ /      \\.______\\__\\/ \\   \\___\\__\\._____\\._____\\__\\  \\__\\__\\_\\\r\n");
                         socket_print(user, "    \\  \\ \\    \\/________\\/     /  /   \\  /   /  /      /      /  /  /  /  / /\r\n");
                         socket_print(user, "     \\__\\/\\____\\        /_____/__/     \\/___/__/______/______/__/ \\/__/__/_/\r\n");
                         socket_print(user, "+----+----------\\______/---+---------+-------+---------------------------------+\r\n");
                         socket_print(user, "| ## | USER     | GROUP    | GBYTES  |AVG K/S| TAGLINE                         |\r\n");
                         socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
      if (global_tub[0]) socket_print(user, "| 01 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[0], global_tug[0], global_tub[0] / 1073741824.0, global_tub[0] / global_tus[0] / 1024.0, global_tut[0]);
      if (global_tub[1]) socket_print(user, "| 02 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[1], global_tug[1], global_tub[1] / 1073741824.0, global_tub[1] / global_tus[1] / 1024.0, global_tut[1]);
      if (global_tub[2]) socket_print(user, "| 03 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[2], global_tug[2], global_tub[2] / 1073741824.0, global_tub[2] / global_tus[2] / 1024.0, global_tut[2]);
      if (global_tub[3]) socket_print(user, "| 04 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[3], global_tug[3], global_tub[3] / 1073741824.0, global_tub[3] / global_tus[3] / 1024.0, global_tut[3]);
      if (global_tub[4]) socket_print(user, "| 05 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[4], global_tug[4], global_tub[4] / 1073741824.0, global_tub[4] / global_tus[4] / 1024.0, global_tut[4]);
      if (global_tub[5]) socket_print(user, "| 06 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[5], global_tug[5], global_tub[5] / 1073741824.0, global_tub[5] / global_tus[5] / 1024.0, global_tut[5]);
      if (global_tub[6]) socket_print(user, "| 07 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[6], global_tug[6], global_tub[6] / 1073741824.0, global_tub[6] / global_tus[6] / 1024.0, global_tut[6]);
      if (global_tub[7]) socket_print(user, "| 08 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[7], global_tug[7], global_tub[7] / 1073741824.0, global_tub[7] / global_tus[7] / 1024.0, global_tut[7]);
      if (global_tub[8]) socket_print(user, "| 09 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[8], global_tug[8], global_tub[8] / 1073741824.0, global_tub[8] / global_tus[8] / 1024.0, global_tut[8]);
      if (global_tub[9]) socket_print(user, "| 10 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tuu[9], global_tug[9], global_tub[9] / 1073741824.0, global_tub[9] / global_tus[9] / 1024.0, global_tut[9]);
                         socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
        if (global_ub)   socket_print(user, ". TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ub / 1073741824.0, global_uf);
}

void local_user_top_up_alltime_colour(struct login_node *user, int number, char *opt)
{
                         socket_print(user, "200- [ TOP UPLOADER ALLTIME ]\r\n");
                         socket_print(user, "[1;37;44m     _______________________________  ___________   ____ ________________       [0;40m\r\n");
                         socket_print(user, "[1;37;44m    /___  ______ \\____  \\\\  \\_____  \\/_\\  _  \\   \\ /\\   \\\\__   __\\______ \\      [0;40m\r\n");
                         socket_print(user, "[1;37;44m     \\/  /\\/   /  / _____\\\\  \\  _____\\ \\   _  \\   \\__\\   \\_/\\  \\ /\\  \\  \\ \\     [0;40m\r\n");
                         socket_print(user, "[1;37;44m     /  / /   /  /    \\\\   \\  \\  \\  /   \\   \\  \\\\  \\  \\\\  \\  \\  \\  \\  \\  \\ \\    [0;40m\r\n");
                         socket_print(user, "[1;37;44m    /__/ /_____ /      \\.______\\__\\/ \\   \\___\\__\\._____\\._____\\__\\  \\__\\__\\_\\   [0;40m\r\n");
                         socket_print(user, "[1;37;44m    \\  \\ \\    \\/________\\/     /  /   \\  /   /  /      /      /  /  /  /  / /   [0;40m\r\n");
                         socket_print(user, "[1;37;44m     \\__\\/\\____\\        /_____/__/     \\/___/__/______/______/__/ \\/__/__/_/    [0;40m\r\n");
                         socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------[1;37;44m\\______/[1;34;44m-[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                         socket_print(user, "[1;36;44m| [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mGBYTES  [1;36;44m|[1;37;44mAVG K/S[1;36;44m| [1;37;44mTAGLINE                         [1;37;44m|[0;40m\r\n");
                         socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
      if (global_tub[0]) socket_print(user, "[1;36;44m| [1;37;44m01 [1;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuu[0], global_tug[0], global_tub[0] / 1073741824.0, global_tub[0] / global_tus[0] / 1024.0, global_tut[0]);
      if (global_tub[1]) socket_print(user, "[0;36;44m| [1;37;44m02 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuu[1], global_tug[1], global_tub[1] / 1073741824.0, global_tub[1] / global_tus[1] / 1024.0, global_tut[1]);
      if (global_tub[2]) socket_print(user, "[1;34;44m| [1;37;44m03 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuu[2], global_tug[2], global_tub[2] / 1073741824.0, global_tub[2] / global_tus[2] / 1024.0, global_tut[2]);
      if (global_tub[3]) socket_print(user, "[1;34;44m| [1;37;44m04 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuu[3], global_tug[3], global_tub[3] / 1073741824.0, global_tub[3] / global_tus[3] / 1024.0, global_tut[3]);
      if (global_tub[4]) socket_print(user, "[1;34;44m| [1;37;44m05 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuu[4], global_tug[4], global_tub[4] / 1073741824.0, global_tub[4] / global_tus[4] / 1024.0, global_tut[4]);
      if (global_tub[5]) socket_print(user, "[1;34;44m| [1;37;44m06 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuu[5], global_tug[5], global_tub[5] / 1073741824.0, global_tub[5] / global_tus[5] / 1024.0, global_tut[5]);
      if (global_tub[6]) socket_print(user, "[1;34;44m| [1;37;44m07 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuu[6], global_tug[6], global_tub[6] / 1073741824.0, global_tub[6] / global_tus[6] / 1024.0, global_tut[6]);
      if (global_tub[7]) socket_print(user, "[1;34;44m| [1;37;44m08 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tuu[7], global_tug[7], global_tub[7] / 1073741824.0, global_tub[7] / global_tus[7] / 1024.0, global_tut[7]);
      if (global_tub[8]) socket_print(user, "[0;36;44m| [1;37;44m09 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tuu[8], global_tug[8], global_tub[8] / 1073741824.0, global_tub[8] / global_tus[8] / 1024.0, global_tut[8]);
      if (global_tub[9]) socket_print(user, "[1;36;44m| [1;37;44m10 [1;36;44m| [0;36;44m%-8.8s [1;36;44m| [0;36;44m%-8.8s [1;36;44m|[0;36;44m%8.2f [1;36;44m|[0;36;44m%6.0f [1;36;44m| [0;36;44m%-31.31s [1;36;44m|[0;40m\r\n", global_tuu[9], global_tug[9], global_tub[9] / 1073741824.0, global_tub[9] / global_tus[9] / 1024.0, global_tut[9]);
                         socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
        if (global_ub)   socket_print(user, "[1;36;44m. [0;36;44mTOTALS:  [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files[0;40m\r\n", global_ub / 1073741824.0, global_uf);
}













void local_user_top_down_mono(struct login_node *user, int number, char *opt)
{
socket_print(user, "200- [ TOP DOWNLOADER DAY ]\r\n");
socket_print(user, " ________________________________________________________________________\r\n");
socket_print(user, "/_______     _____________    \\__________    \\__     \\_____________   ___\\\r\n");
socket_print(user, "  \\ /       / \\ /       /       /    __________\\ \\       \\         \\     \\ \\\r\n");
socket_print(user, "  /       /   /       /       / .        \\ \\       \\       \\ \\       \\     . \\\r\n");
socket_print(user, "/_______/   _____________   / /            \\ . ____________/ \\_________\\_______\\\r\n");
socket_print(user, "\\       \\   \\           \\ /__________________\\ /         /   /       /         /\r\n");
socket_print(user, "  \\_______\\ / \\___________\\                  /__________/ \\ /_______/_________/\r\n");
socket_print(user, "                            \\______________/\r\n");
socket_print(user, "[##][ DAY        GB ] [ WEEK        GB ] [ MONTH       GB ] [ ALLTIME      GB ]\r\n");
socket_print(user, "+--+-----------------+------------------+------------------+-------------------+\r\n");
socket_print(user, "|01| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[0] ? global_tdud[0] : (unsigned char *)"-", global_tdbd[0] / 1073741824.0, global_tdbw[0] ? global_tduw[0] : (unsigned char *)"-", global_tdbw[0] / 1073741824.0, global_tdbm[0] ? global_tdum[0] : (unsigned char *)"-", global_tdbm[0] / 1073741824.0, global_tdb[0] ? global_tdu[0] : (unsigned char *)"-", global_tdb[0] / 1073741824.0);
socket_print(user, "|02| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[1] ? global_tdud[1] : (unsigned char *)"-", global_tdbd[1] / 1073741824.0, global_tdbw[1] ? global_tduw[1] : (unsigned char *)"-", global_tdbw[1] / 1073741824.0, global_tdbm[1] ? global_tdum[1] : (unsigned char *)"-", global_tdbm[1] / 1073741824.0, global_tdb[1] ? global_tdu[1] : (unsigned char *)"-", global_tdb[1] / 1073741824.0);
socket_print(user, "|03| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[2] ? global_tdud[2] : (unsigned char *)"-", global_tdbd[2] / 1073741824.0, global_tdbw[2] ? global_tduw[2] : (unsigned char *)"-", global_tdbw[2] / 1073741824.0, global_tdbm[2] ? global_tdum[2] : (unsigned char *)"-", global_tdbm[2] / 1073741824.0, global_tdb[2] ? global_tdu[2] : (unsigned char *)"-", global_tdb[2] / 1073741824.0);
socket_print(user, "|04| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[3] ? global_tdud[3] : (unsigned char *)"-", global_tdbd[3] / 1073741824.0, global_tdbw[3] ? global_tduw[3] : (unsigned char *)"-", global_tdbw[3] / 1073741824.0, global_tdbm[3] ? global_tdum[3] : (unsigned char *)"-", global_tdbm[3] / 1073741824.0, global_tdb[3] ? global_tdu[3] : (unsigned char *)"-", global_tdb[3] / 1073741824.0);
socket_print(user, "|05| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[4] ? global_tdud[4] : (unsigned char *)"-", global_tdbd[4] / 1073741824.0, global_tdbw[4] ? global_tduw[4] : (unsigned char *)"-", global_tdbw[4] / 1073741824.0, global_tdbm[4] ? global_tdum[4] : (unsigned char *)"-", global_tdbm[4] / 1073741824.0, global_tdb[4] ? global_tdu[4] : (unsigned char *)"-", global_tdb[4] / 1073741824.0);
socket_print(user, "|06| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[5] ? global_tdud[5] : (unsigned char *)"-", global_tdbd[5] / 1073741824.0, global_tdbw[5] ? global_tduw[5] : (unsigned char *)"-", global_tdbw[5] / 1073741824.0, global_tdbm[5] ? global_tdum[5] : (unsigned char *)"-", global_tdbm[5] / 1073741824.0, global_tdb[5] ? global_tdu[5] : (unsigned char *)"-", global_tdb[5] / 1073741824.0);
socket_print(user, "|07| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[6] ? global_tdud[6] : (unsigned char *)"-", global_tdbd[6] / 1073741824.0, global_tdbw[6] ? global_tduw[6] : (unsigned char *)"-", global_tdbw[6] / 1073741824.0, global_tdbm[6] ? global_tdum[6] : (unsigned char *)"-", global_tdbm[6] / 1073741824.0, global_tdb[6] ? global_tdu[6] : (unsigned char *)"-", global_tdb[6] / 1073741824.0);
socket_print(user, "|08| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[7] ? global_tdud[7] : (unsigned char *)"-", global_tdbd[7] / 1073741824.0, global_tdbw[7] ? global_tduw[7] : (unsigned char *)"-", global_tdbw[7] / 1073741824.0, global_tdbm[7] ? global_tdum[7] : (unsigned char *)"-", global_tdbm[7] / 1073741824.0, global_tdb[7] ? global_tdu[7] : (unsigned char *)"-", global_tdb[7] / 1073741824.0);
socket_print(user, "|09| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[8] ? global_tdud[8] : (unsigned char *)"-", global_tdbd[8] / 1073741824.0, global_tdbw[8] ? global_tduw[8] : (unsigned char *)"-", global_tdbw[8] / 1073741824.0, global_tdbm[8] ? global_tdum[8] : (unsigned char *)"-", global_tdbm[8] / 1073741824.0, global_tdb[8] ? global_tdu[8] : (unsigned char *)"-", global_tdb[8] / 1073741824.0);
socket_print(user, "|10| %-8.8s %6.0f  | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |\r\n", global_tdbd[9] ? global_tdud[9] : (unsigned char *)"-", global_tdbd[9] / 1073741824.0, global_tdbw[9] ? global_tduw[9] : (unsigned char *)"-", global_tdbw[9] / 1073741824.0, global_tdbm[9] ? global_tdum[9] : (unsigned char *)"-", global_tdbm[9] / 1073741824.0, global_tdb[9] ? global_tdu[9] : (unsigned char *)"-", global_tdb[9] / 1073741824.0);
socket_print(user, "+--+-----------------+------------------+------------------+-------------------+\r\n");
socket_print(user, " Type SITE TOPDN <D/W/M/A> to get detailed information.\r\n");
}

void local_user_top_down_colour(struct login_node *user, int number, char *opt)
{
socket_print(user, "200- [ TOP DOWNLOADER DAY ]\r\n");
socket_print(user, "[1;37;44m ________________________________________________________________________       [0;40m\r\n");
socket_print(user, "[1;37;44m/_______     _____________    \\__________    \\__     \\_____________   ___\\      [0;40m\r\n");
socket_print(user, "[1;37;44m  \\ /       / \\ /       /       /    __________\\ \\       \\         \\     \\ \\    [0;40m\r\n");
socket_print(user, "[1;37;44m  /       /   /       /       / .        \\ \\       \\       \\ \\       \\     . \\  [0;40m\r\n");
socket_print(user, "[1;37;44m/_______/   _____________   / /            \\ . ____________/ \\_________\\_______\\[0;40m\r\n");
socket_print(user, "[1;37;44m\\       \\   \\           \\ /__________________\\ /         /   /       /         /[0;40m\r\n");
socket_print(user, "[1;37;44m  \\_______\\ / \\___________\\                  /_________/ \\ /_______/_________/  [0;40m\r\n");
socket_print(user, "[1;37;44m                            \\______________/                                    [0;40m\r\n");
socket_print(user, "[1;37;44m[##][ DAY        GB ] [ WEEK        GB ] [ MONTH       GB ] [ ALLTIME      GB ] [0;40m\r\n");
socket_print(user, "[1;37;44m+[1;36;44m--[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
socket_print(user, "[1;36;44m|[1;37;44m01[1;36;44m| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |[0;40m\r\n", global_tdbd[0] ? global_tdud[0] : (unsigned char *)"-", global_tdbd[0] / 1073741824.0, global_tdbw[0] ? global_tduw[0] : (unsigned char *)"-", global_tdbw[0] / 1073741824.0, global_tdbm[0] ? global_tdum[0] : (unsigned char *)"-", global_tdbm[0] / 1073741824.0, global_tdb[0] ? global_tdu[0] : (unsigned char *)"-", global_tdb[0] / 1073741824.0);
socket_print(user, "[0;36;44m|[1;37;44m02[0;36;44m| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |[0;40m\r\n", global_tdbd[1] ? global_tdud[1] : (unsigned char *)"-", global_tdbd[1] / 1073741824.0, global_tdbw[1] ? global_tduw[1] : (unsigned char *)"-", global_tdbw[1] / 1073741824.0, global_tdbm[1] ? global_tdum[1] : (unsigned char *)"-", global_tdbm[1] / 1073741824.0, global_tdb[1] ? global_tdu[1] : (unsigned char *)"-", global_tdb[1] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m03[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.2f [1;34;44m|[0;40m\r\n", global_tdbd[2] ? global_tdud[2] : (unsigned char *)"-", global_tdbd[2] / 1073741824.0, global_tdbw[2] ? global_tduw[2] : (unsigned char *)"-", global_tdbw[2] / 1073741824.0, global_tdbm[2] ? global_tdum[2] : (unsigned char *)"-", global_tdbm[2] / 1073741824.0, global_tdb[2] ? global_tdu[2] : (unsigned char *)"-", global_tdb[2] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m04[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.2f [1;34;44m|[0;40m\r\n", global_tdbd[3] ? global_tdud[3] : (unsigned char *)"-", global_tdbd[3] / 1073741824.0, global_tdbw[3] ? global_tduw[3] : (unsigned char *)"-", global_tdbw[3] / 1073741824.0, global_tdbm[3] ? global_tdum[3] : (unsigned char *)"-", global_tdbm[3] / 1073741824.0, global_tdb[3] ? global_tdu[3] : (unsigned char *)"-", global_tdb[3] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m05[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.2f [1;34;44m|[0;40m\r\n", global_tdbd[4] ? global_tdud[4] : (unsigned char *)"-", global_tdbd[4] / 1073741824.0, global_tdbw[4] ? global_tduw[4] : (unsigned char *)"-", global_tdbw[4] / 1073741824.0, global_tdbm[4] ? global_tdum[4] : (unsigned char *)"-", global_tdbm[4] / 1073741824.0, global_tdb[4] ? global_tdu[4] : (unsigned char *)"-", global_tdb[4] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m06[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.2f [1;34;44m|[0;40m\r\n", global_tdbd[5] ? global_tdud[5] : (unsigned char *)"-", global_tdbd[5] / 1073741824.0, global_tdbw[5] ? global_tduw[5] : (unsigned char *)"-", global_tdbw[5] / 1073741824.0, global_tdbm[5] ? global_tdum[5] : (unsigned char *)"-", global_tdbm[5] / 1073741824.0, global_tdb[5] ? global_tdu[5] : (unsigned char *)"-", global_tdb[5] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m07[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.2f [1;34;44m|[0;40m\r\n", global_tdbd[6] ? global_tdud[6] : (unsigned char *)"-", global_tdbd[6] / 1073741824.0, global_tdbw[6] ? global_tduw[6] : (unsigned char *)"-", global_tdbw[6] / 1073741824.0, global_tdbm[6] ? global_tdum[6] : (unsigned char *)"-", global_tdbm[6] / 1073741824.0, global_tdb[6] ? global_tdu[6] : (unsigned char *)"-", global_tdb[6] / 1073741824.0);
socket_print(user, "[1;34;44m|[1;37;44m08[1;34;44m| [0;36;44m%-8.8s %6.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %7.2f [1;34;44m|[0;36;44m %-8.8s %8.2f [1;34;44m|[0;40m\r\n", global_tdbd[7] ? global_tdud[7] : (unsigned char *)"-", global_tdbd[7] / 1073741824.0, global_tdbw[7] ? global_tduw[7] : (unsigned char *)"-", global_tdbw[7] / 1073741824.0, global_tdbm[7] ? global_tdum[7] : (unsigned char *)"-", global_tdbm[7] / 1073741824.0, global_tdb[7] ? global_tdu[7] : (unsigned char *)"-", global_tdb[7] / 1073741824.0);
socket_print(user, "[0;36;44m|[1;37;44m09[0;36;44m| %-8.8s %6.2f | %-8.8s %7.2f | %-8.8s %7.2f | %-8.8s %8.2f |[0;40m\r\n", global_tdbd[8] ? global_tdud[8] : (unsigned char *)"-", global_tdbd[8] / 1073741824.0, global_tdbw[8] ? global_tduw[8] : (unsigned char *)"-", global_tdbw[8] / 1073741824.0, global_tdbm[8] ? global_tdum[8] : (unsigned char *)"-", global_tdbm[8] / 1073741824.0, global_tdb[8] ? global_tdu[8] : (unsigned char *)"-", global_tdb[8] / 1073741824.0);
socket_print(user, "[1;36;44m|[1;37;44m10[1;36;44m|[0;36;44m %-8.8s %6.2f [1;36;44m|[0;36;44m %-8.8s %7.2f [1;36;44m|[0;36;44m %-8.8s %7.2f [1;36;44m|[0;36;44m %-8.8s %8.2f [1;36;44m|[0;40m\r\n", global_tdbd[9] ? global_tdud[9] : (unsigned char *)"-", global_tdbd[9] / 1073741824.0, global_tdbw[9] ? global_tduw[9] : (unsigned char *)"-", global_tdbw[9] / 1073741824.0, global_tdbm[9] ? global_tdum[9] : (unsigned char *)"-", global_tdbm[9] / 1073741824.0, global_tdb[9] ? global_tdu[9] : (unsigned char *)"-", global_tdb[9] / 1073741824.0);
socket_print(user, "[1;37;44m+[1;36;44m--[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
socket_print(user, "[0;36;44m Type [1;36;44mSITE TOPUP <D/W/M/A>[0;36;44m to get detailed information.                         [0;40m\r\n");
}




void local_user_top_down_day_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP DOWNLOADER DAY ]\r\n");
                          socket_print(user, "         ____________________________________  ______ ________________\r\n");
                          socket_print(user, "        /___   ______  \\_____ \\_   \\______  _\\/\\____  \\_\\ _   \\   \\   \\\r\n");
                          socket_print(user, "        \\/   /\\/   /   /  _____\\\\   \\    \\  \\\\ \\\\  \\   \\   _   \\____  .\\\r\n");
                          socket_print(user, "        /   / /   /   /.    \\\\   \\   \\\\   \\  .\\ \\.  \\   \\   \\   \\   \\  \\\\\r\n");
                          socket_print(user, "       /___/ /______ //      \\.______/\\____\\___\\ \\______/\\___\\___\\_______\\\r\n");
                          socket_print(user, "       \\   \\ \\     \\/_________\\/    / /   /    / /     / /   /   /       /\r\n");
                          socket_print(user, "        \\___\\/\\_____\\         /____/\\/___/____/\\/_____/\\/___/___/_______/\r\n");
                          socket_print(user, "+----+----------+----\\_______/-------+-------+---------------------------------+\r\n");
                          socket_print(user, "| ## | USER     | GROUP    | GBYTES  |AVG K/S| TAGLINE                         |\r\n");
                          socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
      if (global_tdbd[0]) socket_print(user, "| 01 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[0], global_tdgw[0], global_tdbd[0] / 1073741824.0, global_tdbd[0] / global_tdsd[0] / 1024.0, global_tdtd[0]);
      if (global_tdbd[1]) socket_print(user, "| 02 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[1], global_tdgw[1], global_tdbd[1] / 1073741824.0, global_tdbd[1] / global_tdsd[1] / 1024.0, global_tdtd[1]);
      if (global_tdbd[2]) socket_print(user, "| 03 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[2], global_tdgw[2], global_tdbd[2] / 1073741824.0, global_tdbd[2] / global_tdsd[2] / 1024.0, global_tdtd[2]);
      if (global_tdbd[3]) socket_print(user, "| 04 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[3], global_tdgw[3], global_tdbd[3] / 1073741824.0, global_tdbd[3] / global_tdsd[3] / 1024.0, global_tdtd[3]);
      if (global_tdbd[4]) socket_print(user, "| 05 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[4], global_tdgw[4], global_tdbd[4] / 1073741824.0, global_tdbd[4] / global_tdsd[4] / 1024.0, global_tdtd[4]);
      if (global_tdbd[5]) socket_print(user, "| 06 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[5], global_tdgw[5], global_tdbd[5] / 1073741824.0, global_tdbd[5] / global_tdsd[5] / 1024.0, global_tdtd[5]);
      if (global_tdbd[6]) socket_print(user, "| 07 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[6], global_tdgw[6], global_tdbd[6] / 1073741824.0, global_tdbd[6] / global_tdsd[6] / 1024.0, global_tdtd[6]);
      if (global_tdbd[7]) socket_print(user, "| 08 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[7], global_tdgw[7], global_tdbd[7] / 1073741824.0, global_tdbd[7] / global_tdsd[7] / 1024.0, global_tdtd[7]);
      if (global_tdbd[8]) socket_print(user, "| 09 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[8], global_tdgw[8], global_tdbd[8] / 1073741824.0, global_tdbd[8] / global_tdsd[8] / 1024.0, global_tdtd[8]);
      if (global_tdbd[9]) socket_print(user, "| 10 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdud[9], global_tdgw[9], global_tdbd[9] / 1073741824.0, global_tdbd[9] / global_tdsd[9] / 1024.0, global_tdtd[9]);
                          socket_print(user, "+----+----------+----------+---------+-------+--------------------[Hour %2.2d/24]-+\r\n", global_hourofday);
        if (global_tdlbd) socket_print(user, "| YESTERDAYS #1:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tdlud, global_tdlbd / 1073741824.0, global_tdlfd, global_tdlbd / global_tdlsd / 1024.0);
        if (global_tdmbd) socket_print(user, ": DAYTOP RECORD:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tdmud, global_tdmbd / 1073741824.0, global_tdmfd, global_tdmbd / global_tdmsd / 1024.0);
        if (global_dbd)   socket_print(user, ".    DAY TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_dbd / 1073741824.0, global_dfd);
}

void local_user_top_down_day_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP DOWNLOADER DAY ]\r\n");
                          socket_print(user, "[1;37;44m         ____________________________________  ______ ________________          [0;40m\r\n");
                          socket_print(user, "[1;37;44m        /___   ______  \\_____ \\_   \\______  _\\/\\____  \\_\\ _   \\   \\   \\         [0;40m\r\n");
                          socket_print(user, "[1;37;44m        \\/   /\\/   /   /  _____\\\\   \\    \\  \\\\ \\\\  \\   \\   _   \\____  .\\        [0;40m\r\n");
                          socket_print(user, "[1;37;44m        /   / /   /   /.    \\\\   \\   \\\\   \\  .\\ \\.  \\   \\   \\   \\   \\  \\\\       [0;40m\r\n");
                          socket_print(user, "[1;37;44m       /___/ /______ //      \\.______/\\____\\___\\ \\______/\\___\\___\\_______\\      [0;40m\r\n");
                          socket_print(user, "[1;37;44m       \\   \\ \\     \\/_________\\/    / /   /    / /     / /   /   /       /      [0;40m\r\n");
                          socket_print(user, "[1;37;44m        \\___\\/\\_____\\         /____/\\/___/____/\\/_____/\\/___/___/_______/       [0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--[1;37;44m\\_______/[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                          socket_print(user, "[1;36;44m| [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mGBYTES  [1;36;44m|[1;37;44mAVG K/S[1;36;44m| [1;37;44mTAGLINE                         [1;37;44m|[0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
      if (global_tdbd[0]) socket_print(user, "[1;36;44m| [1;37;44m01 [1;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tdud[0], global_tdgw[0], global_tdbd[0] / 1073741824.0, global_tdbd[0] / global_tdsd[0] / 1024.0, global_tdtd[0]);
      if (global_tdbd[1]) socket_print(user, "[0;36;44m| [1;37;44m02 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tdud[1], global_tdgw[1], global_tdbd[1] / 1073741824.0, global_tdbd[1] / global_tdsd[1] / 1024.0, global_tdtd[1]);
      if (global_tdbd[2]) socket_print(user, "[1;34;44m| [1;37;44m03 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdud[2], global_tdgw[2], global_tdbd[2] / 1073741824.0, global_tdbd[2] / global_tdsd[2] / 1024.0, global_tdtd[2]);
      if (global_tdbd[3]) socket_print(user, "[1;34;44m| [1;37;44m04 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdud[3], global_tdgw[3], global_tdbd[3] / 1073741824.0, global_tdbd[3] / global_tdsd[3] / 1024.0, global_tdtd[3]);
      if (global_tdbd[4]) socket_print(user, "[1;34;44m| [1;37;44m05 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdud[4], global_tdgw[4], global_tdbd[4] / 1073741824.0, global_tdbd[4] / global_tdsd[4] / 1024.0, global_tdtd[4]);
      if (global_tdbd[5]) socket_print(user, "[1;34;44m| [1;37;44m06 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdud[5], global_tdgw[5], global_tdbd[5] / 1073741824.0, global_tdbd[5] / global_tdsd[5] / 1024.0, global_tdtd[5]);
      if (global_tdbd[6]) socket_print(user, "[1;34;44m| [1;37;44m07 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdud[6], global_tdgw[6], global_tdbd[6] / 1073741824.0, global_tdbd[6] / global_tdsd[6] / 1024.0, global_tdtd[6]);
      if (global_tdbd[7]) socket_print(user, "[1;34;44m| [1;37;44m08 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdud[7], global_tdgw[7], global_tdbd[7] / 1073741824.0, global_tdbd[7] / global_tdsd[7] / 1024.0, global_tdtd[7]);
      if (global_tdbd[8]) socket_print(user, "[0;36;44m| [1;37;44m09 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tdud[8], global_tdgw[8], global_tdbd[8] / 1073741824.0, global_tdbd[8] / global_tdsd[8] / 1024.0, global_tdtd[8]);
      if (global_tdbd[9]) socket_print(user, "[1;36;44m| [1;37;44m10 [1;36;44m| [0;36;44m%-8.8s [1;36;44m| [0;36;44m%-8.8s [1;36;44m|[0;36;44m%8.2f [1;36;44m|[0;36;44m%6.0f [1;36;44m| [0;36;44m%-31.31s [1;36;44m|[0;40m\r\n", global_tdud[9], global_tdgw[9], global_tdbd[9] / 1073741824.0, global_tdbd[9] / global_tdsd[9] / 1024.0, global_tdtd[9]);
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------------------[[0;36;44mHour [1;36;44m%2.2d[0;36;44m/24[1;34;44m][1;36;44m-[1;37;44m+[0;40m\r\n", global_hourofday);
        if (global_tdlbd) socket_print(user, "[1;36;44m|[0;36;44m YESTERDAYS #1:  [1;36;44m%s [0;36;44m%with [1;36;44m%%.2f Gbytes [0;36;44m%in [1;36;44m%%.0f Files [0;36;44m%at [1;36;44m%%.2f K/s[0;40m\r\n", global_tdlud, global_tdlbd / 1073741824.0, global_tdlfd, global_tdlbd / global_tdlsd / 1024.0);
        if (global_tdmbd) socket_print(user, "[0;36;44m:[0;36;44m DAYTOP RECORD:  [1;36;44m%s [0;36;44m%with [1;36;44m%%.2f Gbytes [0;36;44m%in [1;36;44m%%.0f Files [0;36;44m%at [1;36;44m%%.2f K/s[0;40m\r\n", global_tdmud, global_tdmbd / 1073741824.0, global_tdmfd, global_tdmbd / global_tdmsd / 1024.0);
        if (global_dbd)   socket_print(user, "[1;34;44m.[0;36;44m    DAY TOTALS:  [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files[0;40m\r\n", global_dbd / 1073741824.0, global_dfd);
}



void local_user_top_down_week_mono(struct login_node *user, int number, char *opt)
{

                          socket_print(user, "200- [ TOP DOWNLOADER WEEK ]\r\n");
                          socket_print(user, "   ____________________________________  ____ ____________________________\r\n");
                          socket_print(user, "  /___   ______  \\_____  \\_  \\______  _\\/\\   \\\\   \\    ___\\    ___\\   \\   \\\r\n");
                          socket_print(user, "   \\/   /\\/   /   /  _____\\\\   \\    \\  \\\\ \\   \\\\   \\\\   ___\\\\   ___\\     -'_\r\n");
                          socket_print(user, "   /   / /   /   /.    \\\\   \\   \\\\   \\  .\\ \\   \\\\   \\.  \\   \\.  \\   \\   \\   \\\r\n");
                          socket_print(user, "  /___/ /______ //      \\.______/\\____\\___\\ \\___/\\___\\_______\\_______\\___\\___\\\r\n");
                          socket_print(user, "  \\   \\ \\     \\/_________\\/    / /   /    / /   //   /       /       /   /   /\r\n");
                          socket_print(user, "   \\___\\/\\_____\\         /____/\\/___/____/\\/___/\\___/_______/_______/___/___/\r\n");
                          socket_print(user, "+----+----------\\_______/--+---------+-------+---------------------------------+\r\n");
                          socket_print(user, "| ## | USER     | GROUP    | GBYTES  |AVG K/S| TAGLINE                         |\r\n");
                          socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
      if (global_tdbw[0]) socket_print(user, "| 01 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[0], global_tdgw[0], global_tdbw[0] / 1073741824.0, global_tdbw[0] / global_tdsw[0] / 1024.0, global_tdtw[0]);
      if (global_tdbw[1]) socket_print(user, "| 02 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[1], global_tdgw[1], global_tdbw[1] / 1073741824.0, global_tdbw[1] / global_tdsw[1] / 1024.0, global_tdtw[1]);
      if (global_tdbw[2]) socket_print(user, "| 03 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[2], global_tdgw[2], global_tdbw[2] / 1073741824.0, global_tdbw[2] / global_tdsw[2] / 1024.0, global_tdtw[2]);
      if (global_tdbw[3]) socket_print(user, "| 04 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[3], global_tdgw[3], global_tdbw[3] / 1073741824.0, global_tdbw[3] / global_tdsw[3] / 1024.0, global_tdtw[3]);
      if (global_tdbw[4]) socket_print(user, "| 05 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[4], global_tdgw[4], global_tdbw[4] / 1073741824.0, global_tdbw[4] / global_tdsw[4] / 1024.0, global_tdtw[4]);
      if (global_tdbw[5]) socket_print(user, "| 06 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[5], global_tdgw[5], global_tdbw[5] / 1073741824.0, global_tdbw[5] / global_tdsw[5] / 1024.0, global_tdtw[5]);
      if (global_tdbw[6]) socket_print(user, "| 07 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[6], global_tdgw[6], global_tdbw[6] / 1073741824.0, global_tdbw[6] / global_tdsw[6] / 1024.0, global_tdtw[6]);
      if (global_tdbw[7]) socket_print(user, "| 08 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[7], global_tdgw[7], global_tdbw[7] / 1073741824.0, global_tdbw[7] / global_tdsw[7] / 1024.0, global_tdtw[7]);
      if (global_tdbw[8]) socket_print(user, "| 09 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[8], global_tdgw[8], global_tdbw[8] / 1073741824.0, global_tdbw[8] / global_tdsw[8] / 1024.0, global_tdtw[8]);
      if (global_tdbw[9]) socket_print(user, "| 10 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tduw[9], global_tdgw[9], global_tdbw[9] / 1073741824.0, global_tdbw[9] / global_tdsw[9] / 1024.0, global_tdtw[9]);
                          socket_print(user, "+----+----------+----------+---------+-------+-----------------------[Day %d/7]-+\r\n", global_dayofweek);
        if (global_tdlbw) socket_print(user, "| YESTERDAYS #1:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tdluw, global_tdlbw / 1073741824.0, global_tdlfw, global_tdlbw / global_tdlsw / 1024.0);
        if (global_tdmbw) socket_print(user, ": DAYTOP RECORD:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tdmuw, global_tdmbw / 1073741824.0 ,global_tdmfw,global_tdmbw /global_tdmsw / 1024.0);
        if (global_dbw)   socket_print(user, ".    DAY TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_dbw / 1073741824.0, global_dfw);
}

void local_user_top_down_week_colour(struct login_node *user, int number, char *opt)
{

                          socket_print(user, "200- [ TOP DOWNLOADER WEEK ]\r\n");
                          socket_print(user, "[1;37;44m   ____________________________________  ____ ____________________________      [0;40m\r\n");
                          socket_print(user, "[1;37;44m  /___   ______  \\_____  \\_  \\______  _\\/\\   \\\\   \\    ___\\    ___\\   \\   \\     [0;40m\r\n");
                          socket_print(user, "[1;37;44m   \\/   /\\/   /   /  _____\\\\   \\    \\  \\\\ \\   \\\\   \\\\   ___\\\\   ___\\     -'_    [0;40m\r\n");
                          socket_print(user, "[1;37;44m   /   / /   /   /.    \\\\   \\   \\\\   \\  .\\ \\   \\\\   \\.  \\   \\.  \\   \\   \\   \\   [0;40m\r\n");
                          socket_print(user, "[1;37;44m  /___/ /______ //      \\.______/\\____\\___\\ \\___/\\___\\_______\\_______\\___\\___\\  [0;40m\r\n");
                          socket_print(user, "[1;37;44m  \\   \\ \\     \\/_________\\/    / /   /    / /   //   /       /       /   /   /  [0;40m\r\n");
                          socket_print(user, "[1;37;44m   \\___\\/\\_____\\         /____/\\/___/____/\\/___/\\___/_______/_______/___/___/   [0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------[1;37;44m\\_______/[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                          socket_print(user, "[1;36;44m| [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mGBYTES  [1;36;44m|[1;37;44mAVG K/S[1;36;44m| [1;37;44mTAGLINE                         [1;37;44m|[0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
      if (global_tdbw[0]) socket_print(user, "[1;36;44m| [1;37;44m01 [1;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tduw[0], global_tdgw[0], global_tdbw[0] / 1073741824.0, global_tdbw[0] / global_tdsw[0] / 1024.0, global_tdtw[0]);
      if (global_tdbw[1]) socket_print(user, "[0;36;44m| [1;37;44m02 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tduw[1], global_tdgw[1], global_tdbw[1] / 1073741824.0, global_tdbw[1] / global_tdsw[1] / 1024.0, global_tdtw[1]);
      if (global_tdbw[2]) socket_print(user, "[1;34;44m| [1;37;44m03 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tduw[2], global_tdgw[2], global_tdbw[2] / 1073741824.0, global_tdbw[2] / global_tdsw[2] / 1024.0, global_tdtw[2]);
      if (global_tdbw[3]) socket_print(user, "[1;34;44m| [1;37;44m04 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tduw[3], global_tdgw[3], global_tdbw[3] / 1073741824.0, global_tdbw[3] / global_tdsw[3] / 1024.0, global_tdtw[3]);
      if (global_tdbw[4]) socket_print(user, "[1;34;44m| [1;37;44m05 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tduw[4], global_tdgw[4], global_tdbw[4] / 1073741824.0, global_tdbw[4] / global_tdsw[4] / 1024.0, global_tdtw[4]);
      if (global_tdbw[5]) socket_print(user, "[1;34;44m| [1;37;44m06 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tduw[5], global_tdgw[5], global_tdbw[5] / 1073741824.0, global_tdbw[5] / global_tdsw[5] / 1024.0, global_tdtw[5]);
      if (global_tdbw[6]) socket_print(user, "[1;34;44m| [1;37;44m07 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tduw[6], global_tdgw[6], global_tdbw[6] / 1073741824.0, global_tdbw[6] / global_tdsw[6] / 1024.0, global_tdtw[6]);
      if (global_tdbw[7]) socket_print(user, "[1;34;44m| [1;37;44m08 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tduw[7], global_tdgw[7], global_tdbw[7] / 1073741824.0, global_tdbw[7] / global_tdsw[7] / 1024.0, global_tdtw[7]);
      if (global_tdbw[8]) socket_print(user, "[0;36;44m| [1;37;44m09 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tduw[8], global_tdgw[8], global_tdbw[8] / 1073741824.0, global_tdbw[8] / global_tdsw[8] / 1024.0, global_tdtw[8]);
      if (global_tdbw[9]) socket_print(user, "[1;36;44m| [1;37;44m10 [1;36;44m| [0;36;44m%-8.8s [1;36;44m| [0;36;44m%-8.8s [1;36;44m|[0;36;44m%8.2f [1;36;44m|[0;36;44m%6.0f [1;36;44m| [0;36;44m%-31.31s [1;36;44m|[0;40m\r\n", global_tduw[9], global_tdgw[9], global_tdbw[9] / 1073741824.0, global_tdbw[9] / global_tdsw[9] / 1024.0, global_tdtw[9]);
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---------------------[[0;36;44mDay [1;36;44m%d[0;36;44m/7[1;34;44m][1;36;44m-[1;37;44m+[0;40m\r\n", global_dayofweek);
        if (global_tdlbw) socket_print(user, "[1;36;44m|[0;36;44m  LAST WEEKS #1:  [1;36;44m%s [0;36;44mwith [1;36;44m%.2f Gbytes [1;37;44min %.0f Files [0;36;44mat [1;36;44m%.2f [0;36;44mK/s\r\n", global_tdluw, global_tdlbw / 1073741824.0, global_tdlfw, global_tdlbw / global_tdlsw / 1024.0);
        if (global_tdmbw) socket_print(user, "[0;36;44m:[0;36;44m WEEKTOP RECORD:  [1;36;44m%s [0;36;44mwith [1;36;44m%.2f Gbytes [1;37;44min %.0f Files [0;36;44mat [1;36;44m%.2f [0;36;44mK/s\r\n", global_tdmuw, global_tdmbw / 1073741824.0 ,global_tdmfw,global_tdmbw /global_tdmsw / 1024.0);
        if (global_dbw)   socket_print(user, "[1;34;44m.[0;36;44m    WEEK TOTALS:  [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files\r\n", global_dbw / 1073741824.0, global_dfw);
}


void local_user_top_down_month_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP DOWNLOADER MONTH ]\r\n");
                          socket_print(user, "    ______________________________  _____________________________________\r\n");
                          socket_print(user, "   /___  ______ \\____  \\_ \\_____ _\\/\\_______ \\____  \\____ _\\  ____\\   \\  \\\r\n");
                          socket_print(user, "    \\/  /\\/   /  / _____\\\\  \\   \\ \\\\/ \\  \\  \\ \\   \\  \\   \\ \\\\  \\  /\\   _  \\\r\n");
                          socket_print(user, "    /  / /   /  /    \\\\   \\  \\   \\ .\\  \\  \\  \\ \\   \\  \\   \\ .\\  \\/  \\   \\  \\\r\n");
                          socket_print(user, "   /__/ /_____ /      \\._____/\\___\\__\\  \\__\\__\\_\\______\\___\\__\\__\\   \\___\\__\\\r\n");
                          socket_print(user, "   \\  \\ \\    \\/________\\/   / /   /  /  /  /  / /      /   /  /  /\\  /   /  /\r\n");
                          socket_print(user, "    \\__\\/\\____\\        /___/\\/___/__/ \\/__/__/_/______/___/__/__/  \\/___/__/\r\n");
                          socket_print(user, "+----+---------\\______/----+---------+-------+---------------------------------+\r\n");
                          socket_print(user, "| ## | USER     | GROUP    | GBYTES  |AVG K/S| TAGLINE                         |\r\n");
                          socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
      if (global_tdbm[0]) socket_print(user, "| 01 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[0], global_tdgm[0], global_tdbm[0] / 1073741824.0, global_tdbm[0] / global_tdsm[0] / 1024.0, global_tdtm[0]);
      if (global_tdbm[1]) socket_print(user, "| 02 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[1], global_tdgm[1], global_tdbm[1] / 1073741824.0, global_tdbm[1] / global_tdsm[1] / 1024.0, global_tdtm[1]);
      if (global_tdbm[2]) socket_print(user, "| 03 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[2], global_tdgm[2], global_tdbm[2] / 1073741824.0, global_tdbm[2] / global_tdsm[2] / 1024.0, global_tdtm[2]);
      if (global_tdbm[3]) socket_print(user, "| 04 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[3], global_tdgm[3], global_tdbm[3] / 1073741824.0, global_tdbm[3] / global_tdsm[3] / 1024.0, global_tdtm[3]);
      if (global_tdbm[4]) socket_print(user, "| 05 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[4], global_tdgm[4], global_tdbm[4] / 1073741824.0, global_tdbm[4] / global_tdsm[4] / 1024.0, global_tdtm[4]);
      if (global_tdbm[5]) socket_print(user, "| 06 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[5], global_tdgm[5], global_tdbm[5] / 1073741824.0, global_tdbm[5] / global_tdsm[5] / 1024.0, global_tdtm[5]);
      if (global_tdbm[6]) socket_print(user, "| 07 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[6], global_tdgm[6], global_tdbm[6] / 1073741824.0, global_tdbm[6] / global_tdsm[6] / 1024.0, global_tdtm[6]);
      if (global_tdbm[7]) socket_print(user, "| 08 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[7], global_tdgm[7], global_tdbm[7] / 1073741824.0, global_tdbm[7] / global_tdsm[7] / 1024.0, global_tdtm[7]);
      if (global_tdbm[8]) socket_print(user, "| 09 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[8], global_tdgm[8], global_tdbm[8] / 1073741824.0, global_tdbm[8] / global_tdsm[8] / 1024.0, global_tdtm[8]);
      if (global_tdbm[9]) socket_print(user, "| 10 | %-8.8s | %-8.8s | %7.2f |%6.0f | %-31.31s |\r\n", global_tdum[9], global_tdgm[9], global_tdbm[9] / 1073741824.0, global_tdbm[9] / global_tdsm[9] / 1024.0, global_tdtm[9]);
                          socket_print(user, "+----+----------+----------+---------+-------+---------------------[Day %2.2d/%2.2d]-+\r\n", global_dayofmonth, global_daysinmonth);
        if (global_tdlbm) socket_print(user, "|  LAST MONTHS #1:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tdlum, global_tdlbm / 1073741824.0 , global_tdlfm, global_tdlbm / global_tdlsm / 1024.0);
        if (global_tdmbm) socket_print(user, ": MONTHTOP RECORD:  %s with %.2f Gbytes in %.0f Files at %.2f K/s\r\n", global_tdmum, global_tdmbm / 1073741824.0 ,global_tdmfm,global_tdmbm /global_tdmsm / 1024.0);
        if (global_dbm)   socket_print(user, ".    MONTH TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_dbm / 1073741824.0, global_dfm);
}

void local_user_top_down_month_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- [ TOP DOWNLOADER MONTH ]\r\n");
                          socket_print(user, "[1;37;44m    ______________________________  _____________________________________       [0;40m\r\n");
                          socket_print(user, "[1;37;44m   /___  ______ \\____  \\_ \\_____ _\\/\\_______ \\____  \\____ _\\  ____\\   \\  \\      [0;40m\r\n");
                          socket_print(user, "[1;37;44m    \\/  /\\/   /  / _____\\\\  \\   \\ \\\\/ \\  \\  \\ \\   \\  \\   \\ \\\\  \\  /\\   _  \\     [0;40m\r\n");
                          socket_print(user, "[1;37;44m    /  / /   /  /    \\\\   \\  \\   \\ .\\  \\  \\  \\ \\   \\  \\   \\ .\\  \\/  \\   \\  \\    [0;40m\r\n");
                          socket_print(user, "[1;37;44m   /__/ /_____ /      \\._____/\\___\\__\\  \\__\\__\\_\\______\\___\\__\\__\\   \\___\\__\\   [0;40m\r\n");
                          socket_print(user, "[1;37;44m   \\  \\ \\    \\/________\\/   / /   /  /  /  /  / /      /   /  /  /\\  /   /  /   [0;40m\r\n");
                          socket_print(user, "[1;37;44m    \\__\\/\\____\\        /___/\\/___/__/ \\/__/__/_/______/___/__/__/  \\/___/__/    [0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------[1;37;44m\\______/[1;34;44m--[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                          socket_print(user, "[1;36;44m| [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mGBYTES  [1;36;44m|[1;37;44mAVG K/S[1;36;44m| [1;37;44mTAGLINE                         [1;37;44m|[0;40m\r\n");
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
      if (global_tdbm[0]) socket_print(user, "[1;36;44m| [1;37;44m01 [1;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tdum[0], global_tdgm[0], global_tdbm[0] / 1073741824.0, global_tdbm[0] / global_tdsm[0] / 1024.0, global_tdtm[0]);
      if (global_tdbm[1]) socket_print(user, "[0;36;44m| [1;37;44m02 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tdum[1], global_tdgm[1], global_tdbm[1] / 1073741824.0, global_tdbm[1] / global_tdsm[1] / 1024.0, global_tdtm[1]);
      if (global_tdbm[2]) socket_print(user, "[1;34;44m| [1;37;44m03 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdum[2], global_tdgm[2], global_tdbm[2] / 1073741824.0, global_tdbm[2] / global_tdsm[2] / 1024.0, global_tdtm[2]);
      if (global_tdbm[3]) socket_print(user, "[1;34;44m| [1;37;44m04 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdum[3], global_tdgm[3], global_tdbm[3] / 1073741824.0, global_tdbm[3] / global_tdsm[3] / 1024.0, global_tdtm[3]);
      if (global_tdbm[4]) socket_print(user, "[1;34;44m| [1;37;44m05 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n""", global_tdum[4], global_tdgm[4], global_tdbm[4] / 1073741824.0, global_tdbm[4] / global_tdsm[4] / 1024.0, global_tdtm[4]);
      if (global_tdbm[5]) socket_print(user, "[1;34;44m| [1;37;44m06 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdum[5], global_tdgm[5], global_tdbm[5] / 1073741824.0, global_tdbm[5] / global_tdsm[5] / 1024.0, global_tdtm[5]);
      if (global_tdbm[6]) socket_print(user, "[1;34;44m| [1;37;44m07 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdum[6], global_tdgm[6], global_tdbm[6] / 1073741824.0, global_tdbm[6] / global_tdsm[6] / 1024.0, global_tdtm[6]);
      if (global_tdbm[7]) socket_print(user, "[1;34;44m| [1;37;44m08 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdum[7], global_tdgm[7], global_tdbm[7] / 1073741824.0, global_tdbm[7] / global_tdsm[7] / 1024.0, global_tdtm[7]);
      if (global_tdbm[8]) socket_print(user, "[0;36;44m| [1;37;44m09 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tdum[8], global_tdgm[8], global_tdbm[8] / 1073741824.0, global_tdbm[8] / global_tdsm[8] / 1024.0, global_tdtm[8]);
      if (global_tdbm[9]) socket_print(user, "[1;36;44m| [1;37;44m10 [1;36;44m| [0;36;44m%-8.8s [1;36;44m| [0;36;44m%-8.8s [1;36;44m|[0;36;44m%8.2f [1;36;44m|[0;36;44m%6.0f [1;36;44m| [0;36;44m%-31.31s [1;36;44m|[0;40m\r\n", global_tdum[9], global_tdgm[9], global_tdbm[9] / 1073741824.0, global_tdbm[9] / global_tdsm[9] / 1024.0, global_tdtm[9]);
                          socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------------[[0;36;44mDay [1;36;44m%2.2d[0;36;44m/%2.2d[1;34;44m][1;36;44m-[1;37;44m+[0;40m\r\n", global_dayofmonth, global_daysinmonth);
        if (global_tdlbm) socket_print(user, "[1;36;44m|[0;36;44m  LAST MONTHS #1:  [1;36;44m%s [0;36;44mwith [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files [0;36;44mat [1;36;44m%.2f K/s[0;40m\r\n", global_tdlum, global_tdlbm / 1073741824.0 , global_tdlfm, global_tdlbm / global_tdlsm / 1024.0);
        if (global_tdmbm) socket_print(user, "[0;36;44m:[0;36;44m MONTHTOP RECORD:  [1;36;44m%s [0;36;44mwith [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files [0;36;44mat [1;36;44m%.2f K/s[0;40m\r\n", global_tdmum, global_tdmbm / 1073741824.0 ,global_tdmfm,global_tdmbm /global_tdmsm / 1024.0);
        if (global_dbm)   socket_print(user, "[1;34;44m.[0;36;44m    MONTH TOTALS:  [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files[0;40m\r\n", global_dbm / 1073741824.0, global_dfm);
}



void local_user_top_down_alltime_mono(struct login_node *user, int number, char *opt)
{
                         socket_print(user, "200- [ TOP DOWNLOADER ALLTIME ]\r\n");
                         socket_print(user, "     ______________________________    ___________   ____ ________________\r\n");
                         socket_print(user, "    /___  ______ \\____  \\_ \\_____ _\\  /_\\  _  \\   \\ /\\   \\\\__   __\\______ \\\r\n");
                         socket_print(user, "     \\/  /\\/   /  / _____\\\\  \\   \\ \\\\// \\   _  \\   \\__\\   \\_/\\  \\ /\\  \\  \\ \\\r\n");
                         socket_print(user, "     /  / /   /  /    \\\\   \\  \\   \\ .\\   \\   \\  \\      \\      \\  \\  \\  \\  \\ \\\r\n");
                         socket_print(user, "    /__/ /_____ /      \\._____/\\___\\__\\   \\___\\__\\._____\\._____\\__\\  \\__\\__\\_\\\r\n");
                         socket_print(user, "    \\  \\ \\    \\/________\\/   / /   /  /\\  /   /  //     //     /  /  /  /  / /\r\n");
                         socket_print(user, "     \\__\\/\\____\\        /___/\\/___/__/  \\/___/__//_____//_____/__/ \\/__/__/_/\r\n");
                         socket_print(user, "+----+----------\\______/---+---------+-------+---------------------------------+\r\n");
                         socket_print(user, "| ## | USER     | GROUP    | GBYTES  |AVG K/S| TAGLINE                         |\r\n");
                         socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
      if (global_tdb[0]) socket_print(user, "| 01 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[0], global_tdg[0], global_tdb[0] / 1073741824.0, global_tdb[0] / global_tds[0] / 1024.0, global_tdt[0]);
      if (global_tdb[1]) socket_print(user, "| 02 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[1], global_tdg[1], global_tdb[1] / 1073741824.0, global_tdb[1] / global_tds[1] / 1024.0, global_tdt[1]);
      if (global_tdb[2]) socket_print(user, "| 03 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[2], global_tdg[2], global_tdb[2] / 1073741824.0, global_tdb[2] / global_tds[2] / 1024.0, global_tdt[2]);
      if (global_tdb[3]) socket_print(user, "| 04 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[3], global_tdg[3], global_tdb[3] / 1073741824.0, global_tdb[3] / global_tds[3] / 1024.0, global_tdt[3]);
      if (global_tdb[4]) socket_print(user, "| 05 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[4], global_tdg[4], global_tdb[4] / 1073741824.0, global_tdb[4] / global_tds[4] / 1024.0, global_tdt[4]);
      if (global_tdb[5]) socket_print(user, "| 06 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[5], global_tdg[5], global_tdb[5] / 1073741824.0, global_tdb[5] / global_tds[5] / 1024.0, global_tdt[5]);
      if (global_tdb[6]) socket_print(user, "| 07 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[6], global_tdg[6], global_tdb[6] / 1073741824.0, global_tdb[6] / global_tds[6] / 1024.0, global_tdt[6]);
      if (global_tdb[7]) socket_print(user, "| 08 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[7], global_tdg[7], global_tdb[7] / 1073741824.0, global_tdb[7] / global_tds[7] / 1024.0, global_tdt[7]);
      if (global_tdb[8]) socket_print(user, "| 09 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[8], global_tdg[8], global_tdb[8] / 1073741824.0, global_tdb[8] / global_tds[8] / 1024.0, global_tdt[8]);
      if (global_tdb[9]) socket_print(user, "| 10 | %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |\r\n", global_tdu[9], global_tdg[9], global_tdb[9] / 1073741824.0, global_tdb[9] / global_tds[9] / 1024.0, global_tdt[9]);
                         socket_print(user, "+----+----------+----------+---------+-------+---------------------------------+\r\n");
       if (global_db)    socket_print(user, ". TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ub / 1073741824.0, global_uf);
}

void local_user_top_down_alltime_colour(struct login_node *user, int number, char *opt)
{
                         socket_print(user, "200- [ TOP DOWNLOADER ALLTIME ]\r\n");
                         socket_print(user, "[1;37;44m     ______________________________    ___________   ____ ________________      [0;40m\r\n");
                         socket_print(user, "[1;37;44m    /___  ______ \\____  \\_ \\_____ _\\  /_\\  _  \\   \\ /\\   \\\\__   __\\______ \\     [0;40m\r\n");
                         socket_print(user, "[1;37;44m     \\/  /\\/   /  / _____\\\\  \\   \\ \\\\// \\   _  \\   \\__\\   \\_/\\  \\ /\\  \\  \\ \\    [0;40m\r\n");
                         socket_print(user, "[1;37;44m     /  / /   /  /    \\\\   \\  \\   \\ .\\   \\   \\  \\      \\      \\  \\  \\  \\  \\ \\   [0;40m\r\n");
                         socket_print(user, "[1;37;44m    /__/ /_____ /      \\._____/\\___\\__\\   \\___\\__\\._____\\._____\\__\\  \\__\\__\\_\\  [0;40m\r\n");
                         socket_print(user, "[1;37;44m    \\  \\ \\    \\/________\\/   / /   /  /\\  /   /  //     //     /  /  /  /  / /  [0;40m\r\n");
                         socket_print(user, "[1;37;44m     \\__\\/\\____\\        /___/\\/___/__/  \\/___/__//_____//_____/__/ \\/__/__/_/   [0;40m\r\n");
                         socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m--------[1;37;44m\\______/[1;34;44m-[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                         socket_print(user, "[1;36;44m| [1;37;44m## [1;36;44m| [1;37;44mUSER     [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mGBYTES  [1;36;44m|[1;37;44mAVG K/S[1;36;44m| [1;37;44mTAGLINE                         [1;37;44m|[0;40m\r\n");
                         socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
      if (global_tdb[0]) socket_print(user, "[1;36;44m| [1;37;44m01 [1;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tdu[0], global_tdg[0], global_tdb[0] / 1073741824.0, global_tdb[0] / global_tds[0] / 1024.0, global_tdt[0]);
      if (global_tdb[1]) socket_print(user, "[0;36;44m| [1;37;44m02 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tdu[1], global_tdg[1], global_tdb[1] / 1073741824.0, global_tdb[1] / global_tds[1] / 1024.0, global_tdt[1]);
      if (global_tdb[2]) socket_print(user, "[1;34;44m| [1;37;44m03 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdu[2], global_tdg[2], global_tdb[2] / 1073741824.0, global_tdb[2] / global_tds[2] / 1024.0, global_tdt[2]);
      if (global_tdb[3]) socket_print(user, "[1;34;44m| [1;37;44m04 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdu[3], global_tdg[3], global_tdb[3] / 1073741824.0, global_tdb[3] / global_tds[3] / 1024.0, global_tdt[3]);
      if (global_tdb[4]) socket_print(user, "[1;34;44m| [1;37;44m05 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdu[4], global_tdg[4], global_tdb[4] / 1073741824.0, global_tdb[4] / global_tds[4] / 1024.0, global_tdt[4]);
      if (global_tdb[5]) socket_print(user, "[1;34;44m| [1;37;44m06 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdu[5], global_tdg[5], global_tdb[5] / 1073741824.0, global_tdb[5] / global_tds[5] / 1024.0, global_tdt[5]);
      if (global_tdb[6]) socket_print(user, "[1;34;44m| [1;37;44m07 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdu[6], global_tdg[6], global_tdb[6] / 1073741824.0, global_tdb[6] / global_tds[6] / 1024.0, global_tdt[6]);
      if (global_tdb[7]) socket_print(user, "[1;34;44m| [1;37;44m08 [1;34;44m| [0;36;44m%-8.8s [1;34;44m| [0;36;44m%-8.8s [1;34;44m|[0;36;44m%8.2f [1;34;44m|[0;36;44m%6.0f [1;34;44m| [0;36;44m%-31.31s [1;34;44m|[0;40m\r\n", global_tdu[7], global_tdg[7], global_tdb[7] / 1073741824.0, global_tdb[7] / global_tds[7] / 1024.0, global_tdt[7]);
      if (global_tdb[8]) socket_print(user, "[0;36;44m| [1;37;44m09 [0;36;44m| %-8.8s | %-8.8s |%8.2f |%6.0f | %-31.31s |[0;40m\r\n", global_tdu[8], global_tdg[8], global_tdb[8] / 1073741824.0, global_tdb[8] / global_tds[8] / 1024.0, global_tdt[8]);
      if (global_tdb[9]) socket_print(user, "[1;36;44m| [1;37;44m10 [1;36;44m| [0;36;44m%-8.8s [1;36;44m| [0;36;44m%-8.8s [1;36;44m|[0;36;44m%8.2f [1;36;44m|[0;36;44m%6.0f [1;36;44m| [0;36;44m%-31.31s [1;36;44m|[0;40m\r\n", global_tdu[9], global_tdg[9], global_tdb[9] / 1073741824.0, global_tdb[9] / global_tds[9] / 1024.0, global_tdt[9]);
                         socket_print(user, "[1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-----------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
       if (global_db)    socket_print(user, "[1;36;44m. [0;36;44mTOTALS:  [1;36;44m%.2f Gbytes [0;36;44min [1;36;44m%.0f Files[0;40m\r\n", global_ub / 1073741824.0, global_uf);
}


/*
GROUP DAY-TOP UPLOADS
~~~~~~~~~~~~~~~~~~~~~
global_gugd[0-9] Groupname of group who is in daytop-up #1 - #10
global_gumd[0-9] Members of group who is in daytop-up #1 - #10
global_guad[0-9] Active members of group who is in daytop-up #1 - #10
global_gubd[0-9] Bytes of group who is in daytop-up #1 - #10
global_gufd[0-9] Files of group who is in daytop-up #1 - #10
global_gutd[0-9] Tagline of group who is in daytop-up #1 - #10
global_gulgd     Groupname of group who was yesterdays top uploader
global_gulbd     Bytes of group who was yesterdays top uploader
global_gulfd     Files of group who was yesterdays top uploader
global_gulsd     Seconds of group who was yesterdays top uploader
global_gultd     Tagline of group who was yesterdays top uploader
global_gumgd     Groupname of group who has max. uploaded in one day
global_gumbd     Bytes of group who has max. uploaded in one day
global_gumfd     Files of group who has max. uploaded in one day
global_gumsd     Seconds of group who has max. uploaded in one day
global_gumtd     Tagline of group who has max. uploaded in one day
global_ubd       Total amount of bytes uploaded this day
global_ufd       Total amount of files uploaded this day
global_usd       Total amount of seconds uploaded this day
*/

void local_group_top_up_day_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP DAYTOP UP ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gubd[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[0], global_gumd[0], global_guad[0], global_gubd[0] / 1073741824.0, global_gutd[0]);
      if (global_gubd[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[1], global_gumd[1], global_guad[1], global_gubd[1] / 1073741824.0, global_gutd[1]);
      if (global_gubd[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[2], global_gumd[2], global_guad[2], global_gubd[2] / 1073741824.0, global_gutd[2]);
      if (global_gubd[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[3], global_gumd[3], global_guad[3], global_gubd[3] / 1073741824.0, global_gutd[3]);
      if (global_gubd[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[4], global_gumd[4], global_guad[4], global_gubd[4] / 1073741824.0, global_gutd[4]);
      if (global_gubd[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[5], global_gumd[5], global_guad[5], global_gubd[5] / 1073741824.0, global_gutd[5]);
      if (global_gubd[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[6], global_gumd[6], global_guad[6], global_gubd[6] / 1073741824.0, global_gutd[6]);
      if (global_gubd[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[7], global_gumd[7], global_guad[7], global_gubd[7] / 1073741824.0, global_gutd[7]);
      if (global_gubd[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[8], global_gumd[8], global_guad[8], global_gubd[8] / 1073741824.0, global_gutd[8]);
      if (global_gubd[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[9], global_gumd[9], global_guad[9], global_gubd[9] / 1073741824.0, global_gutd[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+-------------------------[Hour XX/24]-+\r\n");
        if (global_gulbd) socket_print(user, "200- | YESTERDAYS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gulgd, global_gulbd / 1073741824.0, global_gulfd);
        if (global_gumbd) socket_print(user, "200- : DAYTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gumgd, global_gumbd / 1073741824.0 , global_gumfd);
        if (global_ubd)   socket_print(user, "200- . TODAYS TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ubd / 1073741824.0, global_ufd);
}

void local_group_top_up_day_colour(struct login_node *user, int number, char *opt)
{
                         socket_print(user, "200- [1;37m---///--- GROUP DAYTOP UP ---///---[0;40m\r\n");
                         socket_print(user, "200- [1;37;44m+[1;36;44m-[0;36;44m--[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m------[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m----[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m---[0;36;44m-[1;36;44m-[1;37;44m+[1;36;44m-[0;36;44m-[1;34;44m-------------------------[0;36;44m-[1;36;44m-[1;37;44m+[0;40m\r\n");
                         socket_print(user, "200- [1;36;44m| [1;37;44m## [1;36;44m| [1;37;44mGROUP    [1;36;44m| [1;37;44mGIGB[1;36;44m| [1;37;44mACTV[1;36;44m|[1;37;44m FILES [1;36;44m|[1;37;44m GBYTES   [1;36;44m| [1;37;44mTAGLINE              [1;36;44m|[0;40m\r\n");
      if (global_gubd[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[0], global_gumd[0], global_guad[0], global_gubd[0] / 1073741824.0, global_gutd[0]);
      if (global_gubd[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[1], global_gumd[1], global_guad[1], global_gubd[1] / 1073741824.0, global_gutd[1]);
      if (global_gubd[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[2], global_gumd[2], global_guad[2], global_gubd[2] / 1073741824.0, global_gutd[2]);
      if (global_gubd[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[3], global_gumd[3], global_guad[3], global_gubd[3] / 1073741824.0, global_gutd[3]);
      if (global_gubd[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[4], global_gumd[4], global_guad[4], global_gubd[4] / 1073741824.0, global_gutd[4]);
      if (global_gubd[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[5], global_gumd[5], global_guad[5], global_gubd[5] / 1073741824.0, global_gutd[5]);
      if (global_gubd[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[6], global_gumd[6], global_guad[6], global_gubd[6] / 1073741824.0, global_gutd[6]);
      if (global_gubd[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[7], global_gumd[7], global_guad[7], global_gubd[7] / 1073741824.0, global_gutd[7]);
      if (global_gubd[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[8], global_gumd[8], global_guad[8], global_gubd[8] / 1073741824.0, global_gutd[8]);
      if (global_gubd[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugd[9], global_gumd[9], global_guad[9], global_gubd[9] / 1073741824.0, global_gutd[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+-------------------------[Hour XX/24]-+\r\n");
}


/*
GROUP DAY-TOP DOWNLOADS
~~~~~~~~~~~~~~~~~~~~~~~
global_gdgd[0-9] Groupname of group who is in daytop-down #1 - #10
global_gdmd[0-9] Members of group who is in daytop-down #1 - #10
global_gdad[0-9] Active members of group who is in daytop-down #1 - #10
global_gdbd[0-9] Bytes of group who is in daytop-down #1 - #10
global_gdfd[0-9] Files of group who is in daytop-down #1 - #10
global_gdtd[0-9] Tagline of group who is in daytop-down #1 - #10
global_gdlgd     Groupname of group who was yesterdays top downloader
global_gdlbd     Bytes of group who was yesterdays top downloader
global_gdlfd     Files of group who was yesterdays top downloader
global_gdlsd     Seconds of group who was yesterdays top downloader
global_gdltd     Tagline of group who was yesterdays top downloader
global_gdmgd     Groupname of group who has max. downloaded in one day
global_gdmbd     Bytes of group who has max. downloaded in one day
global_gdmfd     Files of group who has max. downloaded in one day
global_gdmsd     Seconds of group who has max. uploaded in one day
global_gdmtd     Tagline of group who has max. uploaded in one day
global_dbd       Total amount of bytes downloaded this day
global_dfd       Total amount of files downloaded this day
global_dsd       Total amount of seconds downloaded this day
*/

void local_group_top_down_day_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP DAYTOP DN ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gdbd[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[0], global_gdmd[0], global_gdad[0], global_gdbd[0] / 1073741824.0, global_gdtd[0]);
      if (global_gdbd[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[1], global_gdmd[1], global_gdad[1], global_gdbd[1] / 1073741824.0, global_gdtd[1]);
      if (global_gdbd[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[2], global_gdmd[2], global_gdad[2], global_gdbd[2] / 1073741824.0, global_gdtd[2]);
      if (global_gdbd[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[3], global_gdmd[3], global_gdad[3], global_gdbd[3] / 1073741824.0, global_gdtd[3]);
      if (global_gdbd[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[4], global_gdmd[4], global_gdad[4], global_gdbd[4] / 1073741824.0, global_gdtd[4]);
      if (global_gdbd[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[5], global_gdmd[5], global_gdad[5], global_gdbd[5] / 1073741824.0, global_gdtd[5]);
      if (global_gdbd[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[6], global_gdmd[6], global_gdad[6], global_gdbd[6] / 1073741824.0, global_gdtd[6]);
      if (global_gdbd[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[7], global_gdmd[7], global_gdad[7], global_gdbd[7] / 1073741824.0, global_gdtd[7]);
      if (global_gdbd[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[8], global_gdmd[8], global_gdad[8], global_gdbd[8] / 1073741824.0, global_gdtd[8]);
      if (global_gdbd[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[9], global_gdmd[9], global_gdad[9], global_gdbd[9] / 1073741824.0, global_gdtd[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+-------------------------[Hour XX/24]-+\r\n");
        if (global_gdlbd) socket_print(user, "200- | YESTERDAYS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdlgd, global_gdlbd / 1073741824.0, global_gdlfd);
        if (global_gdmbd) socket_print(user, "200- : DAYTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdmgd, global_gdmbd / 1073741824.0 , global_gdmfd);
        if (global_dbd)   socket_print(user, "200- . TODAYS TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_dbd / 1073741824.0, global_dfd);
}

void local_group_top_down_day_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP DAYTOP DN ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gdbd[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[0], global_gdmd[0], global_gdad[0], global_gdbd[0] / 1073741824.0, global_gdtd[0]);
      if (global_gdbd[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[1], global_gdmd[1], global_gdad[1], global_gdbd[1] / 1073741824.0, global_gdtd[1]);
      if (global_gdbd[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[2], global_gdmd[2], global_gdad[2], global_gdbd[2] / 1073741824.0, global_gdtd[2]);
      if (global_gdbd[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[3], global_gdmd[3], global_gdad[3], global_gdbd[3] / 1073741824.0, global_gdtd[3]);
      if (global_gdbd[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[4], global_gdmd[4], global_gdad[4], global_gdbd[4] / 1073741824.0, global_gdtd[4]);
      if (global_gdbd[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[5], global_gdmd[5], global_gdad[5], global_gdbd[5] / 1073741824.0, global_gdtd[5]);
      if (global_gdbd[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[6], global_gdmd[6], global_gdad[6], global_gdbd[6] / 1073741824.0, global_gdtd[6]);
      if (global_gdbd[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[7], global_gdmd[7], global_gdad[7], global_gdbd[7] / 1073741824.0, global_gdtd[7]);
      if (global_gdbd[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[8], global_gdmd[8], global_gdad[8], global_gdbd[8] / 1073741824.0, global_gdtd[8]);
      if (global_gdbd[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgd[9], global_gdmd[9], global_gdad[9], global_gdbd[9] / 1073741824.0, global_gdtd[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+-------------------------[Hour XX/24]-+\r\n");
        if (global_gdlbd) socket_print(user, "200- | YESTERDAYS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdlgd, global_gdlbd / 1073741824.0, global_gdlfd);
        if (global_gdmbd) socket_print(user, "200- : DAYTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdmgd, global_gdmbd / 1073741824.0 , global_gdmfd);
        if (global_dbd)   socket_print(user, "200- . TODAYS TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_dbd / 1073741824.0, global_dfd);
}


/*
GROUP WEEK-TOP UPLOADS
~~~~~~~~~~~~~~~~~~~~~~
global_gugw[0-9] Groupname of group who is in weektop-up #1 - #10
global_gumw[0-9] Members of group who is in weektop-up #1 - #10
global_guaw[0-9] Active members of group who is in weektop-up #1 - #10
global_gubw[0-9] Bytes of group who is in weektop-up #1 - #10
global_gufw[0-9] Files of group who is in weektop-up #1 - #10
global_gutw[0-9] Tagline of group who is in weektop-up #1 - #10
global_gulgw     Groupname of group who was last weeks top uploader
global_gulbw     Bytes of group who was last weeks top uploader
global_gulfw     Files of group who was last weeks top uploader
global_gulsw     Seconds of group who was last weeks top uploader
global_gultw     Tagline of group who was last weeks top uploader
global_gumgw     Groupname of group who has max. uploaded in one week
global_gumbw     Bytes of group who has max. uploaded in one week
global_gumfw     Files of group who has max. uploaded in one week
global_gumsw     Seconds of group who has max. uploaded in one week
global_gumtw     Tagline of group who has max. uploaded in one week
global_ubw       Total amount of bytes uploaded this week
global_ufw       Total amount of files uploaded this week
global_usw       Total amount of seconds uploaded this week
*/

void local_group_top_up_week_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP WEEKTOP UP ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gubw[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[0], global_gumw[0], global_guaw[0], global_gubw[0] / 1073741824.0, global_gutw[0]);
      if (global_gubw[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[1], global_gumw[1], global_guaw[1], global_gubw[1] / 1073741824.0, global_gutw[1]);
      if (global_gubw[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[2], global_gumw[2], global_guaw[2], global_gubw[2] / 1073741824.0, global_gutw[2]);
      if (global_gubw[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[3], global_gumw[3], global_guaw[3], global_gubw[3] / 1073741824.0, global_gutw[3]);
      if (global_gubw[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[4], global_gumw[4], global_guaw[4], global_gubw[4] / 1073741824.0, global_gutw[4]);
      if (global_gubw[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[5], global_gumw[5], global_guaw[5], global_gubw[5] / 1073741824.0, global_gutw[5]);
      if (global_gubw[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[6], global_gumw[6], global_guaw[6], global_gubw[6] / 1073741824.0, global_gutw[6]);
      if (global_gubw[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[7], global_gumw[7], global_guaw[7], global_gubw[7] / 1073741824.0, global_gutw[7]);
      if (global_gubw[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[8], global_gumw[8], global_guaw[8], global_gubw[8] / 1073741824.0, global_gutw[8]);
      if (global_gubw[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[9], global_gumw[9], global_guaw[9], global_gubw[9] / 1073741824.0, global_gutw[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+----------------------------[Day X/7]-+\r\n");
        if (global_gulbw) socket_print(user, "200- | LAST WEEKS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gulgw, global_gulbw / 1073741824.0, global_gulfw);
        if (global_gumbw) socket_print(user, "200- : WEEKTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gumgw, global_gumbw / 1073741824.0 , global_gumfw);
        if (global_ubw)   socket_print(user, "200- . WEEK TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ubw / 1073741824.0, global_ufw);
}

void local_group_top_up_week_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP WEEKTOP UP ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gubw[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[0], global_gumw[0], global_guaw[0], global_gubw[0] / 1073741824.0, global_gutw[0]);
      if (global_gubw[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[1], global_gumw[1], global_guaw[1], global_gubw[1] / 1073741824.0, global_gutw[1]);
      if (global_gubw[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[2], global_gumw[2], global_guaw[2], global_gubw[2] / 1073741824.0, global_gutw[2]);
      if (global_gubw[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[3], global_gumw[3], global_guaw[3], global_gubw[3] / 1073741824.0, global_gutw[3]);
      if (global_gubw[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[4], global_gumw[4], global_guaw[4], global_gubw[4] / 1073741824.0, global_gutw[4]);
      if (global_gubw[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[5], global_gumw[5], global_guaw[5], global_gubw[5] / 1073741824.0, global_gutw[5]);
      if (global_gubw[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[6], global_gumw[6], global_guaw[6], global_gubw[6] / 1073741824.0, global_gutw[6]);
      if (global_gubw[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[7], global_gumw[7], global_guaw[7], global_gubw[7] / 1073741824.0, global_gutw[7]);
      if (global_gubw[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[8], global_gumw[8], global_guaw[8], global_gubw[8] / 1073741824.0, global_gutw[8]);
      if (global_gubw[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugw[9], global_gumw[9], global_guaw[9], global_gubw[9] / 1073741824.0, global_gutw[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+----------------------------[Day X/7]-+\r\n");
        if (global_gulbw) socket_print(user, "200- | LAST WEEKS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gulgw, global_gulbw / 1073741824.0, global_gulfw);
        if (global_gumbw) socket_print(user, "200- : WEEKTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gumgw, global_gumbw / 1073741824.0 , global_gumfw);
        if (global_ubw)   socket_print(user, "200- . WEEK TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ubw / 1073741824.0, global_ufw);
}

/*
GROUP WEEK-TOP DOWNLOADS
~~~~~~~~~~~~~~~~~~~~~~~~
global_gdgw[0-9] Groupname of group who is in weektop-down #1 - #10
global_gdmw[0-9] Members of group who is in weektop-down #1 - #10
global_gdaw[0-9] Active members of group who is in weektop-down #1 - #10
global_gdbw[0-9] Bytes of group who is in weektop-down #1 - #10
global_gdfw[0-9] Files of group who is in weektop-down #1 - #10
global_gdtw[0-9] Tagline of group who is in weektop-down #1 - #10
global_gdlgw     Groupname of group who was last weeks top downloader
global_gdlbw     Bytes of group who was last weeks top downloader
global_gdlfw     Files of group who was last weeks top downloader
global_gdlsw     Seconds of group who was last weeks top downloader
global_gdltw     Tagline of group who was last weeks top downloader
global_gdmgw     Groupname of group who has max. downloaded in one week
global_gdmbw     Bytes of group who has max. downloaded in one week
global_gdmfw     Files of group who has max. downloaded in one week
global_gdmsw     Seconds of group who has max. downloaded in one week
global_gdmtw     Tagline of group who has max. downloaded in one week
global_dbw       Total amount of bytes downloaded this week
global_dfw       Total amount of files downloaded this week
global_dsw       Total amount of seconds downloaded this week
*/

void local_group_top_down_week_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP WEEKTOP DN ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gdbw[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[0], global_gdmw[0], global_gdaw[0], global_gdbw[0] / 1073741824.0, global_gdtw[0]);
      if (global_gdbw[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[1], global_gdmw[1], global_gdaw[1], global_gdbw[1] / 1073741824.0, global_gdtw[1]);
      if (global_gdbw[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[2], global_gdmw[2], global_gdaw[2], global_gdbw[2] / 1073741824.0, global_gdtw[2]);
      if (global_gdbw[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[3], global_gdmw[3], global_gdaw[3], global_gdbw[3] / 1073741824.0, global_gdtw[3]);
      if (global_gdbw[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[4], global_gdmw[4], global_gdaw[4], global_gdbw[4] / 1073741824.0, global_gdtw[4]);
      if (global_gdbw[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[5], global_gdmw[5], global_gdaw[5], global_gdbw[5] / 1073741824.0, global_gdtw[5]);
      if (global_gdbw[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[6], global_gdmw[6], global_gdaw[6], global_gdbw[6] / 1073741824.0, global_gdtw[6]);
      if (global_gdbw[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[7], global_gdmw[7], global_gdaw[7], global_gdbw[7] / 1073741824.0, global_gdtw[7]);
      if (global_gdbw[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[8], global_gdmw[8], global_gdaw[8], global_gdbw[8] / 1073741824.0, global_gdtw[8]);
      if (global_gdbw[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[9], global_gdmw[9], global_gdaw[9], global_gdbw[9] / 1073741824.0, global_gdtw[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+----------------------------[Day X/7]-+\r\n");
        if (global_gdlbw) socket_print(user, "200- | LAST WEEKS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdlgw, global_gdlbw / 1073741824.0, global_gdlfw);
        if (global_gdmbw) socket_print(user, "200- : WEEKTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdmgw, global_gdmbw / 1073741824.0 , global_gdmfw);
        if (global_dbw)   socket_print(user, "200- . WEEK TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_dbw / 1073741824.0, global_dfw);
}

void local_group_top_down_week_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP WEEKTOP DN ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gdbw[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[0], global_gdmw[0], global_gdaw[0], global_gdbw[0] / 1073741824.0, global_gdtw[0]);
      if (global_gdbw[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[1], global_gdmw[1], global_gdaw[1], global_gdbw[1] / 1073741824.0, global_gdtw[1]);
      if (global_gdbw[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[2], global_gdmw[2], global_gdaw[2], global_gdbw[2] / 1073741824.0, global_gdtw[2]);
      if (global_gdbw[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[3], global_gdmw[3], global_gdaw[3], global_gdbw[3] / 1073741824.0, global_gdtw[3]);
      if (global_gdbw[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[4], global_gdmw[4], global_gdaw[4], global_gdbw[4] / 1073741824.0, global_gdtw[4]);
      if (global_gdbw[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[5], global_gdmw[5], global_gdaw[5], global_gdbw[5] / 1073741824.0, global_gdtw[5]);
      if (global_gdbw[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[6], global_gdmw[6], global_gdaw[6], global_gdbw[6] / 1073741824.0, global_gdtw[6]);
      if (global_gdbw[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[7], global_gdmw[7], global_gdaw[7], global_gdbw[7] / 1073741824.0, global_gdtw[7]);
      if (global_gdbw[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[8], global_gdmw[8], global_gdaw[8], global_gdbw[8] / 1073741824.0, global_gdtw[8]);
      if (global_gdbw[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgw[9], global_gdmw[9], global_gdaw[9], global_gdbw[9] / 1073741824.0, global_gdtw[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+----------------------------[Day X/7]-+\r\n");
        if (global_gdlbw) socket_print(user, "200- | LAST WEEKS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdlgw, global_gdlbw / 1073741824.0, global_gdlfw);
        if (global_gdmbw) socket_print(user, "200- : WEEKTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdmgw, global_gdmbw / 1073741824.0 , global_gdmfw);
        if (global_dbw)   socket_print(user, "200- . WEEK TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_dbw / 1073741824.0, global_dfw);
}

/*
GROUP MONTH-TOP UPLOADS
~~~~~~~~~~~~~~~~~~~~~~~
global_gugm[0-9] Groupname of group who is in monthtop-up #1 - #10
global_gumm[0-9] Members of group who is in monthtop-up #1 - #10
global_guam[0-9] Active members of group who is in monthtop-up #1 - #10
global_gubm[0-9] Bytes of group who is in monthtop-up #1 - #10
global_gufm[0-9] Files of group who is in monthtop-up #1 - #10
global_gutm[0-9] Tagline of group who is in monthtop-up #1 - #10
global_gulgm     Groupname of group who was last months top uploader
global_gulbm     Bytes of group who was last months top uploader
global_gulfm     Files of group who was last months top uploader
global_gulsm     Seconds of group who was last months top uploader
global_gultm     Tagline of group who was last months top uploader
global_gumgm     Groupname of group who has max. uploaded in one month
global_gumbm     Bytes of group who has max. uploaded in one month
global_gumfm     Files of group who has max. uploaded in one month
global_gumsm     Seconds of group who has max. uploaded in one month
global_gumtm     Tagline of group who has max. uploaded in one month
global_ubm       Total amount of bytes uploaded this month
global_ufm       Total amount of files uploaded this month
global_usm       Total amount of seconds uploaded this month
*/

void local_group_top_up_month_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP MONTHTOP UP ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gubm[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[0], global_gumm[0], global_guam[0], global_gubm[0] / 1073741824.0, global_gutm[0]);
      if (global_gubm[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[1], global_gumm[1], global_guam[1], global_gubm[1] / 1073741824.0, global_gutm[1]);
      if (global_gubm[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[2], global_gumm[2], global_guam[2], global_gubm[2] / 1073741824.0, global_gutm[2]);
      if (global_gubm[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[3], global_gumm[3], global_guam[3], global_gubm[3] / 1073741824.0, global_gutm[3]);
      if (global_gubm[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[4], global_gumm[4], global_guam[4], global_gubm[4] / 1073741824.0, global_gutm[4]);
      if (global_gubm[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[5], global_gumm[5], global_guam[5], global_gubm[5] / 1073741824.0, global_gutm[5]);
      if (global_gubm[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[6], global_gumm[6], global_guam[6], global_gubm[6] / 1073741824.0, global_gutm[6]);
      if (global_gubm[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[7], global_gumm[7], global_guam[7], global_gubm[7] / 1073741824.0, global_gutm[7]);
      if (global_gubm[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[8], global_gumm[8], global_guam[8], global_gubm[8] / 1073741824.0, global_gutm[8]);
      if (global_gubm[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[9], global_gumm[9], global_guam[9], global_gubm[9] / 1073741824.0, global_gutm[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------[Day XX/XX]-+\r\n");
        if (global_gulbm) socket_print(user, "200- | LAST MONTHS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gulgm, global_gulbm / 1073741824.0, global_gulfm);
        if (global_gumbm) socket_print(user, "200- : MONTHTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gumgm, global_gumbm / 1073741824.0 , global_gumfm);
        if (global_ubm)   socket_print(user, "200- . MONTH TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ubm / 1073741824.0, global_ufm);
}

void local_group_top_up_month_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP MONTHTOP UP ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gubm[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[0], global_gumm[0], global_guam[0], global_gubm[0] / 1073741824.0, global_gutm[0]);
      if (global_gubm[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[1], global_gumm[1], global_guam[1], global_gubm[1] / 1073741824.0, global_gutm[1]);
      if (global_gubm[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[2], global_gumm[2], global_guam[2], global_gubm[2] / 1073741824.0, global_gutm[2]);
      if (global_gubm[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[3], global_gumm[3], global_guam[3], global_gubm[3] / 1073741824.0, global_gutm[3]);
      if (global_gubm[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[4], global_gumm[4], global_guam[4], global_gubm[4] / 1073741824.0, global_gutm[4]);
      if (global_gubm[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[5], global_gumm[5], global_guam[5], global_gubm[5] / 1073741824.0, global_gutm[5]);
      if (global_gubm[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[6], global_gumm[6], global_guam[6], global_gubm[6] / 1073741824.0, global_gutm[6]);
      if (global_gubm[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[7], global_gumm[7], global_guam[7], global_gubm[7] / 1073741824.0, global_gutm[7]);
      if (global_gubm[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[8], global_gumm[8], global_guam[8], global_gubm[8] / 1073741824.0, global_gutm[8]);
      if (global_gubm[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gugm[9], global_gumm[9], global_guam[9], global_gubm[9] / 1073741824.0, global_gutm[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------[Day XX/XX]-+\r\n");
        if (global_gulbm) socket_print(user, "200- | LAST MONTHS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gulgm, global_gulbm / 1073741824.0, global_gulfm);
        if (global_gumbm) socket_print(user, "200- : MONTHTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gumgm, global_gumbm / 1073741824.0 , global_gumfm);
        if (global_ubm)   socket_print(user, "200- . MONTH TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ubm / 1073741824.0, global_ufm);
}

/*
GROUP MONTH-TOP DOWNLOADS
~~~~~~~~~~~~~~~~~~~~~~~~~
global_gdgm[0-9] Groupname of group who is in monthtop-down #1 - #10
global_gdmm[0-9] Members of group who is in monthtop-down #1 - #10
global_gdam[0-9] Active members of group who is in monthtop-down #1 - #10
global_gdbm[0-9] Bytes of group who is in monthtop-down #1 - #10
global_gdfm[0-9] Files of group who is in monthtop-down #1 - #10
global_gdtm[0-9] Tagline of group who is in monthtop-down #1 - #10
global_gdlgm     Groupname of group who was last months top downloader
global_gdlbm     Bytes of group who was last months top downloader
global_gdlfm     Files of group who was last months top downloader
global_gdlsm     Seconds of group who was last months top downloader
global_gdltm     Tagline of group who was last months top downloader
global_gdmgm     Groupname of group who has max. downloaded in one month
global_gdmbm     Bytes of group who has max. downloaded in one month
global_gdmfm     Files of group who has max. downloaded in one month
global_gdmsm     Seconds of group who has max. downloaded in one month
global_gdmtm     Tagline of group who has max. downloaded in one month
global_dbm       Total amount of bytes downloaded this month
global_dfm       Total amount of files downloaded this month
global_dsm       Total amount of seconds downloaded this month
*/

void local_group_top_down_month_mono(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP MONTHTOP DN ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gdbm[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[0], global_gdmm[0], global_gdam[0], global_gdbm[0] / 1073741824.0, global_gdtm[0]);
      if (global_gdbm[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[1], global_gdmm[1], global_gdam[1], global_gdbm[1] / 1073741824.0, global_gdtm[1]);
      if (global_gdbm[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[2], global_gdmm[2], global_gdam[2], global_gdbm[2] / 1073741824.0, global_gdtm[2]);
      if (global_gdbm[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[3], global_gdmm[3], global_gdam[3], global_gdbm[3] / 1073741824.0, global_gdtm[3]);
      if (global_gdbm[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[4], global_gdmm[4], global_gdam[4], global_gdbm[4] / 1073741824.0, global_gdtm[4]);
      if (global_gdbm[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[5], global_gdmm[5], global_gdam[5], global_gdbm[5] / 1073741824.0, global_gdtm[5]);
      if (global_gdbm[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[6], global_gdmm[6], global_gdam[6], global_gdbm[6] / 1073741824.0, global_gdtm[6]);
      if (global_gdbm[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[7], global_gdmm[7], global_gdam[7], global_gdbm[7] / 1073741824.0, global_gdtm[7]);
      if (global_gdbm[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[8], global_gdmm[8], global_gdam[8], global_gdbm[8] / 1073741824.0, global_gdtm[8]);
      if (global_gdbm[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[9], global_gdmm[9], global_gdam[9], global_gdbm[9] / 1073741824.0, global_gdtm[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------[Day XX/XX]-+\r\n");
        if (global_gdlbm) socket_print(user, "200- | LAST MONTHS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdlgm, global_gdlbm / 1073741824.0, global_gdlfm);
        if (global_gdmbm) socket_print(user, "200- : MONTHTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdmgm, global_gdmbm / 1073741824.0 , global_gdmfm);
        if (global_dbm)   socket_print(user, "200- . MONTH TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_dbm / 1073741824.0, global_dfm);
}

void local_group_top_down_month_colour(struct login_node *user, int number, char *opt)
{
                          socket_print(user, "200- ---///--- GROUP MONTHTOP DN ---///---\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                          socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gdbm[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[0], global_gdmm[0], global_gdam[0], global_gdbm[0] / 1073741824.0, global_gdtm[0]);
      if (global_gdbm[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[1], global_gdmm[1], global_gdam[1], global_gdbm[1] / 1073741824.0, global_gdtm[1]);
      if (global_gdbm[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[2], global_gdmm[2], global_gdam[2], global_gdbm[2] / 1073741824.0, global_gdtm[2]);
      if (global_gdbm[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[3], global_gdmm[3], global_gdam[3], global_gdbm[3] / 1073741824.0, global_gdtm[3]);
      if (global_gdbm[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[4], global_gdmm[4], global_gdam[4], global_gdbm[4] / 1073741824.0, global_gdtm[4]);
      if (global_gdbm[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[5], global_gdmm[5], global_gdam[5], global_gdbm[5] / 1073741824.0, global_gdtm[5]);
      if (global_gdbm[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[6], global_gdmm[6], global_gdam[6], global_gdbm[6] / 1073741824.0, global_gdtm[6]);
      if (global_gdbm[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[7], global_gdmm[7], global_gdam[7], global_gdbm[7] / 1073741824.0, global_gdtm[7]);
      if (global_gdbm[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[8], global_gdmm[8], global_gdam[8], global_gdbm[8] / 1073741824.0, global_gdtm[8]);
      if (global_gdbm[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdgm[9], global_gdmm[9], global_gdam[9], global_gdbm[9] / 1073741824.0, global_gdtm[9]);
                          socket_print(user, "200- +----+----------+----+----+--------+--------------------------[Day XX/XX]-+\r\n");
        if (global_gdlbm) socket_print(user, "200- | LAST MONTHS #1:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdlgm, global_gdlbm / 1073741824.0, global_gdlfm);
        if (global_gdmbm) socket_print(user, "200- : MONTHTOP EVER:  %s with %.2f Gbytes in %.0f Files\r\n", global_gdmgm, global_gdmbm / 1073741824.0 , global_gdmfm);
        if (global_dbm)   socket_print(user, "200- . MONTH TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_dbm / 1073741824.0, global_dfm);
}


/*
GROUP ALLTIME-TOP UPLOADS
~~~~~~~~~~~~~~~~~~~~~~~~~
global_gug[0-9] Groupname of group who is in alltimetop-up #1 - #10
global_gum[0-9] Members of group who is in alltimetop-up #1 - #10
global_gua[0-9] Active members of group who is in alltimetop-up #1 - #10
global_gub[0-9] Bytes of group who is in alltimetop-up #1 - #10
global_guf[0-9] Files of group who is in alltimetop-up #1 - #10
global_gut[0-9] Tagline of group who is in alltimetop-up #1 - #10
global_ub       Total amount of bytes uploaded alltime
global_uf       Total amount of files uploaded alltime
global_us       Total amount of seconds uploaded alltime
*/

void local_group_top_up_alltime_mono(struct login_node *user, int number, char *opt)
{
                         socket_print(user, "200- ---///--- GROUP ALLTIMETOP UP ---///---\r\n");
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                         socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gub[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[0], global_gum[0], global_gua[0], global_gub[0] / 1073741824.0, global_gut[0]);
      if (global_gub[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[1], global_gum[1], global_gua[1], global_gub[1] / 1073741824.0, global_gut[1]);
      if (global_gub[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[2], global_gum[2], global_gua[2], global_gub[2] / 1073741824.0, global_gut[2]);
      if (global_gub[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[3], global_gum[3], global_gua[3], global_gub[3] / 1073741824.0, global_gut[3]);
      if (global_gub[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[4], global_gum[4], global_gua[4], global_gub[4] / 1073741824.0, global_gut[4]);
      if (global_gub[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[5], global_gum[5], global_gua[5], global_gub[5] / 1073741824.0, global_gut[5]);
      if (global_gub[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[6], global_gum[6], global_gua[6], global_gub[6] / 1073741824.0, global_gut[6]);
      if (global_gub[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[7], global_gum[7], global_gua[7], global_gub[7] / 1073741824.0, global_gut[7]);
      if (global_gub[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[8], global_gum[8], global_gua[8], global_gub[8] / 1073741824.0, global_gut[8]);
      if (global_gub[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[9], global_gum[9], global_gua[9], global_gub[9] / 1073741824.0, global_gut[9]);
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
        if (global_ub)   socket_print(user, "200- . TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ub / 1073741824.0, global_uf);
}

void local_group_top_up_alltime_colour(struct login_node *user, int number, char *opt)
{
                         socket_print(user, "200- ---///--- GROUP ALLTIMETOP UP ---///---\r\n");
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                         socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gub[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[0], global_gum[0], global_gua[0], global_gub[0] / 1073741824.0, global_gut[0]);
      if (global_gub[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[1], global_gum[1], global_gua[1], global_gub[1] / 1073741824.0, global_gut[1]);
      if (global_gub[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[2], global_gum[2], global_gua[2], global_gub[2] / 1073741824.0, global_gut[2]);
      if (global_gub[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[3], global_gum[3], global_gua[3], global_gub[3] / 1073741824.0, global_gut[3]);
      if (global_gub[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[4], global_gum[4], global_gua[4], global_gub[4] / 1073741824.0, global_gut[4]);
      if (global_gub[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[5], global_gum[5], global_gua[5], global_gub[5] / 1073741824.0, global_gut[5]);
      if (global_gub[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[6], global_gum[6], global_gua[6], global_gub[6] / 1073741824.0, global_gut[6]);
      if (global_gub[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[7], global_gum[7], global_gua[7], global_gub[7] / 1073741824.0, global_gut[7]);
      if (global_gub[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[8], global_gum[8], global_gua[8], global_gub[8] / 1073741824.0, global_gut[8]);
      if (global_gub[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gug[9], global_gum[9], global_gua[9], global_gub[9] / 1073741824.0, global_gut[9]);
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
        if (global_ub)   socket_print(user, "200- . TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_ub / 1073741824.0, global_uf);
}


/*
GROUP ALLTIME-TOP DOWNLOADS
~~~~~~~~~~~~~~~~~~~~~~~~~~~
global_gdg[0-9] Groupname of group who is in alltimetop-down #1 - #10
global_gdm[0-9] Members of group who is in alltimetop-down #1 - #10
global_gda[0-9] Active members of group who is in alltimetop-down #1 - #10
global_gdb[0-9] Bytes of group who is in alltimetop-down #1 - #10
global_gdf[0-9] Files of group who is in alltimetop-down #1 - #10
global_gdt[0-9] Tagline of group who is in alltimetop-down #1 - #10
global_db       Total amount of bytes downloaded alltime
global_df       Total amount of files downloaded alltime
global_ds       Total amount of seconds downloaded alltime
*/

void local_group_top_down_alltime_mono(struct login_node *user, int number, char *opt)
{
                         socket_print(user, "200- ---///--- GROUP ALLTIMETOP DN ---///---\r\n");
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                         socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gdb[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[0], global_gdm[0], global_gda[0], global_gdb[0] / 1073741824.0, global_gdt[0]);
      if (global_gdb[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[1], global_gdm[1], global_gda[1], global_gdb[1] / 1073741824.0, global_gdt[1]);
      if (global_gdb[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[2], global_gdm[2], global_gda[2], global_gdb[2] / 1073741824.0, global_gdt[2]);
      if (global_gdb[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[3], global_gdm[3], global_gda[3], global_gdb[3] / 1073741824.0, global_gdt[3]);
      if (global_gdb[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[4], global_gdm[4], global_gda[4], global_gdb[4] / 1073741824.0, global_gdt[4]);
      if (global_gdb[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[5], global_gdm[5], global_gda[5], global_gdb[5] / 1073741824.0, global_gdt[5]);
      if (global_gdb[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[6], global_gdm[6], global_gda[6], global_gdb[6] / 1073741824.0, global_gdt[6]);
      if (global_gdb[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[7], global_gdm[7], global_gda[7], global_gdb[7] / 1073741824.0, global_gdt[7]);
      if (global_gdb[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[8], global_gdm[8], global_gda[8], global_gdb[8] / 1073741824.0, global_gdt[8]);
      if (global_gdb[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[9], global_gdm[9], global_gda[9], global_gdb[9] / 1073741824.0, global_gdt[9]);
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
        if (global_db)   socket_print(user, "200- . TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_db / 1073741824.0, global_df);
}

void local_group_top_down_alltime_colour(struct login_node *user, int number, char *opt)
{
                         socket_print(user, "200- ---///--- GROUP ALLTIMETOP DN ---///---\r\n");
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
                         socket_print(user, "200- | ## | GROUP    |GIGB|ACTV| GBYTES | TAGLINE                              |\r\n");
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
      if (global_gdb[0]) socket_print(user, "200- | 01 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[0], global_gdm[0], global_gda[0], global_gdb[0] / 1073741824.0, global_gdt[0]);
      if (global_gdb[1]) socket_print(user, "200- | 02 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[1], global_gdm[1], global_gda[1], global_gdb[1] / 1073741824.0, global_gdt[1]);
      if (global_gdb[2]) socket_print(user, "200- | 03 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[2], global_gdm[2], global_gda[2], global_gdb[2] / 1073741824.0, global_gdt[2]);
      if (global_gdb[3]) socket_print(user, "200- | 04 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[3], global_gdm[3], global_gda[3], global_gdb[3] / 1073741824.0, global_gdt[3]);
      if (global_gdb[4]) socket_print(user, "200- | 05 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[4], global_gdm[4], global_gda[4], global_gdb[4] / 1073741824.0, global_gdt[4]);
      if (global_gdb[5]) socket_print(user, "200- | 06 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[5], global_gdm[5], global_gda[5], global_gdb[5] / 1073741824.0, global_gdt[5]);
      if (global_gdb[6]) socket_print(user, "200- | 07 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[6], global_gdm[6], global_gda[6], global_gdb[6] / 1073741824.0, global_gdt[6]);
      if (global_gdb[7]) socket_print(user, "200- | 08 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[7], global_gdm[7], global_gda[7], global_gdb[7] / 1073741824.0, global_gdt[7]);
      if (global_gdb[8]) socket_print(user, "200- | 09 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[8], global_gdm[8], global_gda[8], global_gdb[8] / 1073741824.0, global_gdt[8]);
      if (global_gdb[9]) socket_print(user, "200- | 10 | %-8.8s | %2f | %2f | %6.2f | %-27.27s |\r\n", global_gdg[9], global_gdm[9], global_gda[9], global_gdb[9] / 1073741824.0, global_gdt[9]);
                         socket_print(user, "200- +----+----------+----+----+--------+--------------------------------------+\r\n");
        if (global_db)   socket_print(user, "200- . TOTALS:  %.2f Gbytes in %.0f Files\r\n", global_db / 1073741824.0, global_df);
}

