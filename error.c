/********************************************************************
*                            Error.c
*********************************************************************
*  Manage error and warning messages.
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#include <stdio.h>
#include "global.h"
#include "error.h"

struct w_table warning_table[WARNING_TABLE_SIZE];
struct e_table error_table[ERROR_TABLE_SIZE];

/*********************************************************************
 *           warning(code, line_number, column_number)               *
 *********************************************************************
 *  The global structure "warning_table" is updated to include the
 *  warning information passed. Warning codes are determined in the 
 *  include file "masm.h"
 *********************************************************************/

extern void warning(code, line_number, column_number)
int  code;
int  line_number;
int  column_number;
{
   switch(code){

   case SYMBOL_TOO_LONG:
      break;
   default:
      fprintf(stderr,"Invalid warning code. Code=%d. Line=%d. Column=%d\n",
                         code, line_number, column_number);
      exit(3);
   }

   warning_table[wt_length].code = code;
   warning_table[wt_length].line = line_number;
   warning_table[wt_length].column = column_number;
   wt_length++;

   if (wt_length > WARNING_TABLE_SIZE)
   {
      fprintf(stderr,"Too many warnings!\n");
      exit(4);
   }
}

/*********************************************************************
 *           error(code, line_number, column_number)                 *
 *********************************************************************
 *  The global structure "error_table" is updated to include the
 *  error message passed. Warning codes are determined in the 
 *  include file "masm.h"
 *********************************************************************/

extern void error(code, line_number, column_number)
int  code;
int  line_number;
int  column_number;
{
   switch(code){

   case CONSTANT_EXPECTED:
     break;
   case SYNTAX:
      break;
   case SYMBOL_REDEFINITION:
      break;
   case UNDEFINED_SYMBOL:
      break;
   case MISSING_OPERAND:
      break;
   case ILLEGAL_OPERATOR:
      break;
   case DIVIDE_BY_ZERO:
      break;
   case MISSING_PAREN:
      break;
   case UNEXPECTED_PAREN:
      break;
   case EXPECTED_COMMA:
      break;
   case MISSING_OPERATOR:
      break;
   case EXPRESSION_EXPECTED:
      break;
   case INVALID_FIELD:
      break;
   case INVALID_MNEMONIC:
      break;
   case FIELD_REDEFINITION:
      break;
   case FIELD_TOO_LARGE:
      break;
   default:
      fprintf(stderr,"Invalid error code. Code=%d. Line=%d. Column=%d\n",
                       code, line_number, column_number);
      exit(3);
   }

   error_table[et_length].code = code;
   error_table[et_length].line = line_number;
   error_table[et_length].column = column_number;
   et_length++;

   if (et_length > ERROR_TABLE_SIZE)
   {
      fprintf(stderr,"Too many errors!");
      exit(6);
   }
}

/*********************************************************************
 *                  report_errors(listing,line_number)               *
 *********************************************************************
 *  This routine dumps a report of errors and warnings that occured
 *  in the specified line number to both the "listing" and "stdout"
 *  streams.
 *
 *  The errors and warnings are recorded in the structures
 *  "error_table" and "warning_table" in which are kept a code
 *  refering to the error that occurred and the line number and column
 *  number that the error occurred in.
 *********************************************************************/

extern void report_errors(listing, line_number)
FILE *listing;
int  line_number;
{
   int index;
   int space;

   for (index = 0; index < et_length; index++)
      if (error_table[index].line == line_number)
      {
         fprintf(listing,"                    ");
         for (space = 0; space < error_table[index].column; space++)
            fprintf(listing," ");
         fprintf(listing,"^\n");
         print_error(listing, index);

         fprintf(stdout, "Line: %4d, ",line_number);
         print_error(stdout,  index);
      }

   for (index = 0; index < wt_length; index++)
      if (warning_table[index].line == line_number)
      {
         fprintf(listing,"                    ");
         for (space = 0; space < error_table[index].column; space++)
            fprintf(listing," ");
         fprintf(listing,"^\n");
         print_warning(listing, index);

         fprintf(stdout, "Line: %4d, ",line_number);
         print_warning(stdout,  index);
     }
 }

/*********************************************************************
 *                  print_error(output, index)                       *
 *********************************************************************/
extern void print_error(output, index)
FILE *output;
int  index;
{
   int code;

   code = error_table[index].code;

   fprintf(output,"ERROR %d: ",code);

   switch(code)
   {
      case CONSTANT_EXPECTED:
         fprintf(output,"Constant expected.\n");
         break;
      case SYNTAX:
         fprintf(output,"Syntax error.\n");
         break;
        case SYMBOL_REDEFINITION:
         fprintf(output,"Symbol redefinition.\n");
         break;
      case UNDEFINED_SYMBOL:
         fprintf(output,"Undefined symbol.\n");
         break;
      case MISSING_OPERAND:
         fprintf(output,"Missing operand.\n");
         break;
      case ILLEGAL_OPERATOR:
         fprintf(output,"Illegal operator.\n");
         break;
      case DIVIDE_BY_ZERO:
         fprintf(output,"Divide by zero.\n");
         break;
      case MISSING_PAREN:
         fprintf(output,"Missing parenthesis.\n");
         break;
      case UNEXPECTED_PAREN:
         fprintf(output,"Unexpected parenthesis.\n");
         break;
      case EXPECTED_COMMA:
         fprintf(output,"Expected comma.\n");
         break;
      case MISSING_OPERATOR:
         fprintf(output,"Missing operator.\n");
         break;
      case EXPRESSION_EXPECTED:
         fprintf(output,"Expression expected.\n");
         break;
      case INVALID_FIELD:
         fprintf(output,"Invalid field.\n");
         break;
      case INVALID_MNEMONIC:
         fprintf(output,"Invalid mnemonic.\n");
         break;
      case FIELD_REDEFINITION:
         fprintf(output,"Field redefinition.\n");
         break;
      case FIELD_TOO_LARGE:
         fprintf(output,"Field too large.\n");
         break;
      default:
         printf("Invalid error code. Code=%d. Line=%d. Column=%d\n",
                          code, line_number, column_number);
         exit(3);
   }
}

/*********************************************************************
 *                  print_warning(output, index)                     *
 *********************************************************************/
extern void print_warning(output, index)
FILE *output;
int  index;
{
   int code;

   code = warning_table[index].code;

   fprintf(output,"WARNING %d: ",code);

   switch(code)
   {
      case SYMBOL_TOO_LONG:
         fprintf(output,"Symbol too long.           ");
         break;
      default:
         fprintf(stderr,"Invalid warning code. Code=%d. Line=%d. Column=%d\n",
                         code, line_number, column_number);
         exit(3);
   }
 }
