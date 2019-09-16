/********************************************************************
*                            Masm1.c
*********************************************************************
*  This is the micro-assembler data file for board 1.
*  This file defines the data structure "fields" used by the
*  main assembler.  It also defines the pre- and post- word and
*  field exception processing routines.
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#include <stdio.h>
#include "error.h"
#include "symbol.h"
#include "global.h"

int micro_word_width = 32;

char assembler_id[] = { "ONE" };

int null_routine();
int bop_routine();
extern int line_number, column_number;
extern void error();
extern int find_field();
int micro_ad_routine();
int address_routine();

/*
 *  The following statement initializes the fields structure
 *  which is defined as follows:
 */

struct mnemonic
{
   char *name;
   int value;
};

struct field
{
   char              *name;
   int               (*pre_field) ();
   int               (*post_field)();
   struct mnemonic   field_mnu[70];
   int               position;
   int               width;
   int               type;
   int               default_value;
   int               defined;
};


struct field fields[]=
{
   "BOP",               /* start definition of the field "bop" */
   bop_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"SEQ"   ,   5},
     {"BRA"   ,   6},
     {"HOLD"  ,   0},
     {"BRC"   ,   1},
     { ""      ,   0},    /* null string marks the end of mnemonics */
   },8,3,2,5,0,

   "POL",          /* start definition of field "a_source" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"TRUE" ,    1},
     {"FALSE",    0},
     {"PRESS",    1},  
     {"RELEASE",  0}, 
      ""    ,     0,    /* null string marks the end of mnuemonics */
   },12,1,2,1,0,

   "COND",          /* start definition of field "COND" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"IN0"  ,    0},
     {"IN1"  ,    1},
     {"IN2"  ,    2},
     {"IN3"  ,    3},
      ""     ,    0,    /* null string marks the end of mnemonics */
   },13,2,2,0,0,

   "MICRO_AD",          /* start definition of field "micro_ad" */
   micro_ad_routine,
   null_routine,
   {
     {""    ,     0},    /* there are no pre-defined mnemonics for this one */
   },0,8,1,0,0,


   "ADDRESS",          /* start definition of field "ADDRESS" */
   address_routine,
   null_routine,
   {
     {""    ,     0},    /* there are no pre-defined mnemonics for this one */
   },0,8,1,0,0,


   "DATA",          /* start definition of field "DATA" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {""    ,     0},    /* there are no pre-defined mnemonics for this one */
   },17,4,1,0,0,

   "",
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {""    ,     0},
   },0,0,0,0,0
};

/********************************************************************
 *  These null routines are used for the pre_field and post_field   *
 *  processing for the fields that have nothing special about       *
 *  them.                                                           *
 *                                                                  *
 *  Routines could be placed here to do special processing to       *
 *  handle exceptions to the general rules the main assembler       *
 *  imposes.                                                        *
 *                                                                  *
 *  A value is returned through the pointer "value".  This value    *
 *  will be OR'd with the value found by the normal processing.     *
 *                                                                  *
 *  The value returned by these function has the following meanings *
 *     0   -   No processing of the source was done by the function *
 *             Therefore, the next character cannot be a comma.     *
 *     1   -   The function did processing of text from the source  *
 *             file.  Therefore, it is quite possible that the next *
 *             character is a comma.                                *
 *    -1   -   The function encountered some kind of error.  The    *
 *             normal processing code will log an error in the      *
 *             proper structures.                                   *
 ********************************************************************/

int null_routine(source,findex,value)
FILE *source;
int  findex;
int  *value;
{
   return(0);
}

int bop_routine(source,findex,value)
FILE *source;
int  findex;
int  *value;
{
    return(0);
}

/********************************************************************
*                 micro_ad_routine(source,findex,value)             *
*********************************************************************
*  See if ADDRESS is defined.
*********************************************************************/

int micro_ad_routine(source,findex,value) /* see if ADDRESS defined*/
FILE *source;
int findex;
int *value;
{
  int local_index;

  local_index=find_field("ADDRESS");
  if(fields[local_index].defined !=0)
  {
     error(FIELD_REDEFINITION,line_number,column_number);
  }
  return(0);
}

/********************************************************************
*                 address_routine(source,findex,value)              *
*********************************************************************
*  See if MICRO_AD is defined.
*********************************************************************/

int address_routine(source,findex,value)
FILE *source;
int findex;
int *value;
{
  int local_index;

  local_index=find_field("MICRO_AD");
  if(fields[local_index].defined !=0)
  {
     error(FIELD_REDEFINITION,line_number,column_number);
  }
  return(0);
}

/********************************************************************
 *  These subroutines are the pre- and post- word processing        *
 *  routines.  They are executed by the main assembler before a new *
 *  micro-word is started and after a micro-word is finished.  They *
 *  allow for more flexibility.  For example, the pre-word process  *
 *  routine could be used to adjust field default values depending  *
 *  upon certain conditions such as the current memory counter.     *
 ********************************************************************/

int pre_word(source)
FILE *source;
{
   return(0);
}

int post_word(source)
FILE *source;
{
   return(0);
}
