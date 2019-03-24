#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX 100

int ZeraVetor(int n, double *vet) { //Zera todas posiçoes do vetor inserido;
    for (int i = 0; i < n; i++) {
        vet[i] = 0;
    }
    return 0;
}

int ZerarTudo(double *coeficiente, double *aux, double *Y, double *X) { //Zera todos vetores do codigo
    ZeraVetor(MAX, X);
    ZeraVetor(MAX, Y);
    ZeraVetor(MAX, aux);
    ZeraVetor(MAX, coeficiente);
    return 0;
}

//Integral Definida:
double integralDefinida(double *coeficiente, double inicial, double final, int n){
	int i;
	double valor_inicial, valor_final;
	valor_inicial = 0;
	valor_final = 0;
	
	valor_inicial = coeficiente[0];
	valor_final = coeficiente[0];
	
	for(i = 1; i<n; i++){
		valor_inicial += coeficiente[i] * pow(inicial, i);
		valor_final += coeficiente[i] * pow(final, i);
	}
	
	printf("Integral = %lf", valor_final - valor_inicial);
	
	return valor_final - valor_inicial;
}

//Interpolação Newton:
double FuncD(int k, int n, double *aux, double *X, double *Y) { //Retorna o valor de D0, D1 ... assim por diante, dependo do k inserido.
    int i, cont;

    cont = 0;

    for (i = 0; i < n; i++) {
        aux[i] = Y[i];
    }
    while (cont < k) {
        for (i = 0; i < n - cont - 1; i++) {
            aux[i] = ((aux[i + 1] - aux[i]) / (X[i + 1 + cont] - X[i]));
        }
        cont++;
    }
    return aux[0];
}

int Newton(int n, double *coeficiente, double *aux, double *X, double *Y) {
    int i, j, cont;
    double d;

    cont = n - 1;

    while (cont > 0) {
        d = FuncD(cont, n, aux, X, Y);
        ZeraVetor(n, aux);
        aux[0] = 1;
        for (j = 0; j < cont + 1; j++) {
            for (i = j; i > 0; i--) {
                aux[i] += aux[i - 1];
                aux[i - 1] *= -(X[j - 1]);
            }
        }
        for (i = 0; i < n; i++) {
            coeficiente[i] += (aux[i] * d);
        }
        cont--;
    }
    coeficiente[0] += Y[0];

    return 0;
}

int main(int argc, char **argv)
{
	int i;
	double X[MAX], Y[MAX], coeficiente[MAX], aux[MAX];
	
	ZerarTudo(X, Y, aux, coeficiente);
	
	for(i=0; i<MAX; i++){
		X[i] = i;
		Y[i] = (rand() % 11) + 45;
		//printf("%.0lf ", Y[i]);
	}
	
	Newton(MAX, coeficiente, aux, X, Y);
	integralDefinida(coeficiente, 0, 5, MAX);
	
	return 0;
}

