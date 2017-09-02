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
	double z = sqrt((_radius*_radius) - ((x - _x)*(x - _x)) - ((y - _y)*(y - _y)));

	return Vector3D(x - _x, y - _y, z);
}


