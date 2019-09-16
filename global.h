/********************************************************************
*                            Global.h
*********************************************************************
*  Contains declarations for the global variables used in all modules
*  of the assembler
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

/*********************************************************************
 *  Define variables for global use
 *********************************************************************/

extern char c;                    /* holds current input character */

extern int  st_length;            /* length of the symbol table    */
extern int  wt_length;            /* length of the warning table   */
extern int  et_length;            /* length of the error table     */
extern int  line_number;          /* current line number           */
extern int  column_number;        /* current column number         */
extern int  evaluate_level;       /* the depth of parenthesis      */
extern int  num_of_words;         /* number of 16 bit words 
                                  /*     in micro-word             */
