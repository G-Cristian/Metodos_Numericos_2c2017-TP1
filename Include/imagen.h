#ifndef _IMAGEN_H_
#define _IMAGEN_H_

#include "AbstractMatrix.h"
#include "Matrix.h"
#include "rectangulo.h"
#include <utility>
#include "vector.h"


using namespace std;

class Imagen {
public:
	Imagen(const MN::AbstractMatrix<unsigned char> &datos, int canales = 3);
	Imagen(size_t alto, size_t ancho, const unsigned char *datos, size_t canales = 3);
	Imagen(const Imagen &other);
	Imagen( Imagen &&other)noexcept;
	~Imagen();

	inline const MN::Matrix<unsigned char>& datos() const { return _datos; }

	Imagen& operator=(const Imagen &other);
	Imagen& operator=(Imagen &&other) noexcept;

	//Representa el ancho de la imagen EN PIXELS (es decir que si la imagen es a color con 3 canales devuelve al ancho de la Matrix interna dividido 3).
	inline int ancho() const { return (int)(_datos.width() / _canales); }
	inline int alto() const { return _datos.height(); }
	inline int canales() const { return _canales; }
	//Si tiene menos de 3 canales las ultimas posiciones del vector tienen 0.0.
	inline MN::Vector<unsigned char> pixelEnXY(int x, int y) const { return _datos.rangeOfThreeElementsAtYX(y, x*_canales); }

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
	void pintarPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
	void pintarBordeDeRegion(const Rectangulo &region, int r, int g, int b);
private:
	MN::Matrix<unsigned char> _datos;
	int _canales;
};

#endif // !_IMAGEN_H_
