/********************************************************************
*                            Masm4.c
*********************************************************************
*  This is the micro-assembler data file for board 4.
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

int micro_word_width = 64;

char assembler_id[] = { "FOUR" };

extern int line_number,column_number,find_field();
extern void error();
int null_routine();
int bop_routine();
int micro_ad_routine();
int constant_routine();
int alu_func_routine();
int group1_routine();
int group2_routine();

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
   "MICRO_AD",            /* start definition of the field "micro_ad" */
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
   },0,12,1,0,0,

   "SEQ",               /* start definition of the field "seq" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"JZ"         ,   0},
     {"CJS"        ,   1},
     {"JMAP"       ,   2},
     {"CJP"        ,   3},
     {"PUSH"       ,   4},
     {"JSRP"       ,   5},
     {"CJV"        ,   6},
     {"JRP"        ,   7},
     {"RFCT"       ,   8},
     {"RPCT"       ,   9},
     {"CRTN"       ,  10},
     {"CJPP"       ,  11},
     {"LDCT"       ,  12},
     {"LOOP"       ,  13},
     {"CONT"       ,  14},
     {"TWB"        ,  15},
     { ""          ,   0},    /* null string marks the end of mnuemonics */
   },12,4,2,14,0,

   "RLD",                  /* start definition of field "rld" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"RLD"     ,    0},
     {"NORLD"   ,    1},
     {"NO_RLD"  ,    1},
     {""        ,    0},    /* null string marks the end of mnuemonics */
   },16,1,2,1,0,

   "COND",                /* start definition of field "cond" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"TRUE"       ,   5},
     {"FALSE"      ,  13},
     {"Z"          ,  17},
     {"NOT_Z"      ,  25},
     {"INT"        ,  21}, /* BES 3/21/89 */
     {"N"          ,  16},
     {"NOT_N"      ,  24},
     {"C"          ,  18},
     {"NOT_C"      ,  26},
     {"V"          ,  20},
     {"NOT_V"      ,  28},
     {"LT"         ,  22},
     {"LE"         ,  23},
     {"GE"         ,  30},
     {"GT"         ,  31},
     {"HI"         ,  27},
     {"LS"         ,  19},
     {"Z_MICRO"    ,   1},
     {"NOT_Z_MICRO",   9},
     {"N_MICRO"    ,   0},
     {"NOT_N_MICRO",   8},
     {"C_MICRO"    ,   2},
     {"NOT_C_MICRO",  10},
     {"V_MICRO"    ,   4},
     {"NOT_V_MICRO",  12},
     {"LT_MICRO"   ,   6},
     {"LE_MICRO"   ,   7},
     {"GE_MICRO"   ,  14},
     {"GT_MICRO"   ,  15},
     {"HI_MICRO"   ,  11},
     {"LS_MICRO"   ,   3},
     {""        ,   0},    /* null string marks the end of mnuemonics */
   },17,5,2,13,0,

   "SR_FUNC",            /* start definition of field "sr_func" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"MACRO"  ,   2},
     {"MICRO"  ,   1},
     {"BOTH"   ,   0},
     {"NEITHER",   3},
     {""      ,   0},    /* null string marks the end of mnuemonics */
   },22,2,2,3,0,

   "SR_SOURCE",          /* start definition of field "sr_source" */
   null_routine,  /* These routines do absolutely nothing */
   null_routine,
   {
     {"ALU"   ,   0},
     {"STATUS",   0},
     {"Y"     ,   1},
     {"YBUS"  ,   1},
     {""      ,   0},    /* null string marks the end of mnuemonics */
   },24,1,2,0,0,

   "A_SOURCE",             /* start definition of field "a_source" */
   null_routine,       /* These routines do absolutely nothing */
   null_routine,
   {
     {"IR"      ,   0},
     {"R0"      ,  16},
     {"R1"      ,  17},
     {"R2"      ,  18},
     {"R3"      ,  19},
     {"R4"      ,  20},
     {"R5"      ,  21},
     {"R6"      ,  22},
     {"R7"      ,  23},
     {"R8"      ,  24},
     {"R8"      ,  25},
     {"RA"      ,  26},
     {"RB"      ,  27},
     {"RC"      ,  28},
     {"RD"      ,  29},
     {"RE"      ,  30},
     {"RF"      ,  31},
     {"DATA_IN" ,  32},
     {"SR"      ,  33},
     {""        ,   0},    /* null string makrs the end of mnemonix  */
   },25,6,2,16,0,

   "B_SOURCE",             /* start definition of field "b_source" */
   null_routine,       /* These routines do absolutely nothing */
   null_routine,
   {
     {"IR"      ,   0},
     {"R0"      ,  16},
     {"R1"      ,  17},
     {"R2"      ,  18},
     {"R3"      ,  19},
     {"R4"      ,  20},
     {"R5"      ,  21},
     {"R6"      ,  22},
     {"R7"      ,  23},
     {"R8"      ,  24},
     {"R8"      ,  25},
     {"RA"      ,  26},
     {"RB"      ,  27},
     {"RC"      ,  28},
     {"RD"      ,  29},
     {"RE"      ,  30},
     {"RF"      ,  31},
     {"Q"       ,  32},
     {"MQ"      ,  32},
     {"CONSTANT",  64},
     {"CONST"   ,  64},
     {""        ,   0},    /* null string makrs the end of mnemonix  */
   },31,7,2,16,0,

   "C_SOURCE",             /* start definition of field "c_source" */
   null_routine,       /* These routines do absolutely nothing */
   null_routine,
   {
     {"IRA"     ,   8},
     {"IRB"     ,   0},
     {"R0"      ,  16},
     {"R1"      ,  17},
     {"R2"      ,  18},
     {"R3"      ,  19},
     {"R4"      ,  20},
     {"R5"      ,  21},
     {"R6"      ,  22},
     {"R7"      ,  23},
     {"R8"      ,  24},
     {"R8"      ,  25},
     {"RA"      ,  26},
     {"RB"      ,  27},
     {"RC"      ,  28},
     {"RD"      ,  29},
     {"RE"      ,  30},
     {"RF"      ,  31},
     {""        ,   0},    /* null string makrs the end of mnemonix  */
   },38,5,2,16,0,

   "WE",
   null_routine,
   null_routine,
   {
     {"ENABLE"   ,   0},
     {"DISABLE"  ,   1},
     {"NO_ENABLE",   1},
     {""         ,   0},
   },43,1,2,1,0,

   "ALU_FUNC",
   alu_func_routine,
   null_routine,
   {
     {"SET1H"    , 0x008},
     {"SET1L"    , 0x108},
     {"SET0H"    , 0x018},
     {"SET0L"    , 0x118},
     {"TB1H"     , 0x028},
     {"TB1L"     , 0x128},
     {"TB0H"     , 0x038},
     {"TB0L"     , 0x138},
     {"ABS"      , 0x048},
     {"SMTC"     , 0x058},
     {"ADDI"     , 0x068},
     {"SUBI"     , 0x078},
     {"BADDH"    , 0x088},
     {"BADDL"    , 0x188},
     {"BSUBSH"   , 0x098},
     {"BSUBSL"   , 0x198},
     {"BSUBRH"   , 0x0A8},
     {"BSUBRL"   , 0x1A8},
     {"BINCSH"   , 0x0B8},
     {"BINCSL"   , 0x1B8},
     {"BINCNSH"  , 0x0C8},
     {"BINCNSL"  , 0x1C8},
     {"BXORH"    , 0x0D8},
     {"BXORL"    , 0x1D8},
     {"BANDH"    , 0x0E8},
     {"BANDL"    , 0x1E8},
     {"BORH"     , 0x0F8},
     {"BORL"     , 0x1F8},
     {"SEL"      , 0x010},
     {"SNORM"    , 0x020},
     {"DNORM"    , 0x030},
     {"DIVRF"    , 0x040},
     {"SDIVQF"   , 0x050},
     {"SMULI"    , 0x060},
     {"SMULT"    , 0x070},
     {"SDIVIN"   , 0x080},
     {"SDIVIS"   , 0x090},
     {"SDIVI"    , 0x0A0},
     {"UDIVIS"   , 0x0B0},
     {"UDIVI"    , 0x0C0},
     {"UMULI"    , 0x0D0},
     {"SDIVIT"   , 0x0E0},
     {"UDIVIT"   , 0x0F0},
     {"CLR"      , 0x00F},
     {"BCDBIN"   , 0x07F},
     {"EX3BCH"   , 0x08F},
     {"EX3BCL"   , 0x18F},
     {"EX3C"     , 0x09F},
     {"SDIVO"    , 0x0AF},
     {"BINEX3"   , 0x0DF},
     {"NOP"      , 0x0FF},
     {""         ,     0},
   },44,9,2,0x0FF,0,

   "GROUP1",
   group1_routine,
   null_routine,
   {
     {"ADD"   ,   1},
     {"SUBR"  ,   2},
     {"SUBS"  ,   3},
     {"INCS"  ,   4},
     {"INCNS" ,   5},
     {"INCR"  ,   6},
     {"INCNR" ,   7},
     {"XOR"   ,   9},
     {"AND"   ,  10},
     {"OR"    ,  11},
     {"NAND"  ,  12},
     {"NOR"   ,  13},
     {"ANDNR" ,  14},
     {""      ,   0},
   },44,4,2,6,0,

   "GROUP2",
   group2_routine,
   null_routine,
   {
     {"SRA"   ,   0},
     {"SRAD"  ,   1},
     {"SRL"   ,   2},
     {"SRLD"  ,   3},
     {"SLA"   ,   4},
     {"SLAD"  ,   5},
     {"SLC"   ,   6},
     {"SLCD"  ,   7},
     {"SRC"   ,   8},
     {"SRCD"  ,   9},
     {"MQSRA" ,  10},
     {"MQSRL" ,  11},
     {"MQSLL" ,  12},
     {"MQSLC" ,  13},
     {"LOADMQ",  14},
     {"PASS"  ,  15},
     {""      ,   0},
   },48,4,2,15,0,

   "CIN",
   null_routine,
   null_routine,
   {
     {"YES"    ,   3},
     {"NO"     ,   2},
     {"MICRO_C",   0},
     {"MACRO_C",   1},
     {""       ,   0},
   },53,2,2,2,0,

   "SHIFT_COND",
   null_routine,
   null_routine,
   {
     {"ALWAYS",  0},
     {"Z"     ,  1},
     {"NOT_Z" ,  2},
     {"C"     ,  3},
     {"NOT_C" ,  4},
     {"N"     ,  5},
     {"NOT_N" ,  6},
     {"V"     ,  7},
     {""      ,  0},
   },55,3,2,0,0,

   "MEM_ACCESS",
   null_routine,
   null_routine,
   {
     {"READ" ,   1},
     {"WRITE",   0},
     {"IACK" ,   2},
     {"NONE" ,   3},
     {""     ,   0},
   },58,2,2,3,0,

   "HIBERNATE",
   null_routine,
   null_routine,
   {
     {"HIBERNATE"   ,  1},
     {"HIB"         ,  1},
     {"NOHIBERNATE" ,  0},
     {"NO_HIBERNATE",  0},
     {"NO_HIB"      ,  0},
     {""            ,  0},    /* null string marks the end of mnuemonics */
   },60,1,2,0,0,

   "LD_MAR",
   null_routine,
   null_routine,
   {
     {"LD_MAR"   ,  1},
     {"NO_LD_MAR",  0},
     {""         ,  0},    /* null string marks the end of mnuemonics */
   },61,1,2,0,0,

   "LD_IR",
   null_routine,
   null_routine,
   {
     {"LD_IR"   ,  1},
     {"NO_LD_IR",  0},
     {""        ,  0},    /* null string marks the end of mnuemonics */
   },62,1,2,0,0,

   "LD_DOUT",
   null_routine,
   null_routine,
   {
     {"LD_DOUT"   ,  1},
     {"NO_LD_DOUT",  0},
     {""          ,  0},    /* null string marks the end of mnuemonics */
   },63,1,2,0,0,

/*---------------------------- End of Fields --------------------------*/

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

/********************************************************************
*                 micro_ad_routine(source,findex,value)             *
*********************************************************************
*  See if CONSTANT is defined.
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
*                constant_routine(source,findex,value)              *
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
*                alu_func_routine(source,findex,value)              *
*********************************************************************
*  See if GROUP1 and GROUP2 are defined.
*********************************************************************/

int alu_func_routine(source,findex,value)
FILE *source;
int  findex;
int  *value;
{
  int   local_index;

  local_index = find_field("GROUP1");
  if (fields[local_index].defined != 0)
  {
    error(FIELD_REDEFINITION,line_number,column_number);
  }
  fields[local_index].defined = 1;

  local_index = find_field("GROUP2");
  if (fields[local_index].defined != 0)
  {
    error(FIELD_REDEFINITION,line_number,column_number);
  }
  fields[local_index].defined = 1;

  return(0);

}

/********************************************************************
*                group1_routine(source,findex,value)                *
*********************************************************************
*  See if ALU_FUNC and GROUP2 are defined.
*********************************************************************/

int group1_routine(source,findex,value)
FILE *source;
int  findex;
int  *value;
{
  int   local_index,local_index2;

  local_index = find_field("ALU_FUNC");
  if (fields[local_index].defined != 0)
  {
    local_index2=find_field("GROUP2");
    if(fields[local_index2].defined==0)
        error(FIELD_REDEFINITION,line_number,column_number);
  }
  fields[local_index].defined = 1;

  return(0);
}

/********************************************************************
*                group2_routine(source,findex,value)                *
*********************************************************************
*  See if ALU_FUNC and GROUP1 are defined.
*********************************************************************/

int group2_routine(source,findex,value)
FILE *source;
int  findex;
int  *value;
{
  int   local_index,local_index2;

  local_index = find_field("ALU_FUNC");
  if (fields[local_index].defined != 0)
  {
    local_index2=find_field("GROUP1");
    if(fields[local_index2].defined==0)
        error(FIELD_REDEFINITION,line_number,column_number);
  }
  fields[local_index].defined = 1;

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

