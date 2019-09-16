/********************************************************************
*                            Masm3.c
*********************************************************************
*  This is the micro-assembler data file for board 3.
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

int micro_word_width = 48;

char assembler_id[] = { "THREE" };

extern int line_number,column_number,find_field();
extern void error();
int null_routine();
int bop_routine();
int micro_ad_routine();
int constant_routine();

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
   struct mnemonic   field_mnu[70];   /* BES */
   int               position;
   int               width;
   int               type;
   int               default_value;
   int               defined;
};


struct field fields[]=
{
   "MICRO_AD",            /* start definition of the field "count" */
   micro_ad_routine,  /* Check for ADDRESS defined */
   null_routine,
   {
     {""        , 0},    /* null string marks the end of mnuemonics */
   },0,12,1,0,0,

   "CONSTANT",
   constant_routine,  /* Check for MICRO_AD defined */
   null_routine,
   {
     {""       ,   0},
   },4,8,1,0,0,

   "BOP",               /* start definition of the field "bop" */
   bop_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"BRA"        ,  0xd},
     {"BRN"        ,   5},
     {"Z"          ,   0},
     {"NOT_Z"      ,   8},
     {"N"          ,   1},
     {"NOT_N"      ,   9},
     {"C4"         ,  0xa},
     {"NOT_C4"     ,   2},
     {"C8"         ,  0xb},
     {"NOT_C8"     ,   3},
     {"C_MICRO"    ,  0xc},
     {"NOT_C_MICRO",   4},
     { ""          ,   0},    /* null string marks the end of mnuemonics */
   },12,4,2,5,0,

   "COUNT",          /* start definition of field "a_source" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"COUNT"   ,    1},
     {"NO_COUNT",    0},
     {"NOCOUNT" ,    0},
     {""        ,    0},    /* null string marks the end of mnuemonics */
   },16,1,2,1,0,

   "OP_CON_CTL",          /* start definition of field "b_source" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"CONSTANT",   1},
     {"OP_CODE" ,   2},
     {"NORMAL"  ,   0},
     {""        ,   0},    /* null string marks the end of mnuemonics */
   },17,2,2,0,0,


   "ALU_FUNC",          /* start definition of field "b_source" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"A_AND_B",            0x1b},
     {"A_OR_B",             0x1e},
     {"A_XOR_B",            0x19},
     {"A",                    0x1f},
     {"NOT_A",                0x10},
     {"B",                    0x1a},
     {"NOT_B",                0x15},
     {"CLEAR",                0x13},
     {"PRESET",               0x1c},
     {"A_PLUS_C",             0x00},
     {"A_PLUS_B_PLUS_C",      0x09},
     {"A_M_B_M_1_P_C",        0x06},
     {"A_MINUS_ONE_PLUS_C",   0x0f},
     {"A_PLUS_A_PLUS_C",      0x0c},
     {""    ,     0},    /* null string marks the end of mnuemonics */
   },29,5,2,0x1c,0,

   "AD_BUS",             /* start definition of field "ad_bus" */
   null_routine,       /* These routines do absolutely nothing */
   null_routine,
   {
     {"PC"    ,   0},
     {"SP"    ,   1},
     {"MAR"   ,   2},
     {"HIGH_Z",   3},
     {""      ,   0},    /* null string makrs the end of mnemonix  */
   },27,2,2,0,0,

   "ALU_DEST",
   null_routine,
   null_routine,
   {
     {"PCH"    ,   0},
     {"PCL"    ,   1},
     {"SPH"    ,   2},
     {"SPL"    ,   3},
     {"MARH"   ,   4},
     {"MARL"   ,   5},
     {"ACC"    ,   6},
     {"SR"     ,   7},
     {"NONE"   ,   8},
     {""       ,   0},
   },23,4,2,8,0,

   "ALU_SOURCE",
   null_routine,
   null_routine,
   {
     {"PCH"    ,   0},
     {"PCL"    ,   1},
     {"SPH"    ,   2},
     {"SPL"    ,   3},
     {"MARH"   ,   4},
     {"MARL"   ,   5},
     {"ACC"    ,   6},
     {"SR"     ,   7},
     {"NONE"   ,   8},
     {""       ,   0},
   },19,4,2,6,0,

   "VMA",
   null_routine,
   null_routine,
   {
     {"VMA"    ,   0},
     {"NO_VMA" ,   1},
     {""       ,   0},
   },39,1,2,1,0,

   "R_W",
   null_routine,
   null_routine,
   {
     {"READ"   ,   1},
     {"WRITE"  ,   0},
     {""       ,   0},
   },38,1,2,1,0,

   "EN_SR",
   null_routine,
   null_routine,
   {
     {"EN_SR"   ,  0},
     {"NO_EN_SR",  1},
     {""        ,  0},
   },37,1,2,1,0,

   "INC_PC",
   null_routine,
   null_routine,
   {
     {"INC"    ,   1},
     {"NO_INC" ,   0},
     {""       ,   0},
   },36,1,2,1,0,

   "CIN",               /* start definition of field "cin" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"YES"    ,  3},
     {"NO"     ,  2},
     {"MACRO_C",  1},
     {"MICRO_C",  0},
     {""    ,     0},    /* null string marks the end of mnuemonics */
   },34,2,2,2,0,

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
int  findex;
int  *value;
{

  int   local_index;

  local_index = find_field("CONSTANT");
  if (fields[local_index].defined != 0)
  {
    error(FIELD_REDEFINITION,line_number,column_number);
  }
  return(0);

}

/********************************************************************
*                 constant_routine(source,findex,value)              *
*********************************************************************
*  See if MICRO_AD is defined.
*********************************************************************/

int constant_routine(source,findex,value)
FILE *source;
int  findex;
int  *value;
{

  int    local_index;

  local_index = find_field("MICRO_AD");
  if (fields[local_index].defined != 0)
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

