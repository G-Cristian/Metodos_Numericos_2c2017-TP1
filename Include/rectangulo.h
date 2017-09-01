#ifndef _RECTANGULO_H_
#define _RECTANGULO_H_

class Rectangulo {
public:
	Rectangulo(double izquierda, double arriba, double derecha, double abajo) {
		_left = izquierda;
		_right = derecha;
		_top = arriba;
		_bottom = abajo;
	}

	inline double izquierda() const { return _left; }
	inline double derecha() const{ return _right; }
	inline double arriba() const { return _top; }
	inline double abajo() const{ return _bottom; }

	inline double ancho() const { return _right - _left + 1; }
	inline double alto() const { return _bottom - _top + 1; }

private:
	double _left;
	double _top;
	double _right;
	double _bottom;
};

#endif // !_RECTANGULO_H_
