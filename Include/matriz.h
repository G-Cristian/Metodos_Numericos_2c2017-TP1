#ifndef _MATRIZ_H_
#define _MATRIZ_H_

#include <vector>
#include <exception>
#include <assert.h>
#include <iostream>

using namespace std;

template <class T> class Matriz;

template<class T> class SumarMatrizFunctor{
public:
	SumarMatrizFunctor(const Matriz<T> &sumando):_sumando(sumando){
		
	}
	
	T operator()(int fila, int columna, T elemento){
		return elemento + _sumando[fila][columna];
	}
private:
	Matriz<T> _sumando;
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

template <class T> class Matriz{
public:
	Matriz(int alto, int ancho, T valorInicial){
		_ancho = ancho;
		_alto = alto;
	
		_matriz = vector<vector<T> >(alto, vector<T>(ancho,valorInicial));
	}
	
	~Matriz(){
	}
	
	//operador para castear
	template<class tipoACastear> operator Matriz<tipoACastear>()const {
		Matriz<tipoACastear> r = Matriz<tipoACastear>(_alto, _ancho, 0);
		
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
	
	//Deben tener el mismo tamaño.
	Matriz<T> operator+(const Matriz<T> &otra) const {
		//if(_alto != otra._alto || _ancho != otra._ancho)
		//	throw "Deben tener las mismas dimensiones.";
		assert(_alto == otra._alto && _ancho == otra._ancho);
		return crearMatrizAPartirDeOtraAplicandoFuncAElementos(*this, SumarMatrizFunctor<T>(otra));
	}

	Matriz<T> operator-()const {
		return *this * (-1);
	}
	
	Matriz<T> operator-(const Matriz<T> &otra)const {
		return *this + (-otra);
	}
	
	template <class U>
	Matriz<T> operator*(U escalar) const {
		return crearMatrizAPartirDeOtraAplicandoFuncAElementos(*this, MultiplicarPorEscalarMatrizFunctor<T, U>(escalar));
	}
	/*
	Matriz<T> operator/(int escalar) const {
		return *this * (1.0/(double)escalar);
	}
	
	Matriz<T> operator/(double escalar) const {
		return *this * (1.0/escalar);
	}
	*/
	//Satura los valores entre 0 y 255
	Matriz<T> saturar() const {
		return crearMatrizAPartirDeOtraAplicandoFuncAElementos(*this, SaturarMatrizAMatrizCharFunctor<T>());
	}
	
	Matriz<T> filaComoMatriz(int fila) const {
		assert(indiceEnRangoAlto(fila));
		Matriz<T> r = Matri<T>(1, _ancho, T());
		for (int i = 0; i < _ancho; i++) {
			r[1][i] = _matriz[fila][i];
		}

		return r;
	}

	Matriz<T> submatriz(int filaSup, int colIzq, int filaInf, int colDer) const {
		assert(indiceEnRangoAlto(filaSup) && indiceEnRangoAlto(filaInf) && indiceEnRangoAncho(colIzq) && indiceEnRangoAncho(colDer) &&
			filaSup <= filaInf && colIzq <= colDer);
		Matriz<T> r = Matriz<T>(filaInf - filaSup + 1, colDer - colIzq + 1, T());

		for (int i = 0; i < r._alto; i++) {
			for (int j = 0; j < r._ancho; j++) {
				r[i][j] = _matriz[filaSup + i][colDer + j];
			}
		}

		return r;
	}

	//Modifica la matriz original.
	//Func es de tipo T fun(int fila, int columna, T elementoActual) donde fila, columna es la posicion de la matriz que se recorre y elementoActual es el elemento actual de la matriz que se recorre.
	template<class Func> Matriz<T>& aplicarFuncAElementos(Func func) {
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

template<class T> Matriz<T> operator*(int escalar, const Matriz<T> &otra) {
	return otra * escalar;
}

template<class T> Matriz<T> operator*(double escalar, const Matriz<T> &otra) {
	return otra * escalar;
}

template<class T, class Func> Matriz<T> crearMatrizAPartirDeOtraAplicandoFuncAElementos(const Matriz<T> &otra, Func func) {
	Matriz<T> r = otra;
	r.aplicarFuncAElementos(func);
	
	return r;
}

typedef Matriz<char> MatrizChar;
typedef Matriz<unsigned char> MatrizUChar;
typedef Matriz<int> MatrizInt;
typedef Matriz<double> MatrizDouble;

#endif
