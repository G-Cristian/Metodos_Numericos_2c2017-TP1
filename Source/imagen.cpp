#include "../Include/AbstractMatrix.h"
#include "../Include/imagen.h"
#include "../Include/Matrix.h"
#include "../Include/rectangulo.h"
#include "../Include/VectorVecrorImp.h"
#include <algorithm>
#include <iostream>

using namespace std;

Imagen::Imagen(const MN::AbstractMatrix<unsigned char> &datos, int canales):_datos(datos) {
	_canales = canales;
}
//alto y ancho representan las dimenciones en *datos (es decir el ancho de la imagen va a ser ancho / canales).
Imagen::Imagen(size_t alto, size_t ancho, const unsigned char *datos, size_t canales) : _datos(std::move(MN::Matrix<unsigned char>(alto, ancho*canales, datos))) {
	_canales = canales;
}

Imagen::Imagen(const Imagen &other):
	_datos(other._datos)
	,_canales(other._canales){
	cout << "Imagen copy constructor" << endl;
}

Imagen::Imagen(Imagen &&other) noexcept:
	_datos(std::move(other._datos))
	,_canales(other._canales)
{
	cout << "Imagen move constructor" << endl;
}

Imagen::~Imagen() {

}

Imagen& Imagen::operator=(const Imagen &other) {
	cout << "Imagen copy assigment" << endl;
	if (&other != this) {
		_datos = other._datos;
		_canales = other._canales;
	}
	return *this;
}

Imagen& Imagen::operator=(Imagen &&other) noexcept {
	cout << "Imagen move assigment" << endl;
	if (&other != this) {
		_datos = std::move(other._datos);
		_canales = other._canales;
	}
	return *this;
}

//Si se le pasa una mascara descarta lo negro que bordea la imagen.
Imagen Imagen::subImagenReal() const {
	Rectangulo r = subRectanguloConImagenReal();
	return subImagen(r);
}

Imagen Imagen::subImagen(const Rectangulo &regionDeSubImagen) const {
	std::unique_ptr<MN::AbstractMatrix<unsigned char> >m = _datos.submatrix(regionDeSubImagen.arriba(), regionDeSubImagen.izquierda() * _canales,
		regionDeSubImagen.abajo(), regionDeSubImagen.derecha() * _canales + _canales - 1);

	return Imagen(*m, _canales);
}

//Si se le pasa una mascara descarta lo negro que bordea la imagen.
Rectangulo Imagen::subRectanguloConImagenReal() const {
	int ancho = _datos.width();
	int alto = _datos.height();
	int minX = ancho + 1;
	int maxX = -1;
	int minY = alto + 1;
	int maxY = -1;

	for (int i = 0; i < alto; i++) {
		for (int j = 0; j < ancho; j += _canales) {
			if (!_datos.rangeOfThreeElementsAtYX(i, j).isZero()) {
				minX = min(j, minX);
				maxX = max(j, maxX);
				minY = min(i, minY);
				maxY = max(i, maxY);
			}
		}
	}

	return Rectangulo(minX / _canales, minY, maxX / _canales, maxY);
}

double Imagen::brilloDelPixel(int x, int y) const {
	return pixelEnXY(x, y).sumOfElements();
}

double Imagen::brilloDeRegion(const Rectangulo &region) const {
	double brillo = 0;
	int derecha = region.derecha();
	int abajo = region.abajo();
	for (int i = region.arriba(); i <= abajo; i++) {
		for (int j = region.izquierda(); j <= derecha; j++) {
			brillo += brilloDelPixel(j, i);
		}
	}

	return brillo;
}

pair<int, int> Imagen::posicionPixelMasBrillante() const {
	return posicionPixelMasBrillanteDentroDeRegion(Rectangulo(0, 0, ancho() - 1, alto() - 1));
}

pair<int, int> Imagen::posicionPixelMasBrillanteDentroDeRegion(const Rectangulo &region) const {
	int x = 0;
	int y = 0;
	double maxBrillo = -1;

	int derecha = region.derecha();
	int abajo = region.abajo();

	for (int i = region.arriba(); i <= abajo; i++) {
		for (int j = region.izquierda(); j <= derecha; j++) {
			double brillo = brilloDelPixel(j, i);
			if (maxBrillo < brillo) {
				maxBrillo = brillo;
				x = j;
				y = i;
			}
		}
	}

	return pair<int, int>(x, y);
}

//offset define la region siendo cuantos pixel al rededor del pixel alctual se verifican.
//Ejemplo: pixel actual = (x, y) entonces la region es [x-offset, x+oofset], [y-offset, y+offset]
Rectangulo Imagen::regionMasBrillante(int offset) const {
	return regionMasBrillanteDentroDeRegion(offset, Rectangulo(0, 0, ancho() - 1, alto() - 1));
}

//offset define la region siendo cuantos pixel al rededor del pixel alctual se verifican.
//Ejemplo: pixel actual = (x, y) entonces la region es [x-offset, x+oofset], [y-offset, y+offset]
Rectangulo Imagen::regionMasBrillanteDentroDeRegion(int offset, const Rectangulo &region) const {
	int maxXCenter = 0;
	int maxYCenter = 0;
	double maxBrillo = -1;

	int derecha = region.derecha() - offset;
	int abajo = region.abajo() - offset;

	for (int i = region.arriba() + offset; i <= abajo; i++) {
		for (int j = region.izquierda() + offset; j <= derecha; j++) {
			double brillo = brilloDeRegion(Rectangulo(j-offset, i-offset, j+offset, i+offset));
			if (maxBrillo < brillo) {
				maxBrillo = brillo;
				maxXCenter = j;
				maxYCenter = i;
			}
		}
	}

	return Rectangulo(maxXCenter - offset, maxYCenter - offset, maxXCenter + offset, maxYCenter + offset);
}

void Imagen::pintarPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	MN::Vector<unsigned char> color(new MN::VectorVectorImp<unsigned char>(3), { b, g, r });

	_datos.writeThreeElementsInRowColumn(y, x * _canales, color);
}

void Imagen::pintarBordeDeRegion(const Rectangulo &region, int r, int g, int b) {
	int izquierda = region.izquierda();
	int arriba = region.arriba();
	int derecha = region.derecha();
	int abajo = region.abajo();

	for (int i = arriba; i <= abajo; i++) {
		pintarPixel(izquierda, i, r, g, b);
		pintarPixel(derecha, i, r, g, b);
	}
	for (int j = izquierda; j <= derecha; j++) {
		pintarPixel(j, arriba, r, g, b);
		pintarPixel(j, abajo, r, g, b);
	}
}