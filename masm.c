/********************************************************************
*                            Masm.c
*********************************************************************
*  All related system include files are here whether or
*  not they are necessary for functionality for the sake
*  of completeness, argument type checking, etc.
*  Also, necessary declarations related to the functional
*  blocks are defined in separate include files.
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#include <stdio.h>    
#include <string.h>
#include <ctype.h>
#include "masm.h"      
#include "global.h"   
#include "trans.h"
#include "error.h"
#include "symbol.h"
#include "buffer.h"
#include "evaluate.h"

/*********************************************************************
 *  NOTE!!!
 *    In this program, whitespace refers to spaces and tabs.  Note
 *  that this departs from the normal C definition which would include
 *  end-of-line characters.
 *
 *  The code assumes that the size of an integer is at least 16 bits.
 *  Modifications to the code will need to be made if the integer size
 *  is only 8 bits, perhaps making the size of the micro-word buffer
 *  long int.
 *********************************************************************/

/*********************************************************************
 *  Define variables for global use
 *********************************************************************/

char c;                          /* holds current input character */

int  st_length = 0;              /* length of the symbol table    */
int  wt_length = 0;              /* length of the warning table   */
int  et_length = 0;              /* length of the error table     */
int  line_number = 1;            /* current line number           */
int  column_number = 0;          /* current column number         */
int  evaluate_level = 0;         /* indicates the depth of
                                      parenthesis                 */
int  num_of_words;               /* number of 16 bit words in
                                      micro-word                  */


/*********************************************************************
 *  Here starts the main part of the code.
 *********************************************************************/

extern  int main();  

int main(argc, argv)
int  argc;
char **argv;
{
   /******************************************************************
    *  Define the local variables for the main routine.
    ******************************************************************/

   FILE *listing;             /* output stream for the listing file  */
   FILE *line_input;          /* used for line by line access        */
   FILE *source;              /* used for char by char access        */
   int  object;               /* output stream for the object file   */

   int  address_counter = 0;
   int  already_printed = 0;  /* Set when END_S is found.            */
   int  buffer_index = 0;     /* Index into the object output buffer */
   unsigned int current_word[MAX_WORD_SIZE];
                              /* holds current micro word            */
   int  ch;                   /* used in processing option flags     */
   int  findex = 0;           /* field index variable                */
   int  index;                /* general purpose index               */
   int  mindex = 0;           /* mnemonic index variable             */
   char object_buffer[256];   /* buffer for absolute file            */
   int  pre_value = 0;        /* value returned by pre-field function*/
   int  result;               /* status of function call             */
   int  sort_ind[SYMBOL_TABLE_SIZE]; 
                              /* used to sort the symbol table       */
   int  srec_out = 0;         /* srec output flag (output if 1)      */
   int  started_line;         /* used in formatting listing file     */
   int  sym_out = 1;          /* symbol table output flag            */
   int  value;                /* general purpose variable            */
   int  (*function)();        /* a pointer to a function.  Used to   */
                              /* execute the pre- and post- fields   */
                              /* in a syntax acceptable to the VAX.  */

   char current_line[N];      /* string to hold text of current line */
   char field[N];             /* string to hold field names          */
   char label[N];             /* string to hold label names          */
   char lst_file[N];          /* string holding name of listing file */
   char mnemonic[N];          /* string to hold mnemonic names       */
   char obj_file[N];          /* string holding name of object file  */
   char src_file[N];          /* string holding name of source file  */
   char token[N];             /* string to hold token names          */

   int  compare();  
   void Myqsort(); 
   void Myswap(); 

   /*********************************************************************
    *  The following lines declare the variables and the function
    *  prototype for the "get option" function, getopt.  This simulates
    *  the function available in UNIX to parse option flags; characters
    *  preceeding by "-" or "/"
    *********************************************************************/

   extern int  getopt();
   extern int  opterr, optind, optopt;
   extern char *optarg;

   /******************************************************************
    *  Calculate the number of integer words in the micro-word.
    *  Make sure the result is a multiple of 2.
    *  Assume the size of an integer is 16 bits.  If it is really
    *  greater, than we'll waste space.  Oh, well!
    *  This software is unlikely to be run on a machine that has an
    *  8 bit integer.
    ******************************************************************/

   num_of_words = micro_word_width / 16;
   if (num_of_words * 16 != micro_word_width)
      num_of_words++;
   micro_word_width = num_of_words * 16;

   /*
    *  Initialize current micro_word
    */

   for (index = 0; index < MAX_WORD_SIZE; index++)
      current_word[index] = 0;

   /*
    *  Let's identify ourselves to the user.
    */

   printf(PROG_TITLE);

   /*
    *  Process the option switches using getopt
    *  The flags have meanings specified in the
    *  usage section of case '?'
    */

   while ((ch = getopt(argc, argv, "rs")) != EOF)
      switch (ch)
      {
         case 'r':
            srec_out = 1;
            break;
         case 's':
            sym_out = 0;
            break;
         case '?':
            fprintf(stderr,"Usage: masm [-rs] <filename>\n");
            fprintf(stderr,"  r : output Srecords\n");
            fprintf(stderr,"  s : suppress Symbol table\n");
            exit(7);
            break;
      }
   argc -= optind;
   argv += optind;  

   /*
    *  Open the source file from the argument call line for input.
    *  If a file was not specified, print an error message and exit.
    */

   if (argc < 1) 
   {
      /*
       *  If user didn't supply a source file name, prompt for one.
       */

#ifdef THINK_C
          /*
           *  Macs have no command line arguments.  Ask for them...
           */
      char Optstr[100];
      int Optindex;
      printf("Options available:\n");
      printf("       r:   output Srecords\n");
      printf("       s:   suppress Symbol table\n");
      printf("   [return] for no options\n\n Options: ");
      gets(Optstr);
      printf("\n");
      Optindex = 0;
      while (Optstr[Optindex] != '\0')
         switch (Optstr[Optindex++])
         {
            case 'r':
            case 'R':
               srec_out = 1;
               break;
            case 's':
            case 'S':
               sym_out = 0;
               break;
            default:
               break;      
          }
#endif 

/*
 * Now get the file name...
 */
      printf("Source file[.SRC]:");
      gets(src_file);
   }

   if (argc >= 1) 
   {
      /*
       *  If only one argument was given, then the argument must be
       *  the name of the source file.  Check to see if user supplied
       *  a file extension, if not add ".src" to the src_file
       */

      strcpy(src_file,*argv);
   }

   index = 0;
   while(src_file[index] != '\0' && src_file[index] != '.')
      index++;
   if (src_file[index] == '\0')
   {
      strcat(src_file, ".src");
   }

   /*
    *  Create the listing and object filenames.  These will be taken
    *  from the major portion of the source filename with the ".src"
    *  replaced by ".lst" and ".obj"
    */

   index = 0;
   while((src_file[index] != '.') && (src_file[index] != '\0'))
   {
      lst_file[index] = obj_file[index] = src_file[index];
      index++;
   }
   lst_file[index] = obj_file[index] = '\0';
   strcat(obj_file, ".obj");
   strcat(lst_file, ".lst");

   /*
    *  Open the source file for read only access.
    */

   if ((source = fopen(src_file,"r")) == NULL)
   {
      fprintf(stderr,"Error opening source file %s.\n",src_file);
      exit(2);
   }

   /*
    *  Initialize the symbol table to all null strings and 0 values
    */

   for (index=0; index <= 100; index++)
   {
      symbol_table[index].symbol[0] = '\0';
      symbol_table[index].value  = 0;
   }


   /*
    *  The assembly source file should contain an assembler indentifier
    *  on the first line of the source code contained in quotes.
    *  Get this identifier and make sure it matches with the global
    *  variable "assembler_id"
    */

   c = my_getc(source);

   if (c != '"')
   {
      fprintf(stderr,"Syntax error on line 1.  Can not continue.\n");
      exit(4);
   }

   c = my_getc(source);
   get_token(source, token);

   if (c != '"')
   {
      fprintf(stderr,"Syntax error on line 1.  Can not continue.\n");
      exit(4);
   }

   c = my_getc(source);
   if (skip_to_eol(source) != 0)
      error(SYNTAX, line_number, column_number);

   if (strcmp(token, assembler_id) != 0)
   {
      fprintf(stderr,"Assembler id mismatch.");
      fprintf(stderr,"  ID is %s.  Should be %s.\n",token, assembler_id);
      exit(4);
   }


   /******************************************************************
    * Here starts the first pass on the source code.
    *
    * The functions performed are:
    *   1.  Parse input lines for EQU's, ORG's, and label definitions.
    *   2.  Build the symbol table as we find valid symbols and values.
    *   3.  Report errors as we find them.
    ******************************************************************/

   while(!feof(source))  /*  repeat until we hit the end of the file */
   {
      line_number++;
      column_number = 0;
      c = my_getc(source);

      /*
       *  If the character in the first column is the EOL or the EOF or
       *  a comment, then we don't need to process this line, so skip
       *  to the end of the line.
       */

      if ((c!='\n')&&(c!='\r')&&(c!=';')&&(!feof(source)))
      {
         if ((c >= 'A') && (c <= 'Z'))
         {
            /*********************************************************
             *  Character in first column is an alpha character,
             *  therefore, there must be a label defined on this line.
             *  Parse the label and determine a value.
             *  Make an entry in the symbol table.
             *
             *  If there is an "EQU" on this line, then evaluate the
             *  expression that follows and equate it to the symbol.
             *
             *  If there is an "ORG" on this line, then evaluate the
             *  expression that follows, equate the result to the
             *  symbol, and set the address counter equal to this
             *  value.
             *
             *  If there is anything else, do a minimal parse on the
             *  fields looking for the END_S statement at the end.
             *  If there is an END_S, increment the address counter.
             *  If not, do nothing.
             *
             *  Report errors as you find them.
             *********************************************************/

            /* get the label and make sure there is whitespace or EOL
             * after it. 
             */

            if ((get_token(source,label)==0)
                             &&((c==' ')||(c=='\t')||(c=='\n')||(c=='\r')))
            {
               if (get_token(source,token)==0) /* get the token that follows */
               {
                  if (strcmp(token, "EQU") == 0)
                  {
                     if (evaluate_expression(source, &value) == 0)
                        add_st_entry(label, value);
                     else
                        error(EXPRESSION_EXPECTED, line_number, column_number);
                  }
                  else
                  if (strcmp(token, "ORG") == 0)
                  {
                     if (evaluate_expression(source, &value) == 0)
                     {
                        /*
                         *  If there is a valid expression after the
                         *  "ORG" add the symbol to the symbol table and
                         *  make the address counter equal to that value
                         */

                        add_st_entry(label, value);
                        address_counter = value;
                     }
                     else
                        error(SYNTAX, line_number, column_number);
                  }
                  else
                  {
                     /*
                      *  If the token was not either "ORG" or "EQU"
                      *  then process the fields that follow looking
                      *  for the "END_S" token.  Fields must be ended
                      *  with a comma and the "END_S" must have only
                      *  whitespace or a comment after it.
                      *
                      *  If there is an "END_S", increment the address
                      *  counter.
                      */

                     add_st_entry(label, address_counter);

                     while((strcmp(token,"END_S")!=0)
                            &&(c!='\n')&&(c!='\r')&&(c!=';')&&(!feof(source)))
                     {
                        while ((c!=',')&&(c!='\n')&&(c!='\r')&&
                                         (c!=';')&&(!feof(source)))
                           c = my_getc(source);

                        if (c==',')
                        {
                           /*  Look for next field */

                           c = my_getc(source);
                           get_token(source, token);
                        }
                        else
                           error(EXPECTED_COMMA, line_number, column_number);
                     }

                     if (strcmp(token, "END_S") == 0)
                        address_counter++;
                  }
               }
               else
                  add_st_entry(label, address_counter);
            }
            else
               error(SYNTAX, line_number, column_number);
         }
         else
         if ((c==' ')||(c=='\t'))
         {
            /*********************************************************
             *  Character in column one is not an alpha character, and
             *  is either a space or a tab, therefore, there is not a
             *  label on this line.
             *
             *  First skip all whitespace.  If we reach the EOL,
             *  we are all done with this line
             *
             *  If there is an "ORG" on this line, then evaluate the
             *  expression that follows and set the address counter
             *  equal to this value.
             *
             *  If there is anything else, do a minimal parse on the
             *  fields looking for the END_S statement at the end.
             *  If there is an END_S, increment the address counter.
             *  If not, do nothing.
             *
             *  Report errors as you find them.
             *********************************************************/

            skip_white_space(source);

            if ((c!='\n')&&(c!='\r')&&(c!=';')&&(!feof(source)))
            {
               if(get_token(source,token)==0)
               {
                  if (strcmp(token, "ORG") == 0)
                  {
                     if (evaluate_expression(source,&value) == 0)
                        address_counter = value;
                     else
                        error(EXPRESSION_EXPECTED, line_number, column_number);
                  }
                  else
                  {
                     while ((strcmp(token,"END_S")!=0)
                             &&(c!='\n')&&(c!='\r')&&(c!=';')&&(!feof(source)))
                     {
                        while ((c!=',')&&(c!='\n')&&(c!='\r')
                                          &&(!feof(source))&&(c!=';'))
                           c = my_getc(source);

                        if (c==',')
                        {
                           /*  Look for next field */

                           c = my_getc(source);
                           get_token(source, token);
                        }
                        else
                           error(EXPECTED_COMMA, line_number, column_number);
                     }

                     if (strcmp(token, "END_S") == 0)
                        address_counter++;
                  }
               }
               else
                  error(SYNTAX, line_number, column_number);
            }
         }
         else
         {
            /*
             *  There was some character in the first column that was not
             *  an alpha character or whitespace.
             *  This is a syntax error.
             */

            error(SYNTAX, line_number, column_number);
         }
      }

      /*
       *  We are done processing this line.
       *  Everything after this should be whitespace, end of line, end of
       *  file, or a comment.
       *  If anything else shows up, we've got an error.
       */

      if (skip_to_eol(source) != 0)
         error(SYNTAX, line_number, column_number);
   }

   /******************************************************************
    *  We are all done with the first pass on the source code.
    *  Close the input file and return the line and column number
    *  counters to initial values.
    *
    *  Also, re-open the source file.  This time do it twice.  Once
    *  for character by character access and the other for line by
    *  line access.
    *  Also, open the files for the assembly listing.
    ******************************************************************/

   fclose(source);

   line_number = 0;
   column_number = 0;
   address_counter = 0;

   if ((source = fopen(src_file,"r")) == NULL)
   {
      fprintf(stderr,"Error opening source file %s.\n",src_file);
      exit(2);
   }

   if ((line_input = fopen(src_file,"r")) == NULL)
   {
      fprintf(stderr,"Error opening source file %s.\n",src_file);
      exit(2);
   }

   if ((listing = fopen(lst_file,"w")) == NULL)
   {
      fprintf(stderr,"Error opening listing file %s.\n",lst_file);
      exit(2);
   }

   /*
    *  Open the file for the absolute object file.
    *  Output the 4 byte header.
    */

   if ((object = OPENFILE) == -1)
   {
      fprintf(stderr,"Error opening object file %s.\n",obj_file);
      exit(2);
   }

   object_buffer[0] = object_buffer[2] = (micro_word_width / 256);
   object_buffer[1] = object_buffer[3] = (micro_word_width % 256);
   object_buffer[4] = object_buffer[5] = 0;
   object_buffer[6] = object_buffer[7] = 0;
   for (index = 0; index < 8; index++)
      write(object, &object_buffer[index], 1);


   fprintf(listing,"FILE: %s\n\n",src_file);
   fprintf(listing,"LOCATION OBJECT LINE     SOURCE LINE\n\n");

   while (!feof(source))
   {
      line_number++;
      column_number = 0;
      c = my_getc(source);
      started_line = 0;

      if (fgets(current_line,N,line_input) != NULL)
      {

         /***************************************************************
          *  If the character in the first column is a space, skip
          *  whitespace to the end of the line.
          *  If the character in the first column is a letter, skip
          *  characters until you hit whitespace, and then skip the
          *  whitespace to the end of the line.
          *
          *  Since most syntax errors would have been caught by now, we
          *  don't need look as closely as in pass one.
          ***************************************************************/

         if ((c==' ')||(c=='\t'))
         {
            skip_white_space(source);
         }
         else
         if ((c>='A')&&(c<='Z'))
         {
            get_token(source, token);

            skip_white_space(source);
         }
         else
         {
            /*
             *  There is a syntax error on this line, but we should have caught
             *  it in the first pass, so skip this line.
             */

            skip_to_eol(source);
         }

         while((c!='\n')&&(c!='\r')&&(c!=';')&&(!feof(source)))
         {
            /************************************************************
             *  Process tokens on each line as you find them. They can be:
             *
             *  'ORG'   - evaluate expression that follows and update
             *            address counter.
             *  'EQU'   - skip to end of line.
             *  'END_S' - End of word definition.  Do end of word
             *            processing.
             *
             *  Anything else must be a field mnemonic.
             ************************************************************/

            if (get_token(source, field) == 0)
            {
               if (strcmp(field, "ORG") == 0)
               {
                  if (evaluate_expression(source, &value) == 0)
                     address_counter = value;

                  skip_to_eol(source);
                  started_line = 2;
                  fprintf(listing,"               ");

                  /*
                   *  Flush the object output buffer and start a new data
                   *  record.
                   */

                  flush_buffer(object, object_buffer, &buffer_index,
                               address_counter);
               }
               else
               if (strcmp(field, "EQU") == 0)
               {
                  skip_to_eol(source);

                  started_line = 2;
                  fprintf(listing,"         ");
                  for (index = 0; index <= st_length; index++)
                  {
                     if (strcmp(token, symbol_table[index].symbol) == 0)
                        break;
                  }

                  fprintf(listing,"<%04X>",symbol_table[index].value);
               }
               else
               if (strcmp(field, "END_S") == 0)
               {
    /*****************************************************************
     *  Finish the definition of this microword. Do the post-processing
     *  on this word and start the preprocessing on the next word.
     *
     *  The post-processing tasks are:
     *   1.  Copy the defaults for undefined fields into the microword.
     *   2.  Execute the application specific post-processing routine.
     *   3.  Put the word into the output buffer.  If the buffer is 
     *       full, write another entry into the object file.
     *
     *  The pre-processing tasks are:
     *   1.  Increment the address counter.
     *   2.  Clear the current microword and the 'defined' fields.
     *   3.  Execute the application specific pre-processing routine.
     *****************************************************************/

           /*
            *  Skip whitespace after the END_S to avoid it being
            *                  called an error
            */

                  skip_white_space(source);
                  started_line = 1;

                  /*
                   * Insert default values for all undefined fields
                   */

                  index = 0;
                  while ( fields[index].name[0] != '\0') /* for all fields */
                  {
                     if (fields[index].defined == 0)
                        add_field(current_word, index,
                                  fields[index].default_value);
                     index++;
                  }

                  /* Execute the post-word processing routine */

                  post_word(source);

                  /*  Print the current address counter and the microword in
                   *  two bytes per line.  Set the flag "already_printed" to
                   *  indicate that we've already print the rest of the line.
                   */

                  fprintf(listing,"  %04X ",address_counter);
                  fprintf(listing,"%04X%04X",
                                  current_word[num_of_words - 1] & 0xFFFF,
                                  current_word[num_of_words - 2] & 0xFFFF);

                  fprintf(listing," %04d %s",line_number,current_line);
                  if (feof(line_input))
                     fprintf(listing,"\n");

                  index = num_of_words - 2;
                  while (index > 1)
                  {
                     fprintf(listing,"       %04X%04X\n",
                             current_word[index-1] & 0xFFFF,
                             current_word[index-2] & 0xFFFF); /* MP03 */

                     index -= 2;
                  }

                  while (index > 0)
                  {
                     fprintf(listing,"       %04X\n",
                             current_word[index-1] & 0xFFFF);
                     index --;
                  }

                  already_printed = 1;

                  /*
                   *  Check to see if there is any more room in the output
                   *  buffer for the next microword.
                   *  If there isn't, flush the buffer and start a new one.
                   *  Insert the microword into the output buffer.
                   */

                  if ((buffer_index + num_of_words) > 127)
                  {
                     flush_buffer(object, object_buffer, &buffer_index,
                                  address_counter);
                  }

                  /* if no initial org set up buffer anyway */
                  if((address_counter==0)&&(buffer_index==0))
                     flush_buffer(object, object_buffer, &buffer_index,
                                  address_counter);

                  insert_word(current_word, object_buffer, &buffer_index);

                  /*
                   *  Get ready for the next micro-word, by clearing this one.
                   */

                  for (index=0; index < num_of_words; index++)
                     current_word[index] = 0;

                  index = 0;
                  while (fields[index].name[0] != '\0') /* for all fields */
                  {
                     fields[index].defined = 0;
                     index++;
                  }

                  address_counter++;

                  /*  Execute the pre-word processing routine  */

                  pre_word(source);

               }
               else
               {
                  /*
                   *  Token must be a field.  Look through the data structure
                   *  for this field name.
                   */

                  findex = find_field(field);
                  if (findex == -1)
                  {
                     /*
                      *  We didn't find this field in the data structure,
                      *  therefore, this is an error.
                      */

                     error(INVALID_FIELD, line_number, column_number);

                     skip_field(source);
                  }
                  else
                  {
                     /*
                      *  We found the field name in the data structure, now
                      *  either get the mnemonic that follows or evaluate
                      *  the expression that follows.
                      *
                      *  The next non-whitespace character must be '='. 
                      *  Otherwise, report an error.
                      */

                     skip_white_space(source);
                     if (c != '=')
                     {
                        error(SYNTAX, line_number, column_number);

                        skip_field(source);
                     }
                     else
                     {

                        c = my_getc(source);      /* Move past the '='. */

                        /*
                         *  Execute the pre_field processing routine.
                         *  The contortions here are necessary to make
                         *  Vax C happy.  Other C's are quite happy
                         *  with the following:
                         *
                         *  fields[findex].pre_field(.....);
                         */

                        pre_value = 0;
                        value = 0;
                        function = fields[findex].pre_field;
                        result = (*function)(source,findex,&pre_value);
                        if (result == -1)
                        {
                           error(INVALID_MNEMONIC,line_number,column_number);
                        }
                        else
                        if (result != 0 && result != 1)
                        {
                           fprintf(stderr,
                              "Invalid return code from pre-process routine.\n");
                           fprintf(stderr,"  findex = %d\n",findex);
                        }
                        else
                        if (fields[findex].type == 1)
                        {
                           /*
                            *  Type 1 fields are open and allow expressions to
                            *  be given as values.  If a valid expression is not
                            *  found, flag an error.
                            *  Add value found to current word
                            */

                         if(evaluate_expression(source, &value)!=0 && result==0)
                           error(EXPRESSION_EXPECTED,line_number,column_number);
                        }
                        else
                        if (fields[findex].type == 2)
                        {
                           /*
                            *  Type 2 fields are closed and require that a pre-
                            *  defined set of mnemonics be used.
                            */

                           if (get_token(source, mnemonic) != 0 && result == 0)
                              error(SYNTAX, line_number, column_number);
                           else
                           {
                              if (result == 0)
                              {
                                mindex = find_mnemonic(findex, mnemonic);
                                if (mindex == -1)
                                  error(INVALID_MNEMONIC,line_number,
                                         column_number);

                                value = fields[findex].field_mnu[mindex].value;
                              }
                           }
                        }
                        else
                        {
                           fprintf(stderr,
                              "Compiler structure error: invalid type.\n");
                           fprintf(stderr,"   Field index is %d.\n", findex);
                           skip_field(source);
                        }

                        /*
                         *  Add the value found by OR'ing the pre-processing
                         *  result with the normal processing result to the
                         *  current word.
                         */

                        add_field(current_word, findex, (pre_value | value));

                        /*
                         *  skip white space.  Make sure there is a comma
                         *  after the mnemonic.
                         */

                        skip_white_space(source);

                        if (c == ',')
                        {
                           c = my_getc(source);
                           skip_white_space(source);
                        }
                        else
                        {
                           error(SYNTAX, line_number, column_number);

                           skip_field(source);
                        }
                     }

                     /*
                      *  Done processing the field.
                      *  Execute the post field processing routine.
                      */

                     function = fields[findex].post_field;
                     (*function)(source,findex,&mindex);
                  }
               }
            }
            else
            {
               /*
                * We have found some character that does not
                * properly start a token.  Report the error and
                * skip all invalid characters
                */

               error(INVALID_FIELD,line_number,column_number);
               while (c!='\n'&&(c!='\r')&&!feof(source)
                             &&(c!=';')&&!(c>'A'&&c<'Z'))
                  c = my_getc(source);
            }
         }

         skip_to_eol(source);

         if (started_line == 0)
            fprintf(listing,"               ");

         /*  If the flag "already_printed" is set, then don't print
          *  the rest of the line.
          */

         if (already_printed == 0)
         {
            fprintf(listing," %04d %s",line_number,current_line);
            if (feof(line_input))
               fprintf(listing,"\n");
         }
         already_printed = 0;


         report_errors(listing, line_number);
      }
   }

   /*
    *  Flush the buffer of any code that might be in it.
    */

   flush_buffer(object, object_buffer, &buffer_index, address_counter);

   /* Write block with zero byte count */

   for(index=0; index<6; index++)
      write(object,&object_buffer[index],1);

   /*   Report the number of errors.  */

   fprintf(listing,"\nErrors   = %d\n",et_length);
   fprintf(listing,"Warnings = %d\n",wt_length);
   fprintf(stdout, "\nErrors   = %d\n",et_length);
   fprintf(stdout, "Warnings = %d\n",wt_length);

   /*
    *  Sort indexes into the symbol table and add at the end
    *  of the assembly listing (if requested)
    */

   if (sym_out == 1) 
   {
      for (index = 0; index < st_length; index++)
         sort_ind[index] = index;
/*
 *  The Think C implementation crashes when it uses qsort.  I think
 *  the library function is buggy.  Substituted my own quicksort.
 */
 
/*    qsort((void *)sort_ind,st_length,sizeof(int),compare); */
      Myqsort(sort_ind,0,st_length-1);   

      fprintf(listing,"\n\nSymbol Table:\n\n");
      for (index = 0; index < st_length; index++)
      {
         findex = sort_ind[index];
         fprintf(listing,"%-32s   %04X\n",symbol_table[findex].symbol,
                                          symbol_table[findex].value);
      }
   }

   fclose(source);
   fclose(line_input);
   fclose(listing);
   close(object);

   /*   Create the s-record files if requested */

   if (srec_out == 1) 
      srecords(src_file); 

   return(0);
}

/*
 *  This is the quicksort implementation to replace the built-in qsort.
 *  (Taken almost directly from K&R 2nd ed. pg. 110)
 */

void Myswap(v,i,j)
int v[];
int i,j;
{
    int temp;
    
    temp=v[i];
    v[i]=v[j];
    v[j]=temp;
}

void Myqsort(v,left,right)
int v[];
int left, right;
{
    int i, last;
    
    if (left>=right)
       return;
    Myswap(v,left,(left + right)/2);
    last=left;
    for (i=left+1;i<=right;i++)
       if (compare(&v[i],&v[left]) < 0)
          Myswap(v,++last,i);
    Myswap(v,left,last);
    Myqsort(v,left,last-1);
    Myqsort(v,last+1,right);
}


/*********************************************************************
 *                     skip_white_space(source)                      *
 *********************************************************************
 *  This function compares two symbol table symbol entries.
 *  This function has a form strictly defined by the Microsoft C
 *  specifications.  It compares the values pointed to by the two
 *  arquments and returns a value indicating less than, greater than,
 *  or equivalent to.  It also works with other C's.
 *********************************************************************/

int compare(arg1, arg2)
int *arg1;
int *arg2;
{
   return(strcmp(symbol_table[*arg1].symbol,symbol_table[*arg2].symbol));
}

/*********************************************************************
 *                     skip_white_space(source)                      *
 *********************************************************************
 *  This function skips whitespace characters in the input stream.
 *********************************************************************/

void skip_white_space(source)
FILE* source;
{
   while ((c==' ')||(c=='\t'))
      c = my_getc(source);
}

/*********************************************************************
 *                         skip_to_eol(source)                       *
 *********************************************************************
 *  Skip all characters until the newline character is reached.
 *  If anything is found except for whitespace, EOL, EOF, or comment,
 *  an error is returned.
 *********************************************************************/

int skip_to_eol(source)
FILE *source;
{
   while ((c==' ')||(c=='\t'))
      c = my_getc(source);

   if ((c=='\n')||(feof(source)))
      return(0);

   if (c==';')
   {
      while ((c!='\n')&&(!feof(source)))
         c = my_getc(source);

      return(0);
   }

   while ((c!='\n')&&(!feof(source)))
      c = my_getc(source);

   return(-1);
}

/*********************************************************************
 *                           my_getc(source)                         *
 *********************************************************************
 *  This subroutine is used in place of the standard getc subroutine
 *  to keep track of the current column number in a line and convert
 *  to upper case.
 *********************************************************************/

/* modified to crudely handle unicode 9/2016 */

char my_getc(source)
FILE *source;
{
   int ch;
   column_number++;

   ch = getc(source);
//printf("ch is %x \n",ch);
   if((ch & 0x7f)==ch) return toupper(ch);
   if(ch == EOF) return ch;
   if((ch & 0xff) == 0xef)
   {
       ch=getc(source); /* should check for 0xbb) */
       ch=getc(source); /* shoudl check for 0xbf */
//     printf("Doing my recursive call\n");
       column_number--;
       return my_getc(source);
   }
   if((ch &0xff) == 0xc2) /* single byte follows*/
   {
       ch=getc(source);
//   printf("returning question mark\n");
       return '?';  
   }
   if((ch & 0xff) == 0xe2) /* two bytes follow */
   {  ch=getc(source);  /* should check this to be 0x80 */
      ch=getc(source);
//printf("smart quotes \n");
      if((ch&0xff) == 0x9c) return '\"';
      if((ch&0xff) == 0x9d) return '\"';
      else return '?';
   }

   /* should not get here */
   return(ch);  
}
