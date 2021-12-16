#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED


/* Defines */

#include "lion_types.h"

/* #defines */
#define VERSION_STR       "LundFTPD 3.17"

#define CONFIG            "lundftpd.conf"
#define WORDLEN           64
#define MAXKEYWORD        1024
#define INPUT_BUFFER_SIZE 512
#define INPUT_MAXLINE     256   /* If we haven't found a line withing 128 bytes
                                 * flush current buffer contents. */

#define IPMATCHMAX        10    /* Define max # of patterns per user */
#define IPMATCHLEN        30    /* Mex length of a pattern string */

#define MAXUSERNAME		12
#define MAXPASSNAME		16
#define MAXTAGLINE		32

#define MAX_PATH		255


#define NUM_TOP  10  /* 0-9 */
#define NUM_WALL 12
#define NUM_NUKE 20
#define NUM_LAST 20
#define NUM_NEW  30


// Variables

extern time_t global_time_now;

extern char *server_configfile;
extern int server_hasroot;
extern int server_userfilecheck;
extern int server_dupecheck;

//---------------------------------
// NETWORK SETTINGS
extern int server_port;
extern char *server_bindif;
extern int server_useident;
extern int server_data_buffer_size;
extern int server_data_bind_port;
extern int server_capping;
extern int server_cap_download;
extern int server_cap_upload;
extern int server_cap_user_download;
extern int server_cap_user_upload;
extern int server_cap_total_download;
extern int server_cap_total_upload;
extern int server_data_pasv_start;
extern int server_data_pasv_end;


//-----------------------------------------------
// FILE LOCATIONS/FILESYSTEM SETUP
extern char *server_userfile;
extern char *server_msgpath;
extern char *server_wallpath;
extern char *server_quotafile;
extern char *server_dupedbfile;
extern char *server_logxfer;
extern char *server_loguser;
extern char *server_logadmin;
extern char *server_usechroot;
extern int server_logrotate;
extern int server_dupemaxdays;
extern int server_usesoftchroot;
extern int server_permfile;
extern int server_goodfile;
extern int server_permdir;
extern char *server_mkdirstring;
extern int server_num_list_helpers;

//-----------------------------------------------
// SERVER SETTINGS
extern char *server_greeting;
extern char *server_rawgreeting;
extern char *server_email;
extern int server_walllogin;
extern int server_maximumlogins;
extern int server_userquotadaily;
extern int server_userquotaratio;
extern int server_userquotatime;
extern int server_anonymous;
extern int server_forceglobalonly;
extern int server_connect_from_global_only;
extern int server_usesetuid;
extern int server_useridstart;
extern int server_lookupuid;
extern char *server_nobody;
extern int server_useridle;
extern int server_usermatch;
extern int server_showlastlogin;
extern int server_forcetagline;
extern char *server_extractmp3format;
extern int server_unnuke;

//-----------------------------------------------
// ADDUSER DEFAULTS
extern int server_loginlimit;
extern int server_uploadlimit;
extern int server_downloadlimit;
extern int server_usercredits;
extern char *server_defaulttagline;
extern int server_defaultflags;

//-----------------------------------------------
// GROUP DEFAULTS
extern char *server_groupfile;
extern char *server_grouppath;
extern int server_groupmaxlogins;
extern int server_groupmaxratio;
extern int server_groupmaxidle;
extern int server_grouptotalslots;
extern int server_groupleechslots;
extern char *server_nogroupname;
extern int server_group_pattern_limit;


//-----------------------------------------------
// FILECHECK
extern int server_maxsizecheck;
extern int server_backgroundtest;
extern char *server_autoextract_diz;


//-----------------------------------------------
// IRC
extern int server_irccmds;
extern int server_irc_src_port;


//-----------------------------------------------
// ENCRYPTION
extern int server_allow_only_secure;
extern int server_allow_only_secure_data;
extern char *server_srp_passwdfile;
extern char *server_srp_masterkey;
extern char *server_tls_rsafile;
extern char *server_tls_ciphers;
extern char *server_egdsocket;



/* Daytop-up */
extern unsigned char global_tuud[NUM_TOP][MAXUSERNAME+2]; /* Username */
extern unsigned char global_tugd[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_tubd[NUM_TOP];                /* Bytes */
extern unsigned int  global_tufd[NUM_TOP];                /* Files */
extern unsigned int  global_tusd[NUM_TOP];                /* Seconds */
extern unsigned char global_tutd[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Daytop-down */
extern unsigned char global_tdud[NUM_TOP][MAXUSERNAME+2]; /* Username */
extern unsigned char global_tdgd[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_tdbd[NUM_TOP];                /* Bytes */
extern unsigned int  global_tdfd[NUM_TOP];                /* Files */
extern unsigned int  global_tdsd[NUM_TOP];                /* Seconds */
extern unsigned char global_tdtd[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Daytop-up yesterday */
extern unsigned char global_tulud[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tulgd[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tulbd;                        /* Bytes */
extern unsigned int  global_tulfd;                        /* Files */
extern unsigned int  global_tulsd;                        /* Seconds */
extern unsigned char global_tultd[MAXTAGLINE+2];          /* Tagline */

/* Daytop leader up (max) */
extern unsigned char global_tumud[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tumgd[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tumbd;                        /* Bytes */
extern unsigned int  global_tumfd;                        /* Files */
extern unsigned int  global_tumsd;                        /* Seconds */
extern unsigned char global_tumtd[MAXTAGLINE+2];          /* Tagline */

/* Daytop-down yesterday */
extern unsigned char global_tdlud[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tdlgd[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tdlbd;                        /* Bytes */
extern unsigned int  global_tdlfd;                        /* Files */
extern unsigned int  global_tdlsd;                        /* Seconds */
extern unsigned char global_tdltd[MAXTAGLINE+2];          /* Tagline */

/* Daytop leader down (max) */
extern unsigned char global_tdmud[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tdmgd[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tdmbd;                        /* Bytes */
extern unsigned int  global_tdmfd;                        /* Files */
extern unsigned int  global_tdmsd;                        /* Seconds */
extern unsigned char global_tdmtd[MAXTAGLINE+2];          /* Tagline */

/* Total amounts up for today */
extern lion64u_t     global_ubd;                          /* Bytes */
extern unsigned int  global_ufd;                          /* Files */
extern unsigned int  global_usd;                          /* Seconds */

/* Total amounts down for today */
extern lion64u_t     global_dbd;                          /* Bytes */
extern unsigned int  global_dfd;                          /* Files */
extern unsigned int  global_dsd;                          /* Seconds */

/* Weektop-up */
extern unsigned char global_tuuw[NUM_TOP][MAXUSERNAME+2]; /* Username */
extern unsigned char global_tugw[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_tubw[NUM_TOP];                /* Bytes */
extern unsigned int  global_tufw[NUM_TOP];                /* Files */
extern unsigned int  global_tusw[NUM_TOP];                /* Seconds */
extern unsigned char global_tutw[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Weektop-down */
extern unsigned char global_tduw[NUM_TOP][MAXUSERNAME+2]; /* Username */
extern unsigned char global_tdgw[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_tdbw[NUM_TOP];                /* Bytes */
extern unsigned int  global_tdfw[NUM_TOP];                /* Files */
extern unsigned int  global_tdsw[NUM_TOP];                /* Seconds */
extern unsigned char global_tdtw[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Weektop-up last-week */
extern unsigned char global_tuluw[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tulgw[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tulbw;                        /* Bytes */
extern unsigned int  global_tulfw;                        /* Files */
extern unsigned int  global_tulsw;                        /* Seconds */
extern unsigned char global_tultw[MAXTAGLINE+2];          /* Tagline */

/* Weektop-down last-week */
extern unsigned char global_tdluw[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tdlgw[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tdlbw;                        /* Bytes */
extern unsigned int  global_tdlfw;                        /* Files */
extern unsigned int  global_tdlsw;                        /* Seconds */
extern unsigned char global_tdltw[MAXTAGLINE+2];          /* Tagline */

/* Weektop leader up (max) */
extern unsigned char global_tumuw[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tumgw[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tumbw;                        /* Bytes */
extern unsigned int  global_tumfw;                        /* Files */
extern unsigned int  global_tumsw;                        /* Seconds */
extern unsigned char global_tumtw[MAXTAGLINE+2];          /* Tagline */

/* Weektop leader down (max) */
extern unsigned char global_tdmuw[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tdmgw[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tdmbw;                        /* Bytes */
extern unsigned int  global_tdmfw;                        /* Files */
extern unsigned int  global_tdmsw;                        /* Seconds */
extern unsigned char global_tdmtw[MAXTAGLINE+2];          /* Tagline */

/* Total amounts up for week */
extern lion64u_t     global_ubw;                          /* Bytes */
extern unsigned int  global_ufw;                          /* Files */
extern unsigned int  global_usw;                          /* Seconds */

/* Total amounts down for week */
extern lion64u_t     global_dbw;                          /* Bytes */
extern unsigned int  global_dfw;                          /* Files */
extern unsigned int  global_dsw;                          /* Seconds */

/* Monthtop-up */
extern unsigned char global_tuum[NUM_TOP][MAXUSERNAME+2]; /* Username */
extern unsigned char global_tugm[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_tubm[NUM_TOP];                /* Bytes */
extern unsigned int  global_tufm[NUM_TOP];                /* Files */
extern unsigned int  global_tusm[NUM_TOP];                /* Seconds */
extern unsigned char global_tutm[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Monthtop-down */
extern unsigned char global_tdum[NUM_TOP][MAXUSERNAME+2]; /* Username */
extern unsigned char global_tdgm[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_tdbm[NUM_TOP];                /* Bytes */
extern unsigned int  global_tdfm[NUM_TOP];                /* Files */
extern unsigned int  global_tdsm[NUM_TOP];                /* Seconds */
extern unsigned char global_tdtm[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Monthtop-up last-month */
extern unsigned char global_tulum[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tulgm[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tulbm;                        /* Bytes */
extern unsigned int  global_tulfm;                        /* Files */
extern unsigned int  global_tulsm;                        /* Seconds */
extern unsigned char global_tultm[MAXTAGLINE+2];          /* Tagline */

/* Monthtop-down last-month */
extern unsigned char global_tdlum[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tdlgm[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tdlbm;                        /* Bytes */
extern unsigned int  global_tdlfm;                        /* Files */
extern unsigned int  global_tdlsm;                        /* Seconds */
extern unsigned char global_tdltm[MAXTAGLINE+2];          /* Tagline */

/* Monthtop leader up (max) */
extern unsigned char global_tumum[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tumgm[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tumbm;                        /* Bytes */
extern unsigned int  global_tumfm;                        /* Files */
extern unsigned int  global_tumsm;                        /* Seconds */
extern unsigned char global_tumtm[MAXTAGLINE+2];          /* Tagline */

/* Monthtop leader down (max) */
extern unsigned char global_tdmum[MAXUSERNAME+2];         /* Username */
extern unsigned char global_tdmgm[MAXUSERNAME+2];         /* Group */
extern lion64u_t     global_tdmbm;                        /* Bytes */
extern unsigned int  global_tdmfm;                        /* Files */
extern unsigned int  global_tdmsm;                        /* Seconds */
extern unsigned char global_tdmtm[MAXTAGLINE+2];          /* Tagline */

/* Total amounts up for month */
extern lion64u_t     global_ubm;                          /* Bytes */
extern unsigned int  global_ufm;                          /* Files */
extern unsigned int  global_usm;                          /* Seconds */

/* Total amounts down for month */
extern lion64u_t     global_dbm;                          /* Bytes */
extern unsigned int  global_dfm;                          /* Files */
extern unsigned int  global_dsm;                          /* Seconds */

/* Alltimetop-up */
extern unsigned char global_tuu[NUM_TOP][MAXUSERNAME+2]; /* Username */
extern unsigned char global_tug[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_tub[NUM_TOP];                /* Bytes */
extern unsigned int  global_tuf[NUM_TOP];                /* Files */
extern unsigned int  global_tus[NUM_TOP];                /* Seconds */
extern unsigned char global_tut[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Alltimetop-down */
extern unsigned char global_tdu[NUM_TOP][MAXUSERNAME+2]; /* Username */
extern unsigned char global_tdg[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_tdb[NUM_TOP];                /* Bytes */
extern unsigned int  global_tdf[NUM_TOP];                /* Files */
extern unsigned int  global_tds[NUM_TOP];                /* Seconds */
extern unsigned char global_tdt[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Total amounts up for Alltime */
extern lion64u_t     global_ub;                          /* Bytes */
extern unsigned int  global_uf;                          /* Files */
extern unsigned int  global_us;                          /* Seconds */

/* Total amounts down for Alltime */
extern lion64u_t     global_db;                          /* Bytes */
extern unsigned int  global_df;                          /* Files */
extern unsigned int  global_ds;                          /* Seconds */






/* GROUPS */

/* Group Top Up */
extern unsigned char global_gugd[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern unsigned int  global_gumd[NUM_TOP];                /* Members */
extern unsigned int  global_guad[NUM_TOP];                /* Active */
extern lion64u_t     global_gubd[NUM_TOP];                /* Bytes */
extern unsigned int  global_gufd[NUM_TOP];                /* Files */
extern unsigned char global_gutd[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Group Top Down */
extern unsigned char global_gdgd[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern unsigned int  global_gdmd[NUM_TOP];                /* Members */
extern unsigned int  global_gdad[NUM_TOP];                /* Active */
extern lion64u_t     global_gdbd[NUM_TOP];                /* Bytes */
extern unsigned int  global_gdfd[NUM_TOP];                /* Files */
extern unsigned char global_gdtd[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Group Top Up Yesterday */
extern unsigned char global_gulgd[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gulmd;                        /* Members */
extern unsigned int  global_gulad;                        /* Active */
extern lion64u_t     global_gulbd;                        /* Bytes */
extern unsigned int  global_gulfd;                        /* Files */
extern unsigned char global_gultd[MAXTAGLINE+2];          /* Tagline */

/* Group Top Down Yesterday */
extern unsigned char global_gdlgd[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gdlmd;                        /* Members */
extern unsigned int  global_gdlad;                        /* Active */
extern lion64u_t     global_gdlbd;                        /* Bytes */
extern unsigned int  global_gdlfd;                        /* Files */
extern unsigned char global_gdltd[MAXTAGLINE+2];          /* Tagline */

/* Group Top Up Leader Day */
extern unsigned char global_gumgd[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gummd;                        /* Members */
extern unsigned int  global_gumad;                        /* Active */
extern lion64u_t     global_gumbd;                        /* Bytes */
extern unsigned int  global_gumfd;                        /* Files */
extern unsigned char global_gumtd[MAXTAGLINE+2];          /* Tagline */

/* Group Top Down Leader Day */
extern unsigned char global_gdmgd[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gdmmd;                        /* Members */
extern unsigned int  global_gdmad;                        /* Active */
extern lion64u_t     global_gdmbd;                        /* Bytes */
extern unsigned int  global_gdmfd;                        /* Files */
extern unsigned char global_gdmtd[MAXTAGLINE+2];          /* Tagline */

/* Group Top Up Week*/
extern unsigned char global_gugw[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern unsigned int  global_gumw[NUM_TOP];                /* Members */
extern unsigned int  global_guaw[NUM_TOP];                /* Active */
extern lion64u_t     global_gubw[NUM_TOP];                /* Bytes */
extern unsigned int  global_gufw[NUM_TOP];                /* Files */
extern unsigned char global_gutw[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Group Top Down Week*/
extern unsigned char global_gdgw[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern unsigned int  global_gdmw[NUM_TOP];                /* Members */
extern unsigned int  global_gdaw[NUM_TOP];                /* Active */
extern lion64u_t     global_gdbw[NUM_TOP];                /* Bytes */
extern unsigned int  global_gdfw[NUM_TOP];                /* Files */
extern unsigned char global_gdtw[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Group Top Up LastWeek */
extern unsigned char global_gulgw[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gulmw;                        /* Members */
extern unsigned int  global_gulaw;                        /* Active */
extern lion64u_t     global_gulbw;                        /* Bytes */
extern unsigned int  global_gulfw;                        /* Files */
extern unsigned char global_gultw[MAXTAGLINE+2];          /* Tagline */

/* Group Top Down LastWeek */
extern unsigned char global_gdlgw[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gdlmw;                        /* Members */
extern unsigned int  global_gdlaw;                        /* Active */
extern lion64u_t     global_gdlbw;                        /* Bytes */
extern unsigned int  global_gdlfw;                        /* Files */
extern unsigned char global_gdltw[MAXTAGLINE+2];          /* Tagline */

/* Group Top Up Leader week */
extern unsigned char global_gumgw[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gummw;                        /* Members */
extern unsigned int  global_gumaw;                        /* Active */
extern lion64u_t     global_gumbw;                        /* Bytes */
extern unsigned int  global_gumfw;                        /* Files */
extern unsigned char global_gumtw[MAXTAGLINE+2];          /* Tagline */

/* Group Top Down Leader Week */
extern unsigned char global_gdmgw[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gdmmw;                        /* Members */
extern unsigned int  global_gdmaw;                        /* Active */
extern lion64u_t     global_gdmbw;                        /* Bytes */
extern unsigned int  global_gdmfw;                        /* Files */
extern unsigned char global_gdmtw[MAXTAGLINE+2];          /* Tagline */

/* Group Top Up Month */
extern unsigned char global_gugm[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern unsigned int  global_gumm[NUM_TOP];                /* Members */
extern unsigned int  global_guam[NUM_TOP];                /* Active */
extern lion64u_t     global_gubm[NUM_TOP];                /* Bytes */
extern unsigned int  global_gufm[NUM_TOP];                /* Files */
extern unsigned char global_gutm[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Group Top Down Month*/
extern unsigned char global_gdgm[NUM_TOP][MAXUSERNAME+2]; /* Group */
extern unsigned int  global_gdmm[NUM_TOP];                /* Members */
extern unsigned int  global_gdam[NUM_TOP];                /* Active */
extern lion64u_t     global_gdbm[NUM_TOP];                /* Bytes */
extern unsigned int  global_gdfm[NUM_TOP];                /* Files */
extern unsigned char global_gdtm[NUM_TOP][MAXTAGLINE+2];  /* Tagline */

/* Group Top Up LastMonth */
extern unsigned char global_gulgm[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gulmm;                        /* Members */
extern unsigned int  global_gulam;                        /* Active */
extern lion64u_t     global_gulbm;                        /* Bytes */
extern unsigned int  global_gulfm;                        /* Files */
extern unsigned char global_gultm[MAXTAGLINE+2];          /* Tagline */

/* Group Top Down LastMonth */
extern unsigned char global_gdlgm[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gdlmm;                        /* Members */
extern unsigned int  global_gdlam;                        /* Active */
extern lion64u_t     global_gdlbm;                        /* Bytes */
extern unsigned int  global_gdlfm;                        /* Files */
extern unsigned char global_gdltm[MAXTAGLINE+2];          /* Tagline */

/* Group Top Up Leader Month */
extern unsigned char global_gumgm[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gummm;                        /* Members */
extern unsigned int  global_gumam;                        /* Active */
extern lion64u_t     global_gumbm;                        /* Bytes */
extern unsigned int  global_gumfm;                        /* Files */
extern unsigned char global_gumtm[MAXTAGLINE+2];          /* Tagline */

/* Group Top Down Leader Month */
extern unsigned char global_gdmgm[MAXUSERNAME+2];         /* Group */
extern unsigned int  global_gdmmm;                        /* Members */
extern unsigned int  global_gdmam;                        /* Active */
extern lion64u_t     global_gdmbm;                        /* Bytes */
extern unsigned int  global_gdmfm;                        /* Files */
extern unsigned char global_gdmtm[MAXTAGLINE+2];          /* Tagline */

/* Group Top Up Alltime */
extern unsigned char global_gug[NUM_TOP][MAXUSERNAME+2];  /* Group */
extern unsigned int  global_gum[NUM_TOP];                 /* Members */
extern unsigned int  global_gua[NUM_TOP];                 /* Active */
extern lion64u_t     global_gub[NUM_TOP];                 /* Bytes */
extern unsigned int  global_guf[NUM_TOP];                 /* Files */
extern unsigned char global_gut[NUM_TOP][MAXTAGLINE+2];   /* Tagline */

/* Group Top Down Alltime*/
extern unsigned char global_gdg[NUM_TOP][MAXUSERNAME+2];  /* Group */
extern unsigned int  global_gdm[NUM_TOP];                 /* Members */
extern unsigned int  global_gda[NUM_TOP];                 /* Active */
extern lion64u_t     global_gdb[NUM_TOP];                 /* Bytes */
extern unsigned int  global_gdf[NUM_TOP];                 /* Files */
extern unsigned char global_gdt[NUM_TOP][MAXTAGLINE+2];   /* Tagline */



/* MSG WALL */

extern unsigned char global_mwu[NUM_WALL][MAXUSERNAME+2]; /* Username */
extern time_t        global_mwd[NUM_WALL];                /* Date + Time */
extern unsigned char *global_mwm[NUM_WALL];               /* Message */


/* NUKE */

extern unsigned char global_nkn[NUM_NUKE][MAXUSERNAME+2]; /* Nuker Name */
extern unsigned char global_nku[NUM_NUKE][MAXUSERNAME+2]; /* Username */
extern lion64u_t global_nkb[NUM_NUKE];           /* Bytes */
extern unsigned int  global_nkf[NUM_NUKE];                /* Files */
extern unsigned char *global_nkd[NUM_NUKE];               /* Directory */
extern unsigned char *global_nkr[NUM_NUKE];               /* Reason */


/* LAST */

extern unsigned char global_ltu[NUM_LAST][MAXUSERNAME+2]; /* Username */
extern unsigned char *global_ltg[NUM_LAST];               /* Group */
extern time_t        global_lti[NUM_LAST];                /* Date + Time In */
extern time_t        global_lto[NUM_LAST];                /* Date + Time Out */


/* Yesterday's TOP upload DAY */
extern unsigned char global_yuud[MAXUSERNAME+2]; /* Username */
extern unsigned char global_yugd[MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_yubd;                /* Bytes */
extern unsigned int  global_yufd;                /* Files */
extern unsigned int  global_yusd;                /* Seconds */
extern unsigned char global_yutd[MAXTAGLINE+2];  /* Tagline */

/* Last-weeks TOP upload DAY */
extern unsigned char global_yuuw[MAXUSERNAME+2]; /* Username */
extern unsigned char global_yugw[MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_yubw;                /* Bytes */
extern unsigned int  global_yufw;                /* Files */
extern unsigned int  global_yusw;                /* Seconds */
extern unsigned char global_yutw[MAXTAGLINE+2];  /* Tagline */

/* Last-months TOP upload DAY */
extern unsigned char global_yuum[MAXUSERNAME+2]; /* Username */
extern unsigned char global_yugm[MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_yubm;                /* Bytes */
extern unsigned int  global_yufm;                /* Files */
extern unsigned int  global_yusm;                /* Seconds */
extern unsigned char global_yutm[MAXTAGLINE+2];  /* Tagline */

/* Yesterday's TOP download DAY */
extern unsigned char global_ydud[MAXUSERNAME+2]; /* Username */
extern unsigned char global_ydgd[MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_ydbd;                /* Bytes */
extern unsigned int  global_ydfd;                /* Files */
extern unsigned int  global_ydsd;                /* Seconds */
extern unsigned char global_ydtd[MAXTAGLINE+2];  /* Tagline */

/* Last-weeks TOP download DAY */
extern unsigned char global_yduw[MAXUSERNAME+2]; /* Username */
extern unsigned char global_ydgw[MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_ydbw;                /* Bytes */
extern unsigned int  global_ydfw;                /* Files */
extern unsigned int  global_ydsw;                /* Seconds */
extern unsigned char global_ydtw[MAXTAGLINE+2];  /* Tagline */

/* Last-months TOP download DAY */
extern unsigned char global_ydum[MAXUSERNAME+2]; /* Username */
extern unsigned char global_ydgm[MAXUSERNAME+2]; /* Group */
extern lion64u_t     global_ydbm;                /* Bytes */
extern unsigned int  global_ydfm;                /* Files */
extern unsigned int  global_ydsm;                /* Seconds */
extern unsigned char global_ydtm[MAXTAGLINE+2];  /* Tagline */

/* Latest uploads */

extern unsigned char global_newu[NUM_NEW][MAXUSERNAME+2]; /* Username */
extern time_t        global_newd[NUM_NEW];                /* Date + Time */
extern unsigned char *global_newn[NUM_NEW];               /* Dirname */

/* Additional */

extern int global_resync_day;
extern int global_resync_week;
extern int global_resync_month;
extern int global_resync_total;
extern int global_hourofday;
extern int global_dayofweek;
extern int global_dayofmonth;
extern int global_daysinmonth;
extern lion64u_t global_total_bytes_up;
extern lion64u_t global_total_bytes_down;




extern int server_rehash;


// WTF is this doing here
struct global_struct {
	char *matchip;
	struct global_struct *next;
};

extern struct global_struct *global_head;



/* Functions */

void global_update_minute(void);
void global_update_10minutes(void);
void global_update_30minutes(void);
void global_update_hour(void);
void global_update_12hours(void);
void global_update_daily(void);
void global_update_weekly(void);
void global_update_monthly(void);
void global_update_all(void);
void global_readwall(void);

void global_set_default(void);




#endif
