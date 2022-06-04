/*
  FILE: edgeSob.c - WORKS!!
  AUTH: Bill Green
  DESC: 2 3x3 Sobel masks for edge detection
  DATE: 07/23/02
  REFS: edgeLap.c
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
   double        Z;
   int			I, J;
   long			sumX, sumY;
   int			nColors, SUM;
   unsigned long	vectorSize;
   unsigned long	fileSize;
   int			GX[3][3];
   int			GY[3][3];
   unsigned char 	*pChar, someChar;
   unsigned int		row, col;
   unsigned char *dato;
   int          interval;
   int			offsetImg;
   int           k;

   someChar = '0'; pChar = &someChar;

   /* 3x3 GX Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
   GX[0][0] = -1; GX[0][1] = 0; GX[0][2] = 1;
   GX[1][0] = -2; GX[1][1] = 0; GX[1][2] = 2;
   GX[2][0] = -1; GX[2][1] = 0; GX[2][2] = 1;

   /* 3x3 GY Sobel mask.  Ref: www.cee.hw.ac.uk/hipr/html/sobel.html */
   GY[0][0] =  1; GY[0][1] =  2; GY[0][2] =  1;
   GY[1][0] =  0; GY[1][1] =  0; GY[1][2] =  0;
   GY[2][0] = -1; GY[2][1] = -2; GY[2][2] = -1;

   if(argc < 2) {
     printf("Usage: %s bmpInput.bmp\n", argv[0]);
     exit(0);
   };
   printf("Reading filename %s\n", argv[1]);

   /*-------DECLARE INPUT & OUTPUT FILES-------*/
   bmpInput = fopen(argv[1], "rb");
   bmpOutput = fopen("edgeSob.bmp", "wb");

   /*---SET POINTER TO BEGINNING OF FILE----*/
   fseek(bmpInput, 0L, SEEK_END);

   /*-------GET INPUT BMP DATA--------*/
   fileSize = getImageInfo(bmpInput, 2, 4);
   originalImage.cols = (int)getImageInfo(bmpInput, 18, 4);
   originalImage.rows = (int)getImageInfo(bmpInput, 22, 4);
   offsetImg= (int)getImageInfo(bmpInput, 10, 4);
   edgeImage.rows = originalImage.rows;
   edgeImage.cols = originalImage.cols;

   /*--------PRINT DATA TO SCREEN----------*/
   printf("Width: %d\n", originalImage.cols);
   printf("Height: %d\n", originalImage.rows);
   printf("File size: %lu\n", fileSize);
   printf("Offset image: %d\n", offsetImg);
   nColors = (int)getImageInfo(bmpInput, 46, 4);
   printf("nColors: %d\n", nColors);

   /*------ALLOCATE MEMORY FOR FILES--------*/
   vectorSize = fileSize - (14+40+4*nColors);
   //vectorSize = fileSize;
   printf("vectorSize: %lu\n", vectorSize);
   interval = (int)(vectorSize/(originalImage.cols*originalImage.rows));
   //nColors=interval;
   
   edgeImage.data = (unsigned char *) malloc(vectorSize*sizeof(unsigned char));
   if(edgeImage.data == NULL) {
	printf("Failed to malloc edgeImage.data\n");
	exit(0);
   }
   printf("%lu bytes malloc'ed for edgeImage.data\n", vectorSize);

   originalImage.data = (unsigned char *) malloc(vectorSize*sizeof(unsigned char));
   if(originalImage.data == NULL) {
	printf("Failed to malloc originalImage.data\n");
	exit(0);
   }
   printf("%lu bytes malloc'ed for originalImage.data\n", vectorSize);

   /*------COPY HEADER AND COLOR TABLE---------*/
   copyImageInfo(bmpInput, bmpOutput);
   copyColorTable(bmpInput, bmpOutput, nColors);
   fseek(bmpInput, offsetImg, SEEK_SET);
   fseek(bmpOutput, offsetImg, SEEK_SET);

   /* Read input.bmp and store it's raster data into originalImage.data */
   for(row=0; row<=originalImage.rows; row++) {
	for(col=0; col<=originalImage.cols; col++) {
		for (k=0;k<interval;k++){
	     fread(pChar, sizeof(char), 1, bmpInput);
	    
		*(originalImage.data + row*originalImage.cols*interval + col*interval +k) = *pChar;
		
		//fwrite( pChar, sizeof(char), 1, bmpOutput);
		}	 
	}
	
   }
	
   /*---------------------------------------------------
		SOBEL ALGORITHM STARTS HERE
   ---------------------------------------------------*/
   for(Y=0; Y<=(originalImage.rows); Y++)  {
	for(X=0; X<=(originalImage.cols); X++)  {
	     for (k=0;k<interval;k++){
	     
			sumX = 0;
			sumY = 0;
		 
			    /* image boundaries */
			 if(Y==0 || Y==originalImage.rows)
			  SUM = 0;
			 else if(X==0 || X==originalImage.cols)
			 SUM = 0;

	     /* Convolution starts here */
		     else   {

	       /*-------X GRADIENT APPROXIMATION------*/
			   for(I=-1; I<=1; I++)  {
			   for(J=-1; J<=1; J++)  {
			   sumX = sumX + (int)( *(originalImage.data +k+ (X + I)*interval + (Y + J)*(originalImage.cols)*interval) * GX[I+1][J+1]);
			   
			   }
			   }
				if(sumX>255)  sumX=255;
				if(sumX<0)    sumX=0;

	       /*-------Y GRADIENT APPROXIMATION-------*/
				for(I=-1; I<=1; I++)  {
				for(J=-1; J<=1; J++)  {
					   sumY = sumY + (int)( *(originalImage.data +k+ (X + I)*interval + (Y + J )*(originalImage.cols)*interval) * GY[I+1][J+1]);
				}
				}
			   if(sumY>255)   sumY=255;
			   if(sumY<0)     sumY=0;

			     //  SUM = sqrt((double)(pow((double)sumX,(double)2) + pow((double)sumY,(double)2))); /*---GRADIENT MAGNITUDE APPROXIMATION (Myler p.218)----*/
					SUM = abs(sumX) + abs(sumY);
			 }
		
		

	    
	    // *(edgeImage.data + X + Y*originalImage.cols+k) = (255 - (unsigned char)(SUM));  /* make edges black and background white */
		
		 //fwrite(255, sizeof(char), 1, bmpOutput);
		 
		 Z = sqrt((pow((double)X-250,2)+pow((double)Y-250,2))) 	;
		 if( Z >= 30 && Z <= 31){
		 *(edgeImage.data + X*interval+ k+Y*(originalImage.cols)*interval) = (0);	
	     fwrite( (edgeImage.data + X*interval +k+ Y*(originalImage.cols)*interval), sizeof(char), 1, bmpOutput);
		 }
		 else{
			 *(edgeImage.data + X*interval+ k+Y*(originalImage.cols)*interval) = (255 - (unsigned char)(SUM));	
	     fwrite( (edgeImage.data + X*interval +k+ Y*(originalImage.cols)*interval), sizeof(char), 1, bmpOutput);
		 }
		}
	}
   }

   printf("See edgeSob.bmp for results\n");
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
    value = (long)(value + (*ptrC)*(pow((double)256, (double)(i-1))));
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