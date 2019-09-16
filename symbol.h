/********************************************************************
*                            Symbol.h
*********************************************************************
*  Contains labels, data structures, and variable declarations
*  related to the symbol table.  Also contains function prototypes 
*  for all functions that manipulate the symbol table.
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#define SYMBOL_LENGTH          32
#define SYMBOL_TABLE_SIZE     200

/*********************************************************************
 *  Define the structure for the symbol table.
 *********************************************************************/

struct s_table
{
   char symbol[SYMBOL_LENGTH];
   int  value;
};

extern struct s_table symbol_table[SYMBOL_TABLE_SIZE];

extern  void add_st_entry();
extern  int get_token();
extern  void skip_field();
extern  int find_field();
extern  int find_mnemonic();
extern  int add_field();
