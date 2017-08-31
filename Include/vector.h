#ifndef _VECTOR_H_
#define _CIRCULO_H_

class Vector3D {
public:
	Vector3D(double x, double y, double z);

	Vector3D normalizar() const;
	double norma() const;
private:
	double _x;
	double _y;
	double _z;
};

#endif // !_VECTOR_H_
