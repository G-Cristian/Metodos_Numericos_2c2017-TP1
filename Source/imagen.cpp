#include "../Include/imagen.h"
#include "../Include/rectangulo.h"
#include <algorithm>

using namespace std;

Imagen::Imagen(const MatrizUChar &datos, int canales):_datos(datos) {
	_canales = canales;
}
//alto y ancho representan las dimenciones en *datos (es decir el ancho de la imagen va a ser ancho / canales).
Imagen::Imagen(int alto, int ancho, const unsigned char *datos, int canales) : _datos(MatrizUChar(alto, ancho, datos)) {
	_canales = canales;
}

Imagen::~Imagen() {

}

//Si se le pasa una mascara descarta lo negro que bordea la imagen.
Imagen Imagen::subImagenReal() const {
	Rectangulo r = subRectanguloConImagenReal();
	return subImagen(r);
}

Imagen Imagen::subImagen(const Rectangulo &regionDeSubImagen) const {
	MatrizUChar m = _datos.submatriz(regionDeSubImagen.arriba(), regionDeSubImagen.izquierda() * _canales,
									 regionDeSubImagen.abajo(), regionDeSubImagen.derecha() * _canales + _canales - 1);

	return Imagen(m, _canales);
}

//Si se le pasa una mascara descarta lo negro que bordea la imagen.
Rectangulo Imagen::subRectanguloConImagenReal() const {
	int ancho = _datos.ancho();
	int alto = _datos.alto();
	int minX = ancho + 1;
	int maxX = -1;
	int minY = alto + 1;
	int maxY = -1;

	for (int i = 0; i < alto; i++) {
		for (int j = 0; j < ancho; j += _canales) {
			if (!_datos.rangoDeTresElementosEnYX(i, j).esCero()) {
				minX = min(j, minX);
				maxX = max(j, maxX);
				minY = min(i, minY);
				maxY = max(i, maxY);
			}
		}
	}

	return Rectangulo(minX / _canales, minY, maxX / _canales, maxY);
}

pair<int, int> Imagen::posicionPixelMasBrillante() const {
	int x = 0;
	int y = 0;
	double maxBrillo = -1;

	int ancho = this->ancho();
	int alto = this->alto();

	Vector3D aux = Vector3D(0.0, 0.0, 0.0);
	for (int i = 0; i < alto; i++) {
		for (int j = 0; j < ancho; j++) {
			aux = this->pixelEnXY(j, i);
			if (maxBrillo < aux.sumaDeElementos()) {
				x = j;
				y = i;
			}
		}
	}

	return pair<int, int>(x, y);
}