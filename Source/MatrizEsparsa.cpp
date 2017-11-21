#include "../Include/MatrizEsparsa.h"
#include "../Include/matriz.h"
#include <assert.h>

using  namespace std;

MatrizEsparsa::MatrizEsparsa(int alto, int ancho) {
	_alto = alto;
	_ancho = ancho;

	_columnasConElementosEnFilaI = vector<map<int, tipoElementos> >(_alto, map<int, tipoElementos>());
}

template<class T> MatrizEsparsa::MatrizEsparsa(const Matriz<T> &otra){
	_ancho = ancho;
	_alto = alto;

	for (int i = 0; i < _alto; i++) {
		for (int j = 0; j < _ancho; j++) {
			insertarEnYX(i, j, (tipoElementos)otra[i][j]);
		}
	}
}

MatrizEsparsa::~MatrizEsparsa() {

}

tipoElementos MatrizEsparsa::enYX(int y, int x) const {
	assert(0 <= x && x < _ancho && 0 <= y && y < _alto);

	const map<int, tipoElementos> &columnasDeFila = _columnasConElementosEnFilaI[y];

	map<int, tipoElementos>::const_iterator it = columnasDeFila.find(x);

	if (it != columnasDeFila.end()) {
		return it->second;
	}
	else {
		return 0;
	}
}
void MatrizEsparsa::insertarEnYX(int y, int x, tipoElementos elemento) {
	assert(0 <= x && x < _ancho && 0 <= y && y < _alto);

	map<int, tipoElementos> &columnasDeFila = _columnasConElementosEnFilaI[y];
	if (elemento != 0) {
		map<int, tipoElementos>::const_iterator it = columnasDeFila.find(x);

		if (it != columnasDeFila.end()) {
			columnasDeFila.at(x) = elemento;
		}
		else {
			columnasDeFila.insert(pair<int, tipoElementos>(x, elemento));
		}
	}
	else
	{
		columnasDeFila.erase(x);
	}
}

MatrizEsparsa MatrizEsparsa::operator*(const MatrizEsparsa &otra) const {
	assert(_ancho == otra._alto);

	MatrizEsparsa mat = MatrizEsparsa(_alto, otra._ancho);
	tipoElementos aux = 0;

	const map<int, tipoElementos> *columnasDeFilaDeM1;
	const map<int, tipoElementos> *columnasDeFilaDeM2;
	for (int i = 0; i < _alto; i++) {
		columnasDeFilaDeM1 = &(_columnasConElementosEnFilaI[i]);
		for (map<int, tipoElementos>::const_iterator colDeM1 = columnasDeFilaDeM1->begin(); colDeM1 != columnasDeFilaDeM1->end(); colDeM1++) {
			columnasDeFilaDeM2 = &(otra._columnasConElementosEnFilaI[colDeM1->first]);
			for (map<int, tipoElementos>::const_iterator colDeM2 = columnasDeFilaDeM2->begin(); colDeM2 != columnasDeFilaDeM2->end(); colDeM2++) {
				aux = colDeM1->second * colDeM2->second;

				mat.insertarEnYX(i, colDeM2->first, aux + mat.enYX(i, colDeM2->first));
			}
		}
	}

	return mat;
}

MatrizEsparsa MatrizEsparsa::transpuesta() const {
	MatrizEsparsa mat = MatrizEsparsa(_ancho, _alto);
	const map<int, tipoElementos> *columnasDeFila;
	for (int i = 0; i < _alto; i++) {
		columnasDeFila = &(_columnasConElementosEnFilaI[i]);
		for (map<int, tipoElementos>::const_iterator it = columnasDeFila->begin(); it != columnasDeFila->end(); it++) {
			//inserto valor a_(y, x) en mat_(x, y)
			mat.insertarEnYX(it->first, i, it->second);
		}
	}

	return mat;
}
/*
MatrizEsparsa MatrizEsparsa::transpuestaPorOtra(const MatrizEsparsa &otra) const {
	assert(_alto == otra._alto);

	MatrizEsparsa mat = MatrizEsparsa(_ancho, otra._ancho);
	tipoElementos aux = 0;

	for (map<tipoClave, tipoElementos>::const_iterator it = _elementos.begin(); it != _elementos.end(); it++) {
		//aux = _elemento[y][k]t * otra[k][x] = _elemento[k][y]*otra[k][x]
		//y = it->first.first, pero como es transpuesta y = it->first.second
		//x = it->first.second, pero como es transpuesta x = it->first.first
		int y = it->first.second;
		int x = it->first.first;
		for (int k = 0; k < otra._ancho; k++) {
			aux = it->second * otra.enYX(x, k);

			if (aux != 0) {
				map<tipoClave, tipoElementos>::const_iterator e = mat._elementos.find(tipoClave(y, k));
				if (e != mat._elementos.end()) {
					mat._elementos.at(tipoClave(y, k)) += aux;
				}
				else {
					mat._elementos.insert(pair<tipoClave, tipoElementos>(tipoClave(y, k), aux));
				}
			}
		}
	}

	return mat;
}
*/