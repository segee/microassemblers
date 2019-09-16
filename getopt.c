/********************************************************************
*                            Getopt.c
*********************************************************************
*  Implements the function available under UNIX to parse the command
*  line for option flags; characters preceeded by "-" or "/"
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#include <stdio.h>
#include <string.h>

/*********************************************************************
 *                   getopt(argc, argv, opts)                        *
 *********************************************************************
 *  Each time getopt is called the next switch is obtained from the
 *  command line arguments.  If no switches remain, or if there were
 *  no switches in the first place, an EOF is returned.
 *  The list of valid switches is given in the input string "opts".
 *  If the current switch is not found in this string, then the
 *  character '?' is returned.
 *  If the switch in the string "opts" is followed by ':', then the
 *  command line is scanned for an argument for this switch.  The
 *  global variable "optarg" will point to a buffer containing the
 *  option argument.
 *
 *  Switches must follow directly after the calling name and
 *  preceeding the other arguments of the calling program.
 *
 *  INPUTS:
 *      argc          int      The number of arguments given in the
 *                             command line.
 *      argv          int      Character pointer to a list of pointers
 *                             pointing to the argument strings.
 *      opts          int      Pointer to the list of valid option
 *                             characters
 *
 *  Returns the character of the next switch to be processed.  If the
 *  switch takes an argument, the global variable optarg points the
 *  string containing the argument from the command line.
 *
 *  If the current switch is not found in the "opts" input string, then
 *  the character '?' is returned to indicate an error.
 *
 *  OUTPUTS:
 *      optarg        char *   Points to a buffer containing the option
 *                             argument.
 *
 *      optind        int      Gives the number of elements in the command
 *                             line (i.e. argv) processed as switches
 *********************************************************************/


extern int  getopt();

int  opterr = 1;
int  optind = 1;
int  optopt;
char *optarg;

extern int getopt(argc, argv, opts)
int argc;
char **argv, *opts;
{
   int  sp = 1;
   int  c;
   char *cp;

   if(sp == 1)
      if(optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
	 return(EOF);
      else
      if (strcmp(argv[optind], "--") == 0)
      {
	 optind++;
	 return(EOF);
      }
   optopt = c = argv[optind][sp];
   if(c == ':' || (cp=strchr(opts, c)) == NULL)
   {
      fprintf(stderr,"%s: illegal option -- %c\n",argv[0], c);
      if(argv[optind][++sp] == '\0')
      {
	 optind++;
	 sp = 1;
      }
      return('?');
   }
   if (*++cp == ':')
   {
      if(argv[optind][sp+1] != '\0')
	 optarg = &argv[optind++][sp+1];
      else
      if(++optind >= argc)
      {
         fprintf(stderr,"%s: option requires an argument -- %c",argv[0], c);
	 sp = 1;
	 return('?');
      }
      else
	 optarg = argv[optind++];

      sp = 1;
   }
   else
   {
      if(argv[optind][++sp] == '\0')
      {
	 sp = 1;
	 optind++;
      }
      optarg = NULL;
   }
   return(c);
}
