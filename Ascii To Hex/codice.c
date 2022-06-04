#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <io.h>
void char2hex(char h, char hFF[2]);
void main(int argc, char* argv[]){
	char letra1;
	//char letra2;
	char letra1FF[2];
	char puntero;
	int i;
	//char letra2FF[2];
	FILE *fichEntrada1;
	//FILE *fichEntrada2;
	FILE *fichSalida;
	if(argc < 2)
  {
    printf("Usage: %s entrada.txt\n", argv[0]);
    exit(0);
  }
  printf("Reading filename %s\n", argv[1]);

  /*--------READ INPUT FILE------------*/
  
	fichEntrada1 = fopen(argv[1],"rb");
//fichEntrada1 = fopen("loop.cfg","rb");
	//fichEntrada2 = fopen("encoded.ab","rb");
    fichSalida = fopen("salida.txt","wb");
	/*if((fichEntrada1 == NULL)|| (fichEntrada2 == NULL)|| (fichSalida == NULL))

      {

            printf("No se ha podido abrir el archivo.\n");

            printf("Pulse una tecla para finalizar... ");

            getchar();

            exit(1);

      }*/

    

    for(i=0;i<0xFFFF;i++){
		letra1 = fgetc(fichEntrada1);
		char2hex(letra1, &letra1FF[0]);
        //letra2 = fgetc(fichEntrada2);
		//char2hex(letra2, &letra2FF[0]);
		fputc(letra1, fichSalida);
		fputc(' ', fichSalida);
		//fputc(letra2, fichSalida);
		//fputc(' ', fichSalida);
		//fputc(' ', fichSalida);
		fputc(letra1FF[0], fichSalida);
		fputc(letra1FF[1], fichSalida);
        fputc(' ', fichSalida);
		//fputc(letra2FF[0], fichSalida);
		//fputc(letra2FF[1], fichSalida);
        fputc('\n', fichSalida);
	};
    fclose(fichEntrada1);
	//fclose(fichEntrada2);
	fclose(fichSalida);
};

void char2hex(unsigned char h, char hFF[2]){
	int i,j;
	int valor ;
	int hexadec;
	
	valor = (int)h;
	//valor = 129;
	//hFF = &valor;
	//hFF[0] = (char)valor;
	//hFF[1] = "71";
	for (i=0;i <=15;i++){
		for (j=0;j<=15;j++)
		{
			if (valor==(i*16+j) && i > 9){
				hexadec= 55+i;
				hFF[0]= (char)hexadec;}
			else if(valor==(i*16+j) && i <= 9){
				hexadec= 48+i;
			    hFF[0]= (char)hexadec;}
			 if (valor==(i*16+j) && j > 9){
				hexadec= 55+j;
				hFF[1]=(char)hexadec;}
			else if(valor==(i*16+j) && j <= 9){
			    hexadec= 48+j;
				hFF[1]=(char)hexadec;}
			}	
		}
	} 
