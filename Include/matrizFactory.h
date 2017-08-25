#ifndef _MATRIZFACTORY_H_
#define _MATRIZFACTORY_H_

#include "matriz.h"
#include <vector>
#include <assert.h>

using namespace std;

class MatrizFactory {
public:
	template <class T> static Matriz<T> identidad(int n) {
		Matriz<T> r = Matriz<T>(n, n, T());

		for (int i = 0; i < n; i++) {
			r[i][i] = 1;
		}

		return r;
	}

	//vector de particion P = (0,1,2,3,..,n-1)t
	//es decir dada la matriz de particion de n*n M[i][P[i]] = 1 y el resto 0 
	static Matriz<int> vectorDeParticion(int n) {
		assert(0 < n);
		vector<int> v = vector<int>(n);
		for (int i = 0; i < n; i++) {
			v[i] = i;
		}

		return MatrizFactory::vectorDeParticion(v);
	}

	//vector de particion P = (v[0],v[1],..,v[n-1])t
	//es decir dada la matriz de particion de n*n M[i][P[i]] = 1 y el resto 0
	static MatrizInt vectorDeParticion(const vector<int> &valores) {
		int n = valores.size();
		MatrizInt p = MatrizInt(n, 1, 0);
		for (int i = 0; i < n; i++) {
			p[i][0] = valores[i];
		}

		return p;
	}

	//vector de particion P = (0,1,2,3,..,n-1)t
	//es decir dada la matriz de particion de n*n M[i][P[i]] = 1 y el resto 0 
	static MatrizInt matrizDeParticion(int n) {
		return MatrizFactory::matrizDeParticion(MatrizFactory::vectorDeParticion(n));
	}

	//vector de particion P = (p_0,p_1,..,p_n-1)t
	//es decir dada la matriz de particion de n*n M[i][p_i] = 1 y el resto 0
	static MatrizInt matrizDeParticion(const MatrizInt &vectorDeParticion) {
		assert(vectorDeParticion.ancho() == 1);
		int n = vectorDeParticion.alto();
		MatrizInt p = MatrizInt(n, n, 0);
		for (int i = 0; i < n; i++) {
			p[i][vectorDeParticion[i][0]] = 1;
		}

		return p;
	}
};

#endif // !_MATRIZFACTORY_H_
