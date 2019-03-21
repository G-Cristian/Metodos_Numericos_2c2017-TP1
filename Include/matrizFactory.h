#ifndef _MATRIZFACTORY_H_
#define _MATRIZFACTORY_H_

#include "Matrix.h"
#include <vector>
#include <assert.h>

using namespace std;
namespace MN {
	class MatrizFactory {
	public:
		template <class T> static Matrix<T> identidad(int n) {
			Matrix<T> r = Matrix<T>(n, n, T());

			for (int i = 0; i < n; i++) {
				r.setValueInRowColumn(1, i, i);
			}

			return r;
		}

		//vector de particion P = (0,1,2,3,..,n-1)t
		//es decir dada la Matrix de particion de n*n M[i][P[i]] = 1 y el resto 0 
		static Matrix<int> vectorDeParticion(int n) {
			assert(0 < n);
			vector<int> v = vector<int>(n);
			for (int i = 0; i < n; i++) {
				v[i] = i;
			}

			return MatrizFactory::vectorDeParticion(v);
		}

		//vector de particion P = (v[0],v[1],..,v[n-1])t
		//es decir dada la Matrix de particion de n*n M[i][P[i]] = 1 y el resto 0
		static Matrix<int> vectorDeParticion(const vector<int> &valores) {
			int n = valores.size();
			Matrix<int> p(n, 1);
			for (int i = 0; i < n; i++) {
				p.setValueInRowColumn(valores[i], i,0);
			}

			return p;
		}

		//vector de particion P = (0,1,2,3,..,n-1)t
		//es decir dada la Matrix de particion de n*n M[i][P[i]] = 1 y el resto 0 
		static Matrix<int> matrizDeParticion(int n) {
			return MatrizFactory::matrizDeParticion(MatrizFactory::vectorDeParticion(n));
		}

		//vector de particion P = (p_0,p_1,..,p_n-1)t
		//es decir dada la Matrix de particion de n*n M[i][p_i] = 1 y el resto 0
		static Matrix<int> matrizDeParticion(const Matrix<int> &vectorDeParticion) {
			assert(vectorDeParticion.width() == 1);
			int n = vectorDeParticion.height();
			Matrix<int> p = Matrix<int>(n, n, 0);
			for (int i = 0; i < n; i++) {
				p.setValueInRowColumn(1, i,vectorDeParticion[i][0]);
			}

			return p;
		}
	};
}
#endif // !_MATRIZFACTORY_H_
