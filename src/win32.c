#define WINDOWS_MEAN_AND_LEAN
#include <windows.h>
#include <fcntl.h>

#include "win32_dirent.h"


//
// We do this hackery so that the entire application need not
// include windows.h everywhere.
struct DIR_struct {
	HANDLE dirp;
	WIN32_FIND_DATA stbf;
	struct dirent dp;
	int status;
};
#define STRUCT_DIR_DEFINED

#include "win32.h"


void bzero(void *ptr, unsigned int len)
{
	memset(ptr, 0, len);
}

void bcopy(void *src, void *dst, unsigned int len)
{
	memcpy(dst, src, len);
}


uid_t geteuid()
{
	return 0;
}

int seteuid(uid_t uid)
{
	return 0;
}

uid_t getuid()
{
	return 0;
}

int setuid(uid_t uid)
{
	return 0;
}

int chown(char *name, uid_t uid, unsigned int gid)
{

	return 0;

}


int random(void)
{
	return rand();
}

void srandom(int p)
{
	srand(p);
}

int mkstemp(char *temp)
{
	char *nam;
	int result;

	nam = mktemp(temp);

	result = open(nam, O_RDWR|O_TRUNC|O_CREAT|O_BINARY, 060);

	return result;

}

char *getlogin()
{
	static char nam[80];
	int siz;

	nam[0] = 0;
	siz = sizeof(nam);
	GetUserName(nam, &siz);

	return nam;
}




#if 0 // this is now defined in lion
void gettimeofday(now)
  struct timeval *now;
{

      FILETIME win_time;

      GetSystemTimeAsFileTime(&win_time);
      /* dwLow is in 100-s nanoseconds, not microseconds */
      now->tv_usec = win_time.dwLowDateTime % 10000000 / 10;

      /* dwLow contains at most 429 least significant seconds, since 32 bits maxint is 4294967294 */
      win_time.dwLowDateTime /= 10000000;

      /* Make room for the seconds of dwLow in dwHigh */
      /* 32 bits of 1 = 4294967295. 4294967295 / 429 = 10011578 */
      win_time.dwHighDateTime %= 10011578;
      win_time.dwHighDateTime *= 429;

      /* And add them */
      now->tv_sec = win_time.dwHighDateTime + win_time.dwLowDateTime;
}
#endif




DIR *opendir(const char *filename)
{
	DIR *dirp;
	char path[1024];

	dirp = (DIR *) malloc(sizeof(DIR));
	if (!dirp) return NULL;

	memset(dirp, 0, sizeof(*dirp));

	snprintf(path, sizeof(path), "%s/*", filename);

	dirp->dirp = FindFirstFile(path, &dirp->stbf);

	if (dirp->dirp == INVALID_HANDLE_VALUE) {
		free(dirp);
		return NULL;
	}

	dirp->status = 1;

	return dirp;
}



struct dirent *readdir(DIR *dirp)
{
	int err;

	// If we don't already have a node, fetch the next.
	if (!dirp->status) {
		
		err = FindNextFile(dirp->dirp, &dirp->stbf);
		if (!err) return NULL;

	}

	// We have a windows node in stbf, convert it to what they expect
	strncpy(dirp->dp.d_name, dirp->stbf.cFileName, sizeof(dirp->dp.d_name));
	dirp->dp.d_name[MAXNAMLEN] = 0;

	dirp->dp.d_namlen = strlen(dirp->dp.d_name);

	// MSDN says it has a OID but apparently we do not
	//	dirp->dp.d_fileno = dirp->stbf.dwOID;

	if (dirp->stbf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		dirp->dp.d_type = DT_DIR;
	else
		dirp->dp.d_type = DT_REG;

	// Set reclen to something?
	//printf("[win32] opendir '%s' %d\n", dirp->dp.d_name, dirp->dp.d_type);
	
	dirp->status = 0;
	return &dirp->dp;
}

int closedir(DIR *dirp)
{

	FindClose(dirp->dirp);

	free(dirp);
	dirp = NULL;

	return 0;

}




void sleep(int sec)
{

	Sleep(sec * 1000);

}

char *ctime_r(const unsigned long *clock, char *buf)
{
	char *r;

	r = ctime(clock);

	strcpy(buf, r);

	return buf;
}

int utimes(const char *path, const struct timeval *times)
{
  utime(path, NULL); // FIXME if we ever want to set the time specifically
}

