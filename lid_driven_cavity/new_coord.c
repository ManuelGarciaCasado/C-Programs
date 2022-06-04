#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void main(int argc, char* argv[])
{
  FILE			*ufieldInput, *vfieldInput,*pfieldInput,*Output1,*Output2;
  unsigned char		someChar;
  unsigned char *pChar;
  int a,b,i,j,k,l,xnum,ynum;
  int uSize = 0, vSize = 0, pSize = 0;
  unsigned char uBuffer[80]={0},vBuffer[80]={0},pBuffer[80]={0};
  unsigned char xval[25]={0},yval[25]={0},uval[25]={0},vval[25]={0},pval[25]={0};
  char ch;
  double *xData = 0;
  double *yData = 0;
  double *uData = 0;
  double *vData = 0;
  double *pData = 0;
  double *dblBuffer = 0;
  double modulo = 0;

  char Xchar[5],Ychar[5],Rchar[5],Tini[3], Tfin[3], Tchar[3];
  char Unamefile[10]={0},Vnamefile[10]={0},Pnamefile[10]={0};
  int  X,Y, ti,tf,t,Xini,Yini;
  double x,y,f,r,u,v,p;
  double xinc, yinc, zinc;
  /* initialize pointer */
  
  if(argc < 4)
  {
    printf("Usage: %s u v p\n", argv[0], argv[1], argv[2], argv[3]);
    exit(0);
  }
  
  Output1 = fopen("OUTPUT_MOD_V", "w");
  fprintf(Output1,"X	Y	SQRT(V)\n");
  fclose(Output1);
  Output2 = fopen("OUTPUT_P", "w");
  fprintf(Output2,"X	Y	P\n");
  fclose(Output2);
  //sprintf(Tchar,"%d",t); 
  //itoa(t,Tchar,10);
  Unamefile[0]=NULL;
  Vnamefile[0]=NULL;
  Pnamefile[0]=NULL;
  strncat(Unamefile,argv[1],10);
//  strncat(Unamefile,Tchar,10);
  strncat(Vnamefile,argv[2],10);
//  strncat(Vnamefile,Tchar,10);
  strncat(Pnamefile,argv[3],10);
//  strncat(Pnamefile,Tchar,10);
  printf("Reading filename %s %s %s\n", Unamefile, Vnamefile, Pnamefile);

  /*--------READ INPUT FILE------------*/
  ufieldInput = fopen(Unamefile, "r");
  vfieldInput = fopen(Vnamefile, "r");
  pfieldInput = fopen(Pnamefile, "r");
  //fseek(ufieldInput, 0L, SEEK_END);
  //fseek(vfieldInput, 0L, SEEK_END);
  //fseek(pfieldInput, 0L, SEEK_END);
  uSize = 0;
  vSize = 0;
  pSize = 0;
  while(fgets(uBuffer,80,ufieldInput)!=NULL){
	  uSize++;
	  }
  while(fgets(vBuffer,80,vfieldInput)!=NULL){
	  vSize++;
	  }
  while(fgets(pBuffer,80,pfieldInput)!=NULL){
	  pSize++;
      }
  printf("Size u = %d, Size v = %d, Size p = %d\n", uSize,vSize,pSize);
	rewind(ufieldInput);
	rewind(vfieldInput);
	rewind(pfieldInput);
  /*-------RESERVAR MEMORIA------------*/
	
  xData = (double *) malloc(uSize * sizeof(double));
  yData = (double *) malloc(uSize * sizeof(double));
  uData = (double *) malloc(uSize * sizeof(double));
  vData = (double *) malloc(uSize * sizeof(double));
  pData = (double *) malloc(uSize * sizeof(double));
  if(xData == NULL || yData == NULL || uData == NULL
    || vData == NULL || pData == NULL) {
	printf("Failed to malloc data\n");
	exit(0);
   }
   
  /*--------GET FIELD DATA---------------*/
  fgets(uBuffer,80,ufieldInput);
  fgets(vBuffer,80,vfieldInput);
  fgets(pBuffer,80,pfieldInput);
  printf("%s", uBuffer);
  printf("%s", vBuffer);
  printf("%s", pBuffer);
  
  for(j = 0; j < uSize; j++){
  //for(j = 0; j < 1; j++){
  fgets(uBuffer,80,ufieldInput);
  fgets(vBuffer,80,vfieldInput);
  fgets(pBuffer,80,pfieldInput);
  //printf("%s", uBuffer);
  //printf("%s", vBuffer);
  //printf("%s", pBuffer);
	for(i = 0; i < 25; i++){
	//*(xData+j*25+i) = uBuffer[i];
	xval[i] = uBuffer[i];
	//*(yData+j*25+i) = uBuffer[(i+25)];
	yval[i] = uBuffer[i+25];
	//*(uData+j*25+i) = uBuffer[(i+50)];
	uval[i] = uBuffer[i+50];
	//*(vData+j*25+i) = vBuffer[(i+50)];
	vval[i] = vBuffer[i+50];
	//*(pData+j*25+i) = pBuffer[(i+50)];
	pval[i] = pBuffer[i+50];
	}
	*(xData+j) = atof(xval);
	*(yData+j) = atof(yval);
	*(uData+j) = atof(uval);
	*(vData+j) = atof(vval);
	*(pData+j) = atof(pval);
/*	printf("x: %s = %+.20e\n", xval, atof(xval));
	printf("y: %s = %+.20e\n", yval, atof(yval));
	printf("u: %s = %+.20e\n", uval, atof(uval));
	printf("v: %s = %+.20e\n", vval, atof(vval));
	printf("p: %s = %+.20e\n", pval, atof(pval));*/
  }
//  printf("x: %s = %+.20e\n", xval, atof(xval));
  fclose(ufieldInput);
  fclose(vfieldInput);
  fclose(pfieldInput);

  //for(j = 0; j < uSize; j++){
  
	/*for(i = 0; i < 25; i++){
		xval[i] = *(xData+j*25+i);
		yval[i] = *(yData+j*25+i);
		uval[i] = *(uData+j*25+i);
		vval[i] = *(vData+j*25+i);
		pval[i] = *(pData+j*25+i);
		
	}*/
	//printf("%c",xval);
	//printf("x: %s = %+.20e\n", xval, atof(xval));
	//printf("y: %s\n%+.20e\n", yval, atof(yval));
	//printf("u: %s\n%+.20e\n", uval, atof(uval));
	//*(xData+j*25) = xval;
	//*(yData+j*25) = yval;
	//*(uData+j*25) = uval;
	//*(vData+j*25) = vval;
	//*(pData+j*25) = pval; 
  //}
  //printf("x: %s = %+.20e\n", xval, atof(xval));
  /*printf("y: %s\n%+.20e\n", yval, atof(yval));
  printf("u: %s\n%+.20e\n", uval, atof(uval));
  printf("v: %s\n%+.20e\n", vval, atof(vval));
  printf("p: %s\n%+.20e\n", pval, atof(pval));*/
  /*printf ("Center R radius (R <= 98): ");
  gets (Rchar);
  printf ("Center X axe (R+2 < X < 440-R-2): ");
  gets (Xchar);
  printf ("Center Y axe (R+2<Y<82-R-2): ");
  gets (Ychar);*/
  Output1 = fopen("OUTPUT_MOD_V", "a");
  Output2 = fopen("OUTPUT_P", "a");
  for (X = 1; X <= 200; X++){
  for (Y = 1; Y <= 200; Y++){
  x = xData[X * 200 + Y];
  y = yData[X * 200 + Y];
  //r = xData[(X+R)* 82 + Y]-xData[X * 82 + Y];
  u = uData[X * 200 + Y];
  v = vData[X * 200 + Y];
  p = pData[X * 200 + Y];
  /*xinc = (xData[439 * 82 + 0]-xData[0 * 82 + 0])/440;
  yinc = (yData[0 * 82 + 81]-yData[0 * 82 + 0])/82;
  zinc = xinc;
  printf("xinc, yinc: %+.20e, %+.20e\n\n", xinc, yinc);

  //y = getValue(a,b, yData );
  //u = getValue(a,b, uData );
  //v = getValue(a,b, vData );
  //p = getValue(a,b, pData );
  //printf ("Your circunference is:\n ( (%+.3e) - x )^2 + ( (%+.3e) - y)^2 = ( %+.3e )^2 \n",x,y,r);
  printf("x: %+.20e\n", x);
  printf("y: %+.20e\n", y);
  printf("u: %+.20e\n", u);
  printf("v: %+.20e\n", v);
  printf("p: %+.20e\n", p);*/
  modulo = sqrt(pow(u,2)+pow(v,2));
 
  fprintf(Output1,"%d	%d	%+.10e\n", X, Y, modulo);
  fprintf(Output2,"%d	%d	%+.10e\n", X, Y, p);
  //fclose(Output);

  /*x = getValue(a,b, xData );
  y = getValue(a,b, yData );
  u = getValue(a,b, uData );
  v = getValue(a,b, vData );
  p = getValue(a,b, pData );*/
  /*for(i = 0; i < 25; i++){
  pChar[i]=
  printf("%c",*(xval+i));
  }*/
  /*printf("x: %+.20e\n", x);
  printf("y: %+.20e\n", y);
  printf("u: %+.20e\n", u);
  printf("v: %+.20e\n", v);
  printf("p: %+.20e\n", p);*/
  //printf("u vector: %s\n", uData);
  //printf("y vector: %s\n", *yData);
  //printf("u vector: %s\n", *uData);
  
  
  /*originalImage.cols = (int)getImageInfo(bmpInput, 18, 4);
  originalImage.rows = (int)getImageInfo(bmpInput, 22, 4);
  fileSize = getImageInfo(bmpInput, 2, 4);
  nColors = getImageInfo(bmpInput, 46, 4);
  headerSize = getImageInfo(bmpInput, 14, 4);
  vectorSize = fileSize-14-headerSize;
  offsetData = getImageInfo(bmpInput, 10, 4);
  //vectorSize = fileSize - (14 + 40 + 4*nColors);

  /*-------PRINT DATA TO SCREEN-------------*/
  /*printf("Width: %d\n", originalImage.cols);
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
  /*fseek(bmpInput, offsetData, SEEK_SET);
  fseek(bmpOutput, offsetData, SEEK_SET);
  /*----------READ RASTER DATA----------*/
  /*for(r=0; r<=originalImage.rows; r++)
  {
    for(c=0; c<=originalImage.cols ; c++)
    {	
		
		i=0;
      /*-----read data, reflect and write to output file----*/
		
		/*for(i=0;i<interval;i++) {
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
  }*/

	} //cierre de for (X = R+3; X<= 200-R-3; X++)
	}  //cierre de for (Y = R+3; Y<= 200-R-3; Y++)
	fclose(Output1);
	fclose(Output2);
	free(xData);
	free(yData);
	free(uData);
	free(vData);
	free(pData); 
 }
