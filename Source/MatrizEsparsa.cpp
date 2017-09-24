#include "../Include/MatrizEsparsa.h"
#include <assert.h>

using  namespace std;

MatrizEsparsa::MatrizEsparsa(int alto, int ancho) {
	_alto = alto;
	_ancho = ancho;
}

MatrizEsparsa::~MatrizEsparsa() {

}

tipoElementos MatrizEsparsa::enYX(int y, int x) const {
	assert(0 <= x && x < _ancho && 0 <= y && y < _alto);

	map<tipoClave, tipoElementos>::const_iterator it = _elementos.find(tipoClave(y, x));

	if (it != _elementos.end()) {
		return it->second;
	}
	else {
		return 0;
	}
}
void MatrizEsparsa::insertarEnYX(int y, int x, tipoElementos elemento) {
	assert(0 <= x && x < _ancho && 0 <= y && y < _alto);

	if (elemento != 0) {
		map<tipoClave, tipoElementos>::const_iterator it = _elementos.find(tipoClave(y, x));

		if (it != _elementos.end()) {
			_elementos.at(tipoClave(y, x)) = elemento;
		}
		else {
			_elementos.insert(pair<tipoClave, tipoElementos>(tipoClave(y, x), elemento));
		}
	}
}

MatrizEsparsa MatrizEsparsa::operator*(const MatrizEsparsa &otra) const {
	assert(_ancho == otra._alto);

	MatrizEsparsa mat = MatrizEsparsa(_alto, otra._ancho);
	tipoElementos aux = 0;
	for (int y = 0; y < mat._alto; y++) {
		for (int x = 0; x < mat._ancho; x++) {
			aux = 0;
			for (int k = 0; k < _ancho; k++) {
				aux += enYX(y, k) * otra.enYX(k, x);
			}

			if (aux != 0) {
				mat.insertarEnYX(y, x, aux);
			}
		}
	}

	return mat;
}

MatrizEsparsa MatrizEsparsa::transpuesta() const {
	MatrizEsparsa mat = MatrizEsparsa(_ancho, _alto);

	for (map<tipoClave, tipoElementos>::const_iterator it = _elementos.begin(); it != _elementos.end(); it++) {
		//inserto valor a_(y, x) en mat_(x, y)
		//donce y = it->first.first, x = it->first.second
		mat.insertarEnYX(it->first.second, it->first.first, it->second);
	}

	return mat;
}

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