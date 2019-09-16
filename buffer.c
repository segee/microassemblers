/********************************************************************
*                            Buffer.c
*********************************************************************
*  Functions to insert a microword into the output buffer, and
*  to write the buffer to a file.
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#include <stdio.h>
#include "masm.h"
#include "global.h"
#include "buffer.h"

/*********************************************************************
 *            flush_buffer(output, buffer, index, address)           *
 *********************************************************************
 *  This function writes the current object output buffer to the
 *  object file and initializes the buffer for the next data record.
 *
 *  INPUTS:
 *     output           -  FILE handle for the object file.
 *     buffer           -  Pointer to the object buffer.
 *     index            -  Pointer to the available space in output buffer.
 *     address          -  Current value of the address counter.
 *
 *  OUTPUTS:
 *     buffer           -  Initialized to contain a 3 word header for the
 *                         next data record.
 *     index            -  Next available word in buffer.  Set to 3.
 *********************************************************************/

extern void flush_buffer(output, buffer, index, address)
int  output;
char buffer[256];
int  *index;
int  address;
{
   int n;

   /*
    *  Write the current buffer.
    *  If the index is less than or equal to 3, then the buffer is
    *  either empty or this is the first "ORG" statement we've run
    *  across.  If so, don't write the buffer.
    */

   if (*index > 3)
   {
      buffer[0] = (char )((*index * 2 - 6) / 256);
      buffer[1] = (char )((*index * 2 - 6) % 256);
      for (n = 0; n < (*index * 2); n++)
         write(output, &buffer[n], 1);
   }

   /*
    *  Initialize the next data record.
    */

   buffer[0] = buffer[1] = (char)0;
   buffer[2] = (char)((address % 65536) / 256);
   buffer[3] = (char)((address % 65536) % 256);
   buffer[4] = (char)((address / 65536) / 256);
   buffer[5] = (char)((address / 65536) % 256);
   *index = 3;
}

/*********************************************************************
 *             insert_word(current_word, buffer, index)              *
 *********************************************************************
 *  This function inserts into the object output buffer the current
 *  micro word passed into it.  The buffer index is updated.
 *
 *  INPUTS:
 *     current_word     -  Contains the current micro word.
 *     buffer           -  A 128 word output buffer.
 *     index            -  An index into the ouput buffer showing the
 *                          next available word.
 *
 *  OUTPUTS:
 *     index            -  Updated to point to the next available word.
 *********************************************************************/

extern void insert_word(current_word, buffer, index)
unsigned int current_word[MAX_WORD_SIZE];
char buffer[256];
int  *index;
{
   int n;

   for (n = 0; n < num_of_words; n++, (*index)++, (buffer[0])++)
   {
      buffer[(*index) * 2]  = (char)(current_word[num_of_words - n - 1] / 256);
      buffer[((*index)*2)+1]= (char)(current_word[num_of_words - n - 1] % 256);
   }
}
