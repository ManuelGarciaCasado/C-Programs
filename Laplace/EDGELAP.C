/*
  FILE: edgeLap.c - WORKS!
  AUTH: P.Oh
  DESC: 5x5 Laplace mask for edge detection
  DATE: 05/02/02 23:30
  REFS: edgedeta.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <alloc.h>

/*-------STRUCTURES---------*/
typedef struct {int rows; int cols; unsigned char* data;} sImage;

/*-------PROTOTYPES---------*/
long getImageInfo(FILE*, long, int);
void copyImageInfo(FILE* inputFile, FILE* outputFile);
void copyColorTable(FILE* inputFile, FILE* outputFile, int nColors);

int main(int argc, char* argv[])
{
   FILE			*bmpInput, *bmpOutput;
   sImage		originalImage;
   sImage		edgeImage;
   unsigned int		X, Y;
   int			I, J;
   long			SUM;
   int			nColors;
   unsigned long	vectorSize;
   unsigned long	fileSize;
   int			MASK[5][5];
   unsigned char 	*pChar, someChar;
   unsigned int		row, col;

   someChar = '0'; pChar = &someChar;

   /* 5x5 Laplace mask.  Ref: Myler Handbook p. 135 */
   MASK[0][0] = -1; MASK[0][1] = -1; MASK[0][2] = -1; MASK[0][3] = -1; MASK[0][4] = -1;
   MASK[1][0] = -1; MASK[1][1] = -1; MASK[1][2] = -1; MASK[1][3] = -1; MASK[1][4] = -1;
   MASK[2][0] = -1; MASK[2][1] = -1; MASK[2][2] = 24; MASK[2][3] = -1; MASK[2][4] = -1;
   MASK[3][0] = -1; MASK[3][1] = -1; MASK[3][2] = -1; MASK[3][3] = -1; MASK[3][4] = -1;
   MASK[4][0] = -1; MASK[4][1] = -1; MASK[4][2] = -1; MASK[4][3] = -1; MASK[4][4] = -1;

   if(argc < 2) {
     printf("Usage: %s bmpInput.bmp\n", argv[0]);
     exit(0);
   };
   printf("Reading filename %s\n", argv[1]);

   /* open files for reading and writing to */
   bmpInput = fopen(argv[1], "rb");
   bmpOutput = fopen("edgeLap.bmp", "wb");

   /* start pointer at beginning of file */
   fseek(bmpInput, 0L, SEEK_END);

   /* retrieve and print filesize and number of cols and rows */
   fileSize = getImageInfo(bmpInput, 2, 4);
   originalImage.cols = (int)getImageInfo(bmpInput, 18, 4);
   originalImage.rows = (int)getImageInfo(bmpInput, 22, 4);
   edgeImage.rows = originalImage.rows;
   edgeImage.cols = originalImage.cols;

   printf("Width: %d\n", originalImage.cols);
   printf("Height: %d\n", originalImage.rows);
   printf("File size: %lu\n", fileSize);

   /* retrieve and print Number of colors */
   nColors = (int)getImageInfo(bmpInput, 46, 4);
   printf("nColors: %d\n", nColors);

   vectorSize = fileSize - (14+40+4*nColors);
   printf("vectorSize: %lu\n", vectorSize);
   edgeImage.data = malloc(vectorSize*sizeof(unsigned char));
   if(edgeImage.data == NULL) {
	printf("Failed to malloc edgeImage.data\n");
	exit(0);
   }
   printf("%lu bytes malloc'ed for edgeImage.data\n", vectorSize);

   originalImage.data = malloc(vectorSize*sizeof(unsigned char));
   if(originalImage.data == NULL) {
	printf("Failed to malloc originalImage.data\n");
	exit(0);
   }
   printf("%lu bytes malloc'ed for originalImage.datt\n", vectorSize);

   copyImageInfo(bmpInput, bmpOutput);
   copyColorTable(bmpInput, bmpOutput, nColors);
   fseek(bmpInput, (14+40+4*nColors), SEEK_SET);
   fseek(bmpOutput, (14+40+4*nColors), SEEK_SET);

   /* Read input.bmp and store it's raster data into originalImage.data */
   for(row=0; row<=originalImage.rows-1; row++) {
	for(col=0; col<=originalImage.cols-1; col++) {
	     fread(pChar, sizeof(char), 1, bmpInput);
	     *(originalImage.data + row*originalImage.cols + col) = *pChar;
	}
   }

   for(Y=0; Y<=(originalImage.rows-1); Y++)  {
	for(X=0; X<=(originalImage.cols-1); X++)  {
	     SUM = 0;

	  /* image boundaries */
	  if(Y==0 || Y==1 || Y==originalImage.rows-2 || Y==originalImage.rows-1)
		  SUM = 0;
	  else if(X==0 || X==1 || X==originalImage.cols-2 || X==originalImage.cols-1)
		  SUM = 0;

	  /* Convolution starts here */
	  else   {
	     for(I=-2; I<=2; I++)  {
		 for(J=-2; J<=2; J++)  {
		    SUM = SUM + (int)( (*(originalImage.data + X + I + (Y + J)*originalImage.cols)) * MASK[I+2][J+2]);

		 }
	     }
	  }
	     if(SUM>255)  SUM=255;
	     if(SUM<0)    SUM=0;

	     *(edgeImage.data + X + Y*originalImage.cols) = 255 - (unsigned char)(SUM);  /* make edges black and background white */
	     fwrite( (edgeImage.data + X + Y*originalImage.cols), sizeof(char), 1, bmpOutput);
	}
   }

   printf("See edgeLap.bmp for results\n");
   fclose(bmpInput);
   fclose(bmpOutput);
   free(edgeImage.data);      /* Finished with edgeImage.data */
   free(originalImage.data);  /* Finished with originalImage.data */
   return 0;
}

/*----------GET IMAGE INFO SUBPROGRAM--------------*/
long getImageInfo(FILE* inputFile, long offset, int numberOfChars)
{
  unsigned char			*ptrC;
  long				value = 0L;
  unsigned char			dummy;
  int				i;

  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, offset, SEEK_SET);

  for(i=1; i<=numberOfChars; i++)
  {
    fread(ptrC, sizeof(char), 1, inputFile);
    /* calculate value based on adding bytes */
    value = (long)(value + (*ptrC)*(pow(256, (i-1))));
  }
  return(value);

} /* end of getImageInfo */

/*-------------COPIES HEADER AND INFO HEADER----------------*/
void copyImageInfo(FILE* inputFile, FILE* outputFile)
{
  unsigned char		*ptrC;
  unsigned char		dummy;
  int				i;

  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, 0L, SEEK_SET);
  fseek(outputFile, 0L, SEEK_SET);

  for(i=0; i<=50; i++)
  {
    fread(ptrC, sizeof(char), 1, inputFile);
    fwrite(ptrC, sizeof(char), 1, outputFile);
  }

}

/*----------------COPIES COLOR TABLE-----------------------------*/
void copyColorTable(FILE* inputFile, FILE* outputFile, int nColors)
{
  unsigned char		*ptrC;
  unsigned char		dummy;
  int				i;

  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, 54L, SEEK_SET);
  fseek(outputFile, 54L, SEEK_SET);

  for(i=0; i<=(4*nColors); i++)  /* there are (4*nColors) bytesin color table */
  {
    fread(ptrC, sizeof(char), 1, inputFile); 
    fwrite(ptrC, sizeof(char), 1, outputFile);
  }

}