#ifndef _MATRIZ_H_
#define _MATRIZ_H_

#include <vector>
#include <exception>
#include <assert.h>
#include <iostream>

#include "vector.h"
#include "MatrizEsparsa.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

template <class T> class Matrix;
//class MatrizEsparsa;

template<class T> class SumarMatrizFunctor{
public:
	SumarMatrizFunctor(const Matrix<T> &sumando):_sumando(sumando){
		
	}
	
	T operator()(int fila, int columna, T elemento){
		return elemento + _sumando[fila][columna];
	}
private:
	Matrix<T> _sumando;
};

template<class tipoMatriz, class tipoEscalar> class MultiplicarPorEscalarMatrizFunctor{
public:
	MultiplicarPorEscalarMatrizFunctor(const tipoEscalar &escalar){
		_escalar = escalar;
	}
	
	tipoMatriz operator()(int fila, int columna, tipoMatriz elemento){
		return (tipoMatriz)(elemento * _escalar);
	}
private:
	tipoEscalar _escalar;
};

template<class T> class SaturarMatrizAMatrizCharFunctor{
public:
	SaturarMatrizAMatrizCharFunctor(){
	}
	
	T operator()(int fila, int columna, T elemento){
		if (elemento < 0) return 0;
		if (elemento > 255) return 255;
		return elemento;
	}
};

template<class T> class CopiarElementosDeMatrizEsparsaFunctor {
public:
	CopiarElementosDeMatrizEsparsaFunctor(const MatrizEsparsa &otra) :_otra(otra) {

	}

	T operator()(int fila, int columna, T elemento) {
		return (T)_otra.enYX(fila, columna);
	}
private:
	MatrizEsparsa _otra;
};

template <class T> class Matrix{
public:
	Matrix(int alto, int ancho, T valorInicial){
		_ancho = ancho;
		_alto = alto;
	
		_matriz = vector<vector<T> >(alto, vector<T>(ancho,valorInicial));
	}

	Matrix(int alto, int ancho, const T *m) {
		_alto = alto;
		_ancho = ancho;
		_matriz = vector<vector<T> >(alto, vector<T>(ancho, T()));

		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < ancho; j++) {
				_matriz[i][j] = m[i*ancho + j];
			}
		}
	}

	Matrix(const MatrizEsparsa &m) {
		_ancho = m.ancho();
		_alto = m.alto();

		_matriz = vector<vector<double> >(_alto, vector<double>(_ancho, 0.0));

		this->aplicarFuncAElementos(CopiarElementosDeMatrizEsparsaFunctor<T>(m));
	}

	Matrix(const Vector3D &v) {
		_ancho = 1;
		_alto = 3;
		_matriz = vector<vector<T> >(_alto, vector<T>(_ancho, T()));

		_matriz[0][0] = (T)v.x();
		_matriz[1][0] = (T)v.y();
		_matriz[2][0] = (T)v.z();
	}

	Matrix(const vector<Vector3D> &filas) {
		_ancho = 3;
		_alto = filas.size();
		_matriz = vector<vector<T> >(_alto, vector<T>(_ancho, T()));

		for (int i = 0; i < _alto;i++) {
			_matriz[i][0] = (T)filas[i].x();
			_matriz[i][1] = (T)filas[i].y();
			_matriz[i][2] = (T)filas[i].z();
		}
	}
	
	~Matrix(){
	}
	/*
	Matrix<T> & operator=(const Matrix<T> &otra) {
		_alto = otra._alto;
		_ancho = otra._ancho;
		_matriz = otra._matriz;

		return *this;
	}
	*/
	//operador para castear
	template<class tipoACastear>
	operator Matrix<tipoACastear> () const{
		Matrix<tipoACastear> r = Matrix<tipoACastear>(_alto, _ancho, tipoACastear());
		
		for(int i = 0; i < _alto; i++){
			for(int j = 0; j < _ancho; j++){
				r[i][j] = (tipoACastear)_matriz[i][j];
			}
		}
		
		return r;
	}
	
	const vector<T> &operator[](int indice) const{
		assert(indiceEnRangoAlto(indice));
		return _matriz[indice];
	}

	vector<T> &operator[](int indice){
		assert(indiceEnRangoAlto(indice));
		return _matriz[indice];
	}
	
	inline bool indiceEnRangoAlto(int indice) const {
		return 0 <= indice && indice < _alto;
	}

	inline bool indiceEnRangoAncho(int indice) const {
		return 0 <= indice && indice < _ancho;
	}

	inline int ancho() const{
		return _ancho;
	}
	
	inline int alto() const {
		return _alto;
	}

	Vector3D rangoDeTresElementosEnYX(int y, int x) const {
		assert(x < _ancho && y < _alto);

		double a = (double)_matriz[y][x];
		double b = 0.0;
		double c = 0.0;

		if (x + 1 < _ancho) {
			b = (double)_matriz[y][x + 1];
			if (x + 2 < _ancho)
				c = (double)_matriz[y][x + 2];
		}

		return Vector3D(a, b, c);
	}

	void escribirTresElementosEnYX(int y, int x, const Vector3D &elementos) {
		assert(x < _ancho && y < _alto);

		_matriz[y][x] = (T)elementos.x();
		if (x + 1 < _ancho) {
			_matriz[y][x + 1] = (T)elementos.y();
			if (x + 2 < _ancho)
				_matriz[y][x + 2] = (T)elementos.z();
		}
	}
	
	//Deben tener el mismo tamaño.
	Matrix<T> operator+(const Matrix<T> &otra) const {
		//if(_alto != otra._alto || _ancho != otra._ancho)
		//	throw "Deben tener las mismas dimensiones.";
		assert(_alto == otra._alto && _ancho == otra._ancho);
		return crearMatrizAPartirDeOtraAplicandoFuncAElementos(*this, SumarMatrizFunctor<T>(otra));
	}

	Matrix<T> operator-()const {
		return *this * (-1);
	}
	
	Matrix<T> operator-(const Matrix<T> &otra)const {
		return *this + (-otra);
	}
	
	Matrix<T> operator*(const Matrix<T> &otra) const {
		assert(_ancho == otra._alto);
		int m = _alto;
		int n = _ancho;
		int s = otra._ancho;

		Matrix<T> r = Matrix<T>(m, s, T());
		T sum = 0;
		for (int f = 0; f < m; f++) {
			for (int c = 0; c < s; c++) {
				sum = 0;
				for (int k = 0; k < n; k++) {
					sum += _matriz[f][k] * otra._matriz[k][c];
				}

				r._matriz[f][c] = sum;
			}
		}

		return r;
	}

	template <class U>
	Matrix<T> operator*(U escalar) const {
		return crearMatrizAPartirDeOtraAplicandoFuncAElementos(*this, MultiplicarPorEscalarMatrizFunctor<T, U>(escalar));
	}

	/*
	Matrix<T> operator/(int escalar) const {
		return *this * (1.0/(double)escalar);
	}
	
	Matrix<T> operator/(double escalar) const {
		return *this * (1.0/escalar);
	}
	*/
	T norma() const {
		assert(_alto == 1 || _ancho == 1);

		T sum = T();
		int n = (_alto == 1) ? _ancho : _alto;
		for (int i = 0; i < n; i++) {
			if (_alto == 1)
				sum += _matriz[0][i] * _matriz[0][i];
			else
				sum += _matriz[i][0] * _matriz[i][0];
		}

		return (T)sqrt(sum);
	}

	//Crea puntero. El usuario debe encargarse de liberar la memoria.
	T* comoPuntero() const {
		T * ret = new T[_ancho*_alto];

		for (int i = 0; i < _alto; i++) {
			for (int j = 0; j < _ancho; j++) {
				ret[i*_ancho + j] = _matriz[i][j];
			}
		}

		return ret;
	}

	//Satura los valores entre 0 y 255
	Matrix<T> saturar() const {
		return crearMatrizAPartirDeOtraAplicandoFuncAElementos(*this, SaturarMatrizAMatrizCharFunctor<T>());
	}
	
	Matrix<T> filaComoMatriz(int fila) const {
		assert(indiceEnRangoAlto(fila));
		Matrix<T> r = Matrix<T>(1, _ancho, T());
		for (int i = 0; i < _ancho; i++) {
			r[0][i] = _matriz[fila][i];
		}

		return r;
	}

	Matrix<T> & agreagrleAFilaIOtraFila(int filaI, const Matrix<T> &otraFila) {
		assert(_ancho == otraFila._ancho && otraFila._alto == 1);

		return agreagrleAFilaIOtraFila(filaI, otraFila[0]);
	}

	Matrix<T> & agreagrleAFilaIOtraFila(int filaI, const vector<T> &otraFila) {
		assert(_ancho == otraFila.size());
		
		for (int c = 0; c < _ancho; c++) {
			_matriz[filaI][c] += otraFila[c];
		}

		return *this;
	}

	Matrix<T> submatriz(int filaSup, int colIzq, int filaInf, int colDer) const {
		assert(indiceEnRangoAlto(filaSup) && indiceEnRangoAlto(filaInf) && indiceEnRangoAncho(colIzq) && indiceEnRangoAncho(colDer) &&
			filaSup <= filaInf && colIzq <= colDer);
		Matrix<T> r = Matrix<T>(filaInf - filaSup + 1, colDer - colIzq + 1, T());

		for (int i = 0; i < r._alto; i++) {
			for (int j = 0; j < r._ancho; j++) {
				r[i][j] = _matriz[filaSup + i][colIzq + j];
			}
		}

		return r;
	}

	Matrix<T> transpuesta() const {
		Matrix<T> t = Matrix<T>(_ancho, _alto, T());

		for (int i = 0; i < _alto; i++) {
			for (int j = 0; j < _ancho; j++) {
				t[j][i] = _matriz[i][j];
			}
		}

		return t;
	}

	//Modifica la Matrix original.
	//Func es de tipo T fun(int fila, int columna, T elementoActual) donde fila, columna es la posicion de la Matrix que se recorre y elementoActual es el elemento actual de la Matrix que se recorre.
	template<class Func> Matrix<T>& aplicarFuncAElementos(Func func) {
		for(int i = 0; i < _alto; i++){
			for(int j = 0; j < _ancho; j++){
				_matriz[i][j] = func(i,j,_matriz[i][j]);
			}
		}
		return *this;
	}

	void mostrar() const {
		for (int i = 0; i < _alto; i++) {
			for (int j = 0; j < _ancho; j++) {
				cout << _matriz[i][j] << " ";
			}
			cout << endl;
		}
	}

private:
	vector<vector<T> > _matriz;
	int _ancho;
	int _alto;
};

template<class T> Matrix<T> operator*(int escalar, const Matrix<T> &otra) {
	return otra * escalar;
}

template<class T> Matrix<T> operator*(double escalar, const Matrix<T> &otra) {
	return otra * escalar;
}

template<class T, class Func> Matrix<T> crearMatrizAPartirDeOtraAplicandoFuncAElementos(const Matrix<T> &otra, Func func) {
	Matrix<T> r = otra;
	r.aplicarFuncAElementos(func);
	
	return r;
}

//typedef Matrix<char> MatrizChar;
//typedef Matrix<unsigned char> MatrizUChar;
//typedef Matrix<int> MatrixInt;
//typedef Matrix<double> MatrixDoule;

#endif
