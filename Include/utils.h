#include <iostream>
#include <iomanip>
#include <vector>
#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

using namespace std;

typedef vector<vector<double> > Matriz;

const double eps = 1e-5;

/// Utils

bool eq(double a, double b) { return abs(a-b)<eps; }

int filas(const Matriz& A) { 
  if (A.size()==0) throw runtime_error("No es una matriz valida.");
  return A.size(); 
}

int columnas(const Matriz& A) {
  int N = filas(A);
  for(int i=0; i<N-1; i++) {
    if (A[i].size() != A[i+1].size()) throw runtime_error("No es una matriz valida.");
  }
  return A[0].size();
}

std::ostream& operator<<(std::ostream& os, const Matriz& A) {
  for(int i=0; i<(int)A.size(); i++) {
    for(int j=0; j<(int)A[i].size(); j++) {
      os << setprecision(4) << A[i][j] << "\t";
    }
    os << "\n";
  }
  return os;
}

bool esCuadrada(const Matriz& A) {
  int N = filas(A);
  int M = columnas(A);
  if (N != M) return false;
  return true;
}

bool igualDimensiones(const Matriz& A, const Matriz& B) {
  int N_a = filas(A), N_b = filas(B);
  int M_a = columnas(A), M_b = columnas(B);
  if ((N_a != N_b) || (M_a != M_b)) return false;
  return true;
}

bool operator==(const Matriz& A, const Matriz& B) {
  if (!igualDimensiones(A, B)) return false;
  int N = filas(A);
  int M = columnas(A);
  for(int i=0; i<N; i++) for(int j=0; j<M; j++) if(!eq(A[i][j], B[i][j])) return false;
  return true;
}

double random01() {
  return ((double)rand() / RAND_MAX);
}

Matriz randomMatriz(int N) {
  Matriz R = Matriz(N, vector<double>(N, 0));
  for(int i=0; i<N; i++)
  for(int j=0; j<N; j++)
    R[i][j] = random01();
  return R;
}
