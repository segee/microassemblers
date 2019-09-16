/********************************************************************
*                            Trans.c
*********************************************************************
*  This file contains the functions necessary to read the
*  object file in HP Absolute format, and to write s-records.
*
*  This source is based on code written by Erik Johnson.
*********************************************************************/

#include <stdio.h>
#include <string.h>
#include "masm.h"
#include "trans.h"

/*********************************************************************
 *                   srecords(src_file)                              *
 *********************************************************************
 *  Read object file and write out s-record files. 
 *********************************************************************/

void srecords(src_file)
char src_file[N];
{

   char obj_file[16];
   char out_file[8][16];
   int  ocode;
   int  width;                /* width of microword in bytes */
   int  address,bcount;
   int  csum[8];
   char blk_buf[256][8];
   FILE *srec[8];

   int  i,j;
   int	index;		      /* general purpose index		     */
   int  num, passes;

   strcpy(obj_file,src_file);
   index = 0;
   while( obj_file[index] != '.')
      index++;
   obj_file[index]='\0';
   strcat(obj_file, ".obj");
   index = 0;

   while((obj_file[index] != '.') && (obj_file[index] != '\0'))
   {
      out_file[0][index] = out_file[1][index] = out_file[2][index]=
      out_file[3][index] = out_file[4][index] = out_file[5][index]=
      out_file[6][index] = out_file[7][index] = obj_file[index];
      index++;
   }

   for(i=0; i < 8; i++)
      out_file[i][index] = '\0';

   strcat(out_file[0], ".0");
   strcat(out_file[1], ".1");
   strcat(out_file[2], ".2");
   strcat(out_file[3], ".3");
   strcat(out_file[4], ".4");
   strcat(out_file[5], ".5");
   strcat(out_file[6], ".6");
   strcat(out_file[7], ".7");

   if ((ocode = open(obj_file,O_RDONLY)) == -1)
   {
      fprintf(stderr,"Error opening object file %s.\n",obj_file);
      exit(2);
   }

   /* read first block, get number of bytes in word */
   width = block1(ocode);

   /* read first data block */
   block(ocode, width, &bcount, &address, blk_buf);

   for(i=0;i<width;i++)  /* open as many files as needed*/
   {
      if((srec[i]=fopen(out_file[i],"w"))==NULL)
      {
         fprintf(stderr,"error opening file %s \n",out_file[i]);
         exit(2);
      }
   }

   while(bcount>0)
   {
      num=bcount/width;
      passes=0;
      while(num>15)
      {
         for(j=0; j < width; j++) /* output s1 byte count and address */
         {
            fprintf(srec[j],"S113%04X",address&0xffff);
            csum[j]=0x13+((address/256)&0xff)+((address%256)&0xff);
         } /* end for j */

         for(i=0; i < 16; i++)
            for(j=0;j<width;j++)
            {
               /* put one data byte */

               fprintf(srec[j],"%02X",(blk_buf[i+(passes*16)][j])&0xff);
               csum[j]+=(blk_buf[i+(passes*16)][j]&0xff);
            }

         address+=16;
         passes++;
         num-=16;
         for(j=0;j<width;j++)
         {
            /* write checksums */

            csum[j]&=0xff;
            csum[j]=0xff-csum[j];
            fprintf(srec[j],"%02X\n",csum[j]);
         }

      } /* to get here num<16*/

      if(num>0)
      {
         for(j=0;j<width;j++)
         {
            /* output s1 byte count and address */

            fprintf(srec[j],"S1%02X%04X",((num&0xff)+3),address&0xffff);
            csum[j]=((num&0xff)+3)+((address/256)&0xff)+((address%256)&0xff);
         }

         for(i=0;i<num;i++)
            for(j=0;j<width;j++)
            {
               fprintf(srec[j],"%02X",(blk_buf[i+(passes*16)][j]&0xff));
               csum[j]+=(blk_buf[i+(passes*16)][j]&0xff);
            } /* put one data byte */

         address+=16;
         passes++;
         num=0;
         for(j=0;j<width;j++)
         {
            /* write checksums */

            csum[j]&=0xff;
            csum[j]=0xff-csum[j];
            fprintf(srec[j],"%02X\n",csum[j]);
         } /* checksums written */
      }

      block(ocode, width, &bcount, &address, blk_buf);

   } /* to get here bcount<=0 */


   printf("Srecords created.\n");

   for(i=0; i < width; i++)
   {
      /* close all files*/

      fprintf(srec[i],"S9030000FC\n");
      fclose(srec[i]);
   }

   close(ocode);
   return;
}

/*********************************************************************
 *                       block1(ocode)                               *
 *********************************************************************
 *  Read first block of object file.
 *********************************************************************/

int block1(ocode)
int ocode;
{
   char buf[8];
   int  width;

   read(ocode,buf,8);
   width=(buf[1]&0xff)/8;
   return(width);
}

/*********************************************************************
 *           block(ocode, width, bcount, address, blk_buf)           *
 *********************************************************************
 *  Read subsequent blocks of object file.
 *********************************************************************/

void block(ocode, width, bcount, address, blk_buf)
int  ocode, width;
int  *bcount, *address;
char blk_buf[256][8];
{
   char buf[256];
   int i,j;

   *bcount = getbcount(ocode);
   *address = getaddress(ocode);
   read(ocode, buf, *bcount);

   for(i=0; i < (*bcount / width); i++)
   {
      for(j=0; j < width; j++)
      {
         blk_buf[i][width-j-1]= (buf[i*width+j]&0xff);
      }
   }
   return;
}

/*********************************************************************
 *                      getbcount(ocode)                             *
 *********************************************************************/

int getbcount(ocode)
int ocode;
{
   char buf[2];
   int temp;

   buf[0]=buf[1]=0;
   read(ocode,buf,2);
   temp=(buf[1]&0xff);
   return(temp);
}

/*********************************************************************
 *                     getaddress(ocode)                             *
 *********************************************************************
 *  NOTE: Address is never more than 16 bits.
 *********************************************************************/

int getaddress(ocode)
int ocode;
{
   char buf[4];
   int temp;

   read(ocode,buf,4);
   temp=(buf[0]&0xff)*256+(buf[1]&0xff);
   return(temp);
}
