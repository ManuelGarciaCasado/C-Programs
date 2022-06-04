// VelocidadesPresionFluido.cpp: define el punto de entrada de la aplicación de consola.
//

#pragma warning(disable : 4996)
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <malloc.h>
#include <string.h>
#include <float.h>


double const rho = 100;
double const nu = 0.01;
double lambda = -1;
int const largo = 6;
int const ancho = 6;
int const alto = 6;
int const size = (largo - 3) * (ancho - 3) * (alto - 3) * 4;
double const PI = 3.14159265359;
struct puntos {
	double** coordenadas;
	int tamano;
};
struct puntos ReadFile(char file_name[250]);
double ** CampoMagnetico(struct puntos coord_tamano, double** B, double cambioBase[3][3], double traslacion[3], double a, double signo);
double comp_ellint_1(double m, double error);
double comp_ellint_2(double m, double error);
int factorial(int n);

//int norma(double lambdamx1[m], int n, double *modlamb);
//int derimatrix(double a[n][m], double x[n], double y[n], double lambda[m], int nfil);

int gauss(double a[size][size + 1], double x[size]);


int main()
{
	char nombre_fichero[250];
	//double u[m][m][m], v[m][m][m], w[m][m][m], p[m + 2][m + 2][m + 2];
	
	double **coordenadas;
	double **B;
	struct puntos coord_tamano;
	double A[3][3];	
	double C[3][3];
	double R[3][3];	
	double b[3];
	double c[3];
	double a;
	//double anxm[n][m], amxm[m][m], bmx1[m], inclamb[m], incbeta[n], lambdamx1[m],
	//	suma, xnx1[n], ynx1[n], vectmx1[m];
	double* modlamb;

	double* aux1;
	double* aux2;	
	FILE* fp_out;
	printf("Introduza nombre de fichero .csv\n");
	gets_s(nombre_fichero);
	
	coord_tamano = ReadFile(nombre_fichero);

	double x = coord_tamano.coordenadas[0][1];
	int tamano = coord_tamano.tamano;
	B = (double**)malloc(3 * sizeof(double*));
	B[0] = (double*)malloc(tamano * sizeof(double));
	B[1] = (double*)malloc(tamano * sizeof(double));
	B[2] = (double*)malloc(tamano * sizeof(double));
	double signo = 1;
	for (int i = 0; i < tamano; i++)
	{
		B[0][i] = 0.;
		B[1][i] = 0.;
		B[2][i] = 0.;
	}
	/*for (int i = 0; i < 10; i++) {
		A[0][0] = 1;
		A[0][1] = 0;
		A[0][2] = 0;
		A[1][0] = 0;
		A[1][1] = 1;
		A[1][2] = 0;
		A[2][0] = 0;
		A[2][1] = 0;
		A[2][2] = 1;
		b[0] = 0;
		b[1] = 0;
		b[2] = i* 0.1;
		B = CampoMagnetico(coord_tamano, B, A, b);
	}*/
	for (int i = 0; i < 20;i++) {
		a = (1. + i * 0.05) / (2. * sqrt(3));
		/* 1 */
		A[0][0] = 0;
		A[0][1] = -1;
		A[0][2] = 0;
		A[1][0] = 1. / sqrt(3);
		A[1][1] = 0;
		A[1][2] = -sqrt(2) / sqrt(3);
		A[2][0] = sqrt(2) / sqrt(3);
		A[2][1] = 0;
		A[2][2] = 1. / sqrt(3);
		b[0] = (1. + i * 0.05) * 1. / 3.;
		b[1] = (1. + i * 0.05) * 0;
		b[2] = (1. + i * 0.05) * 1. / (3 * sqrt(2));
		B = CampoMagnetico(coord_tamano, B, A, b, a, +1.);

		/* 2 */
		A[0][0] = 1;
		A[0][1] = 0;
		A[0][2] = 0;
		A[1][0] = 0;
		A[1][1] = 1. / sqrt(3);;
		A[1][2] = -sqrt(2) / sqrt(3);
		A[2][0] = 0;
		A[2][1] = sqrt(2) / sqrt(3);
		A[2][2] = 1. / sqrt(3);
		b[0] = (1. + i * 0.05) * 0;
		b[1] = (1. + i * 0.05) * 1. / 3.;
		b[2] = (1. + i * 0.05) * 1. / (3 * sqrt(2));
		B = CampoMagnetico(coord_tamano, B, A, b, a, -1.);

		/* 3 */
		A[0][0] = 0;
		A[0][1] = 1;
		A[0][2] = 0;
		A[1][0] = -1. / sqrt(3);
		A[1][1] = 0;
		A[1][2] = -sqrt(2) / sqrt(3);
		A[2][0] = -sqrt(2) / sqrt(3);
		A[2][1] = 0;
		A[2][2] = 1. / sqrt(3);
		b[0] = (1. + i * 0.05) * -1. / 3.;
		b[1] = (1. + i * 0.05) * 0;
		b[2] = (1. + i * 0.05) * 1. / (3 * sqrt(2));
		B = CampoMagnetico(coord_tamano, B, A, b, a, +1);

		/* 4 */
		A[0][0] = -1;
		A[0][1] = 0;
		A[0][2] = 0;
		A[1][0] = 0;
		A[1][1] = -1. / sqrt(3);;
		A[1][2] = -sqrt(2) / sqrt(3);
		A[2][0] = 0;
		A[2][1] = -sqrt(2) / sqrt(3);
		A[2][2] = 1. / sqrt(3);
		b[0] = (1. + i * 0.05) * 0;
		b[1] = (1. + i * 0.05) * -1. / 3.;
		b[2] = (1. + i * 0.05) * 1. / (3 * sqrt(2));
		B = CampoMagnetico(coord_tamano, B, A, b, a, -1.);

		/* 5 */
		A[0][0] = 0;
		A[0][1] = 1;
		A[0][2] = 0;
		A[1][0] = 1. / sqrt(3);
		A[1][1] = 0;
		A[1][2] = sqrt(2) / sqrt(3);
		A[2][0] = sqrt(2) / sqrt(3);
		A[2][1] = 0;
		A[2][2] = -1. / sqrt(3);
		b[0] = (1. + i * 0.05) * 1. / 3.;
		b[1] = (1. + i * 0.05) * 0;
		b[2] = (1. + i * 0.05) * -1. / (3 * sqrt(2));
		B = CampoMagnetico(coord_tamano, B, A, b, a, -1.);

		/* 6 */
		A[0][0] = -1;
		A[0][1] = 0;
		A[0][2] = 0;
		A[1][0] = 0;
		A[1][1] = 1. / sqrt(3);;
		A[1][2] = sqrt(2) / sqrt(3);
		A[2][0] = 0;
		A[2][1] = sqrt(2) / sqrt(3);
		A[2][2] = -1. / sqrt(3);
		b[0] = (1. + i * 0.05) * 0;
		b[1] = (1. + i * 0.05) * 1. / 3;
		b[2] = (1. + i * 0.05) * -1. / (3 * sqrt(2));
		B = CampoMagnetico(coord_tamano, B, A, b, a, +1.);

		/* 7 */
		A[0][0] = 0;
		A[0][1] = -1;
		A[0][2] = 0;
		A[1][0] = -1. / sqrt(3);
		A[1][1] = 0;
		A[1][2] = sqrt(2) / sqrt(3);
		A[2][0] = -sqrt(2) / sqrt(3);
		A[2][1] = 0;
		A[2][2] = -1. / sqrt(3);
		b[0] = (1. + i * 0.05) * -1. / 3.;
		b[1] = (1. + i * 0.05) * 0;
		b[2] = (1. + i * 0.05) * -1. / (3 * sqrt(2));
		B = CampoMagnetico(coord_tamano, B, A, b, a, -1.);

		/* 8 */
		A[0][0] = 1;
		A[0][1] = 0;
		A[0][2] = 0;
		A[1][0] = 0;
		A[1][1] = -1. / sqrt(3);;
		A[1][2] = sqrt(2) / sqrt(3);
		A[2][0] = 0;
		A[2][1] = -sqrt(2) / sqrt(3);
		A[2][2] = -1. / sqrt(3);
		b[0] = (1. + i * 0.05) * 0;
		b[1] = (1. + i * 0.05) * -1. / 3.;
		b[2] = (1. + i * 0.05) * -1. / (3 * sqrt(2));
		B = CampoMagnetico(coord_tamano, B, A, b, a, 1.);
	}
	fp_out = fopen("salida.csv", "w");
	for (int i = 0; i < tamano; i++ )
	{
		//printf("%lf;%lf;%lf;%lf;%lf;%lf\n", coord_tamano.coordenadas[0][i], coord_tamano.coordenadas[1][i], coord_tamano.coordenadas[2][i], B[0][i], B[1][i], B[2][i]);
		fprintf(fp_out, "%lf;%lf;%lf;%lf;%lf;%lf\n", coord_tamano.coordenadas[0][i], coord_tamano.coordenadas[1][i], coord_tamano.coordenadas[2][i], B[0][i], B[1][i], B[2][i]);
	}
	//x = B[2][5];
	fclose(fp_out);
	free(B[0]);
	free(B[1]);
	free(B[2]);
	free(B);
	free(coord_tamano.coordenadas[0]);
	free(coord_tamano.coordenadas[1]);
	free(coord_tamano.coordenadas[2]);
	free(coord_tamano.coordenadas);
	printf("Terminado.\n");

	
	return 0;

}
struct puntos ReadFile(char file_name[250])
{
	char ch1;
	double* aux1;
	double* aux2;
	double * x_ini = 0, * y_ini = 0, * z_ini = 0;
	double** x;
	FILE* fp;
	fp = fopen(file_name, "r"); // read mode

	if (fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	printf("Reading the contents of %s file...\n", file_name);
	char* s_numero_leido = (char*) "";

	int num_ptocoma = 0;
	int num_linea = 0;
	int num_char = 0;
	
	
	while ((ch1 = fgetc(fp)) != EOF)
	{

		if (ch1 == ';')
		{
			
			if (num_ptocoma == 0 && num_linea > 0)
			{

				if (*s_numero_leido != '\0') {
					double numero = atof(s_numero_leido);

					x_ini = (double*)realloc(x_ini, num_linea * sizeof(double));
					//	aux = (double*)malloc(num_linea * sizeof(double));
					x_ini[num_linea - 1] = numero;
					//free(s_numero_leido);*/
				}
				else
				{

					x_ini = (double*)realloc(x_ini, num_linea * sizeof(double));
					//	aux = (double*)malloc(num_linea * sizeof(double));
					x_ini[num_linea - 1] = NULL;
				}

			}
			else if (num_ptocoma == 1 && num_linea > 0)
			{

				if (*s_numero_leido != '\0') {
					double numero = atof(s_numero_leido);

					y_ini = (double*)realloc(y_ini, num_linea * sizeof(double));
					//	aux = (double*)malloc(num_linea * sizeof(double));
					y_ini[num_linea - 1] = numero;
					//	free(s_numero_leido);*/
				}
				else
				{

					y_ini = (double*)realloc(y_ini, num_linea * sizeof(double));
					//	aux = (double*)malloc(num_linea * sizeof(double));
					y_ini[num_linea - 1] = NULL;
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


					z_ini = (double*)realloc(z_ini, num_linea * sizeof(double));
					z_ini[num_linea - 1] = numero;
					//	free(s_numero_leido);*/
				}
				else
				{

					z_ini = (double*)realloc(z_ini, num_linea * sizeof(double));
					z_ini[num_linea - 1] = NULL;
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
			char* str2;
			str2 = (char*)malloc((len + 3) * sizeof(char));
			strcpy(str2, s_numero_leido);
			str2[len] = ch1;
			str2[len + 1] = '\0';
			s_numero_leido = (char*)malloc((len + 3) * sizeof(char));
			strcpy(s_numero_leido, str2);
			free(str2);
			num_char++;
		}
		printf("%c", ch1);

	}
	printf("The contents of %s file are read.\n", file_name);
	free(s_numero_leido);
	fclose(fp);
	x = (double**)malloc(3 * sizeof(double*));
	x[0] = (double*)malloc((num_linea - 1) * sizeof(double));
	x[1] = (double*)malloc((num_linea - 1) * sizeof(double));
	x[2] = (double*)malloc((num_linea - 1) * sizeof(double));
	
	
	//B = (double*)malloc(num_linea * sizeof(double));
	for (int l = 0; l < num_linea-1; l++)
	{
			/*for (int i = 0; i < num_linea-1; i++)
			{
				printf("%lf;%lf;%lf\n", x_ini[i], y_ini[i], z_ini[i]);
			}*/
			x[0][l] = x_ini[l];
			x[1][l] = y_ini[l];
			x[2][l] = z_ini[l];
			/*printf("-----------------------------------\n");
			for (int i = 0; i < num_linea-1; i++)
			{
				printf("%lf;%lf;%lf\n",x[0][i], x[1][i], x[2][i]);
			}*/
			
	}
	free(x_ini);
	free(y_ini);
	free(z_ini);
	return {x,num_linea-1};
}
double** CampoMagnetico(struct puntos coord_tamano, double** B, double cambioBase[3][3], double translacion[3], double a, double signo)
{
   int largo = coord_tamano.tamano;
 
   double **x;
   double **x_p;
   //double a = 1./(2. * sqrt(3));
   double rho;
   double m;
   x = (double**)malloc(3 * sizeof(double*));
   x[0] = (double*)malloc(largo * sizeof(double));
   x[1] = (double*)malloc(largo * sizeof(double));
   x[2] = (double*)malloc(largo * sizeof(double));
  /* B = (double**)malloc(3 * sizeof(double*));
   B[0] = (double*)malloc(largo * sizeof(double));
   B[1] = (double*)malloc(largo * sizeof(double));
   B[2] = (double*)malloc(largo * sizeof(double));*/
   x = coord_tamano.coordenadas;
  
   x_p = (double**)malloc(3 * sizeof(double*));
   x_p[0] = (double*)malloc(largo * sizeof(double));
   x_p[1] = (double*)malloc(largo * sizeof(double));
   x_p[2] = (double*)malloc(largo * sizeof(double));
   for(int l =0; l < coord_tamano.tamano;l++){
      rho = 0;
	  
	  for(int i=0; i<3;i++)
	  {
		  x_p[i][l] = 0;
		  for (int j = 0; j < 3; j++)
		  {
			  x_p[i][l] += cambioBase[i][j] * (x[j][l] - translacion[j]);
		  }
	  }
      /*x_p[0][l] = x[1][l];
      x_p[1][l] = (x[0][l]-1.0/3.0)/sqrt(3.0) - (sqrt(2.0)/sqrt(3.0))*(x[2][l] - 1.0/(3.0*sqrt(2.0)));
      x_p[2][l] = (x[0][l]-1.0/3.0)*sqrt(2.0)/3.0 + (1.0/sqrt(3.0))*(x[2][l] - 1.0/(3.0*sqrt(2.0)));*/
      rho = sqrt(pow(x_p[0][l],2) + pow(x_p[1][l],2));
	  m = (4.0 * a * rho) / (pow(a + rho, 2) + pow(x_p[2][l], 2));
      B[0][l] += signo * a * x_p[2][l] / pow(2.0 * a * rho,3./2.)*(-sqrt(2.0 * m) * comp_ellint_1(m, 0.00000000001) + (2.0 - m)/(2.0 - 2.0 * m) * sqrt(2.0 * m) * comp_ellint_2(m, 0.00000000001)) * x_p[0][l]/rho;
      B[1][l] += signo * a * x_p[2][l] / pow(2.0 * a * rho,3./2.)*(-sqrt(2.0 * m) * comp_ellint_1(m, 0.00000000001) + (2.0 - m)/(2.0 - 2.0 * m) * sqrt(2.0 * m) * comp_ellint_2(m, 0.00000000001)) * x_p[1][l]/rho;
      B[2][l] += signo * a / pow(2.0 * a * rho,3./2.)*(a*m/(2.0+2.0* m)*sqrt(2.0 *m) * comp_ellint_2(m, 0.00000000001) + rho* sqrt(2.0 * m) * comp_ellint_1(m, 0.00000000001) - rho*(2.0 - m)/(2.0 - 2.0 * m) * sqrt(2.0 * m) * comp_ellint_2(m, 0.00000000001));
    
	/* B[0][l] = l;
	 B[1][l] = l;
	 B[2][l] = l;*/
   }
   free(x_p[0]);
   free(x_p[1]);
   free(x_p[2]);
   free(x_p);   
   return B;
}
int factorial(int n)
{
	int ret = 1;
	while (n > 1)
		ret *= n--;
	return ret;
}
double comp_ellint_1(double m, double error)
{
	double integral = 0;
	double incremento = 0;
	int n = 0;
	
	do{
		incremento = PI / 2 *pow(((double)factorial(2 * n)) / (pow(2,2 * n)* pow((double)factorial(n), 2)), 2)* pow(m,2 * n);
		integral = integral + incremento;
		n++;
	} while (incremento > error);
	return integral;
}
double comp_ellint_2(double m, double error)
{
	double integral = 0;
	double incremento = 0;
	int n = 0;
	
	do{
		incremento = PI / 2 * pow(((double)factorial(2 * n)) / (pow(2, 2 * n) * pow((double)factorial(n), 2)), 2) * pow(m, 2 * n) / (1. - (2. * n));
		integral = integral + incremento;
		n++;
	} while (incremento > error);
	return integral;
}
int gauss(double A[size][size + 1], double x[size])
{
	int N = size;
	for (int i = 0; i < N; i++) {
		// Search for maximum in this column
		double maxEl = fabs(A[i][i]);
		int maxRow = i;
		for (int k = i + 1; k < N; k++) {
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
