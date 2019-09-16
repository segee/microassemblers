/********************************************************************
*                            Masm.h
*********************************************************************
*  Contains labels, data structures and variable declarations
*             related to the main module.
*  Also contains function prototypes for all functions defined
*             in the source file "main.c"
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#include "version.h"

#define N 100
#define MAX_WORD_SIZE          24

#ifdef VAX
#include <file.h>
#define PLATFORM "VMS "
#define OPENFILE open(obj_file,O_CREAT|O_RDWR,0000)
#endif

#if defined (__MSDOS__) || defined(_MSDOS_) || defined(MSDOS)
#include <fcntl.h>
#define PLATFORM "MSDOS "
#define OPENFILE open(obj_file,O_CREAT|O_RDWR|O_BINARY,0777)
#endif

#ifdef THINK_C
#include <fcntl.h>
#define PLATFORM "Macintosh "
#define OPENFILE open(obj_file,O_CREAT|O_RDWR|O_BINARY)
#endif

#ifndef PLATFORM
#include <fcntl.h>
#define PLATFORM "Unix "
#define OPENFILE open(obj_file,O_CREAT|O_RDWR,0666)
#endif

#define PROG_TITLE "Board %s micro-assembler for %s %s released to public domain. Modified: %s\n\n",\
                    assembler_id,PLATFORM,VERSION,CPW

/*********************************************************************
 *  Define the structures needed for the micro-word definitions.
 *  All of these structures and variables are defined and initialized
 *  in an external C module.
 *********************************************************************/

extern  int  pre_field();
extern  int  post_field();
extern  char my_getc();
extern  void skip_white_space();
extern  int  skip_to_eol();
extern  int  pre_word();
extern  int  post_word();

extern struct mnemonic
{
   char *name;
   int  value;
};

extern struct field
{
   char             *name;
   int              (*pre_field) ();
   int              (*post_field)();
   struct mnemonic  field_mnu[70];
   int              position;
   int              width;
   int              type;
   int              default_value;
   int              defined;
};

extern struct field fields[];
extern int    micro_word_width;
extern char   assembler_id[];
