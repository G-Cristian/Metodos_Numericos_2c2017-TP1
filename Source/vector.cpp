#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../Include/vector.h"


Vector3D::Vector3D(double x, double y, double z)
{
 _x = x;
 _y = y;
 _z = z;
	
}

bool Vector3D::esCero()const
{
	return _x == 0.0 && _y == 0.0 && _z == 0.0;
}

Vector3D Vector3D::normalizar() const
{	
	double n = norma();
	return Vector3D(_x/n, _y/n, _z/n);
}


double Vector3D::norma() const
{
	return sqrt( _x*_x + _y*_y + _z*_z );	
}

