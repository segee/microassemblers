/********************************************************************
*                            Error.h
*********************************************************************
*  Contains the labels, structures, and variables used by the modules
*  reporting errors and warnings.  Also contains the function proto-
*  types for the function defined in the source file "error.c"
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#define ERROR_TABLE_SIZE      100
#define WARNING_TABLE_SIZE    100

#define CONSTANT_EXPECTED     100
#define SYNTAX                101
#define SYMBOL_REDEFINITION   102
#define UNDEFINED_SYMBOL      103
#define DIVIDE_BY_ZERO        104
#define MISSING_OPERAND       105
#define ILLEGAL_OPERATOR      106
#define MISSING_PAREN         107
#define UNEXPECTED_PAREN      108
#define EXPECTED_COMMA        109
#define MISSING_OPERATOR      110
#define EXPRESSION_EXPECTED   111

#define INVALID_FIELD         200
#define INVALID_MNEMONIC      201
#define FIELD_REDEFINITION    202
#define FIELD_TOO_LARGE       203

#define SYMBOL_TOO_LONG       300

/*********************************************************************
 *  Define structures for the the error table, and the warning table.
 *********************************************************************/

struct w_table
{
   int code;
   int line;
   int column;
};

struct e_table
{
   int code;
   int line;
   int column;
};

extern struct w_table warning_table[WARNING_TABLE_SIZE];
extern struct e_table error_table[ERROR_TABLE_SIZE];

extern  void error();
extern  void warning();
extern  void report_errors();
extern  void print_error();
extern  void print_warning();
