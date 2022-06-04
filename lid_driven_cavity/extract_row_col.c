#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double getValue(int ipos, int jpos, int Size, unsigned char * Array );
double Integrando1(int a, int b, int c, int d, int Size, unsigned char *xArray,
		unsigned char *yArray, unsigned char *uArray,
		unsigned char *vArray,unsigned char *pArray);
double Integrando2(int a, int b, int c, int d, int Size, unsigned char *xArray,
		unsigned char *yArray, unsigned char *uArray,
		unsigned char *vArray,unsigned char *pArray);
double circunferencia1 = 0,circunferencia2 = 0;
double campo1 = 0, campo2 = 0;


void main(int argc, char* argv[])
{
  FILE			*ufieldInput, *vfieldInput,*pfieldInput,*Output1,*Output2;
  unsigned char		someChar;
  unsigned char *pChar;
  int a,b,i,j,k,xnum,ynum;
  int uSize = 0, vSize = 0, pSize = 0;
  unsigned char uBuffer[80]={0},vBuffer[80]={0},pBuffer[80]={0};
  unsigned char xval[25]={0},yval[25]={0},uval[25]={0},vval[25]={0},pval[25]={0};
  char ch;
  unsigned char *xData = 0;
  unsigned char *yData = 0;
  unsigned char *uData = 0;
  unsigned char *vData = 0;
  unsigned char *pData = 0;
  unsigned char *dblBuffer = 0;
  char Xchar[5],Ychar[5],Rchar[5],Tini[3], Tfin[3], Tchar[3];
  char Unamefile[10]={0},Vnamefile[10]={0},Pnamefile[10]={0};
  int  X,Y,R, ti,tf,t;
  double x,y,z,r,u,v,p;
  double xinc, yinc;
  /* initialize pointer */
  
  if(argc < 4)
  {
    printf("Usage: %s u v p\n", argv[0], argv[1], argv[2], argv[3]);
    exit(0);
  }
//  printf ("Center R radius (R <= 97): ");
//  gets (Rchar);
  printf ("Center X axe (0 <= X < 200): ");
  gets (Xchar);
  printf ("Center Y axe (0 <= Y < 200): ");
  gets (Ychar);
//  R= atoi(Rchar);
  X= atoi(Xchar);
  Y= atoi(Ychar);
  printf ("Time initial file name: ");
  gets (Tini);
  ti= atoi(Tini);
  printf ("Time final file name: ");
  gets (Tfin);
  tf= atoi(Tfin);
  Output1 = fopen("ROW", "w");
  Output2 = fopen("COL", "w");
  fprintf(Output1,"X	U	V	P\n");
  fprintf(Output2,"Y	U	V	P\n");
  fclose(Output1);
  fclose(Output2);
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
	
  xData = (unsigned char *) malloc(uSize*25*sizeof(unsigned char));
  yData = (unsigned char *) malloc(uSize*25*sizeof(unsigned char));
  uData = (unsigned char *) malloc(uSize*25*sizeof(unsigned char));
  vData = (unsigned char *) malloc(uSize*25*sizeof(unsigned char));
  pData = (unsigned char *) malloc(uSize*25*sizeof(unsigned char));
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
	*(xData+j*25+i) = uBuffer[i];
	*(yData+j*25+i) = uBuffer[(i+25)];
	*(uData+j*25+i) = uBuffer[(i+50)];
	*(vData+j*25+i) = vBuffer[(i+50)];
	*(pData+j*25+i) = pBuffer[(i+50)];
	
	xval[i] = *(xData+j*25+i);
	}
//	printf("x: %s = %+.20e\n", xval, atof(xval));
  }
//  printf("x: %s = %+.20e\n", xval, atof(xval));
  fclose(ufieldInput);
  fclose(vfieldInput);
  fclose(pfieldInput);

  for(j = 0; j < uSize; j++){
  
	for(i = 0; i < 25; i++){
		xval[i] = *(xData+j*25+i);
		yval[i] = *(yData+j*25+i);
		uval[i] = *(uData+j*25+i);
		vval[i] = *(vData+j*25+i);
		pval[i] = *(pData+j*25+i);
		
	}
	//printf("%c",xval);
	//printf("x: %s = %+.20e\n", xval, atof(xval));
	//printf("y: %s\n%+.20e\n", yval, atof(yval));
	//printf("u: %s\n%+.20e\n", uval, atof(uval));
	//*(xData+j*25) = xval;
	//*(yData+j*25) = yval;
	//*(uData+j*25) = uval;
	//*(vData+j*25) = vval;
	//*(pData+j*25) = pval; 
  }
  Output1 = fopen("ROW", "a");
  for(i = 0; i < 200; i++){
	x = getValue(i,Y,uSize, xData);
	//y = getValue(i,Y,uSize, yData);
	u = getValue(i,Y,uSize, uData);
	v = getValue(i,Y,vSize, vData);
	p = getValue(i,Y,pSize, pData);
	fprintf(Output1,"%+.10e	%+.10e	%+.10e	%+.10e\n",x,u,v,p);

  }
  fclose(Output1);
  Output2 = fopen("COL", "a");
  for(i = 0; i < 200; i++){
	//x = getValue(i,Y,uSize, xData);
	y = getValue(X,i,uSize, yData);
	u = getValue(X,i,uSize, uData);
	v = getValue(X,i,vSize, vData);
	p = getValue(X,i,pSize, pData);
	fprintf(Output2,"%+.10e	%+.10e	%+.10e	%+.10e\n",y,u,v,p);
  }
  fclose(Output2);


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






