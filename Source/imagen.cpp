#include "../Include/imagen.h"
#include "../Include/rectangulo.h"
#include <algorithm>

using namespace std;

Imagen::Imagen(const MatrizUChar &datos, int canales):_datos(datos) {
	_canales = canales;
}
//alto y ancho representan las dimenciones en *datos (es decir el ancho de la imagen va a ser ancho / canales).
Imagen::Imagen(int alto, int ancho, const unsigned char *datos, int canales) : _datos(MatrizUChar(alto, ancho*canales, datos)) {
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

double Imagen::brilloDelPixel(int x, int y) const {
	return pixelEnXY(x, y).sumaDeElementos();
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

void Imagen::pintarPixel(int x, int y, int r, int g, int b) {
	Vector3D color = Vector3D(b, g, r);

	_datos.escribirTresElementosEnYX(y, x * _canales, color);
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