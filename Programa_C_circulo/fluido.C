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
typedef struct {double xaxis; double yaxis; double data;} sField;

/*-------PROTOTYPES---------*/
long getFileInfo(FILE*, long, int);
//void copyFileInfo(FILE* inputFile, FILE* outputFile);
//void copyColorTable(FILE* inputFile, FILE* outputFile, int nColors);

int main(int argc, char* argv[])
{
   FILE			*uInput, *vInput, *pInput;
   sField		originalFile;
   sField		edgeFile;
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
   double	xaxis, yaxis;
   double *dato;
   int          interval;
   int			offsetImg;
   int           k;

   

   if(argc < 2) {
     printf("Usage: %s u v p\n", argv[0]);
     exit(0);
   };
   printf("Reading filename %s %s %s\n", argv[1],argv[2],argv[3]);

   /*-------DECLARE INPUT & OUTPUT FILES-------*/
   uInput = fopen(argv[1], "r");
   vInput = fopen(argv[2], "r");
   pInput = fopen(argv[3], "r");
   

   /*---SET POINTER TO BEGINNING OF FILE----*/
   fseek(uInput, 0L, SEEK_END);
   fseek(vInput, 0L, SEEK_END);
   fseek(pInput, 0L, SEEK_END);

   /*-------GET INPUT BMP DATA--------*/

   fileSize = getFileInfo(uInput, 0, 72);
  
   /*--------PRINT DATA TO SCREEN----------*/
   printf("Width: %d\n", originalImage.cols);
   printf("Height: %d\n", originalImage.rows);
   printf("File size: %lu\n", fileSize);
   printf("Offset image: %d\n", offsetImg);
   nColors = (int)getImageInfo(bmpInput, 46, 4);
   printf("nColors: %d\n", nColors);

   /*------ALLOCATE MEMORY FOR FILES--------*/
   /*vectorSize = fileSize - (14+40+4*nColors);
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
   printf("%lu bytes malloc'ed for originalImage.data\n", vectorSize);*/

   /*------COPY HEADER AND COLOR TABLE---------*/
   /*copyImageInfo(bmpInput, bmpOutput);
   copyColorTable(bmpInput, bmpOutput, nColors);
   fseek(bmpInput, offsetImg, SEEK_SET);
   fseek(bmpOutput, offsetImg, SEEK_SET);

   /* Read input.bmp and store it's raster data into originalImage.data */
   /*for(row=0; row<=originalImage.rows; row++) {
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
   /*for(Y=0; Y<=(originalImage.rows); Y++)  {
	for(X=0; X<=(originalImage.cols); X++)  {
	     for (k=0;k<interval;k++){
	     
			sumX = 0;
			sumY = 0;
		 
		
		 
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
   }*/

   printf("See edgeSob.bmp for results\n");
   fclose(uInput);
   fclose(vInput);
   fclose(pInput);
   fclose(bmpOutput);
   /*free(edgeImage.data);      /* Finished with edgeImage.data */
   /*free(originalImage.data);  /* Finished with originalImage.data */
   return 0;
}

/*----------GET IMAGE INFO SUBPROGRAM--------------*/
long getFileInfo(FILE* inputFile)
{
  
  int				i= 0;
  unsigned char c;

  do {
      c = fgetc (pFile);
	  i++;
    } while (c != EOF);
  }

  // Rewind file
  fseek(inputFile, 0L, SEEK_END);
  return(i);

} /* end of getImageInfo */

/*-------------COPIES HEADER AND INFO HEADER----------------*/
/*void copyImageInfo(FILE* inputFile, FILE* outputFile)
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
/*void copyColorTable(FILE* inputFile, FILE* outputFile, int nColors)
{
  unsigned char		*ptrC;
  unsigned char		dummy;
  int				i;

  dummy = '0';
  ptrC = &dummy;

  fseek(inputFile, 54L, SEEK_SET);
  fseek(outputFile, 54L, SEEK_SET);

  for(i=0; i<=(4*nColors); i++)  /* there are (4*nColors) bytesin color table */
  /*{
    fread(ptrC, sizeof(char), 1, inputFile); 
    fwrite(ptrC, sizeof(char), 1, outputFile);
  }

}*/