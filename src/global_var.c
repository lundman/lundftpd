/*
 *
 * This defines global variables which are updated independently
 * at subjective time intervals
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif


#ifndef WIN32
#include <unistd.h>
#endif
#include <stdio.h>
#include <time.h>
#include <sys/types.h>

#include "global.h"
#include "login.h"
#include "global.h"





/* Daytop-up */
unsigned char global_tuud[NUM_TOP][MAXUSERNAME+2]; /* Username */
unsigned char global_tugd[NUM_TOP][MAXUSERNAME+2];               /* Group */
lion64u_t global_tubd[NUM_TOP];           /* Bytes */
unsigned int  global_tufd[NUM_TOP];                /* Files */
unsigned int  global_tusd[NUM_TOP];                /* Seconds */
unsigned char global_tutd[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Daytop-down */
unsigned char global_tdud[NUM_TOP][MAXUSERNAME+2]; /* Username */
unsigned char global_tdgd[NUM_TOP][MAXUSERNAME+2];               /* Group */
lion64u_t global_tdbd[NUM_TOP];           /* Bytes */
unsigned int  global_tdfd[NUM_TOP];                /* Files */
unsigned int  global_tdsd[NUM_TOP];                /* Seconds */
unsigned char global_tdtd[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Daytop-up yesterday */
unsigned char global_tulud[MAXUSERNAME+2];         /* Username */
unsigned char global_tulgd[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tulbd;                   /* Bytes */
unsigned int  global_tulfd;                        /* Files */
unsigned int  global_tulsd;                        /* Seconds */
unsigned char global_tultd[MAXTAGLINE+2];         /* Tagline */

/* Daytop leader up (max) */
unsigned char global_tumud[MAXUSERNAME+2];         /* Username */
unsigned char global_tumgd[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tumbd;                   /* Bytes */
unsigned int  global_tumfd;                        /* Files */
unsigned int  global_tumsd;                        /* Seconds */
unsigned char global_tumtd[MAXTAGLINE+2];         /* Tagline */

/* Daytop-down yesterday */
unsigned char global_tdlud[MAXUSERNAME+2];         /* Username */
unsigned char global_tdlgd[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tdlbd;                   /* Bytes */
unsigned int  global_tdlfd;                        /* Files */
unsigned int  global_tdlsd;                        /* Seconds */
unsigned char global_tdltd[MAXTAGLINE+2];         /* Tagline */

/* Daytop leader down (max) */
unsigned char global_tdmud[MAXUSERNAME+2];         /* Username */
unsigned char global_tdmgd[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tdmbd;                   /* Bytes */
unsigned int  global_tdmfd;                        /* Files */
unsigned int  global_tdmsd;                        /* Seconds */
unsigned char global_tdmtd[MAXTAGLINE+2];         /* Tagline */

/* Total amounts up for today */
lion64u_t global_ubd;                            /* Bytes */
unsigned int  global_ufd;                                 /* Files */
unsigned int  global_usd;                                 /* Seconds */

/* Total amounts down for today */
lion64u_t global_dbd;                            /* Bytes */
unsigned int  global_dfd;                                 /* Files */
unsigned int  global_dsd;                                 /* Seconds */

/* Weektop-up */
unsigned char global_tuuw[NUM_TOP][MAXUSERNAME+2]; /* Username */
unsigned char global_tugw[NUM_TOP][MAXUSERNAME+2];               /* Group */
lion64u_t global_tubw[NUM_TOP];           /* Bytes */
unsigned int  global_tufw[NUM_TOP];                /* Files */
unsigned int  global_tusw[NUM_TOP];                /* Seconds */
unsigned char global_tutw[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Weektop-down */
unsigned char global_tduw[NUM_TOP][MAXUSERNAME+2]; /* Username */
unsigned char global_tdgw[NUM_TOP][MAXUSERNAME+2];               /* Group */
lion64u_t global_tdbw[NUM_TOP];           /* Bytes */
unsigned int  global_tdfw[NUM_TOP];                /* Files */
unsigned int  global_tdsw[NUM_TOP];                /* Seconds */
unsigned char global_tdtw[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Weektop-up last-week */
unsigned char global_tuluw[MAXUSERNAME+2];         /* Username */
unsigned char global_tulgw[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tulbw;                   /* Bytes */
unsigned int  global_tulfw;                        /* Files */
unsigned int  global_tulsw;                        /* Seconds */
unsigned char global_tultw[MAXTAGLINE+2];         /* Tagline */

/* Weektop-down last-week */
unsigned char global_tdluw[MAXUSERNAME+2];         /* Username */
unsigned char global_tdlgw[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tdlbw;                   /* Bytes */
unsigned int  global_tdlfw;                        /* Files */
unsigned int  global_tdlsw;                        /* Seconds */
unsigned char global_tdltw[MAXTAGLINE+2];         /* Tagline */

/* Weektop leader up (max) */
unsigned char global_tumuw[MAXUSERNAME+2];         /* Username */
unsigned char global_tumgw[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tumbw;                   /* Bytes */
unsigned int  global_tumfw;                        /* Files */
unsigned int  global_tumsw;                        /* Seconds */
unsigned char global_tumtw[MAXTAGLINE+2];         /* Tagline */

/* Weektop leader down (max) */
unsigned char global_tdmuw[MAXUSERNAME+2];         /* Username */
unsigned char global_tdmgw[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tdmbw;                   /* Bytes */
unsigned int  global_tdmfw;                        /* Files */
unsigned int  global_tdmsw;                        /* Seconds */
unsigned char global_tdmtw[MAXTAGLINE+2];         /* Tagline */

/* Total amounts up for week */
lion64u_t global_ubw;                            /* Bytes */
unsigned int  global_ufw;                                 /* Files */
unsigned int  global_usw;                                 /* Seconds */

/* Total amounts down for week */
lion64u_t global_dbw;                            /* Bytes */
unsigned int  global_dfw;                                 /* Files */
unsigned int  global_dsw;                                 /* Seconds */

/* Monthtop-up */
unsigned char global_tuum[NUM_TOP][MAXUSERNAME+2]; /* Username */
unsigned char global_tugm[NUM_TOP][MAXUSERNAME+2];               /* Group */
lion64u_t global_tubm[NUM_TOP];           /* Bytes */
unsigned int  global_tufm[NUM_TOP];                /* Files */
unsigned int  global_tusm[NUM_TOP];                /* Seconds */
unsigned char global_tutm[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Monthtop-down */
unsigned char global_tdum[NUM_TOP][MAXUSERNAME+2]; /* Username */
unsigned char global_tdgm[NUM_TOP][MAXUSERNAME+2];               /* Group */
lion64u_t global_tdbm[NUM_TOP];           /* Bytes */
unsigned int  global_tdfm[NUM_TOP];                /* Files */
unsigned int  global_tdsm[NUM_TOP];                /* Seconds */
unsigned char global_tdtm[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Month-up last-month */
unsigned char global_tulum[MAXUSERNAME+2];         /* Username */
unsigned char global_tulgm[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tulbm;                   /* Bytes */
unsigned int  global_tulfm;                        /* Files */
unsigned int  global_tulsm;                        /* Seconds */
unsigned char global_tultm[MAXTAGLINE+2];         /* Tagline */

/* Monthtop-down last-month */
unsigned char global_tdlum[MAXUSERNAME+2];         /* Username */
unsigned char global_tdlgm[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tdlbm;                   /* Bytes */
unsigned int  global_tdlfm;                        /* Files */
unsigned int  global_tdlsm;                        /* Seconds */
unsigned char global_tdltm[MAXTAGLINE+2];         /* Tagline */

/* Monthtop leader up (max) */
unsigned char global_tumum[MAXUSERNAME+2];         /* Username */
unsigned char global_tumgm[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tumbm;                   /* Bytes */
unsigned int  global_tumfm;                        /* Files */
unsigned int  global_tumsm;                        /* Seconds */
unsigned char global_tumtm[MAXTAGLINE+2];         /* Tagline */

/* Monthtop leader down (max) */
unsigned char global_tdmum[MAXUSERNAME+2];         /* Username */
unsigned char global_tdmgm[MAXUSERNAME+2];                       /* Group */
lion64u_t global_tdmbm;                   /* Bytes */
unsigned int  global_tdmfm;                        /* Files */
unsigned int  global_tdmsm;                        /* Seconds */
unsigned char global_tdmtm[MAXTAGLINE+2];         /* Tagline */

/* Total amounts up for month */
lion64u_t global_ubm;                            /* Bytes */
unsigned int  global_ufm;                                 /* Files */
unsigned int  global_usm;                                 /* Seconds */

/* Total amounts down for month */
lion64u_t global_dbm;                            /* Bytes */
unsigned int  global_dfm;                                 /* Files */
unsigned int  global_dsm;                                 /* Seconds */

/* Alltimetop-up */
unsigned char global_tuu[NUM_TOP][MAXUSERNAME+2]; /* Username */
unsigned char global_tug[NUM_TOP][MAXUSERNAME+2];               /* Group */
lion64u_t global_tub[NUM_TOP];           /* Bytes */
unsigned int  global_tuf[NUM_TOP];                /* Files */
unsigned int  global_tus[NUM_TOP];                /* Seconds */
unsigned char global_tut[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Alltimetop-down */
unsigned char global_tdu[NUM_TOP][MAXUSERNAME+2]; /* Username */
unsigned char global_tdg[NUM_TOP][MAXUSERNAME+2];               /* Group */
lion64u_t global_tdb[NUM_TOP];           /* Bytes */
unsigned int  global_tdf[NUM_TOP];                /* Files */
unsigned int  global_tds[NUM_TOP];                /* Seconds */
unsigned char global_tdt[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Total amounts up for Alltime */
lion64u_t global_ub;                             /* Bytes */
unsigned int  global_uf;                                  /* Files */
unsigned int  global_us;                                  /* Seconds */

/* Total amounts down for Alltime */
lion64u_t global_db;                             /* Bytes */
unsigned int  global_df;                                  /* Files */
unsigned int  global_ds;                                  /* Seconds */






/* GROUPS */

/* Group Top Up */
unsigned char global_gugd[NUM_TOP][MAXUSERNAME+2];               /* Group */
unsigned int  global_gumd[NUM_TOP];                /* Members */
unsigned int  global_guad[NUM_TOP];                /* Active */
lion64u_t global_gubd[NUM_TOP];           /* Bytes */
unsigned int  global_gufd[NUM_TOP];                /* Files */
unsigned char global_gutd[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Group Top Down */
unsigned char global_gdgd[NUM_TOP][MAXUSERNAME+2];               /* Group */
unsigned int  global_gdmd[NUM_TOP];                /* Members */
unsigned int  global_gdad[NUM_TOP];                /* Active */
lion64u_t global_gdbd[NUM_TOP];           /* Bytes */
unsigned int  global_gdfd[NUM_TOP];                /* Files */
unsigned char global_gdtd[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Group Top Up Yesterday */
unsigned char global_gulgd[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gulmd;                        /* Members */
unsigned int  global_gulad;                        /* Active */
lion64u_t global_gulbd;                   /* Bytes */
unsigned int  global_gulfd;                        /* Files */
unsigned char global_gultd[MAXTAGLINE+2];         /* Tagline */

/* Group Top Down Yesterday */
unsigned char global_gdlgd[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gdlmd;                        /* Members */
unsigned int  global_gdlad;                        /* Active */
lion64u_t global_gdlbd;                   /* Bytes */
unsigned int  global_gdlfd;                        /* Files */
unsigned char global_gdltd[MAXTAGLINE+2];         /* Tagline */

/* Group Top Up Leader Day */
unsigned char global_gumgd[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gummd;                        /* Members */
unsigned int  global_gumad;                        /* Active */
lion64u_t global_gumbd;                   /* Bytes */
unsigned int  global_gumfd;                        /* Files */
unsigned char global_gumtd[MAXTAGLINE+2];         /* Tagline */

/* Group Top Down Leader Day */
unsigned char global_gdmgd[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gdmmd;                        /* Members */
unsigned int  global_gdmad;                        /* Active */
lion64u_t global_gdmbd;                   /* Bytes */
unsigned int  global_gdmfd;                        /* Files */
unsigned char global_gdmtd[MAXTAGLINE+2];         /* Tagline */

/* Group Top Up Week*/
unsigned char global_gugw[NUM_TOP][MAXUSERNAME+2];               /* Group */
unsigned int  global_gumw[NUM_TOP];                /* Members */
unsigned int  global_guaw[NUM_TOP];                /* Active */
lion64u_t global_gubw[NUM_TOP];           /* Bytes */
unsigned int  global_gufw[NUM_TOP];                /* Files */
unsigned char global_gutw[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Group Top Down Week*/
unsigned char global_gdgw[NUM_TOP][MAXUSERNAME+2];               /* Group */
unsigned int  global_gdmw[NUM_TOP];                /* Members */
unsigned int  global_gdaw[NUM_TOP];                /* Active */
lion64u_t global_gdbw[NUM_TOP];           /* Bytes */
unsigned int  global_gdfw[NUM_TOP];                /* Files */
unsigned char global_gdtw[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Group Top Up LastWeek */
unsigned char global_gulgw[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gulmw;                        /* Members */
unsigned int  global_gulaw;                        /* Active */
lion64u_t global_gulbw;                   /* Bytes */
unsigned int  global_gulfw;                        /* Files */
unsigned char global_gultw[MAXTAGLINE+2];         /* Tagline */

/* Group Top Down LastWeek */
unsigned char global_gdlgw[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gdlmw;                        /* Members */
unsigned int  global_gdlaw;                        /* Active */
lion64u_t global_gdlbw;                   /* Bytes */
unsigned int  global_gdlfw;                        /* Files */
unsigned char global_gdltw[MAXTAGLINE+2];         /* Tagline */

/* Group Top Up Leader week */
unsigned char global_gumgw[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gummw;                        /* Members */
unsigned int  global_gumaw;                        /* Active */
lion64u_t global_gumbw;                   /* Bytes */
unsigned int  global_gumfw;                        /* Files */
unsigned char global_gumtw[MAXTAGLINE+2];         /* Tagline */

/* Group Top Down Leader Week */
unsigned char global_gdmgw[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gdmmw;                        /* Members */
unsigned int  global_gdmaw;                        /* Active */
lion64u_t global_gdmbw;                   /* Bytes */
unsigned int  global_gdmfw;                        /* Files */
unsigned char global_gdmtw[MAXTAGLINE+2];         /* Tagline */

/* Group Top Up Month */
unsigned char global_gugm[NUM_TOP][MAXUSERNAME+2];               /* Group */
unsigned int  global_gumm[NUM_TOP];                /* Members */
unsigned int  global_guam[NUM_TOP];                /* Active */
lion64u_t global_gubm[NUM_TOP];           /* Bytes */
unsigned int  global_gufm[NUM_TOP];                /* Files */
unsigned char global_gutm[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Group Top Down Month*/
unsigned char global_gdgm[NUM_TOP][MAXUSERNAME+2];               /* Group */
unsigned int  global_gdmm[NUM_TOP];                /* Members */
unsigned int  global_gdam[NUM_TOP];                /* Active */
lion64u_t global_gdbm[NUM_TOP];           /* Bytes */
unsigned int  global_gdfm[NUM_TOP];                /* Files */
unsigned char global_gdtm[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Group Top Up LastMonth */
unsigned char global_gulgm[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gulmm;                        /* Members */
unsigned int  global_gulam;                        /* Active */
lion64u_t global_gulbm;                   /* Bytes */
unsigned int  global_gulfm;                        /* Files */
unsigned char global_gultm[MAXTAGLINE+2];         /* Tagline */

/* Group Top Down LastMonth */
unsigned char global_gdlgm[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gdlmm;                        /* Members */
unsigned int  global_gdlam;                        /* Active */
lion64u_t global_gdlbm;                   /* Bytes */
unsigned int  global_gdlfm;                        /* Files */
unsigned char global_gdltm[MAXTAGLINE+2];         /* Tagline */

/* Group Top Up Leader Month */
unsigned char global_gumgm[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gummm;                        /* Members */
unsigned int  global_gumam;                        /* Active */
lion64u_t global_gumbm;                   /* Bytes */
unsigned int  global_gumfm;                        /* Files */
unsigned char global_gumtm[MAXTAGLINE+2];         /* Tagline */

/* Group Top Down Leader Month */
unsigned char global_gdmgm[MAXUSERNAME+2];                       /* Group */
unsigned int  global_gdmmm;                        /* Members */
unsigned int  global_gdmam;                        /* Active */
lion64u_t global_gdmbm;                   /* Bytes */
unsigned int  global_gdmfm;                        /* Files */
unsigned char global_gdmtm[MAXTAGLINE+2];         /* Tagline */

/* Group Top Up Alltime */
unsigned char global_gug[NUM_TOP][MAXUSERNAME+2];               /* Group */
unsigned int  global_gum[NUM_TOP];                /* Members */
unsigned int  global_gua[NUM_TOP];                /* Active */
lion64u_t global_gub[NUM_TOP];           /* Bytes */
unsigned int  global_guf[NUM_TOP];                /* Files */
unsigned char global_gut[NUM_TOP][MAXTAGLINE+2]; /* Tagline */

/* Group Top Down Alltime*/
unsigned char global_gdg[NUM_TOP][MAXUSERNAME+2];               /* Group */
unsigned int  global_gdm[NUM_TOP];                /* Members */
unsigned int  global_gda[NUM_TOP];                /* Active */
lion64u_t global_gdb[NUM_TOP];           /* Bytes */
unsigned int  global_gdf[NUM_TOP];                /* Files */
unsigned char global_gdt[NUM_TOP][MAXTAGLINE+2]; /* Tagline */



/* MSG WALL */

unsigned char global_mwu[NUM_WALL][MAXUSERNAME+2]; /* Username */
time_t        global_mwd[NUM_WALL];                /* Date + Time */
unsigned char *global_mwm[NUM_WALL];               /* Message */


/* NUKE */

unsigned char global_nkn[NUM_NUKE][MAXUSERNAME+2]; /* Nuker Name */
unsigned char global_nku[NUM_NUKE][MAXUSERNAME+2]; /* Username */
lion64u_t global_nkb[NUM_NUKE];           /* Bytes */
unsigned int  global_nkf[NUM_NUKE];                /* Files */
unsigned char *global_nkd[NUM_NUKE];               /* Directory */
unsigned char *global_nkr[NUM_NUKE];               /* Reason */


/* LAST */

unsigned char global_ltu[NUM_LAST][MAXUSERNAME+2]; /* Username */
unsigned char *global_ltg[NUM_LAST];               /* Group */
time_t        global_lti[NUM_LAST];                /* Date + Time In */
time_t        global_lto[NUM_LAST];                /* Date + Time Out */

/* NEW, latest uploads */

unsigned char global_newu[NUM_NEW][MAXUSERNAME+2]; /* Username */
time_t        global_newd[NUM_NEW];                /* Date + Time */
unsigned char *global_newn[NUM_NEW];               /* Dirname */

/* OTHERS */

/*
mxl   Max # of allowed logins at the same time (all users)
mxu   Max # of allowed upload sessions (all users)
mxd   Max # of allowed download sessions (all users)
cul   Current logins (all users)
cuu   Current upload sessions (all users)
cud   Current download sessions (all users)
stu   Users on the site overall and not current (user counter)
stg   Groups on the site overall and not current (group counter)
stl   Login times all users together (visit counter)

fhd0  free hd-space on device&path #0
uhd0  used hd-space on device&path (recursive) #0
   ^- 0 - 9, dev&path specified in src or cfg-file.

*/

/* Yesterday's TOP upload DAY */
unsigned char global_yuud[MAXUSERNAME+2]; /* Username */
unsigned char global_yugd[MAXUSERNAME+2];               /* Group */
lion64u_t global_yubd;           /* Bytes */
unsigned int  global_yufd;                /* Files */
unsigned int  global_yusd;                /* Seconds */
unsigned char global_yutd[MAXTAGLINE+2]; /* Tagline */

/* Last-weeks TOP upload DAY */
unsigned char global_yuuw[MAXUSERNAME+2]; /* Username */
unsigned char global_yugw[MAXUSERNAME+2];               /* Group */
lion64u_t global_yubw;           /* Bytes */
unsigned int  global_yufw;                /* Files */
unsigned int  global_yusw;                /* Seconds */
unsigned char global_yutw[MAXTAGLINE+2]; /* Tagline */

/* Last-months TOP upload DAY */
unsigned char global_yuum[MAXUSERNAME+2]; /* Username */
unsigned char global_yugm[MAXUSERNAME+2];               /* Group */
lion64u_t global_yubm;           /* Bytes */
unsigned int  global_yufm;                /* Files */
unsigned int  global_yusm;                /* Seconds */
unsigned char global_yutm[MAXTAGLINE+2]; /* Tagline */

/* Yesterday's TOP download DAY */
unsigned char global_ydud[MAXUSERNAME+2]; /* Username */
unsigned char global_ydgd[MAXUSERNAME+2];               /* Group */
lion64u_t global_ydbd;           /* Bytes */
unsigned int  global_ydfd;                /* Files */
unsigned int  global_ydsd;                /* Seconds */
unsigned char global_ydtd[MAXTAGLINE+2]; /* Tagline */

/* Last-weeks TOP download DAY */
unsigned char global_yduw[MAXUSERNAME+2]; /* Username */
unsigned char global_ydgw[MAXUSERNAME+2];               /* Group */
lion64u_t global_ydbw;           /* Bytes */
unsigned int  global_ydfw;                /* Files */
unsigned int  global_ydsw;                /* Seconds */
unsigned char global_ydtw[MAXTAGLINE+2]; /* Tagline */

/* Last-months TOP download DAY */
unsigned char global_ydum[MAXUSERNAME+2]; /* Username */
unsigned char global_ydgm[MAXUSERNAME+2];               /* Group */
lion64u_t global_ydbm;           /* Bytes */
unsigned int  global_ydfm;                /* Files */
unsigned int  global_ydsm;                /* Seconds */
unsigned char global_ydtm[MAXTAGLINE+2]; /* Tagline */




