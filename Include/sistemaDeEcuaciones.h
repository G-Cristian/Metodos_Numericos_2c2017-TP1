#ifndef _SISTEMA_DE_ECUACIONES_H_
#define _SISTEMA_DE_ECUACIONES_H_

#include "Matrix.h"
#include "MatrizEsparsa.h"
#include "matrizFactory.h"
#include "Semaphore.h"

#include <assert.h>
#include <memory>
#include <thread>
#include <vector>

using namespace std;

class SistemaDeEcuaciones {
public:
	template <class T>
	class ResultadosDeEliminacionGausseana {
	public:
		ResultadosDeEliminacionGausseana(const MN::Matrix<T> &matrix, const MN::Matrix<T> &resultados, const MN::Matrix<T> &L, const MN::Matrix<T> &U, const MN::Matrix<T> &particion)
			:matrizLuegoDeEliminacionGausseana(matrix), resultadosLuegoDeEliminacionGausseana(resultados),L(L),U(U), matrizDeParticion(particion)
		{

		}

		MN::Matrix<T> matrizLuegoDeEliminacionGausseana;
		MN::Matrix<T> resultadosLuegoDeEliminacionGausseana;
		MN::Matrix<T> L;
		MN::Matrix<T> U;
		MN::Matrix<T> matrizDeParticion;
	};

	SistemaDeEcuaciones() {}
	~SistemaDeEcuaciones() {}

	template<class T>
	ResultadosDeEliminacionGausseana<T> eliminacionGausseana(const MN::Matrix<T> &matrix, const MN::Matrix<T> &resultados) {
		assert(matrix.height() == matrix.width() && resultados.height() == matrix.height() && resultados.width() == 1);
		int n = matrix.width();
		MN::Matrix<T> matrizFinal = matrix;
		MN::Matrix<T> resultadosFinal = resultados;
		MN::Matrix<T> L = MN::MatrizFactory::identidad<T>(n);
		MN::Matrix<int> vectorDeParticion = MN::MatrizFactory::vectorDeParticion(n);
		
		for (int i = 0; i < n - 1; i++) {
			int indiceMayor = indiceDeFilaConMayorValorEnColumnaCDesdeFilaF<T>(matrizFinal, i, i, vectorDeParticion);
			intercambiar(vectorDeParticion, i, indiceMayor);
			int indiceFilaI = vectorDeParticion[i][0];
			T aii = matrizFinal[indiceFilaI][i];
			//TODO: Si es igual a 0 va a tener infinitas soluciones creo. Que onda con eso?
			if (aii != 0) {
				for (int j = i + 1; j < n; j++) {
					//Yo no estoy trabajando con la fila i y fila j necesariamente, ya que pude haber intercambiado esas filas.
					//Entonces aprovechando que la matrix es cuadrada, sea P=(p_0, ..., p_n-1)t el vector de particion.
					//Si en p_j no hice intercambio, p_j = columna j = fila j.
					//Si en p_j hice intercambio, p_j me dice cual es la fila que tendria que estar en la fila j.
					//Ej. P=(0,1,2,3)
					//Intercambio 0 con 1
					//P=(1,0,2,3)
					//Si j = 1 quiere decir que tengo que trabajar con la fila 1. Pero en esta fila tendría que estar la fila 0 original.

					int indiceFilaJ = vectorDeParticion[j][0];

					T aji = matrizFinal[indiceFilaJ][i];
					T mji = aji / aii;

					matrizFinal.getRow(indiceFilaJ) += (-mji*matrizFinal.getRow(indiceFilaI));
					auto aux = resultadosFinal[indiceFilaJ][0];
					resultadosFinal.setValueInRowColumn(aux - mji * resultadosFinal[indiceFilaI][0], indiceFilaJ, 0);
					L.setValueInRowColumn(-mji, j, i);
				}
			}
		}
		
		MN::Matrix<T> matrizDeParticion = MN::MatrizFactory::matrizDeParticion(vectorDeParticion).operator MN::Matrix<T>();
		matrizFinal = *(matrizDeParticion*matrizFinal);
		MN::Matrix<T> U = obtenerUAPartirDeM<T>(matrizFinal);
		resultadosFinal = *(matrizDeParticion*resultadosFinal);

		return	ResultadosDeEliminacionGausseana<T>(matrizFinal, resultadosFinal, L, U, matrizDeParticion);
	}

	//LUx = Pb
	template<class T> MN::Matrix<T> resolverAPartirDePLU(const MN::AbstractMatrix<T> &b, const MN::AbstractMatrix<T> &P, const MN::AbstractMatrix<T> &L, const MN::AbstractMatrix<T> &U) const {
		//PA = LU
		//Ax = b
		//PAx = Pb
		//LUx = Pb
		//Ux = y
		//Ly = Pb

		unique_ptr<MN::AbstractMatrix<T> > Pb = P * b;

		MN::Matrix<T> y = resolverAPartirDeTriangularInferior(L, *Pb);

		return resolverAPartirDeTriangularSuperior(U, y);
	}

	//TODO: Como es si la matrix no es cuadrada
	//TODO: Esta bien que alto sea mayor o igual a ancho
	//M debe ser triangularSuperior
	template<class T> MN::Matrix<T> resolverAPartirDeTriangularSuperior(const MN::AbstractMatrix<T> &m, const MN::AbstractMatrix<T> &b) const {
		assert(b.width() == 1 && m.height() == b.height() && m.height() >= m.width());
		int alto = m.height();
		int ancho = m.width();
		MN::Matrix<T> r = MN::Matrix<T>(ancho, 1, T());

		for (int i = ancho - 1; i >= 0; i--) {
			//TODO: Que se hace si m[i][i] = 0
			r.setValueInRowColumn(resolverIncognitaIUsandoSiguientesDeResueltas<T>(i, m.getRow(i), r, b[i][0]), i, 0);
		}

		return r;
	}

	//TODO: Como es si la matrix no es cuadrada
	//TODO: Esta bien que alto sea mayor o igual a ancho
	//M debe ser triangularInferior
	template<class T> MN::Matrix<T> resolverAPartirDeTriangularInferior(const MN::AbstractMatrix<T> &m, const MN::AbstractMatrix<T> &b) const {
		assert(b.width() == 1 && m.height() == b.height() && m.height() >= m.width());
		int alto = m.height();
		int ancho = m.width();
		MN::Matrix<T> r = MN::Matrix<T>(ancho, 1, T());

		for (int i = 0; i < ancho; i++) {
			//TODO: Que se hace si m[i][i] = 0
			r.setValueInRowColumn(resolverIncognitaIUsandoAnterioresDeResueltas<T>(i, m[i], r, b[i][0]), i, 0);
		}

		return r;
	}

	void choleskyAux(const MN::AbstractMatrix<double> *matrix, MN::AbstractMatrix<double> *L, int j, double l_jj, K9::Semaphore *sem) {
		int alto = matrix->height();

		for (int i = j + 1; i < alto; ++i) {
			double sumatoria = 0.0;
			MN::Vector<double> filaDeMatriz = L->getRow(i);
			for (auto it = filaDeMatriz.begin(); it != filaDeMatriz.end() && (*it).first < i; ++it) {
				sumatoria += filaDeMatriz[(*it).first] * L->atRowColumn(j, (*it).first);
			}
			double l_ij = (matrix->atRowColumn(j, i) - sumatoria) / l_jj;
			L->setValueInRowColumn(l_ij, i, j);
		}

		sem->signal();
	}

	//TODO: quizas sacar
	//'Matrix' debe ser Simetrica Definida Positiva.
	unique_ptr<MN::AbstractMatrix<double> > cholesky(const unique_ptr<MN::AbstractMatrix<double> > &matrix) {
		assert(matrix->height() == matrix->width());
		int alto = matrix->height();
		int ancho = matrix->width();

		unique_ptr<MN::AbstractMatrix<double> > L(new MN::SparceMatrix<double>(alto, ancho));
		double l_00 = sqrt(matrix->atRowColumn(0, 0));
		L->setValueInRowColumn(l_00, 0, 0);
		MN::Vector<double> filaDeMatriz(matrix->getRow(0));
		//int sizeFilaDeMatriz = filaDeMatriz.size();

		for (auto it = filaDeMatriz.begin(); it != filaDeMatriz.end(); ++it)
			L->setValueInRowColumn((*it).second / l_00, (*it).first, 0);

		vector<thread> threads;
		K9::Semaphore sem(0);
		int threadsCount = 0;
		for (int j = 1; j < ancho - 1; ++j) {
			double l_jj = sqrt(matrix->atRowColumn(j, j) - sumatoriaDePrimerosKElementosDeFilaIAlCuadrado(L, j, j));
			L->setValueInRowColumn(l_jj, j, j);
			
			threads.push_back(thread(&SistemaDeEcuaciones::choleskyAux, this, matrix.get(), L.get(), j, l_jj, &sem));
			++threadsCount;
			while (threadsCount >= 8) {
				sem.wait();
				--threadsCount;
			}
		}

		for (auto it = threads.begin(); it != threads.end(); ++it) {
			it->join();
		}

		L->setValueInRowColumn(sqrt(matrix->atRowColumn(alto - 1, ancho - 1) - sumatoriaDePrimerosKElementosDeFilaIAlCuadrado(L, alto - 1, alto - 1)), alto - 1, ancho - 1);

		return L;
	}

	//TODO: quiza sacar
	double sumatoriaDePrimerosKElementosDeFilaIAlCuadrado(const unique_ptr<MN::AbstractMatrix<double> > &L, const int k, const int i) {
		double sumatoria = 0;
		double aux = 0.0;
		const MN::Vector3D columnas(L->getRow(i));

		for (auto it = columnas.begin(); it != columnas.end() && (*it).first < k; ++it) {
			aux = (*it).second;
			sumatoria += aux*aux;
		}

		return sumatoria;
	}
	/*
	//'Matrix' debe ser Simetrica Definida Positiva.
	template<class T> MN::Matrix<T> cholesky(const MN::Matrix<T> &matrix) {
		assert(matrix.alto() == matrix.ancho());
		int alto = matrix.alto();
		int ancho = matrix.ancho();

		Matrix<T> L = Matrix<T>(alto, ancho, T());
		T l_00 = sqrt(Matrix[0][0]);
		L[0][0] = l_00;

		for (int i = 1; i < alto; i++)
			L[i][0] = matrix[0][i] / l_00;

		for (int j = 1; j < ancho - 1; j++) {
			T l_jj = sqrt(matrix[j][j] - sumatoriaDePrimerosKElementosDeFilaIAlCuadrado(L, j, j));
			L[j][j] = l_jj;

			for (int i = j + 1; i < alto; i++) {
				T sumatoria = 0.0;
				for (int k = 0; k < i; k++) {
					sumatoria += L[i][k] * L[j][k];
				}
				T l_ij = (matrix[j][i] - sumatoria) / l_jj;
				L[i][j] = l_ij;
			}
		}

		L[alto - 1][ancho - 1] = sqrt(Matrix[alto - 1][ancho - 1] - sumatoriaDePrimerosKElementosDeFilaIAlCuadrado(L, alto - 1, alto - 1));

		return L;
	}*/

	template<class T> T sumatoriaDePrimerosKElementosDeFilaIAlCuadrado(const MN::Matrix<T> &L, const int k, const int i) {
		T sumatoria = 0;
		T aux = 0.0;
		for (int j = 0; j < k; j++) {
			aux = L[i][j];
			sumatoria += aux*aux;
		}

		return sumatoria;
	}

	//LLtx = b
	//L triangular inferior con diagonal positiva.
	template<class T> MN::Matrix<T> resolverAPartirDeLLt(const MN::AbstractMatrix<double> &b, const MN::AbstractMatrix<double> &L) const {
		//A = LLt
		//Ax = b
		//LLtx = b
		//Ltx = y
		//Ly = b

		MN::Matrix<T> y = resolverAPartirDeTriangularInferior(L, b);

		return resolverAPartirDeTriangularSuperior(*(L.transpose()), y);
	}

private:
	template<class T>
	int indiceDeFilaConMayorValorEnColumnaCDesdeFilaF(const MN::Matrix<T> &m, int c, int f, const MN::Matrix<int> &vectorDeParticion) const {
		assert(0 <= c && c < m.width() && 0 <= f && f < m.height() && vectorDeParticion.height() == m.height() && vectorDeParticion.width() == 1);
		
		//Yo no estoy trabajando con la fila i y fila j necesariamente, ya que pude haber intercambiado esas filas.
		//Entonces aprovechando que la Matrix es cuadrada, sea P=(p_0, ..., p_n-1)t el vector de particion.
		//Si en p_j no hice intercambio, p_j = columna j = fila j.
		//Si en p_j hice intercambio, p_j me dice cual es la fila que tendria que estar en la fila j.
		//Ej. P=(0,1,2,3)
		//Intercambio 0 con 1
		//P=(1,0,2,3)
		//Si j = 1 quiere decir que tengo que trabajar con la fila 1. Pero en esta fila tendría que estar la fila 0 original.

		int n = m.height();
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

	void intercambiar(MN::Matrix<int> &vectorDeParticion, int i, int j) const {
		assert(0 <= i && i < vectorDeParticion.height() && 0 <= j && j < vectorDeParticion.height() && vectorDeParticion.width() == 1);
		int aux = vectorDeParticion[i][0];
		vectorDeParticion.setValueInRowColumn(vectorDeParticion[j][0], i, 0);
		vectorDeParticion.setValueInRowColumn(aux, j, 0);
	}

	//m debe ser cuadrada.
	template<class T> MN::Matrix<T> obtenerUAPartirDeM(const MN::Matrix<T> &m) const {
		assert(m.width() == m.height());

		int n = m.height();
		MN::Matrix<T> U = MN::Matrix<T>(n, n, T());

		for (int f = 0; f < n; f++) {
			for (int c = f; c < n; c++) {
				U.setValueInRowColumn(m[f][c], f, c);
			}
		}

		return U;

	}

	//TODO: Que se hace si fila[i] = 0
	template <class T>
	T resolverIncognitaIUsandoSiguientesDeResueltas(int i, const MN::Vector<T> &fila, const MN::AbstractMatrix<T> &resueltas, const T &b) const {
		int n = fila.size();
		T sumatoria = b;
		//TODO:Use MN::Vector iterators when implemented.
		for (int j = i+1; j < n; j++) {
			//TODO When MN::Vector iterators are implemented use this implementation
			//sumatoria -= resueltas[it->first][0] * it->second;
			//instead of this one.
			sumatoria -= resueltas[j][0]*fila[j];
		}

		return sumatoria / (double)fila[i];
	}

	//TODO: Que se hace si fila[i] = 0
	template <class T>
	T resolverIncognitaIUsandoAnterioresDeResueltas(int i, const MN::Vector<T> &fila, const MN::AbstractMatrix<T> &resueltas, const T &b) const {
		int n = fila.size();
		T sumatoria = b;
		//TODO: replace for MN::Vector iterator
		for (int j = 0; j < i; j++) {
			//TODO: use this when iterating using MN::Vector iterator
			//sumatoria -= resueltas[it->first][0] * it->second;
			sumatoria -= resueltas[j][0] * fila[j];
		}

		return sumatoria / (double)fila[i];
	}
};

#endif // !_SISTEMA_DE_ECUACIONES_H_
