#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../Include/circulo.h"



Circulo::Circulo(double x, double y, double radio)
{
	_x = x;
	_y = y;
	_radius = radio;
}


Vector3D Circulo::normalEnPuntoXY(double x, double y) const
{
	double z = sqrt(pow(_radius, 2) - pow(x - _x, 2) - pow(y - _y, 2));
	Vector3D v = Vector3D(x - _x, y - _y, z);

	return v;
}


