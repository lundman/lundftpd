/*
                          DMSDIZ V2.0 SANS Software.

Copyright (C) 1994, 1995 and 1996 Jorgen Lundman.

Authors:                Jorgen Lundman  - lordlund@dircon.co.uk

THE SOFTWARE IS PROVIDED ``AS IS'' WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT LIMITATION
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. THE AUTHORS WILL IN NO WAY BE LIABLE FOR DAMAGES RESULTING
FROM THE USE, MISUSE AND/OR INABILITY TO USE THIS SOFTWARE.

*/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

/*
 * DMS utility to print desc in dms files
 *
 * Version 2.2 - Added test only flag for better integration with lundftpd
 * Version 2.1 - Fixed -a bug found by The_Kid.
 * Version 2.0 - Stripping banners and track banners.
 * Version 1.5 - Added diz add support for dms files without diz's
 * Version 1.3 - Join output to one, bloody Netrunner complained too much.
 * Version 1.2 - Adding LHA and various other formats
 * Version 1.1 - Bug fixes
 * Version 1.0 - endian support
 * For bugs reports, further info, mail lundman@kauri.vuw.ac.nz.
 *
 * Diz add support was written by L.Metcalfe in 1994 for AcCeSsIoN, and
 * rather than rewriting it myself I'll use his code with his permission.
 */

/* Various formats follow ... */
/* Use $FILENAME to be replaced with the file */

/* Warning, file starting with '-' might be evaluated as a switch, if you
 * wish to avoid this, put a "--" argument befor ethe "${FILENAME}" argument.
 */

/* You have to set the paths for the archivers yourself */

typedef char *formats[];

formats lha = { ".lha",                           /* The extension */
		       "/usr/local/bin/lha", /* The executable path */
		       "p",                       /* Argument 1*/
		       "${FILENAME}",             /* Argument 2*/
		       "FILE_ID.DIZ",             /* Argument 3*/
		  "file_id.diz",                  /* lha is case sensitive:( */
		       NULL };

formats lzh = { ".lzh",                           /* The extension */
		       "/usr/local/bin/lha", /* The executable path */
		       "p",                       /* Argument 1*/
		       "${FILENAME}",             /* Argument 2*/
		       "FILE_ID.DIZ",             /* Argument 3*/
		  "file_id.diz",                  /* lha is case sensitive:( */
		       NULL };

formats zip = { ".zip",                        /* The extension */
		       "/usr/local/bin/unzip", /* The executable path */
		       "-p",                   /* Argument 1*/
		       "$FILENAME",            /* Argument 2*/
		       "FILE_ID.DIZ",          /* Argument 3*/
		       "file_id.diz",          /* Argument 3*/
		       NULL };
/* List, add the formats here too, remember NULL on the end. */

typedef formats *formatlist[];
formatlist format = { &lha, &zip, &lzh, NULL };
/* extensions are searched in this order... except dms first. */

/* No need to change anything after this line */
FILE *text = stdout;

int ask_for_desc = 0, strip_banners = 0;
 
void usage(char **argv)
{
  fprintf(text,"This program scans *.dms (and various other formats) for a file description\r\n");
  fprintf(text,"in the form of FILE_ID.DIZ.\r\n");
  fprintf(text,"Usage:\r\n");
  fprintf(text,"\t%s [-sar] file [file...]\r\n",argv[0]);
  fprintf(text,"\t-s : Seperate desc output (stdout) and prgmessages (stderr)\r\n");
  fprintf(text,"\t-a : Ask for description if none is found (*.dms only)\r\n");
  fprintf(text,"\t-r : Remove banners, track banners and tail banners\r\n\r\n");
  fprintf(text,"\t-t : Test only (DMS files only) 0 for ok\r\n");
  fprintf(text,"This is V2.2. If you have any problems please email me at\r\n");
  fprintf(text,"lordlund@dircon.co.uk. Use this on your own risk.\r\n\r\n");
  exit (1);

}

extern char *optarg;
extern int optind;
int test_only = 0;

main(int argc, char **argv)
{
  int  opt;
  /* just checking args for switches */

  while ((opt=getopt(argc, argv, "hsart")) != -1) {
    switch(tolower(opt)) {
    case 'h':
      usage(argv);
      break;
    case 's':
      text = stderr;
      break;
    case 'a':
      ask_for_desc = 1;
      break;
    case 'r':
      strip_banners = 1;
      break;
    case 't':
      test_only = 1;
      break;
    }
  }

  if (optind == argc) usage(argv);

  if (text == stderr) 
    fprintf(text,
	    "dmsdiz by Accident (c) 1995 V2.2\r\n");
  else
    fprintf(text,
	    "dmsdiz by Accident (c) 1995 V2.2\n");


  for ( ; optind < argc; optind++) {
    if (!access(argv[optind],F_OK)) {
      fprintf(text,"Processing file %s...\r\n",argv[optind]);
      do_file(argv[optind]);
    }
    else 
      fprintf(text,"Sorry, no such file `%s'.\r\n",argv[optind]);
  }
}

unsigned short DoBlockCRC();

/* case insensitive */
int cmptail(char *string, char *sub)
{
  int l1 = strlen(string), l2 = strlen(sub);
  if (l1 < l2)
    return 0;
  string = string + l1 - l2;
  if (strcasecmp(string, sub))
    return 0;
  return 1;
}


do_file(char *s)
{
  formats *runner;
  int t=0;

  /* First we check for dms files */
  if (cmptail(s,".dms")) {
    dms_file(s);
    return;
  }

  /* Now parse other formats... */


  while(runner = format[t]) {
    if(runner)
      if (cmptail(s,(*runner)[0])) {
	int y=1;
	char cmd[256],buf[256];
	FILE *ptr;
	char **p;

	sprintf(buf,"FILENAME=%s",s);
	putenv(buf);

	p = (*runner);
	*cmd = (char) 0;
	
	while(p[y]) {
	  strcat(cmd,p[y]);
	  strcat(cmd,"  ");/* Double space, just incase one str end with \ :)*/
	  y++; /* Count number of args */
	}
	
	if ((ptr = popen(cmd, "r")) != NULL)
	  while (fgets(buf, 80, ptr) != NULL)
	    (void) printf("%s",buf);
	pclose(ptr);
      }
    t++;
  }
}


/* Some data definitions */

typedef char           BYTE;
typedef unsigned char  UBYTE;
typedef short          SHORT;
typedef unsigned short USHORT;
typedef long           LONG;
typedef unsigned long  ULONG;


struct {			/* offset - sizeof(File_Head) = 50				*/
	long u1;                /* 0 - ???			         					*/
	long info;				/* 4 - General info ( 0x01: NoZero (no bitmap)	*/
							/* 					  0x08: Banner )			*/
	long date;              /* 8 - Creation date of file 					*/
	short from;             /* 12 - index of first track 					*/
	short to;               /* 14 - index of last track  					*/
	ULONG  size_after;      /* 16 - total size of data after compression	*/
	ULONG  size_before;     /* 20 - total size of data before compression	*/
	long u3;                /* 24 -  Always 0								*/
	short cpu;              /* 28 -  Cpu type ( 1: 68000,					*/
							/*					2: 68010,					*/
							/*					3: 68020,					*/
							/*					4: 68030,					*/
							/*					5: 68040 )					*/
	short copross;          /* 30 -  Cpu coprocessor ( 1: 68881,			*/
							/*						   2: 68882 )			*/
	short machine;          /* 32 -  Machine used    ( 1: Amiga, 			*/
							/*						   2: PC )				*/
	short u4;               /* 34 -  ???                 					*/
	short cpu_speed;        /* 36 -  CPU Speed. Mostly left blank.			*/

	/* Many compilers have problems putting this long at a word boundary*/
/*	long time;              /* 38 -  Time to create archive in sec.			*/
	short time, time1; /* and cast it to (long) */

	short c_version;        /* 42 -  Version of creator (0x64 = v1.00		*/
							/*							 0x65 = v1.01		*/
							/*							 0x66 = v1.02		*/
							/*							 0x67 = v1.03		*/
							/*							 0x6F = v1.11	)	*/
	short n_version;        /* 44 -  Version needed to extract				*/
	short disk_type;        /* 46 -  Disk type 	(1 = Amiga OFS v1.0)		*/
	short cmode;            /* 48 -  Compression mode (0 = NONE,			*/
							/*						   1 = SIMPLE,			*/
							/*						   2 = QUICK(?)			*/
							/*						   3 = MEDIUM,			*/
							/*						   4 = DEEP,			*/
							/*						   5 = HEAVY1,			*/
							/*						   6 = HEAVY2 )			*/
	USHORT hcrc;            /* This header's CRC, sizeof()-2 */
} t_Disk;

struct {            /* offset -  sizeof(Track_Head) = 18	*/
	short delim;            /* 0 - delimiter, 0x5452, is 'TR'		*/
	short number;           /* 2 - track number, -1 if text			*/
	ULONG size;             /* 4 - size of data part     			*/
	USHORT plength;         /* 8 - length of non-encoded data		*/
	USHORT ulength;         /* 10 - length of encoded data			*/
	short mode;             /* 12 - encryption mode (  0: SIMPLE,	*/
							/*						 102: QUICK )	*/
	USHORT usum;            /* 14 - raw data checksum    			*/
	USHORT dcrc;            /* 16 - data CRC             			*/
	USHORT hcrc;            /* 18 - header CRC */
} t_Track;

int check_dms(char *file)
{
  /* This check a dms file */
  /* return 0 on error or any kind. */

  int fd;
  char *databuf, DMS[5];

  if ((fd = open (file, O_RDONLY)) < 0) {
    fprintf(stderr,"Failed to open file %s: ", file); perror("");
    return 0;
  }

  if ((read(fd, DMS, 4) != 4) || (strncmp(DMS,"DMS!",4))) {
    DMS[5] = (char) 0;
    fprintf(text,"   Not a DMS file! '%s' != 'DMS!'\r\n",DMS);
    close(fd);
    return 0;
  }

  if (read(fd, &t_Disk, sizeof(t_Disk)) != sizeof(t_Disk)) {
    fprintf(text,"   Could not read DMS header!\r\n");
    close(fd);
    return 0;
  }

  if (htons(t_Disk.hcrc) != DoBlockCRC(&t_Disk, 
									   sizeof(t_Disk) - sizeof(t_Disk.hcrc))) {
	fprintf(text, "   Corrupt DMS file, DMS header CRC is invalid.\r\n");
	fprintf(text, "     Found %04X but calculated %04X!\r\n",
			htons(t_Disk.hcrc), 
			DoBlockCRC(&t_Disk, sizeof(t_Disk) - sizeof(t_Disk.hcrc)));
	close(fd);
	return 0;
  }

  /* We now have the DMS info block - who cares */

  while (read(fd,&t_Track, sizeof(t_Track)) == sizeof(t_Track)) {
    if (htons(t_Track.delim) != 0x5452) { /* "TR" */
      fprintf(text,"   Corrupt DMS file, offset start did not contain 'TR'!\r\n");
      close(fd);
      return 0;
    }
    
    if (htons(t_Track.hcrc) != 
		DoBlockCRC(&t_Track, sizeof(t_Track) - sizeof(t_Track.hcrc))) {
	  fprintf(text,"   Corrupt DMS file, %s(%d) header CRC is invalid.\r\n", 
			  htons(t_Track.number)?"track":"banner", htons(t_Track.number));
	  fprintf(text,"     Found %04X but calculated %04X!\r\n",
			  htons(t_Track.hcrc),
			  DoBlockCRC(&t_Track, sizeof(t_Track) - sizeof(t_Track.hcrc)));
	  close(fd);
	  return 0;
	}

	databuf = (char *)malloc(htonl(t_Track.size));
	if (!databuf) {
	  fprintf(text,"?Failed to allocate (%d bytes) buffer for track data\r\n",
			  htonl(t_Track.size));
	  close(fd);
	  return 0;
	}
	if (read(fd, databuf, htonl(t_Track.size)) != htonl(t_Track.size)) {
	  fprintf(text,"  Corrupt DMS file, short read in track %d\r\n", 
			  htons(t_Track.number));
	  free(databuf);
	  close(fd);
	  return 0;
	}

	if(htons(t_Track.dcrc) != 
	   DoBlockCRC(databuf, htonl(t_Track.size))) {
	  fprintf(text, "  Corrupt DMS file, %s(%d) data CRC is invalid.\r\n",
			  htons(t_Track.number)?"track":"banner", htons(t_Track.number));
	  fprintf(text,"     Found %04X but calculated %04X!\r\n",
			  htons(t_Track.dcrc),
			  DoBlockCRC(databuf, htonl(t_Track.size)));
	  free(databuf);
	  close(fd);
	  return 0;
	}
	free(databuf);
  }
  close(fd);
  return 1;
}

void strip_me_first(char *file)
{
  /* DMS file has been checked. */
  int fd, fd_tmp, has_written=0;
  char *databuf, *tmp_nam;
  int after_banner=0, before_banner=0, track0=0;

  /* I need to open a tmp file */
  if (!(tmp_nam = tmpnam(NULL)) || 
	  ((fd_tmp = open(tmpnam(NULL), O_RDWR|O_CREAT|O_TRUNC, 0600)) < 0)) {
	perror("Failed to open tmp file for banner stripping: ");
	return;
  }

  if ((fd = open(file, O_RDWR)) < 0) {
	fprintf(text, "Failed to open file %s for read/write: ", file); perror("");
	close(fd_tmp);
	unlink(tmp_nam);
	return;
  }

  if ((write(fd_tmp, "DMS!", 4) != 4) || (lseek(fd, 4, SEEK_SET) != 4)) {
	perror("write/lseek: ");
	close(fd);
	close(fd_tmp);
	unlink(tmp_nam);
	return;
  }

  if ((read(fd, &t_Disk, sizeof(t_Disk)) - 
	   write(fd_tmp, &t_Disk, sizeof(t_Disk)))) {
	perror("Failed to read/write: ");
	close(fd); close(fd_tmp);
	unlink(tmp_nam);
	return;
  }

  while(read(fd, &t_Track, sizeof(t_Track)) == sizeof(t_Track)) {

	if (!(databuf = (char *) malloc(htonl(t_Track.size)))) {
	  fprintf(text, "Tried to allocate %d bytes\r\n", htonl(t_Track.size));
	  perror("malloc: "); close(fd); close(fd_tmp);
	  unlink(tmp_nam);
	  return;
	}
	if (read(fd, databuf, htonl(t_Track.size)) != htonl(t_Track.size)) {
	  perror("read: "); close(fd); close(fd_tmp);
	  free(databuf);
	  unlink(tmp_nam);
	  return;
	}
	/* Header and Data of current track read. */

	if (htons(t_Track.number) == 0) {
	  /* This is a track 0 */
	  track0++;

	  if (has_written == 0) {
		if ((write(fd_tmp, &t_Track, sizeof(t_Track)) != sizeof(t_Track)) ||
	   (write(fd_tmp, databuf, htonl(t_Track.size)) != htonl(t_Track.size))) {
		  perror("write: "); close(fd); close(fd_tmp);
		  free(databuf);
		  unlink(tmp_nam);
		  return;
		}
		has_written = 1;
	  } else if (has_written == 1) {
		if ((lseek(fd_tmp, 4+sizeof(t_Disk), SEEK_SET) != 4+sizeof(t_Disk)) ||
			(write(fd_tmp, &t_Track, sizeof(t_Track)) != sizeof(t_Track)) ||
		(write(fd_tmp, databuf, htonl(t_Track.size)) != htonl(t_Track.size))) {
		  perror("write: "); close(fd); close(fd_tmp);
		  free(databuf);
		  unlink(tmp_nam);
		  return ;
		}
	  }
	} else if((htons(t_Track.number) > 0) && 
		  (htons(t_Track.number) < 100)) {
	  has_written = 2;
	  if ((write(fd_tmp, &t_Track, sizeof(t_Track)) != sizeof(t_Track)) ||
	   (write(fd_tmp, databuf, htonl(t_Track.size)) != htonl(t_Track.size))) {
		perror("write: "); close(fd); close(fd_tmp);
		free(databuf);
		unlink(tmp_nam);
		return ;
	  }
	}  
	 
	if ((signed short)htons(t_Track.number) == (signed short) -1) 
	  if (!has_written) before_banner++;
	  else after_banner++;

	free(databuf);
  }

  /* If we reach this point we managed to create the tmp file */

  if ((lseek(fd, 0L, SEEK_SET) != 0L) || (lseek(fd_tmp, 0L, SEEK_SET) != 0L)){
	perror("lseek: ");
	close(fd); close(fd_tmp);
	free(databuf);
	unlink(tmp_nam);
	return ;
  }

  /* Here I hope everything works ;) But then, there should be space on
   * original since we should only be shorter, or the same. 
   * But, I'll only write if we did strip something...
   */

  if (after_banner || before_banner || (track0 > 1)) {
	int readd;

	if (!(databuf = (char *) malloc(10240))) {
	  perror("malloc: ");
	  close(fd); close(fd_tmp);
	  unlink(tmp_nam);
	  return ;
	}
	
	while ((readd = read(fd_tmp, databuf, 10240)) > 0)
	  write(fd, databuf, readd);

	ftruncate(fd, lseek(fd, 0L, SEEK_CUR)); /* heheheh */
  }

  /* and we're done */
  close(fd);
  close(fd_tmp);
  printf("Stripped %d head, %d tail and %d track banners\n",
		 before_banner, after_banner, track0-1);

  unlink(tmp_nam);
  free(databuf);

}


dms_file(char *file)
{
  int fd;

  if (!check_dms(file)) {
    if (test_only) exit(1); /* bad file */
    return;
  }

  if (test_only) exit(0); /* file good */

  /* DMS file has been checked. it is now OK! */

  if (strip_banners) {
	strip_me_first(file);
	return;
  }

  if (ask_for_desc)
	fd = open(file,O_RDWR);
  else
	fd = open(file,O_RDONLY);

  if (fd<0) {
    perror("Failed to open: ");
    return;
  }

  if (lseek(fd, 4+sizeof(t_Disk), SEEK_SET) != 4+sizeof(t_Disk)) {
	fprintf(text, "  Failed to seek in file %s?! (Even though it tested OK.)\r\n", file);
	perror("");
	close(fd);
	return;
  }

  while (read(fd,&t_Track, sizeof(t_Track)) == sizeof(t_Track)) {

	/* This is excessive but who cares */
	if (htons(t_Track.delim) != 0x5452) { /* "TR" */
	  fprintf(text,"   Corrupt DMS file!\r\n");
	  close(fd);
	  return;
	}

	if (htons(t_Track.number) == 80 ) { /* Magical number for DIZ */
	  /* we have description */
	  print_desc(fd);
	  close(fd);
	  return;
	}

	/*skip the DATA */
	if (lseek(fd, htonl(t_Track.size), SEEK_CUR) == -1) {
	  perror("   Could not seek past track data!");
	  close(fd);
	  return;
	}
  }
  if (ask_for_desc) write_desc(fd,file); /*Filename is only used for printing*/
  else 
    fprintf(text,"   Sorry, no desc in this file.\r\n");

  close(fd);
}


write_desc(int fd, char *name)
{
  int x=1,y;
  char bufs[46*15]; /* 44 chars + NL + 0 for 15 lines */
  char buf[256],*p;

  bufs[0] = (char) 0; /* To be sure */

  printf("Please enter file_id.diz description for file '%s'...\n",name);
  printf("Finish with a blank line, you can enter up to 15 lines\n\n");
  printf("[00][---------+---------+---------+---------+----]\n");

  while(x < 16) {
    printf("[%02d]:",x);
    fgets(buf,255,stdin);

    if ((buf[0] == (char) 0x0a) || (!*buf)) break;

    buf[44] = (char) 0x0a;
    buf[45] = (char) 0 ;
    strcat(bufs,buf);
    x++;
  }
  x--;

  printf("\n[%02d] lines entered, packing desc...",x);

  if(!dizpack(fd,bufs))
	printf("done.\r\n");
}

print_desc(int fd)
{
  /* Okay, t_Track is assumed to be ok */

  int length;
  char *data;

  if (htons(t_Track.mode)) {
	fprintf(text,"   Sorry, can't handle compressed desc's yet.\r\n");
	return;
  }

  length = htonl(t_Track.size);
  data = (char *) malloc(length);
  if (!data) {
	fprintf(text,"   Failed to allocate %d bytes\r\n",length);
	return;
  }

  if (read(fd, data, length)!= length) {
	fprintf(text,"   Couldn't read desc?!\r\n");
	return;
  }

  while (length>0) {
	putchar(*data);
	data++;
	length--;
  }
  fflush(stdout);
}




/* The rest is L.Metcalfe's code, all bugs to him! */




/*
Well here's a VERY simple program to add a file_id.diz file to a dms,
It seems to work but hasn't really been tested yet, don't blame me if it
screws up!  ;)

Coded by L.Metcalfe in 1994 for AcCeSsIoN

cc -o dizadd dizadd.c
*/

/* Accident woz here! */
#define MAX_DIZLENGTH 32768	/* Max size in bytes for the diz text... */
/*
#define THEAD_IDENT(tab)        (*((unsigned short *) (tab+0)))
#define THEAD_TNUM(tab)         (*((short *)  (tab+2)))
#define THEAD_CSIZE(tab)        (*((unsigned long *)  (tab+4)))
#define THEAD_PLEN(tab)         (*((unsigned short *) (tab+8)))
#define THEAD_SIZE(tab)         (*((unsigned short *) (tab+10)))
#define THEAD_MODE(tab)         (*((short *) (tab+12)))
#define THEAD_USUM(tab)         (*((unsigned short *) (tab+14)))
#define THEAD_DCRC(tab)         (*((unsigned short *) (tab+16)))
#define THEAD_HCRC(tab)         (*((unsigned short *) (tab+18)))
*/

static unsigned short CRCTab[256] =
{
        0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
        0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
        0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
        0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
        0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
        0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
        0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
        0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
        0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
        0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
        0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
        0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
        0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
        0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
        0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
        0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
        0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
        0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
        0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
        0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
        0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
        0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
        0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
        0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
        0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
        0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
        0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
        0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
        0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
        0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
        0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
        0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
};

unsigned short DoBlockCRC(Mem, Size)
unsigned char* Mem;
int Size ;
{
        register unsigned short CRC = 0;

        while(Size--)
                CRC = CRCTab[((CRC ^ *Mem++) & 255)] ^ ((CRC >> 8) & 255);

        return(CRC) ;
}

dizpack(int filed, char *desc)
{
  int dizlength;
  
  t_Track.delim = htons(0x5452);
  t_Track.number = htons(80);

  dizlength = strlen(desc);

  t_Track.size = htonl( (long) dizlength );
  t_Track.plength = htons( dizlength );
  t_Track.ulength = htons( dizlength);
  t_Track.mode = 0;
  t_Track.usum = 0;
  t_Track.dcrc = htons( DoBlockCRC(desc, dizlength) );
  t_Track.hcrc = htons( DoBlockCRC(&t_Track, 
				   sizeof(t_Track) - sizeof(t_Track.hcrc)));

/* Warning, raw data checksum not set, anybody know how to calculate it? */
/* Does it matter?  8-) */

  if (lseek(filed, 0, SEEK_END) < 0) {
	perror("Failed to seek to the end of the file: ");
	return 1;
  }

  if (write(filed, &t_Track, sizeof(t_Track)) != sizeof(t_Track)) {
	fprintf(stderr,"Couldn't write header part?!?\r\n");
	return 1;
  }

  if (write(filed, desc, strlen(desc)) != strlen(desc)) {
	fprintf(stderr,"Couldn't write data part?!?\r\n");
	return 1;
  }
  return 0;
}



