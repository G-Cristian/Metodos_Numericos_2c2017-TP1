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
	class ResultadosDeEliminacionGausseana {
	public:
		ResultadosDeEliminacionGausseana(const Matriz<T> &matriz, const Matriz<T> &resultados, const Matriz<T> &L, const Matriz<T> &U, const MatrizInt &particion)
			:matrizLuegoDeEliminacionGausseana(matriz), resultadosLuegoDeEliminacionGausseana(resultados),L(L),U(U), matrizDeParticion(particion)
		{

		}

		Matriz<T> matrizLuegoDeEliminacionGausseana;
		Matriz<T> resultadosLuegoDeEliminacionGausseana;
		Matriz<T> L;
		Matriz<T> U;
		MatrizInt matrizDeParticion;
	};

	SistemaDeEcuaciones() {}
	~SistemaDeEcuaciones() {}

	template<class T>
	ResultadosDeEliminacionGausseana<T> eliminacionGausseana(const Matriz<T> &matriz, const Matriz<T> &resultados) {
		assert(matriz.alto() == matriz.ancho() && resultados.alto() == matriz.alto() && resultados.ancho() == 1);
		int n = matriz.ancho();
		Matriz<T> matrizFinal = matriz;
		Matriz<T> resultadosFinal = resultados;
		Matriz<T> L = MatrizFactory::identidad<T>(n);
		MatrizInt vectorDeParticion = MatrizFactory::vectorDeParticion(n);
		
		for (int i = 0; i < n - 1; i++) {
			int indiceMayor = indiceDeFilaConMayorValorEnColumnaCDesdeFilaF<T>(matrizFinal, i, i, vectorDeParticion);
			intercambiar(vectorDeParticion, i, indiceMayor);
			int indiceFilaI = vectorDeParticion[i][0];
			T aii = matrizFinal[indiceFilaI][i];
			//TODO: Si es igual a 0 va a tener infinitas soluciones creo. Que onda con eso?
			if (aii != 0) {
				for (int j = i + 1; j < n; j++) {
					//Yo no estoy trabajando con la fila i y fila j necesariamente, ya que pude haber intercambiado esas filas.
					//Entonces aprovechando que la matriz es cuadrada, sea P=(p_0, ..., p_n-1)t el vector de particion.
					//Si en p_j no hice intercambio, p_j = columna j = fila j.
					//Si en p_j hice intercambio, p_j me dice cual es la fila que tendria que estar en la fila j.
					//Ej. P=(0,1,2,3)
					//Intercambio 0 con 1
					//P=(1,0,2,3)
					//Si j = 1 quiere decir que tengo que trabajar con la fila 1. Pero en esta fila tendría que estar la fila 0 original.

					int indiceFilaJ = vectorDeParticion[j][0];

					T aji = matrizFinal[indiceFilaJ][i];
					T mji = aji / aii;

					

					matrizFinal.agreagrleAFilaIOtraFila(indiceFilaJ, -mji * matrizFinal.filaComoMatriz(indiceFilaI));
					resultadosFinal[indiceFilaJ][0] -= mji * resultadosFinal[indiceFilaI][0];
					L[j][i] = -mji;
				}
			}
		}
		
		MatrizInt matrizDeParticion = MatrizFactory::matrizDeParticion(vectorDeParticion);
		matrizFinal = (Matriz<T>)matrizDeParticion*matrizFinal;
		Matriz<T> U = obtenerUAPartirDeM<T>(matrizFinal);
		resultadosFinal = (Matriz<T>)(matrizDeParticion)*resultadosFinal;

		return	ResultadosDeEliminacionGausseana<T>(matrizFinal, resultadosFinal, L, U, matrizDeParticion);
	}

	//LUx = Pb
	template<class T> Matriz<T> resolverAPartirDePLU(const Matriz<T> &Pb, const Matriz<T> &L, const Matriz<T> &U) const {
		//PA = LU
		//Ax = b
		//PAx = Pb
		//LUx = Pb
		//Ux = y
		//Ly = Pb

		Matriz<T> y = resolverAPartirDeTriangularInferior(L, Pb);

		return resolverAPartirDeTriangularSuperior(U, y);
	}

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
	template<class T>
	int indiceDeFilaConMayorValorEnColumnaCDesdeFilaF(const Matriz<T> &m, int c, int f, const MatrizInt &vectorDeParticion) const {
		assert(0 <= c && c < m.ancho() && 0 <= f && f < m.alto() && vectorDeParticion.alto() == m.alto() && vectorDeParticion.ancho() == 1);
		
		//Yo no estoy trabajando con la fila i y fila j necesariamente, ya que pude haber intercambiado esas filas.
		//Entonces aprovechando que la matriz es cuadrada, sea P=(p_0, ..., p_n-1)t el vector de particion.
		//Si en p_j no hice intercambio, p_j = columna j = fila j.
		//Si en p_j hice intercambio, p_j me dice cual es la fila que tendria que estar en la fila j.
		//Ej. P=(0,1,2,3)
		//Intercambio 0 con 1
		//P=(1,0,2,3)
		//Si j = 1 quiere decir que tengo que trabajar con la fila 1. Pero en esta fila tendría que estar la fila 0 original.

		int n = m.alto();
		int indiceReal = vectorDeParticion[f][0];
		int indiceDeMaximo = f;
		T maximo = m[indiceReal][c];

		for (int i = f+1; i < n; i++) {
			indiceReal = vectorDeParticion[i][0];
			if (m[indiceReal][c] > maximo) {
				indiceDeMaximo = i;
				maximo = m[indiceReal][c];
			}
		}

		return indiceDeMaximo;
	}

	void intercambiar(MatrizInt &vectorDeParticion, int i, int j) const {
		assert(0 <= i && i < vectorDeParticion.alto() && 0 <= j && j < vectorDeParticion.alto() && vectorDeParticion.ancho() == 1);
		int aux = vectorDeParticion[i][0];
		vectorDeParticion[i][0] = vectorDeParticion[j][0];
		vectorDeParticion[j][0] = aux;
	}

	//m debe ser cuadrada.
	template<class T> Matriz<T> obtenerUAPartirDeM(const Matriz<T> &m) const {
		assert(m.ancho() == m.alto());

		int n = m.alto();
		Matriz<T> U = Matriz<T>(n, n, 0);

		for (int f = 0; f < n; f++) {
			for (int c = f; c < n; c++) {
				U[f][c] = m[f][c];
			}
		}

		return U;

	}

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
