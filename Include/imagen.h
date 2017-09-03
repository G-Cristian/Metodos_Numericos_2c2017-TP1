#ifndef _IMAGEN_H_
#define _IMAGEN_H_

#include "matriz.h"
#include "vector.h"
#include "rectangulo.h"
#include <utility>

using namespace std;

class Imagen {
public:
	Imagen(const MatrizUChar &datos, int canales = 3);
	Imagen(int alto, int ancho, const unsigned char *datos, int canales = 3);
	~Imagen();

	inline MatrizUChar datos() const { return _datos; }
	//Representa el ancho de la imagen EN PIXELS (es decir que si la imagen es a color con 3 canales devuelve al ancho de la matriz interna dividido 3).
	inline int ancho() const { return (int)(_datos.ancho() / _canales); }
	inline int alto() const { return _datos.alto(); }
	//Si tiene menos de 3 canales las ultimas posiciones del vector tienen 0.0.
	inline Vector3D pixelEnXY(int x, int y) const { return _datos.rangoDeTresElementosEnYX(y, x*_canales); }

	//Si se le pasa una mascara descarta lo negro que bordea la imagen.
	Imagen subImagenReal() const;
	Imagen subImagen(const Rectangulo &regionDeSubImagen) const;
	//Si se le pasa una mascara descarta lo negro que bordea la imagen.
	Rectangulo subRectanguloConImagenReal() const;

	double brilloDelPixel(int x, int y) const;
	double brilloDeRegion(const Rectangulo &region) const;
	pair<int, int> posicionPixelMasBrillante() const;
	pair<int, int> posicionPixelMasBrillanteDentroDeRegion(const Rectangulo &region) const;
	//offset define la region siendo cuantos pixel al rededor del pixel alctual se verifican.
	//Ejemplo: pixel actual = (x, y) entonces la region es [x-offset, x+oofset], [y-offset, y+offset]
	Rectangulo regionMasBrillante(int offset) const;
	//offset define la region siendo cuantos pixel al rededor del pixel alctual se verifican.
	//Ejemplo: pixel actual = (x, y) entonces la region es [x-offset, x+oofset], [y-offset, y+offset]
	Rectangulo regionMasBrillanteDentroDeRegion(int offset, const Rectangulo &region) const;
	void pintarPixel(int x, int y, int r, int g, int b);
	void pintarBordeDeRegion(const Rectangulo &region, int r, int g, int b);
private:
	MatrizUChar _datos;
	int _canales;
};

#endif // !_IMAGEN_H_
