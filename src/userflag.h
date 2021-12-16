
    /*
      ALL, KICK, OVERWRITE, DELETE, DIRDELETE, GROUP, PASSWORD(*), RESTART(*),
      PUNISH(*), MOVE, LOGINS
     */

/* User Flag Levels */

#define UF_NONE        0       /* No additional priviledges.         */
#define UF_OVERWRITE   1       /* Overwrite files (not owned)        */
#define UF_DELETE      2       /* Delete files (not owned)           */ 
#define UF_DIRDELETE   4       /* Delete directories (not owned)     */ 
#define UF_GROUP       8       /* Allow group changes                */
#define UF_PASSWORD    16      /* Change others passwords            */ 
#define UF_STAT        32      /* Can stat users                     */
#define UF_PUNISH      64      /* User can revoke quota for upload   */ 
#define UF_MOVE        128     /* Move's of files or directories     */ 
#define UF_SETFLAGS    256     /* May change users level flags       */
#define UF_KICK        512     /* Allow remote logoff of others      */
#define UF_DUPE        1024    /* May modify DUPE database           */
#define UF_CREDITS     2048    /* May give and take credits          */
#define UF_ADDUSER     4096    /* May add users and other user tasks */
#define UF_SETIP       8192    /* May modify users patterns          */
#define UF_SETRATIO   16384    /* May modify users ratio             */
#define UF_SETLIMIT   32768    /* May modify users limits            */
#define UF_SEEALL     65536    /* Can see in stealth areas           */

#define UF_OLDALL      1023    /* Dumb dumb mistake                  */
#define UF_ALL         0xffffffff /* All stats on */

