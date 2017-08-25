#ifndef _SISTEMA_DE_ECUACIONES_H_
#define _SISTEMA_DE_ECUACIONES_H_

#include "matriz.h"
#include "matrizFactory.h"

#include <assert.h>
#include <vector>

using namespace std;

class SistemaDeEcuaciones {
public:
	template <class T>
	struct resultadosDeEliminacionGausseana {
		Matriz<T> matrizTriangulada;
		Matriz<T> L;
		Matriz<T> U;
		MatrizInt vectorDeParticion;
	};

	SistemaDeEcuaciones() {}
	~SistemaDeEcuaciones() {}
	//TODO: Como es si la matriz no es cuadrada
	//TODO: Esta bien que alto sea mayor o igual a ancho
	//M debe ser triangularSuperior
	template<class T> Matriz<T> resolverAPartirDeTriangularSuperior(const Matriz<T> &m, const Matriz<T> &b) const {
		assert(b.ancho() == 1 && m.alto() == b.alto() && m.alto() >= m.ancho());
		int alto = m.alto();
		int ancho = m.ancho();
		Matriz<T> r = Matriz<T>(ancho, 1, T());

		for (int i = ancho - 1; i >= 0; i--) {
			//TODO: Que se hace si m[i][i] = 0
			r[i][0] = resolverIncognitaIUsandoSiguientesDeResueltas<T>(i, m[i], r, b[i][0]);
		}

		return r;
	}

	//TODO: Como es si la matriz no es cuadrada
	//TODO: Esta bien que alto sea mayor o igual a ancho
	//M debe ser triangularInferior
	template<class T> Matriz<T> resolverAPartirDeTriangularInferior(const Matriz<T> &m, const Matriz<T> &b) const {
		assert(b.ancho() == 1 && m.alto() == b.alto() && m.alto() >= m.ancho());
		int alto = m.alto();
		int ancho = m.ancho();
		Matriz<T> r = Matriz<T>(ancho, 1, T());

		for (int i = 0; i < ancho; i++) {
			//TODO: Que se hace si m[i][i] = 0
			r[i][0] = resolverIncognitaIUsandoAnterioresDeResueltas<T>(i, m[i], r, b[i][0]);
		}

		return r;
	}

private:
	//TODO: Que se hace si fila[i] = 0
	template <class T>
	T resolverIncognitaIUsandoSiguientesDeResueltas(int i, const vector<T> &fila, const Matriz<T> &resueltas, const T &b) const {
		int n = fila.size();
		T sumatoria = b;
		for (int j = i+1; j < n; j++) {
			sumatoria -= resueltas[j][0]*fila[j];
		}

		return sumatoria / (double)fila[i];
	}

	//TODO: Que se hace si fila[i] = 0
	template <class T>
	T resolverIncognitaIUsandoAnterioresDeResueltas(int i, const vector<T> &fila, const Matriz<T> &resueltas, const T &b) const {
		int n = fila.size();
		T sumatoria = b;
		for (int j = 0; j < i; j++) {
			sumatoria -= resueltas[j][0] * fila[j];
		}

		return sumatoria / (double)fila[i];
	}
};

#endif // !_SISTEMA_DE_ECUACIONES_H_
