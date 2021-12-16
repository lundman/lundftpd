
#ifdef WIN32

#ifndef DIRENT_H_INCLUDED
#define DIRENT_H_INCLUDED


// We don't need the whole structure personally, but lets be copatible
#define DT_UNKNOWN      0
#define DT_FIFO         1
#define DT_CHR          2
#define DT_DIR          4
#define DT_BLK          6
#define DT_REG          8
#define DT_LNK          10
#define DT_SOCK         12
#define DT_WHT          14

struct dirent {
	unsigned int d_fileno;
	unsigned short int d_reclen;
	unsigned char d_type;
	unsigned char d_namlen;
#define MAXNAMLEN 255
	char d_name[MAXNAMLEN+1];
};



#endif
#endif
