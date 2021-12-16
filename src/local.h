/*
 *
 * File of local FTP protocol replies.
 *
 */

#include "colour.h"



/* STATS DEFINES */


#define USER_BUD ((struct quota_node *)user->quota)->bytes_up_day
#define USER_BUW ((struct quota_node *)user->quota)->bytes_up_week
#define USER_BUM ((struct quota_node *)user->quota)->bytes_up_month
#define USER_BU  ((struct quota_node *)user->quota)->bytes_up
#define USER_BDD ((struct quota_node *)user->quota)->bytes_down_day
#define USER_BDW ((struct quota_node *)user->quota)->bytes_down_week
#define USER_BDM ((struct quota_node *)user->quota)->bytes_down_month
#define USER_BD  ((struct quota_node *)user->quota)->bytes_down

#define USER_FUD ((struct quota_node *)user->quota)->files_up_day
#define USER_FUW ((struct quota_node *)user->quota)->files_up_week
#define USER_FUM ((struct quota_node *)user->quota)->files_up_month
#define USER_FU  ((struct quota_node *)user->quota)->files_up
#define USER_FDD ((struct quota_node *)user->quota)->files_down_day
#define USER_FDW ((struct quota_node *)user->quota)->files_down_week
#define USER_FDM ((struct quota_node *)user->quota)->files_down_month
#define USER_FD  ((struct quota_node *)user->quota)->files_down

#define USER_SUD ((struct quota_node *)user->quota)->seconds_up_day
#define USER_SUW ((struct quota_node *)user->quota)->seconds_up_week
#define USER_SUM ((struct quota_node *)user->quota)->seconds_up_month
#define USER_SU  ((struct quota_node *)user->quota)->seconds_up
#define USER_SDD ((struct quota_node *)user->quota)->seconds_down_day
#define USER_SDW ((struct quota_node *)user->quota)->seconds_down_week
#define USER_SDM ((struct quota_node *)user->quota)->seconds_down_month
#define USER_SD  ((struct quota_node *)user->quota)->seconds_down


#define USER_LT ((struct quota_node *)user->quota)->login_times

#define USER_CRD ((struct quota_node *)user->quota)->credits
#define USER_RATIO ((struct quota_node *)user->quota)->ratio

#define REMOTE_BUD ((struct quota_node *)remote->quota)->bytes_up_day
#define REMOTE_BUW ((struct quota_node *)remote->quota)->bytes_up_week
#define REMOTE_BUM ((struct quota_node *)remote->quota)->bytes_up_month
#define REMOTE_BU  ((struct quota_node *)remote->quota)->bytes_up
#define REMOTE_BDD ((struct quota_node *)remote->quota)->bytes_down_day
#define REMOTE_BDW ((struct quota_node *)remote->quota)->bytes_down_week
#define REMOTE_BDM ((struct quota_node *)remote->quota)->bytes_down_month
#define REMOTE_BD  ((struct quota_node *)remote->quota)->bytes_down

#define REMOTE_FUD ((struct quota_node *)remote->quota)->files_up_day
#define REMOTE_FUW ((struct quota_node *)remote->quota)->files_up_week
#define REMOTE_FUM ((struct quota_node *)remote->quota)->files_up_month
#define REMOTE_FU  ((struct quota_node *)remote->quota)->files_up
#define REMOTE_FDD ((struct quota_node *)remote->quota)->files_down_day
#define REMOTE_FDW ((struct quota_node *)remote->quota)->files_down_week
#define REMOTE_FDM ((struct quota_node *)remote->quota)->files_down_month
#define REMOTE_FD  ((struct quota_node *)remote->quota)->files_down

#define REMOTE_SUD ((struct quota_node *)remote->quota)->seconds_up_day
#define REMOTE_SUW ((struct quota_node *)remote->quota)->seconds_up_week
#define REMOTE_SUM ((struct quota_node *)remote->quota)->seconds_up_month
#define REMOTE_SU  ((struct quota_node *)remote->quota)->seconds_up
#define REMOTE_SDD ((struct quota_node *)remote->quota)->seconds_down_day
#define REMOTE_SDW ((struct quota_node *)remote->quota)->seconds_down_week
#define REMOTE_SDM ((struct quota_node *)remote->quota)->seconds_down_month
#define REMOTE_SD  ((struct quota_node *)remote->quota)->seconds_down

#define REMOTE_CRD ((struct quota_node *)remote->quota)->credits
#define REMOTE_RATIO ((struct quota_node *)remote->quota)->ratio
#define REMOTE_LT ((struct quota_node *)remote->quota)->login_times


/* FTP RFC replies */

#define MSG_214_HEADER \
 "214-The following commands are recognized (* =>'s unimplemented).\r\n"
#define MSG_214C_HEADER \
 "214-"_COLB_"The following commands are recognized (* =>'s unimplemented)."\
 _COLO_"\r\n"


#define MSG_214_TAIL "214 Direct comments to %s.\r\n", server_email
#define MSG_214C_TAIL "214 "_COLA_"Direct comments to "\
 _COLD_"%s."_COLO_"\r\n", server_email

/* LundFTPD replies */





 /* Functions */

#ifndef login_node
struct login_node;
#endif

void local_login(struct login_node *);
void local_site_who(struct login_node *);
