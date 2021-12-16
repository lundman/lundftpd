#ifndef CHECKCHILD_H_INCLUDED
#define CHECKCHILD_H_INCLUDED

#include "lion.h"


// Defines




// Variables




// Functions
int   checkchild_init                ( lion_t *, void *, void * );
void  checkchild_process             ( char * );
int   checkchild_handler             ( lion_t *, void *, int, int, char * );
int   checkchild_ext_handler         ( lion_t *, void *, int, int, char * );
void  checkchild_test_ext            ( unsigned int, char *, char * );
void  checkchild_test_int            ( unsigned int, char *, int );
void  checkchild_processnode         ( void );



#endif
