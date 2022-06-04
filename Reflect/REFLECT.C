#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
  unsigned char		someChar;
  unsigned char*	pChar;
  int			nColors;  /* BMP number of colors */
  long			fileSize; /* BMP file size */
  int			vectorSize; /* BMP vector size */
  int			headerSize; /* BMP vector size */
  int			r, c;       /* r = rows, c = cols */
  int           interval;
  int            offsetData;
  int  i;
  /* initialize pointer */
  someChar = '0';
  pChar = &someChar;

  if(argc < 2)
  {
    printf("Usage: %s bmpInput.bmp\n", argv[0]);
    exit(0);
  }
  printf("Reading filename %s\n", argv[1]);

  /*--------READ INPUT FILE------------*/
  bmpInput = fopen(argv[1], "rb");
  fseek(bmpInput, 0L, SEEK_END);

  /*--------DECLARE OUTPUT FILE--------*/
  bmpOutput = fopen("reflect.bmp", "wb");

  /*--------GET BMP DATA---------------*/
  originalImage.cols = (int)getImageInfo(bmpInput, 18, 4);
  originalImage.rows = (int)getImageInfo(bmpInput, 22, 4);
  fileSize = getImageInfo(bmpInput, 2, 4);
  nColors = getImageInfo(bmpInput, 46, 4);
  headerSize = getImageInfo(bmpInput, 14, 4);
  vectorSize = fileSize-14-headerSize;
  offsetData = getImageInfo(bmpInput, 10, 4);
  //vectorSize = fileSize - (14 + 40 + 4*nColors);

  /*-------PRINT DATA TO SCREEN-------------*/
  printf("Width: %d\n", originalImage.cols);
  printf("Height: %d\n", originalImage.rows);
  printf("File size: %ld\n", fileSize);
  printf("# Colors: %d\n", nColors);
  printf("Vector size: %d\n", vectorSize);
  printf("Header size: %d\n", headerSize);
  printf("Beginning of data: %d\n", offsetData);  
  copyImageInfo(bmpInput, bmpOutput);
  copyColorTable(bmpInput, bmpOutput, nColors);
  interval = (int)(vectorSize/(originalImage.rows*originalImage.cols));

  /*----START AT BEGINNING OF RASTER DATA-----*/
  fseek(bmpInput, offsetData, SEEK_SET);
  fseek(bmpOutput, offsetData, SEEK_SET);
  /*----------READ RASTER DATA----------*/
  for(r=0; r<=originalImage.rows; r++)
  {
    for(c=0; c<=originalImage.cols ; c++)
    {	
		
		i=0;
      /*-----read data, reflect and write to output file----*/
		
		for(i=0;i<interval;i++) {
			*pChar=0;
			fread(pChar, sizeof(char), 1, bmpInput);
			if(*pChar >= pow((double) 255,(double) interval)) {
				*pChar = pow((double) 255,(double) interval);
			}
			else (*pChar = pow((double) 255,(double) interval) - *pChar);
			fwrite(pChar, sizeof(char), 1, bmpOutput);
		}
	  //printf("%d\n",pChar);
    }
  }

  fclose(bmpInput);
  fclose(bmpOutput);


}


/*----------GET IMAGE INFO SUBPROGRAM--------------*/
long getImageInfo(FILE* inputFile, long offset, int numberOfChars)
{
  unsigned char			*ptrC;
  long				value = 0;
  long			dummy;
  int				i;
  double j;

  dummy = 0;
  ptrC = &dummy;

  fseek(inputFile, offset, SEEK_SET);
  for(i=1; i<=numberOfChars; i++)
  {
  
	  fread(ptrC, sizeof(char), 1, inputFile);
	 j=i-1;
    /* calculate value based on adding bytes */

    value = value + *ptrC*(double)pow((double)256,j);
	
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

  for(i=0; i<=54; i++)
  {
    fread(ptrC, sizeof(char), 1, inputFile);
    fwrite(ptrC, sizeof(char), 1, outputFile);
	//printf("%s\n",ptrC);
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
	//printf("%s\n",ptrC);
  }

}






