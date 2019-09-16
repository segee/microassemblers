/********************************************************************
*                            Evaluate.c
*********************************************************************
*  Parsing and format conversion routines live here.
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "masm.h"
#include "global.h"
#include "error.h"
#include "evaluate.h"
#include "symbol.h"

/*********************************************************************
 *                  evaluate_expression(source, &value)              *
 *********************************************************************
 *  This function parses the next field and evaluates it as a simple
 *  arithmetic expression.  The operations allowed are +, -, *, /,
 *  .OR, and .AND   Operations are performed as they are found without 
 *  any precedance checking.
 *********************************************************************/

int evaluate_expression(source, value)
FILE *source;
int  *value;
{
   char operator[30];
   int  operand;
   int  operator_result;
   int  operand_result;

   /*
    *  Increment the global variable "evaluate_level".
    *  This will tell us if this is a top level or lower level
    *  call, when we exit.
    */

   evaluate_level++;

   /*  Initialize value to zero. */

   *value = 0;

   /*
    *  Skip spaces and tabs.  Check to see if we have a simple value
    *  or an expression - "(".
    *  Get the value or evaluate the expression
    *  If one is not found, an error has occured.
    */

   skip_white_space(source);

   if ( c == '(' )
   {
      /*
       *  First thing encountered is an expression, so evaluate it.
       *  But first, skip past the "(".
       */

      c = my_getc(source);
      if (evaluate_expression(source,value) == -1)
      {
         evaluate_level--;
         return(-1);   /* If one not found, error */
      }
   }
   else
   if (get_value(source,value) != 0)
   {
      evaluate_level--;
      return(-1);
   }

   /*
    *  Fetch operators and operands until one is not found, at which
    *  point either the expression is done, or an error has occured.
    *
    *  Divide by zero and illegal operators are detected and reported.
    *  However, evaluation of the expression continues to allow detection
    *  of further errors in the expression.
    *
    *  If at this point, we don't find an operator, then we are done.
    */

   if ((operator_result = get_operator(source,operator)) != 0)
      if (evaluate_level == 1)
      {
         evaluate_level--;
         return(0);
      }

   skip_white_space(source);

   if ( c == '(' )
   {
      c = my_getc(source);
      operand_result = evaluate_expression(source,&operand);
   }
   else
      operand_result = get_value(source,&operand);

   /*
    *  As long as we have a valid operator and operand, do the operation
    *  required and attempt to fetch the next operator and operand.
    */

   while ((operator_result == 0)&&(operand_result == 0))
   {
      if(strcmp(operator,"+") == 0)
         *value = *value + operand;
      else if(strcmp(operator,"-") == 0)
         *value = *value - operand;
      else if(strcmp(operator,"*") == 0)
         *value = *value * operand;
      else if(strcmp(operator,"/") == 0)
     {
         if (operand == 0)
            error(DIVIDE_BY_ZERO,line_number, column_number);
         else
            *value = *value / operand;
      }
      else if(strcmp(operator,".OR") == 0)
         *value = *value | operand;
      else if(strcmp(operator,".AND") == 0)
         *value = *value & operand;
      else
         error(ILLEGAL_OPERATOR,line_number, column_number);

      /*
       *  Look ahead to get the next non-whitespace character.
       */

      skip_white_space(source);

      /*
       *  If this character is ")",
       *     and we are in a lower level call, return valid result.
       *     and we are at the top level, return an invalid result.
       */

      if (c == ')')
      {
         c = my_getc(source);

         if (evaluate_level > 1)
         {
            evaluate_level--;
            return(0);
         }
         else
         {
            error(UNEXPECTED_PAREN, line_number, column_number);
            evaluate_level--;
            return(-1);
         }
      }

      /*
       *  If this character is "," or  "\n"
       *     and we are on a lower level call, an error has occured.
       *     and we are on the top level call, we are all done.
       */

      if ((c==',')||(c=='\n'))
      {
         if (evaluate_level > 1)
         {
            evaluate_level--;
            error(MISSING_PAREN, line_number, column_number);
            return(-1);
         }
         else
         {
            evaluate_level--;
            return(0);
         }
      }

      /*  Get the next operator */

      operator_result = get_operator(source,operator);

      /*
       *  Get the next operand.
       *
       *  If the next non-whitespace character is '(', then it is an
       *  expression.  If not, it is a simple value.
       */

      skip_white_space(source);

      if ( c == '(' )
      {
         c = my_getc(source);
         operand_result = evaluate_expression(source,&operand);
      }
      else
         operand_result = get_value(source,&operand);
   }


   /*
    *  We have either reached the end of the expression or we have
    *  encountered an error. Find out which.
    */


   /*
    *  If didn't find an operator, but did find an operand, then we
    *  have found an error.
    */

   if ((operand_result == 0)&&(operator_result == -1))
   {
      error(MISSING_OPERATOR, line_number, column_number);
      evaluate_level--;
      return(-1);
   }

   /*
    *  If we found an operator, but no operand, then we have an error.
    */

   if ((operand_result == -1)&&(operator_result == 0))
   {
      error(MISSING_OPERAND,line_number, column_number);
      evaluate_level--;
      return(-1);
   }

   /*
    *  If we didn't find either, then check to see if the next
    *  character is ')'.  If it is, and this is the top level call,
    *  we have an error.  Otherwise, return a valid result.
    */

   if (c == ')')
   {
      if (evaluate_level>1)
      {
         c = my_getc(source);
         evaluate_level--;
         return(0);
      }
      else
      {
         error(UNEXPECTED_PAREN, line_number, column_number);
         evaluate_level--;
         return(-1);
      }
   }
   else
   if (evaluate_level > 1)
   {
      error(MISSING_PAREN, line_number, column_number);
      evaluate_level--;
      return(-1);
   }

   /*
    *  If we got this far, then we found neither an operator or an
    *  operand, we didn't find a ')', and we are on the top level
    *  call.  This is a valid way to end an expression.
    */

   evaluate_level--;
   return(0);
}

/*********************************************************************
 *                        get_value(source, &value)                  *
 *********************************************************************
 *  This subroutine parses characters from the input stream, source,
 *  and extracts:
 *       1.  a constant value
 *         a) decimal if no postfix is given.
 *         b) hexadecimal if postfix "H" is given.
 *       2.  a label, in which case it searches the symbol table for
 *           its value.
 *
 *  Errors are indicated by returning -1 for a value
 *********************************************************************/

int get_value(source, value)
FILE *source;
int  *value;
{
   char string[N];
   char label[N];
   int  index;

   skip_white_space(source);

   if ((c == '\n') || (feof(source)) || (c == ';'))  /* if at end of line, */
      return(-1);    /* an error has occured. */

   if ((c >= 'A') && (c <= 'Z'))
   {
      /*
       *  A label has been given as a constant value.  Parse the label
       *  and search for it in the symbol table.  If found, return its
       *  value.  If not, report an undefined label error.
       */


      get_token(source, label);

      for (index = 0; index < st_length; index++)
         if (strcmp(symbol_table[index].symbol, label) == 0)
         {
            *value = symbol_table[index].value;
            return(0);
         }

      if ((index == st_length)||(st_length == 0))
      {
         error(UNDEFINED_SYMBOL, line_number, column_number);
         return(-1);
      }
   }
   else
   {
      index = 0;
      while (((c >= 'A')&&(c <= 'F'))||((c >= '0')&&(c <= '9')))
      {
         string[index++] = c;
         c = my_getc(source);
      }
      string[index] = '\0';

      if (index == 0)
      {
         /*
          *  If index is 0, then we did not find any valid hex characters
          *  to convert.  Obviously, there is no value here.
          */

         return(-1);
      }
      else
      if (c == 'H')
      {
         c = my_getc(source);
         *value = convert_hex(string);
         return(0);
      }
      else
      if (c == 'O')
      {
         c = my_getc(source);
         *value = convert_oct(string);
         return(0);
      }
      else
      if (string[index-1]=='B')
      {
         string[index-1] = '\0';
         *value = convert_bin(string);
         return(0);
      }
      else
      if (string[index-1]=='D')
      {
         string[index-1] = '\0';
         *value = convert_dec(string);
         return(0);
      }
      else
      {
         *value = convert_dec(string);
         return(0);
      }
   }
}

/*********************************************************************
 *                        convert_dec(string)                        *
 *                        convert_bin(string)                        *
 *                        convert_hex(string)                        *
 *                        convert_oct(string)                        *
 *********************************************************************
 *  These subroutines convert the string passed to them to a decimal
 *  integer assuming that the string is ASCII coded decimal, binary,
 *  hexidecimal, or octal, respectively.
 *
 *  Error checking of the string is performed in all but convert_hex,
 *  which assumes the string passed into it is a valid ASCII coded
 *  hex number
 *********************************************************************/

int convert_dec(string)
char *string;
{
   int value = 0;

   /* Convert until you hit a character invalid in a decimal number */

   while ((*string >= '0') && (*string <= '9'))
      value = (value * 10) + (*(string++) - '0');

   if (*string == '\0')
      return(value);

   /* If character hit was not NULL, a syntax error has occured */

   else 
   {
      error(SYNTAX, line_number, column_number);
      return(value);
   }
}

int convert_bin(string)
char *string;
{
   int value = 0;

   /*  Convert until you hit a character invalid in a binary number */

   while((*string == '0')||(*string == '1'))
      value = (value * 2) + (*(string++) - '0');

   if (*string == '\0')
      return(value);

   /* If character hit was not NULL, a syntax error has occured */

   else 
   {
      error(SYNTAX, line_number, column_number);
      return(value);
   }
}


int convert_hex(string)
char *string;
{
   int value = 0;

   /* Convert until you hit a NULL character */

   while (*string != '\0')
   {
      value = value * 16;
      if (*string >= 'A' && *string <= 'F')
         value = value + (*(string++) - 'A' + 10);
      else
         value = value + (*(string++) - '0');
   }

   return(value);
}

int convert_oct(string)
char *string;
{
   int value = 0;

   /* Convert until you hit a character invalid in a octal character */

   while ((*string >= '0') && (*string <= '7'))
      value = (value * 8) + (*(string++) - '0');

   if (*string == '\0')
      return(value);

   /* If character hit was not NULL, a syntax error has occured */

   else 
   {
      error(SYNTAX, line_number, column_number);
      return(value);
   }
}


/*********************************************************************
 *                   get_operator(source, operator)                  *
 *********************************************************************
 *  This subroutine parses characters from the input stream, source,
 *  to obtain the next operator.
 *
 *  All whitespace characters are skipped.
 *  An operator is defined to be the characters '+', '-', '/', '*', and
 *  any string starting with '.' and ending with whitespace.
 *
 *  Errors are reported by returning a -1
 *********************************************************************/

int get_operator(source, operator)
FILE *source;
char *operator;
{
   /*
    *  Skip whitespace characters
    */

   skip_white_space(source);

   /*
    *  If we hit the end of the line, or the end of the file, or a comment
    *  before we find the operator, then an error has occured.
    */

   if ((c == '\n') || (feof(source)) || (c == ';'))
      return(-1);

   /*
    *  If the first non-whitespace character is one of the single character
    *  operators, prepare the operator string and return.
    */

   if ((c == '+')||(c == '-')||(c == '/')||(c == '*'))
   {
      *(operator++) = c;
      *operator++ = '\0';
      c = my_getc(source);
      return(0);
   }

   /*
    *  If the first non-whitespace character is '.', then copy the '.' ,
    *  get the token that follows it, if there is one, and return.
    */

   if (c == '.')
   {
      *(operator++) = c;
      c = my_getc(source);

      if (get_token(source, operator) == 0)
         return(0);
      else
         return(-1);
   }

   /*  If neither case was true, we end up here and report an error.  */

   return(-1);
}
