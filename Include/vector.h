#ifndef _VECTOR_H_
#define _VECTOR_H_

class Vector3D {
public:
	Vector3D(double x, double y, double z);

	bool esCero() const;

	inline double x() const { return _x; }
	inline double y() const { return _y; }
	inline double z() const { return _z; }

	Vector3D normalizar() const;
	double norma() const;

	inline double sumaDeElementos() const { return _x + _y + _z; }
private:
	double _x;
	double _y;
	double _z;
};

#endif // !_VECTOR_H_
