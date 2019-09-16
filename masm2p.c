/********************************************************************
*                            Masm2p.c
*********************************************************************
*  This is the micro-assembler data file for board 2 (positive logic).
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

char assembler_id[] = { "TWOP" };

int null_routine();
int bop_routine();

extern int line_number,column_number;
extern  void  error();
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
   "COUNT",             /* start definition of the field "count" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"COUNT"   , 1},
     {"NO_COUNT", 0},
     {"NOCOUNT" , 0},
     {""        , 0},    /* null string marks the end of mnuemonics */
   },8,1,2,1,0,

   "BOP",               /* start definition of the field "bop" */
   bop_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"BRA"   , 0xe},
     {"BRN"   ,   6},
     {"Z"     , 0xd},
     {"NOT_Z" , 0x5},
     {"N"     ,   1},
     {"NOT_N" ,   9},
     {"C4"    , 0xa},
     {"NOT_C4",   2},
     {"C8"    , 0xb},
     {"NOT_C8",   3},
     {"GO"    ,  12},
     {"NOT_GO",   4},
     {"ZN"    ,   0},
     {"NOT_ZN",   8},
     {"OP"    ,  15},
     { ""      ,   0},    /* null string marks the end of mnuemonics */
   },9,4,2,6,0,

   "A_SOURCE",          /* start definition of field "a_source" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"TA"   ,    0},
     {"IA"   ,    1},
      ""    ,     0,    /* null string marks the end of mnuemonics */
   },13,1,2,0,0,

   "B_SOURCE",          /* start definition of field "b_source" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"TB"   ,    0},
     {"IB"   ,    1},
      ""     ,    0,    /* null string marks the end of mnuemonics */
   },14,1,2,0,0,

   "ALU_FUNC",          /* start definition of field "b_source" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"A",                    31},
     {"NOT_A",                16},
     {"B",                    26},
     {"NOT_B",                21},
     {"A_AND_B",            0x1b},
     {"A_OR_B",             0x1e},
     {"A_XOR_B",            0x16},	/* bit pattern corrected BRL */
     {"CLEAR",              0x13},
     {"PRESET",             0x1c},
     {"A_PLUS_C",              0},
     {"A_PLUS_B_PLUS_C",       9},
     {"A_M_B_M_1_P_C",         6},
     {"A_MINUS_ONE_PLUS_C",   15},
     {"A_M_1_P_C",            15},
     {"A_PLUS_A_PLUS_C",      12},
     {""    ,     0},    /* null string marks the end of mnuemonics */
   },15,5,2,0x1c,0,	/* default corrected BRL */

   "CIN",               /* start definition of field "cin" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"YES",      0},
     {"NO" ,      1},
     {""    ,     0},    /* null string marks the end of mnuemonics */
   },20,1,2,1,0,	/* default corrected BRL */

   "ALU_DEST",          /* start definition of field "alu_dest" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"A"     ,   6},
     {"A_B"   ,   4},
     {"A_F"   ,   2},
     {"A_B_F" ,   0},
     {"B"     ,   5},
     {"B_F"   ,   1},
     {"F"     ,   3},
     {"NONE"  ,   7},
     {""    ,     0},   /* null string marks the end of mnuemonics */
   },21,3,2,7,0,


   "MICRO_AD",          /* start definition of field "micro_ad" */
   micro_ad_routine,
   null_routine,
   {
     {""    ,     0},    /* there are no pre-defined mnuemonics for this one */
   },0,8,1,0,0,


   "ADDRESS",          /* start definition of field "ADDRESS" */
   address_routine,
   null_routine,
   {
     {""    ,     0},    /* there are no pre-defined mnuemonics for this one */
   },0,8,1,0,0,


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

int micro_ad_routine(source,findex,value) 
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
