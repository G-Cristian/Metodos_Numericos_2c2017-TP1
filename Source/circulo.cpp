#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../Include/circulo.h"
#include "../Include/VectorVecrorImp.h"



Circulo::Circulo(double x, double y, double radio)
{
	_x = x;
	_y = y;
	_radius = radio;
}


MN::Vector3D Circulo::normalEnPuntoXY(double x, double y) const
{
	double z = sqrt((_radius*_radius) - ((x - _x)*(x - _x)) - ((y - _y)*(y - _y)));

	MN::Vector3D tmp(new MN::VectorVectorImp<double>(3));
	tmp.setValueAtIndex(x, 0);
	tmp.setValueAtIndex(y, 1);
	tmp.setValueAtIndex(z, 2);

	return tmp;
}


