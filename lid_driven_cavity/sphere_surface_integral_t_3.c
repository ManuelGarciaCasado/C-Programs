#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double getValue(int ipos, int jpos, int Size, unsigned char * Array );
double Integrando1(int a, int b, int c, int d, double *xArray,
		double *yArray, double *uArray,
		double *vArray, double *pArray);
double Integrando2(int a, int b, int c, int d, double *xArray,
		double *yArray, double *uArray,
		double *vArray, double *pArray);


void main(int argc, char* argv[])
{
  FILE			*ufieldInput, *vfieldInput,*pfieldInput,*Output;
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
  double circunferencia1[100] = {0}, circunferencia2[100] = {0};
  double campo1 = 0, campo2 = 0;
  double esfera1 = 0, esfera2 =0; 

  char Xchar[5],Ychar[5],Rchar[5],Tini[3], Tfin[3], Tchar[3];
  char Unamefile[10]={0},Vnamefile[10]={0},Pnamefile[10]={0};
  int  X,Y,R, ti,tf,t;
  double x,y,f,r,u,v,p;
  double xinc, yinc, zinc;
  /* initialize pointer */
  
  if(argc < 4)
  {
    printf("Usage: %s u v p\n", argv[0], argv[1], argv[2], argv[3]);
    exit(0);
  }
   printf ("Center R radius (R <= 97): ");
  gets (Rchar);
  printf ("Center X axe (R+2 < X < 200-R-2): ");
  gets (Xchar);
  printf ("Center Y axe (R+2 < Y < 200-R-2): ");
  gets (Ychar);
  R= atoi(Rchar);
  X= atoi(Xchar);
  Y= atoi(Ychar);
  printf ("Time initial file name: ");
  gets (Tini);
  ti= atoi(Tini);
  printf ("Time final file name: ");
  gets (Tfin);
  tf= atoi(Tfin);
  Output = fopen("OUTPUT", "w");
  fprintf(Output,"Time | Convexity Int.        | Minimum Int.\n");
  fclose(Output);

  for (t=ti; t<=tf; t++){
  sprintf(Tchar,"%d",t); 
  //itoa(t,Tchar,10);
  Unamefile[0]=NULL;
  Vnamefile[0]=NULL;
  Pnamefile[0]=NULL;
  strncat(Unamefile,argv[1],10);
  strncat(Unamefile,Tchar,10);
  strncat(Vnamefile,argv[2],10);
  strncat(Vnamefile,Tchar,10);
  strncat(Pnamefile,argv[3],10);
  strncat(Pnamefile,Tchar,10);
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
	
  xData = (double *) malloc(uSize*sizeof(double));
  yData = (double *) malloc(uSize*sizeof(double));
  uData = (double *) malloc(uSize*sizeof(double));
  vData = (double *) malloc(uSize*sizeof(double));
  pData = (double *) malloc(uSize*sizeof(double));
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
//	printf("x: %s = %+.20e\n", xval, atof(xval));
  }
//  printf("x: %s = %+.20e\n", xval, atof(xval));
  fclose(ufieldInput);
  fclose(vfieldInput);
  fclose(pfieldInput);

//  for(j = 0; j < uSize; j++){
  
/*	for(i = 0; i < 25; i++){
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
  x = xData[X * 200 + Y];
  y = yData[X * 200 + Y];
  r = xData[(X+R)* 200 + Y]-xData[X * 200 + Y];
  u = uData[X * 200 + Y];
  v = vData[X * 200 + Y];
  p = pData[X * 200 + Y];
  xinc = (xData[199 * 200 + 0]-xData[0 * 200 + 0])/200;
  yinc = (yData[0 * 200 + 199]-yData[0 * 200 + 0])/200;
  zinc = xinc;
  printf("xinc, yinc: %+.20e, %+.20e\n\n", xinc, yinc);

  //y = getValue(a,b, yData );
  //u = getValue(a,b, uData );
  //v = getValue(a,b, vData );
  //p = getValue(a,b, pData );
  printf ("Your circunference is:\n ( (%+.3e) - x )^2 + ( (%+.3e) - y)^2 = ( %+.3e )^2 \n",x,y,r);
  printf("x: %+.20e\n", x);
  printf("y: %+.20e\n", y);
  printf("u: %+.20e\n", u);
  printf("v: %+.20e\n", v);
  printf("p: %+.20e\n", p);
  circunferencia1[0] = 0;
  circunferencia2[0] = 0;
  for(k = 1; k < R+1; k++){
	for(i = 0; i < 200; i++){
		for(j = 0; j < 200; j++){
			f = sqrt(pow((i-X),2) + pow((j-Y)*yinc/xinc,2));
			if ( f >= (double)k-0.5 && f <= (double)k+0.5 ){
			campo1 = Integrando1(i, j, X, Y, xData, yData, uData, vData, pData);
			campo2 = Integrando2(i, j, X, Y, xData, yData, uData, vData, pData);
			circunferencia1[k] = circunferencia1[k] + campo1 * xinc ;
			circunferencia2[k] = circunferencia2[k] + campo2 * xinc;
			printf("Int convex: %+.10e, Int min: %+.10e, i: %d, j: %d, k: %d\n",
			 campo1, campo2, i,j,k);
			}
		}
	}
	
  }
  for(k = 0; k < 100; k++){
	for(l = -100; l < 100; l++){
	f = sqrt(pow(k,2) + pow(l,2));
		if ( f >= (double)R-0.5 && f <= (double)R+0.5 ){
			esfera1 = esfera1 + circunferencia1[k]*zinc;
			esfera2 = esfera2 + circunferencia2[k]*zinc;
			printf("Int convex: %+.10e, Int min: %+.10e, k: %d, l: %d\n",
			 circunferencia1[abs(k)], circunferencia1[abs(k)],k,l);
		}
	}
  }
  
  printf("Integral de convexidad (>=0): %+.20e\n", esfera1);
  printf("Integral de mínimo (=0): %+.20e\n", esfera2);
  Output = fopen("OUTPUT", "a");
  fprintf(Output,"%d	%+.10e	%+.10e\n", t, esfera1, esfera2);
  fclose(Output);

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


  free(xData);
  free(yData);
  free(uData);
  free(vData);
  free(pData); 
  }

}

/*----------GET VALUE IN DOUBLE--------------------*/
double getValue(int ipos, int jpos,int Size, unsigned char *Array ){
	unsigned char xyuvpval[25]={0};
	double value;
	int i,j;
	//int row = 200;
	int col =200;

	for(j = 0; j < Size; j++){
	for(i = 0; i < 25; i++){
	//	printf("%c",*(Array + j + i))
		xyuvpval[i] = *(Array + j*25 + i);
	//	printf("%c", xyuvpval[i]);		
	}
	//  if (j == ipos + jpos * row ){
	if ( j == ipos * col + jpos){
	  value = atof(xyuvpval);
	  }
	//printf("xyuvp: %s\n", xyuvpval);
	}
	//printf("xyuvp: %s\n", xyuvpval);
	//printf("x: %s\n%+.20e\n", xval, atof(xval));
	//printf("y: %s\n%+.20e\n", yval, atof(yval));
	//printf("u: %s\n%+.20e\n", uval, atof(uval));
	/* *(xData+j*25) = xval;
	*(yData+j*25) = yval;
	*(uData+j*25) = uval;
	*(vData+j*25) = vval;
	*(pData+j*25) = pval; */
	//value = atof(xyuvpval);
	return(value);
}

double Integrando1(int a, int b,int c, int d, double *xArray,
double *yArray, double *uArray, double *vArray,double *pArray){
  double x=0,y=0,X=0,Y=0,dx=0,dy=0;
  double u=0,v=0,p=0;
  double dxu=0,dxv=0,dxp=0;
  double dyu=0,dyv=0,dyp=0;
  double dxyu=0,dxyv=0,dxyp=0,dyxp=0;
  double dxxu=0,dxxv=0,dxxp=0;
  double dyyu=0,dyyv=0,dyyp=0;
  double dxxxu=0,dxxyu=0,dxxxv=0,dxxyv=0;
  double dyyxu=0,dyyyu=0,dyyxv=0,dyyyv=0;
  double Valor = 0, nu=0.001, Divergencia=0;
  /*X = getValue(c,d,Size, xArray);
  Y = getValue(c,d,Size, yArray);
  x = getValue(a,b,Size, xArray);
  y = getValue(a,b,Size, yArray);
  u = getValue(a,b,Size, uArray);
  v = getValue(a,b,Size, vArray);
  p = getValue(a,b,Size, pArray);
  dx=getValue(a+1,b,Size, xArray)-getValue(a,b,Size, xArray);
  dy=getValue(a,b+1,Size, yArray)-getValue(a,b,Size, yArray);
  dxu=(getValue(a+1,b,Size, uArray)-getValue(a-1,b,Size, uArray))/(2*dx);
  dxv=(getValue(a+1,b,Size, vArray)-getValue(a-1,b,Size, vArray))/(2*dx);
  dyu=(getValue(a,b+1,Size, uArray)-getValue(a,b-1,Size, uArray))/(2*dy);
  dyv=(getValue(a,b+1,Size, vArray)-getValue(a,b-1,Size, vArray))/(2*dy);
  dxxu=(getValue(a+1,b,Size, uArray)-2*getValue(a,b,Size, uArray)
       +getValue(a-1,b,Size, uArray))/(dx*dx);
  dyyu=(getValue(a,b+1,Size, uArray)-2*getValue(a,b,Size, uArray)
       +getValue(a,b-1,Size, uArray))/(dx*dx);
  dxyu=(getValue(a+1,b+1,Size, uArray)-getValue(a-1,b+1,Size, uArray)
	   -getValue(a+1,b-1,Size, uArray)+getValue(a-1,b-1,Size, uArray))/(4*dx*dy);
  dxxv=(getValue(a+1,b,Size, vArray)-2*getValue(a,b,Size, vArray)
       +getValue(a-1,b,Size, vArray))/(dx*dx);
  dyyv=(getValue(a,b+1,Size, vArray)-2*getValue(a,b,Size, vArray)
       +getValue(a,b-1,Size, vArray))/(dx*dx);
  dxyv=(getValue(a+1,b+1,Size, vArray)-getValue(a-1,b+1,Size, vArray)
	   -getValue(a+1,b-1,Size, vArray)+getValue(a-1,b-1,Size, vArray))/(4*dx*dy);
  dxxp=(getValue(a+1,b,Size, pArray)-2*getValue(a,b,Size, pArray)
       +getValue(a-1,b,Size, pArray))/(dx*dx);
  dyyp=(getValue(a,b+1,Size, pArray)-2*getValue(a,b,Size, pArray)
       +getValue(a,b-1,Size, pArray))/(dx*dx);
  dxyp=(getValue(a+1,b+1,Size, pArray)-getValue(a-1,b+1,Size, pArray)
	   -getValue(a+1,b-1,Size, pArray)+getValue(a-1,b-1,Size, pArray))/(4*dx*dy);
  dyxp=dxyp;
  dxxxu=(getValue(a+2,b,Size, uArray)-2*getValue(a+1,b,Size, uArray)
	    +2*getValue(a-1,b,Size, uArray)-getValue(a-2,b,Size, uArray))/(2*dx*dx*dx);
  dxxyu=(getValue(a+1,b+1,Size, uArray)-2*getValue(a,b+1,Size, uArray)
	    +getValue(a-1,b+1,Size, uArray)
		-getValue(a+1,b-1,Size, uArray)+2*getValue(a,b-1,Size, uArray)
	    -getValue(a-1,b-1,Size, uArray))/(2*dx*dx*dy);
  dyyyu=(getValue(a,b+2,Size, uArray)-2*getValue(a,b+1,Size, uArray)
	    +2*getValue(a,b-1,Size, uArray)-getValue(a,b-2,Size, uArray))/(2*dy*dy*dy);
  dyyxu=(getValue(a+1,b+1,Size, uArray)-2*getValue(a+1,b,Size, uArray)
	    +getValue(a+1,b-1,Size, uArray)
		-getValue(a-1,b+1,Size, uArray)+2*getValue(a-1,b,Size, uArray)
	    -getValue(a-1,b-1,Size, uArray))/(2*dy*dy*dx);
  dxxxv=(getValue(a+2,b,Size, vArray)-2*getValue(a+1,b,Size, vArray)
	    +2*getValue(a-1,b,Size, vArray)-getValue(a-2,b,Size, vArray))/(2*dx*dx*dx);
  dxxyv=(getValue(a+1,b+1,Size, vArray)-2*getValue(a,b+1,Size, vArray)
	    +getValue(a-1,b+1,Size, vArray)
		-getValue(a+1,b-1,Size, vArray)+2*getValue(a,b-1,Size, vArray)
	    -getValue(a-1,b-1,Size, vArray))/(2*dx*dx*dy);
  dyyyv=(getValue(a,b+2,Size, vArray)-2*getValue(a,b+1,Size, vArray)
	    +2*getValue(a,b-1,Size, vArray)-getValue(a,b-2,Size, vArray))/(2*dy*dy*dy);
  dyyxv=(getValue(a+1,b+1,Size, vArray)-2*getValue(a+1,b,Size, vArray)
	    +getValue(a+1,b-1,Size, vArray)
		-getValue(a-1,b+1,Size, vArray)+2*getValue(a-1,b,Size, vArray)
	    -getValue(a-1,b-1,Size, vArray))/(2*dy*dy*dx);*/
  X = xArray[c*200+d];
  Y = yArray[c*200+d];
  x = xArray[a*200+b];
  y = yArray[a*200+b];
  u = uArray[a*200+b];
  v = vArray[a*200+b];
  p = pArray[a*200+b];
  dx=xArray[(a+1)*200+b] - xArray[a*200+b];
  dy=yArray[a*200+(b+1)] - yArray[a*200+b];
  dxu=(uArray[(a+1)*200+b] - uArray[(a-1)*200+b])/(2*dx);
  dxv=(vArray[(a+1)*200+b] - vArray[(a-1)*200+b])/(2*dx);
  dyu=(uArray[a*200+(b+1)] - uArray[a*200+(b-1)])/(2*dy);
  dyv=(vArray[a*200+(b+1)] - vArray[a*200+(b-1)])/(2*dy);
  dxxu=(uArray[(a+1)*200+b] - 2*uArray[a*200+b]
	   + uArray[(a-1)*200+b])/(dx*dx);
  dyyu=(uArray[a*200+(b+1)] - 2*uArray[a*200+b]
       + uArray[a*200+(b-1)])/(dx*dx);
  dxyu=(uArray[(a+1)*200+(b+1)] - uArray[(a-1)*200+(b+1)]
  	   - uArray[(a+1)*200+(b-1)] + uArray[(a-1)*200+(b-1)])/(4*dx*dy);
  dxxv=(vArray[(a+1)*200+b] - 2*vArray[a*200+b]
       + vArray[(a-1)*200+b])/(dx*dx);
  dyyv=(vArray[a*200+(b+1)] - 2*vArray[a*200+b]
       + vArray[a*200+(b-1)])/(dx*dx);
  dxyv=(vArray[(a+1)*200+(b+1)] - vArray[(a-1)*200+(b+1)]
	   - vArray[(a+1)*200+(b-1)] + vArray[(a-1)*200+(b-1)])/(4*dx*dy);
  dxxp=(pArray[(a+1)*200+b] - 2*pArray[a*200+b]
       + pArray[(a-1)*200+b])/(dx*dx);
  dyyp=(pArray[a*200+(b+1)] - 2*pArray[a*200+b]
       + pArray[a*200+(b-1)])/(dx*dx);
  dxyp=(pArray[(a+1)*200+(b+1)] - pArray[(a-1)*200+(b+1)]
	   - pArray[(a+1)*200+(b-1)] + pArray[(a-1)*200+(b-1)])/(4*dx*dy);
  dyxp=dxyp;
  dxxxu=(uArray[(a+2)*200+b] - 2*uArray[(a+1)*200+b]
	    + 2*uArray[(a-1)*200+b] - uArray[(a-2)*200+b])/(2*dx*dx*dx);
  dxxyu=(uArray[(a+1)*200+(b+1)] - 2*uArray[a*200+(b+1)]
	    + uArray[(a-1)*200+(b+1)]
		- uArray[(a+1)*200+(b-1)] + 2*uArray[a*200+(b-1)]
	    - uArray[(a-1)*200+(b-1)])/(2*dx*dx*dy);
  dyyyu=(uArray[a*200+(b+2)] - 2*uArray[a*200+(b+1)]
	    + 2*uArray[a*200+(b-1)] - uArray[a*200+(b-2)])/(2*dy*dy*dy);
  dyyxu=(uArray[(a+1)*200+(b+1)] - 2*uArray[(a+1)*200+b]
	    + uArray[(a+1)*200+(b-1)] - uArray[(a-1)*200+(b+1)]
	    + 2*uArray[(a-1)*200+b] - uArray[(a-1)*200+(b-1)])/(2*dy*dy*dx);
  dxxxv=(vArray[(a+2)*200+b] - 2*vArray[(a+1)*200+b]
		+ 2*vArray[(a-1)*200+b] - vArray[(a-2)*200+b])/(2*dx*dx*dx);
  dxxyv=(vArray[(a+1)*200+(b+1)] - 2*vArray[a*200+(b+1)]
	    + vArray[(a-1)*200+(b+1)] - vArray[(a+1)*200+(b-1)]
	    + 2*vArray[a*200+(b-1)] - vArray[(a-1)*200+(b-1)])/(2*dx*dx*dy);
  dyyyv=(vArray[a*200+(b+2)] - 2*vArray[a*200+(b+1)]
	    + 2*vArray[a*200+(b-1)] - vArray[a*200+(b-2)])/(2*dy*dy*dy);
  dyyxv=(vArray[(a+1)*200+(b+1)] - 2*vArray[(a+1)*200+b]
		+ vArray[(a+1)*200+(b-1)] - vArray[(a-1)*200+(b+1)]
		+ 2*vArray[(a-1)*200+b] - vArray[(a-1)*200+(b-1)])/(2*dy*dy*dx);
  
  Valor =   (X-x)*(X-x)*(dxxp-nu*(dxxxu+dyyxu)+2*dxu*dxu+2*dxv*dyu)/(pow(X-x,2)+pow(Y-y,2))
		   +(Y-y)*(X-x)*(dyxp-nu*(dxxyu+dyyyu)+2*dyu*dxu+2*dyv*dyu)/(pow(X-x,2)+pow(Y-y,2))
		   +(X-x)*(Y-y)*(dxyp-nu*(dxxxv+dyyxv)+2*dxu*dxv+2*dxv*dyv)/(pow(X-x,2)+pow(Y-y,2))
		   +(Y-y)*(Y-y)*(dyyp-nu*(dxxyv+dyyyv)+2*dyu*dxv+2*dyv*dyv)/(pow(X-x,2)+pow(Y-y,2))
		+pow((X-x)*(X-x)*dxu+(Y-y)*(X-x)*dyu+(X-x)*(Y-y)*dxv+(Y-y)*(Y-y)*dyv,2)
			/pow(pow(X-x,2)+pow(Y-y,2),2)
		+pow((X-x)*dxu+(Y-y)*dxv,2)/(pow(X-x,2)+pow(Y-y,2))
		+pow((X-x)*dyu+(Y-y)*dyv,2)/(pow(X-x,2)+pow(Y-y,2));
   
   //printf("Integrand: %+.10e, Div: %+.10e, i: %d, j: %d\n", Valor, Divergencia, a, b);
  return(Valor);

}

double Integrando2(int a, int b,int c, int d, double *xArray,
double *yArray, double *uArray, double *vArray, double *pArray){
  double x=0,y=0,X=0,Y=0,dx=0,dy=0;
  double u=0,v=0,p=0;
  double dxu=0,dxv=0,dxp=0;
  double dyu=0,dyv=0,dyp=0;
  double dxyu=0,dxyv=0,dxyp=0,dyxp=0;
  double dxxu=0,dxxv=0,dxxp=0;
  double dyyu=0,dyyv=0,dyyp=0;
  double dxxxu=0,dxxyu=0,dxxxv=0,dxxyv=0;
  double dyyxu=0,dyyyu=0,dyyxv=0,dyyyv=0;
  double Valor = 0;
  X = xArray[c*200+d];
  Y = yArray[c*200+d];
  x = xArray[a*200+b];
  y = yArray[a*200+b];
  u = uArray[a*200+b];
  v = vArray[a*200+b];
  p = pArray[a*200+b];
  dx=xArray[(a+1)*200+b] - xArray[a*200+b];
  dy=yArray[a*200+(b+1)] - yArray[a*200+b];
  dxu=(uArray[(a+1)*200+b] - uArray[(a-1)*200+b])/(2*dx);
  dxv=(vArray[(a+1)*200+b] - vArray[(a-1)*200+b])/(2*dx);
  dyu=(uArray[a*200+(b+1)] - uArray[a*200+(b-1)])/(2*dy);
  dyv=(vArray[a*200+(b+1)] - vArray[a*200+(b-1)])/(2*dy);
  
  Valor = -((X-x)*(X-x)*dxu + (Y-y)*(X-x)*dyu
	      + (X-x)*(Y-y)*dxv + (Y-y)*(Y-y)*dyv)/(pow(X-x,2) + pow(Y-y,2));

   //printf("Integrand: %+.10e, Div: %+.10e, i: %d, j: %d\n", Valor, Divergencia, a, b);
  return(Valor);

}
/*----------GET IMAGE INFO SUBPROGRAM--------------*/
/*long getImageInfo(FILE* inputFile, long offset, int numberOfChars)
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
    // calculate value based on adding bytes 

    value = value + *ptrC*(double)pow((double)256,j);
	
  }
  return(value);

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

  for(i=0; i<=54; i++)
  {
    fread(ptrC, sizeof(char), 1, inputFile);
    fwrite(ptrC, sizeof(char), 1, outputFile);
	//printf("%s\n",ptrC);
  }

}*/

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

  for(i=0; i<=(4*nColors); i++)  // there are (4*nColors) bytesin color table
  {
    fread(ptrC, sizeof(char), 1, inputFile); 
    fwrite(ptrC, sizeof(char), 1, outputFile);
	//printf("%s\n",ptrC);
  }

}*/






