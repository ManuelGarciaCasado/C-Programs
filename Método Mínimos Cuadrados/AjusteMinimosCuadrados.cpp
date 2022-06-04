// AjusteMinimosCuadrados.cpp: define las funciones exportadas de la aplicación DLL.
//
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

int const n = 366;
int const m = 4;
int norma(double lambdamx1[m], int n, double *modlamb);
int derimatrix(double a[n][m], double x[n], double y[n], double lambda[m], int nfil);
int regresionlineal(double x[n], double y[n], double* beta_0, double* beta_1, double *R2, int nfil);
int gaussj(double a[m][m], int nfila, double bsol[m], int ncol);
int gauss(double A[m][m+1], double x[m]);

//__declspec(dllexport) int __stdcall calcularParametos(char file_name[250], double *A, double *B, double *C, double *D);
//__declspec(dllexport) int __stdcall calcularParametos(char file_name[250], double *A, double *B, double *C, double *D)
	
	

int norma(double x[m], int tam, double *modulo)
{

	double suma = 0;

	for (int i = 0; i < tam; i++)
	{
		suma = suma + x[i] * x[i];
	}

	*modulo = sqrt(suma);
	return 0;
}
int gaussj(double a[m][m], int nfila, double bsol[m], int ncol)
{
	/*ENTRADA: a(matriz nxn de dimensión física npxnp), b(matriz nxm
	de dimensión física npxmp)
	SALIDA : a(matriz inversa de a), b(vector x solución
	del sistema ax = b)*/
	int ifil = 0;
	int icol = 0;
	int indfil[50];
	int indcol[50];
	int ipiv[50];
	double big = 0;
	double dum = 0;
	double pivinv = 0;
	double b[m][m];

	for (int j = 0; j < nfila; j++)
	{
		ipiv[j] = 0;
	}
	for (int j = 0; j < m; j++)
	{
		for (int i = 0; i < m; i++)
		{

			if (j == 0) {

				b[i][j] = bsol[i];
			}
			else
			{
				b[i][j] = 0;
			}
		}
	}
	for (int i = 0; i < nfila; i++)
	{
		big = 0;
		for (int j = 0; j < nfila; j++)
		{
			if (ipiv[j] != 1)
			{
				for (int k = 0; k < nfila; k++)
				{
					if (ipiv[k] == 0) {

						if (fabs(a[j][k]) >= big) {
							big = fabs(a[j][k]);
							ifil = j;
							icol = k;
						}
					}
					else if (ipiv[k] > 1) {
						printf("%s", "singular matrix in gaussj ");

					}
				}
			}

		}
		ipiv[icol] = ipiv[icol] + 1;
		if (ifil != icol)
		{

			for (int l = 0; l < nfila; l++) {
				dum = a[ifil][l];
				a[ifil][l] = a[icol][l];
				a[icol][l] = dum;
			}

			for (int l = 0; l < m; l++) {
				dum = b[ifil][l];
				b[ifil][l] = b[icol][l];
				b[icol][l] = dum;
			}
		}
		indfil[i] = ifil;
		indcol[i] = icol;

		if (a[icol][icol] == 0)
		{
			printf("s%", "singular matrix in gaussj ");
		}
		pivinv = 1 / a[icol][icol];
		a[icol][icol] = 1;

		for (int l = 0; l < nfila; l++) {
			a[icol][l] = a[icol][l] * pivinv;
		}

		for (int l = 0; l < m; l++) {
			b[icol][l] = b[icol][l] * pivinv;
		}

		for (int ll = 0; ll < nfila; ll++)
		{

			if (ll != icol) {
				dum = a[ll][icol];
				a[ll][icol] = 0;

				for (int l = 0; l < nfila; l++)
				{
					a[ll][l] = a[ll][l] - a[icol][l] * dum;
				}

				for (int l = 0; l < m; l++)
				{
					b[ll][l] = b[ll][l] - b[icol][l] * dum;
				}
			}
		}
	}

	for (int l = 0; l < nfila; l++) {

		if (indfil[nfila - l] != indcol[nfila - l]) {

			for (int k = 0; k < nfila; k++) {
				dum = a[k][indfil[nfila - l]];
				a[k][indfil[nfila - l]] = a[k][indcol[nfila - l]];
				a[k][indcol[nfila - l]] = dum;
			}
		}
	}

	for (int j = 0; j < m; j++)
	{
		for (int i = 0; i < m; i++)
		{
			if (j == 0) {

				bsol[i] = b[i][j];
			}
		}
	}
	return 0;
}
int derimatrix(double a[n][m], double x[n], double y[n], double lambda[m], int nfil)
{
	for (int i = 0; i < nfil; i++)
	{

		/*  a[i][0] = powf(1 + powf((x[i] - lambda[2]) / lambda[1],2),-1);
		a[i][1] = (2 * lambda[0] / powf(lambda[1],3))*powf((x[i] - lambda[2]),2)
		/ powf(1 + powf((x[i] - lambda[2]) / lambda[1], 2), 2);

		a[i][2] = (2 * lambda[0] /powf( lambda[1],2))*(x[i] - lambda[2])
		/powf(1 + powf((x[i] - lambda[2]) / lambda[1],2), 2);
		a[i][3] = 1;    */

		/*a[i][0] = 1.;
		a[i][1] = x[i];
		a[i][2] = (lambda[2] + x[i] * lambda[3] )/sqrt(lambda[4] + pow(lambda[2]+x[i]* lambda[3],2));
		a[i][3] = x[i] * (lambda[2] + x[i] * lambda[3]) / sqrt(lambda[4] + pow(lambda[2] + x[i] * lambda[3], 2));
		a[i][4] = 1./(2.* sqrt(lambda[4] + pow(lambda[2] + x[i] * lambda[3], 2)));*/
		a[i][0] = 1.;
		a[i][1] = x[i];
		a[i][2] = (lambda[2] + x[i] * lambda[3]) / sqrt(1 + pow(lambda[2] + x[i] * lambda[3], 2));
		a[i][3] = x[i] * (lambda[2] + x[i] * lambda[3]) / sqrt(1 + pow(lambda[2] + x[i] * lambda[3], 2));

	}

	return 0;
}
int gauss(double A[m][m+1], double x[m])
{
	int N = m;

	for (int i = 0; i < N; i++) {
		// Search for maximum in this column
		double maxEl = fabs(A[i][i]);
		int maxRow = i;
		for (int k = i + 1; k<N; k++) {
			if (fabs(A[k][i]) > maxEl) {
				maxEl = fabs(A[k][i]);
				maxRow = k;
			}
		}

		// Swap maximum row with current row (column by column)
		for (int k = i; k < N + 1; k++) {
			double tmp = A[maxRow][k];
			A[maxRow][k] = A[i][k];
			A[i][k] = tmp;
		}

		// Make all rows below this one 0 in current column
		for (int k = i + 1; k < N; k++) {
			double c = -A[k][i] / A[i][i];
			for (int j = i; j < N + 1; j++) {
				if (i == j) {
					A[k][j] = 0;
				}
				else {
					A[k][j] += c * A[i][j];
				}
			}
		}
		/*printf("\n\n colunna %d:\n", i);
		for (int ia = 0; ia < 22; ia++)
		{
		for (int ja = 0; ja < 23; ja++)
		{
		if (ja != 22) {
		printf("%f,", A[ia][ja]);
		}
		else
		{
		printf("%f\n", A[ia][ja]);
		}

		}

		}*/
	}



	// Solve equation Ax=b for an upper triangular matrix A

	for (int i = N - 1; i >= 0; i--) {
		x[i] = A[i][N] / A[i][i];
		for (int k = i - 1; k >= 0; k--) {
			A[k][N] -= A[k][i] * x[i];
		}
	}

	return 0;
}


extern "C" __declspec(dllexport) int __cdecl calcularParametros(char file_name[250], double *A, double *B, double *C, double *D);
	__declspec(dllexport) int __cdecl calcularParametros(char file_name[250],double *A, double *B,double *C,double *D)
	{
		char ch1, ch2;
	
		double temp_med[n];
		double ener_act[n];
		double anxm[n][m], amxm[m][m], bmx1[m], inclamb[m], incbeta[n], lambdamx1[m],
			suma, xnx1[n], ynx1[n], vectmx1[m];
		double* modlamb;

		double* aux1;
		double* aux2;
		FILE *fp;
		double * beta_0, *beta_1, *coef_det;
		beta_0 = (double*)malloc(sizeof(double));
		beta_1 = (double*)malloc(sizeof(double));
		coef_det = (double*)malloc(sizeof(double));
		printf("El nombre de fichero .csv es %s\n", file_name);
		//gets_s(file_name);

		fp = fopen(file_name, "r"); // read mode

		if (fp == NULL)
		{
			perror("Error while opening the file.\n");
			exit(EXIT_FAILURE);
		}

		printf("The contents of %s file are:\n", file_name);
		char *s_numero_leido = "";

		int num_ptocoma = 0;
		int num_linea = 0;
		int num_char = 0;

		aux1 = (double*)malloc(sizeof(double));
		aux2 = (double*)malloc(sizeof(double));
		while ((ch1 = fgetc(fp)) != EOF) {

			if (ch1 == ';')
			{
				if (num_ptocoma == 1 && num_linea > 0)
				{

					if (*s_numero_leido != '\0') {
						double numero = atof(s_numero_leido);

						temp_med[num_linea - 1] = numero;


						aux1 = (double*)realloc(aux1, num_linea * sizeof(double));
						//	aux = (double*)malloc(num_linea * sizeof(double));
						aux1[num_linea - 1] = numero;
					//	free(s_numero_leido);
					}
					else
					{
						temp_med[num_linea - 1] = NAN;
						aux1 = (double*)realloc(aux1, num_linea * sizeof(double));
						//	aux = (double*)malloc(num_linea * sizeof(double));
						aux1[num_linea - 1] = NAN;
					}

				}

				*s_numero_leido = '\0';
				num_char = 0;
				num_ptocoma++;
			}
			else if (ch1 == '\n')
			{
				if (num_linea > 0)
				{
					if (*s_numero_leido != '\0') {
						double numero = atof(s_numero_leido);


						ener_act[num_linea - 1] = numero;
						aux2 = (double*)realloc(aux2, num_linea * sizeof(double));
						aux2[num_linea - 1] = numero;
						//free(s_numero_leido);
					}
					else
					{
						ener_act[num_linea - 1] = NAN;
						aux2 = (double*)realloc(aux2, num_linea * sizeof(double));
						aux2[num_linea - 1] = NAN;
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
				strcpy(str2, s_numero_leido);
				str2[len] = ch1;
				str2[len + 1] = '\0';
				s_numero_leido = (char *)malloc((len + 3) * sizeof(char));
				strcpy(s_numero_leido, str2);
				free(str2);
				num_char++;
			}
			printf("%c", ch1);
		}
		fclose(fp);
		for (int j = 0; j < m; j++) {

			for (int i = 0; i < n; i++) {
				xnx1[i] = 0;
				ynx1[i] = 0;				
				incbeta[i] = 0;
				anxm[i][j] = 0;				
			}
			inclamb[j] = 0;
			bmx1[j] = 0;
			lambdamx1[j] = 0;
		}
		for (int j = 0; j < m; j++) {

			for (int i = 0; i < m; i++) {
				
					amxm[i][j] = 0;
			}		
		}
		
		int num_lin_real = 0;
		for (int j = 0; j < num_linea - 1; j++)
		{
			double *t;
			double *e;
			if (isnan(aux1[j]) == 0 && isnan(aux2[j]) == 0) {
				xnx1[num_lin_real] = aux1[j];
				ynx1[num_lin_real] = aux2[j];
				num_lin_real++;
			}
		}
		num_linea = num_lin_real;
		regresionlineal(xnx1, ynx1, beta_0, beta_1, coef_det, num_linea - 1);
		printf("Calculando parámetros A, B, C, D, E:\n\n");
		printf("                                      1/2  \n");
		printf("                  /              2   \\    \n");
		printf("y = A + B x +    |  1 + (C + D x)     |    \n");
		printf("                  \\                 /     \n");
		lambdamx1[0] = *beta_0 - sqrt(2);
		lambdamx1[1] = *beta_1;
		lambdamx1[2] = 1;
		lambdamx1[3] = 0;
		//lambdamx1[4] = 1;
		for (int iterconv = 0; iterconv < 1000; iterconv++) {
			//int imax = 0;
			//for (int i = 0; i < num_linea - 1; i++)
			//{
			//if (lambdamx1[0] != fmax(lambdamx1[0], ynx1[i]))
			//{
			// imax = i;
			//}
			//lambdamx1[0] = fmax(lambdamx1[0], ynx1[i]);
			//}
			//lambdamx1[2] = xnx1[imax];
			modlamb = (double*)malloc(sizeof(double));;
			norma(lambdamx1, m, modlamb);
		
			derimatrix(anxm, xnx1, ynx1, lambdamx1, num_linea - 1);
			for (int i = 0; i < num_linea - 1; i++) {
				//incbeta[i] = 0;
				//incbeta[i] = ynx1[i] - (lambdamx1[0] + lambdamx1[1] * xnx1[i]
				//	+ sqrt(lambdamx1[4]+pow(lambdamx1[2] + lambdamx1[3] * xnx1[i],2)));
				incbeta[i] = ynx1[i] - (lambdamx1[0] + lambdamx1[1] * xnx1[i]
				+ sqrt(1 + pow(lambdamx1[2] + lambdamx1[3] * xnx1[i], 2)));
			}
			
			//amxm igual a la traspuesta de anxm por anxm
		
			for (int i = 0; i < m; i++) {
				for (int j = 0; j < m; j++) {
					amxm[i][j] = 0;
					for (int k = 0; k < num_linea - 1; k++) {
						amxm[i][j] = amxm[i][j] + anxm[k][i] * anxm[k][j];
					}
					
				}
				
			}
			for (int j = 0; j < m; j++) {
			bmx1[j] = 0;
			for (int i = 0; i < num_linea - 1; i++) {
			bmx1[j] = bmx1[j] + anxm[i][j] * incbeta[i];
			}
			}
			int fil = m;
			int col = 1;
			for (int j = 0; j < m; j++) {
			vectmx1[j] = bmx1[j];
			}
			/*printf("Entrada: \n");
			for (int i = 0; i < m; i++) {
				for (int j = 0; j < m; j++) {
					
					printf("%f,", amxm[i][j]);
				}
				printf("%f\n", vectmx1[i]);
			}*/

			gaussj(amxm, fil, vectmx1, col);

			/*printf("Salida: \n");
			for (int i = 0; i < m; i++) {
				for (int j = 0; j < m; j++) {
				
					
						printf("%f,", amxm[i][j]);
				}
				printf("%f\n", vectmx1[i]);
			}*/

			for (int j = 0; j < m; j++)
			{
			inclamb[j] = vectmx1[j];
			lambdamx1[j] = lambdamx1[j] + inclamb[j];
			}
			printf("A: %f \t", lambdamx1[0]);
			printf("B: %f \t", lambdamx1[1]);
			printf("C: %f \t", lambdamx1[2]);
			printf("D: %f \n", lambdamx1[3]);
			//printf("E: %f \n", lambdamx1[4]);
			if (isnan(lambdamx1[0]))
			{
				break;
			}
			*A = lambdamx1[0];
			*B = lambdamx1[1];
			*C = lambdamx1[2];
			*D = lambdamx1[3];
		}
		/*lambdamx1[0] = 1000;
		lambdamx1[1] = 1;
		lambdamx1[2] = 1000;
		lambdamx1[3] = 1;
		 *A = lambdamx1[0];
		*B = lambdamx1[1];
		*C = lambdamx1[2];
		*D = lambdamx1[3];*/
		printf("Fin de cálculo.\n");
		

		return 0;

}

