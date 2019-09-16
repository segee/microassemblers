/********************************************************************
*                            Symbol.c
*********************************************************************
*  Routines to parse tokens and write to the symbol table.
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "masm.h"
#include "global.h"
#include "error.h"
#include "symbol.h"

struct s_table symbol_table[SYMBOL_TABLE_SIZE];

/*********************************************************************
 *                   add_st_entry(symbol, value)                     *
 *********************************************************************
 *  Subroutine puts the symbol and value passed as arguments into the
 *  symbol table. The global variable, st_length, is incremented by 1.
 *  If st_length gets too big, a fatal error occurs.
 *
 *  INPUTS:
 *      symbol[N] char    The alpha label to put in the symbol table.
 *      value     int     The value of the label.
 *********************************************************************/

extern void add_st_entry(symbol, value)
char *symbol;
int  value;
{
   int index;

   if (strlen(symbol) > SYMBOL_LENGTH)
      warning(SYMBOL_TOO_LONG, line_number, column_number);

   for (index = 0; index < st_length; index++)
      if (strcmp(symbol_table[index].symbol, symbol) == 0)
      {
         error(SYMBOL_REDEFINITION,line_number, column_number);
         return;
      }

   strncpy(symbol_table[st_length].symbol, symbol, SYMBOL_LENGTH);
   symbol_table[st_length].value = value;

   st_length++;
   if (st_length > SYMBOL_TABLE_SIZE)
   {
      printf("Symbol table too large.");
      exit(5);
   }
}

/*********************************************************************
 *                        get_token(source, token)                   *
 *********************************************************************
 *  This subroutine parses characters from the input stream, source,
 *  to obtain the next token.
 *  A token is found by skipping all whitespace characters.  If the
 *  next character is non-alpha, an error has occured.  If the next
 *  character is alpha, then the token consists of following characters
 *  until the next non-alphanumeric character occures.
 *  The underscore is considered alphanumeric in this case.
 *
 *  Errors are reported by returning a -1
 *********************************************************************/

int get_token(source, token)
FILE *source;
char *token;
{
   skip_white_space(source);

   if ((c == '\n') || (c == '\r') || (feof(source)) || (c == ';'))
      return(-1);

   /*  If first character is not valid in a label, return error */

   if (!((c >= 'A' && c <= 'Z')||(c >= '0' && c <= '9')||(c=='_')))
      return(-1);

   while ((c >= 'A' && c <= 'Z')||(c >= '0' && c <= '9')||(c=='_'))
   {
      *(token++) = c;
      c = my_getc(source);
   }
   *token = '\0';

   return(0);
}

/*********************************************************************
 *                          skip_field(source)                       *
 *********************************************************************
 *  This function skips all characters up to the first comma, EOL,
 *  or EOF character.  If a comma is encountered, it is skipped also.
 *********************************************************************/

void skip_field(source)
FILE *source;
{
   /*
    *  Skip to the next ',' or EOL, or EOF
    *  Skip the ',' if you find one
    */

   while ((c!=',')&&(c!='\0')&&(!feof(source)))
      c = my_getc(source);
   if (c == ',')
      c = my_getc(source);
}

/*********************************************************************
 *                         find_field(field)                         *
 *********************************************************************
 *  This function looks through the .name field of the "fields"
 *  structure, looking for a match with the string passed in as
 *  "field".  An index into the structure is returned.
 *  -1 is returned if the field is not found.
 *********************************************************************/

int find_field(field)
char field[N];
{
int  index = 0;

   while (fields[index].name[0] != '\0')
   {
      if (strcmp(fields[index].name, field) == 0)
         return(index);
      else
         index++;
   }

   return(-1);
}

/*********************************************************************
 *                  find_mnemonic(findex, mnemonic)                  *
 *********************************************************************
 *  This function looks through the .field_mnu.name field of the
 *  "fields" structure, looking for a match with the string passed in
 *  as "mnemonic" for the specified field.  An index into the structure
 *  is returned.  A -1 is returned if the mnemonic is not found.
 *********************************************************************/

int find_mnemonic(findex,mnemonic)
int  findex;
char mnemonic[N];
{
int  index = 0;

   while (fields[findex].field_mnu[index].name[0] != '\0')
   {
      if (strcmp(fields[findex].field_mnu[index].name, mnemonic) == 0)
         return(index);
      else
         index++;
   }

   return(-1);
}

/*********************************************************************
 *                 add_field(current_word, findex, value)            *
 *********************************************************************
 *  This function merges the value of the indicated field into the
 *  current microword as follows:
 *
 *  1. Mark this field as defined.
 *  2. Make sure the value is within the limits specified by the
 *     variable "width" in the structure "fields" for this field.
 *  
 *  Errors are reported.
 *********************************************************************/

extern int  add_field(current_word, findex, value)
unsigned int current_word[MAX_WORD_SIZE];
int  findex;
unsigned int value;
{
   int index;
   int left_shift;
   int bits_left;

   if (fields[findex].defined == 0)
      fields[findex].defined = 1;
   else
   {
      error(FIELD_REDEFINITION, line_number, column_number);
      return(0);
   }

   if (value >= (1 << fields[findex].width))
   {
      error(FIELD_TOO_LARGE, line_number, column_number);
      return(0);
   }

   /*
    *  Determine the element of the integer array we need to start putting
    *  the value into.  Also determine, the bit offset into this integer.
    */

   index      = fields[findex].position / 16;
   left_shift = fields[findex].position % 16;
   current_word[index] = current_word[index] | (value << left_shift);
   index++;
   value = value >> (16-left_shift);
   bits_left  = fields[findex].width - (16 - left_shift);

   while (bits_left > 0)
   {
      current_word[index] = current_word[index] | value;
      value = value >> 16;
      bits_left = bits_left - 16;
   }

   return(0);
}
