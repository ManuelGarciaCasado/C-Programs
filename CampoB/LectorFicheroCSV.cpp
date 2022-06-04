// ConsoleApplication5.cpp: define el punto de entrada de la aplicación de consola.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

int main()
{
		char ch1,ch2, file_name[250];
	
		float temp_med[366];
		float ener_act[366];
		float* aux1;
		float* aux2;
		FILE *fp;
		printf("Introduza nombre de fichero .csv\n");
		gets_s(file_name);

		fp = fopen(file_name, "r"); // read mode

		if (fp == NULL)
		{
			perror("Error while opening the file.\n");
			exit(EXIT_FAILURE);
		}

		printf("The contents of %s file are:\n", file_name);
		char *s_numero_leido="";
		
		int num_ptocoma = 0;
		int num_linea = 0;
		int num_char = 0;
		
		aux1 = (float*)malloc( sizeof(float));
		aux2 = (float*)malloc(sizeof(float));
		while ((ch1 = fgetc(fp)) != EOF){

			if (ch1 == ';')
			{
				if (num_ptocoma == 3 && num_linea > 0 )
				{
					
						if (s_numero_leido != "") {
							float numero = atof(s_numero_leido);

							temp_med[num_linea - 1] = numero;
							
							
							aux1 = (float*)realloc(aux1,num_linea*sizeof(float));
							
							//	aux = (float*)malloc(num_linea * sizeof(float));
							aux1[num_linea-1] = numero;
							free(s_numero_leido);
						}
					
				}
			
				*s_numero_leido = '\0';
				num_char = 0;
				num_ptocoma++;
			}
			else if (ch1 == '\n')
			{
				if (num_linea > 0 )
				{
					if (s_numero_leido != "") {
						float numero = atof(s_numero_leido);


						ener_act[num_linea - 1] = numero;
						aux2 = (float*)realloc(aux2, num_linea * sizeof(float));
						aux2[num_linea - 1] = numero;
						free(s_numero_leido);
					}
					
				}
				
				*s_numero_leido = '\0';
				num_char = 0;
				num_ptocoma = 0;
				num_linea++;
			}
			else
			{
				if (ch1 == ',')ch1 = '.';
				size_t len = strlen(s_numero_leido);
				char * str2;
				str2 = (char *)malloc(len + 3);
				strcpy( str2, s_numero_leido);
				str2[len]=ch1;
				str2[len+1] = '\0';
				s_numero_leido = (char *)malloc((len + 3)*sizeof(char));
				strcpy(s_numero_leido, str2);
				free(str2);
				num_char++;
			}
			printf("%c", ch1);
		}		
		fclose(fp);
		for (int j = 0; j < num_linea-1;j++)
		{
			float *t;
			float *e;
			t = &aux1[j];
			e = &aux2[j];
			printf("%f %f\n",  *t, *e);
			;
		}
		return 0;
	
}

