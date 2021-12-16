/*
 * Win32 API need O_BINARY on all open() and creat() calls.
 *
 * I redefine the O_RDONLY, O_WRONLY, O_RDWR to have O_BINARY tagged on.
 * nasty, but oh well.
 *
 */

//#define open(X,Y) open((X), O_BINARY| Y )
//#define open(X,Y,Z) open((X), O_BINARY| Y, Z )
#ifndef WIN32_H_INCLUDED
#define WIN32_H_INCLUDED

#include "win32_dirent.h"

//typdef unsigned long mode_t;

typedef unsigned int uid_t;


#ifndef F_OK
#define F_OK 00
#define X_OK 00
#define R_OK 04
#endif

//typedef __int64 off_t;
#define S_IRUSR _S_IREAD 
#define S_IWUSR _S_IWRITE 
#define S_IXUSR _S_IEXEC  
#define S_IRGRP _S_IREAD 
#define S_IWGRP _S_IWRITE 
#define S_IXGRP _S_IEXEC  
#define S_IROTH _S_IREAD 
#define S_IWOTH _S_IWRITE 
#define S_IXOTH _S_IEXEC  
#define S_IFREG _S_IFREG

#define S_ISUID 0
#define S_ISGID 0
#define S_ISVTX 0

#define S_ISDIR(m)      ((m & _S_IFMT) == _S_IFDIR)     /* directory */
#define S_ISREG(m)      ((m & _S_IFMT) == _S_IFREG)     /* regular file */


// We also need to simulate opendir and readdir().

#ifndef STRUCT_DIR_DEFINED
struct DIR_struct;
#endif

typedef struct DIR_struct DIR;




DIR *          opendir(const char *filename);
struct dirent *readdir(DIR *dirp);
int            closedir(DIR *dirp);


#define lstat stat


#endif
