#ifndef _FOTOMETRIAESTEREO_H_
#define _FOTOMETRIAESTEREO_H_

#include "matriz.h"
#include "imagen.h"
#include <vector>

using namespace std;

class FotometriaEstereo {
public:
	FotometriaEstereo(const Imagen &mascaraCirculo, const vector<Imagen> &imagenesCirculos, const Imagen &mascaraImagenesACalcular, const vector<Imagen> &imagenesACalcular, const vector<int> &imagenesAUsar);
	~FotometriaEstereo();

	inline vector<Vector3D> luces() const { return _luces; }

private:
	void calibrar();
	Matriz<Vector3D> obtenerNormales();
	MatrizDouble obtenerLucesAUsar() const;
	//Obtiene M = I_0 * Ro * N en pixel x,y
	MatrizDouble obtenerIRoNEnPixelXYAPartirDeSistemaPLU(int x, int y, const MatrizDouble &P, const MatrizDouble &L, const MatrizDouble &U);
	MatrizDouble intensidadesEnPixelXY(int x, int y);

	Imagen _mascaraCirculo;
	vector<Imagen> _imagenesCirculos;
	Imagen _mascaraImagenesACalcular;
	vector<Imagen> _imagenesACalcular;
	vector<int> _imagenesAUsar;
	MatrizDouble _matrizLucesAUsar;
	vector<Vector3D> _luces;
};

#endif // !_FOTOMETRIAESTEREO_H_
