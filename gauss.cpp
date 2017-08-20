#include <iostream>
//extern int n; //matriz de R^nxn
//floats
/*
  M[x][y] = a_{j,i} (de la matriz)
  +----->x
  |+---->j
  ||
  ||
  VV
  yi
*/
unsigned int n;

void imprimir_m(void * M, unsigned int n){
	double (*p_M)[n][n] = (double (*)[n][n]) M;
	for (unsigned int i = 0; i < n; i++){
		for (unsigned int j = 0; j < n; j++){
			std::cout << (*p_M)[i][j] << ", ";
		}
		std::cout << std::endl;
	}
	return;
}


void restar_fila(void* M, unsigned int j, double m, unsigned int i){
	double (*p_M)[n][n] = (double (*)[n][n]) M;
	//F_j <- F_j - m * F_i

	for (unsigned int k = 0; k < n; k++){
		(*p_M)[j][k] = (*p_M)[j][k] - m * (*p_M)[i][k];
	}

}

unsigned int buscar_nocero(void * M, unsigned int i, unsigned int j){
	double (*p_M)[n][n] = (double (*)[n][n]) M;

	for (unsigned int k = j; k < n; k++){
		if ((*p_M)[k][i] != 0) return k;
	}

	return i;
}

void permutar_f(void * M, unsigned int g, unsigned int i){
	//permuta filas g e i
	//(M[g] con M[i])
	double (*p_M)[n][n] = (double (*)[n][n]) M;
	for (unsigned int k = 0; k < n; k++){
		double temp = (*p_M)[i][k];
		(*p_M)[i][k] = (*p_M)[g][k];
		(*p_M)[g][k] = temp;
	}
	return;
}

void gauss(void* M){
	//tratar a M como matriz:
	double (*p_M)[n][n] = (double (*)[n][n]) M;

	for (unsigned int i = 0; i < n-1; i++){
		for(unsigned int j = i+1; j < n; j++){
			//buscar_nocero debería devolver g = i si M[i]
			//es fila que empieza en no cero (M[i][j] es no cero)
			unsigned int g = buscar_nocero(M, i, j);
			if ((*p_M)[g][j] != 0){
				permutar_f(M, g, i);
				double m = (((*p_M)[j][i]) / ((*p_M)[i][i]));
				restar_fila(M, j, m, i);
			}
		}
	
	}
	return;
}

int main(){
//	unsigned int n = 3;
	n=3;
	double M[n][n];
	// M = { {5.0, 4.0, 3.0},
	//       {5.0, 4.0, 3.0},
	//       {0.0, 0.0, 1.0}};
	for (int i = 0; i < 2; i++){
		M[i][0] = 5.0;
		M[i][1] = 4.0;
		M[i][2] = 3.0;
	}
	for (int i = 0; i < 3; i++)	M[2][i] = 0.0;
	M[2][2] = 1.0;
	imprimir_m(M,n);
	gauss(M);
	imprimir_m(M, n);

	return 0;
}
