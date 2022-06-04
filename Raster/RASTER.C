#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*-------STRUCTURES---------*/
typedef struct {int rows; int cols; unsigned char* data;} sImage;

/*-------PROTOTYPES---------*/
long getImageInfo(FILE*, long, int);
long copyImageInfo(FILE*, FILE*, int);
long copyColorTable(FILE*, FILE*);

int main(int argc, char* argv[])
{
  FILE			*bmpInput, *rasterOutput;
  sImage		originalImage;
  unsigned char		someChar;
  unsigned char*	pChar;
  int			nColors;
  long			fileSize;
  int			vectorSize, r, c;


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
  rasterOutput = fopen("data.txt", "w");

  /*--------GET BMP DATA---------------*/
  originalImage.cols = (int)getImageInfo(bmpInput, 18, 4);
  originalImage.rows = (int)getImageInfo(bmpInput, 22, 4);
  fileSize = getImageInfo(bmpInput, 2, 4);
  nColors = getImageInfo(bmpInput, 46, 4);
  vectorSize = fileSize - (14 + 40 + 4*nColors);

  /*-------PRINT TO SCREEN-------------*/
  printf("Width: %d\n", originalImage.cols);
  printf("Height: %d\n", originalImage.rows);
  printf("File size: %ld\n", fileSize);
  printf("# Colors: %d\n", nColors);
  printf("Vector size: %d\n", vectorSize);

  /*----------READ RASTER DATA---------*/
  fseek(bmpInput, (54 + 4*nColors), SEEK_SET);

  for(r=0; r<=originalImage.rows - 1; r++)
  {
    for(c=0; c<=originalImage.cols - 1; c++)
    {
      fread(pChar, sizeof(char), 1, bmpInput);
      fprintf(rasterOutput, "(%d, %d) = %d\n", r, c, *pChar);
    }
  }

  fclose(bmpInput);
  fclose(rasterOutput);


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





