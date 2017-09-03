#include "stdafx.h"
#include "../Include/utils.h"

/// Ejercicios

/// 1) esSimetrica
// Completar la función esSimetrica.
// Recibe una matriz A y devuelve un booleano. 
// Devuelve true si y solo si la matriz es simétrica. 
// Tratar de minimizar la cantidad de comparaciones. 
bool esSimetrica(const Matriz& A) {
  if (!esCuadrada(A)) throw runtime_error("Matriz no SDP."); //solo me importan estas
  int N = filas(A);
  for(int i=0; i<N; i++){ 
	for(int j=0; j<i; j++){ //for(int j=i+1; j<M; j++) 
		if(!eq(A[i][j], A[j][i]))
		return false;
	}
  }
  return true;
}

/// 2) Cholesky
// Completar la función que calcula la factorización de cholesky. 
// Recibe una matriz A y debe devolver una nueva matriz con la L de cholesky.
// Debe chequear que la matriz sea cuadrada, simétrica, y luego 
// durante el algoritmo agregar los chequeos necesarios donde haga falta. 
// En cualquier caso que falle, debe lanzar una excepción con el mensaje 
// "Matriz no SDP." 
Matriz cholesky(const Matriz &A) {
	if (!esCuadrada(A)) throw runtime_error("Matriz no SDP.");
	if (!esSimetrica(A)) throw runtime_error("Matriz no SDP.");
	int N = filas(A);
	Matriz L = Matriz(N, vector<double>(N, 0));
	for(int j=0; j<N; j++) {
		double sumatoria= 0;
		for(int k=0; k<j; k++) sumatoria += (L[j][k]*L[j][k]);
		double arg = A[j][j]- sumatoria;
		if (arg<=0) throw runtime_error("Matriz no SDP.");
		L[j][j]= sqrt(arg);
		
		for(int i=j+1; i<N; i++){ 
			double sumatoria2= 0;
			for(int k=0; k<j; k++) sumatoria2 += (L[i][k]* L[j][k]);
			L[i][j]= (A[i][j]- sumatoria2)/L[j][j];
		}
	}
  return L;
}

/// 3) Multiplica A por A'
// Completar la función multiplicarPorTraspuesta. 
// Toma una matriz A y devuelve una nueva matriz con 
// el resultad de hacer A * A'. 
// Tratar de minimizar la cantidad de operaciones. 
// Considerar que (A * A')' = A'' * A' = A * A'
Matriz multiplicarPorTraspuesta(const Matriz& A) {
	Matriz R = Matriz(filas(A), vector<double>(filas(A), 0));
	int N = filas(A);
	int M = columnas(A);	
	for( int i=0; i<N; i++){
		for( int j=0; j<N; j++){
			for(int k=0; k<M; k++) R[i][j] += A[i][k]*A[j][k];
		}
	}
	return R; 
}

/// Tests
void test1() {
  Matriz A1_test = {
    {5.99246,   0.45174,   0.50921,   0.45632,   0.41578},
    {0.45174,   5.02065,   0.40611,   0.52172,   0.11483},
    {0.50921,   0.40611,   5.25767,   0.78430,   0.24938},
    {0.45632,   0.52172,   0.78430,   5.98219,   0.45188},
    {0.41578,   0.11483,   0.24938,   0.45188,   5.28701}
  };

  Matriz L1_test = {
    {2.44795,   0.00000,   0.00000,   0.00000,   0.00000},
    {0.18454,   2.23307,   0.00000,   0.00000,   0.00000},
    {0.20802,   0.16467,   2.27756,   0.00000,   0.00000},
    {0.18641,   0.21823,   0.31156,   2.40889,   0.00000},
    {0.16985,   0.03739,   0.09128,   0.15925,   2.28540}
  };
  Matriz L1 = cholesky(A1_test);
  assert(L1==L1_test);
  Matriz A1 = multiplicarPorTraspuesta(L1);
  assert(A1==A1_test);
}

void test2() {
  Matriz A2_test = {
    {7.21217,   0.25028,   0.42166,   0.22054,   0.53918,   0.38809,   0.21220},
    {0.25028,   7.99049,   0.21212,   0.46048,   0.47818,   0.85571,   0.41787},
    {0.42166,   0.21212,   7.43058,   0.40467,   0.52578,   0.63224,   0.39279},
    {0.22054,   0.46048,   0.40467,   7.82612,   0.38070,   0.74682,   0.43779},
    {0.53918,   0.47818,   0.52578,   0.38070,   7.82015,   0.72274,   0.35334},
    {0.38809,   0.85571,   0.63224,   0.74682,   0.72274,   7.23679,   0.70013},
    {0.21220,   0.41787,   0.39279,   0.43779,   0.35334,   0.70013,   7.05207}
  };

  Matriz L2_test = {
    {2.68555,   0.00000,   0.00000,   0.00000,   0.00000,   0.00000,   0.00000},
    {0.09320,   2.82521,   0.00000,   0.00000,   0.00000,   0.00000,   0.00000},
    {0.15701,   0.06990,   2.72049,   0.00000,   0.00000,   0.00000,   0.00000},
    {0.08212,   0.16028,   0.13989,   2.78821,   0.00000,   0.00000,   0.00000},
    {0.20077,   0.16263,   0.17750,   0.11237,   2.77656,   0.00000,   0.00000},
    {0.14451,   0.29812,   0.21640,   0.23560,   0.20902,   2.64216,   0.00000},
    {0.07901,   0.14530,   0.13609,   0.13951,   0.09869,   0.21288,   2.63281}
  };
  Matriz L2 = cholesky(A2_test);
  assert(L2==L2_test);
  Matriz A2 = multiplicarPorTraspuesta(L2);
  assert(A2==A2_test);
}

void test3() {
  // Se espera recibir una excepción porque la matriz no es simétrica. 
  Matriz A3_test = {
    {0.066076,   0.181880},
    {1.181880,   0.624953}
  };
  try {
    Matriz L3 = cholesky(A3_test);
    assert(false);
  } catch (exception& e) {
    assert(strcmp(e.what(),"Matriz no SDP.")==0);
    cout << "La matriz 3 no es SDP.\n";
  }
}

void test4() {
  // Se espera recibir una excepción porque la matriz no es definida positiva. 
  Matriz A4_test = {
    {-0.066076,  0.181880},
    {0.181880,   0.624953}
  };
  try {
    Matriz L4 = cholesky(A4_test);
    assert(false);
  } catch (exception& e) {
    assert(strcmp(e.what(),"Matriz no SDP.")==0);
    cout << "La matriz 4 no es SDP.\n";
  }
}

void testPerformance() {
  Matriz A = randomMatriz(800);
  clock_t start = clock();
  Matriz T = multiplicarPorTraspuesta(A);
  clock_t end = clock();
  double segs = (double)(end-start) / CLOCKS_PER_SEC;
  cout << "Pasaron " << segs << " segundos.\n";
}
/*
int main() {
  srand(time(NULL));
  
  test1();
  test2();
  test3();
  test4();
    
  cout << "Pasan todos los tests :)\n";
  
  //testPerformance();
  return 0;
}
*/