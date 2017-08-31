#ifndef _CIRCULO_H_
#define _CIRCULO_H_

#include "vector.h"

class Circulo {
public:
	Circulo(double x, double y, double radio);

	Vector3D normalEnPuntoXY(double x, double y) const;
private:
	double _x;
	double _y;
	double _radius;
};

#endif // !_CIRCULO_H_
