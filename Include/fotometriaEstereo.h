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

	pair<Matriz<Vector3D>, MatrizDouble> resolverNormalesYProfundidades();
private:
	void calibrar();
	Matriz<Vector3D> obtenerNormales();
	MatrizDouble obtenerLucesAUsar() const;
	//Obtiene M = I_0 * Ro * N en pixel x,y
	MatrizDouble obtenerIRoNEnPixelXYAPartirDeSistemaPLU(int x, int y, const MatrizDouble &P, const MatrizDouble &L, const MatrizDouble &U);
	MatrizDouble intensidadesEnPixelXY(int x, int y);
	//'mascara' representa una imagen mascara (blanco donde esta el objeto de la imagen y negro en el resto)
	bool pixelEstaEnImagen(int x, int y, const Imagen &mascara) const;
	void obtenerTablaIndicesNumeroDePixelYCantidad();
	void obtenerMV(const Matriz<Vector3D> &normales);
	MatrizDouble obtenerProfundidades(const Matriz<Vector3D> &normales);


	Imagen _mascaraCirculo;
	vector<Imagen> _imagenesCirculos;
	Imagen _mascaraImagenesACalcular;
	vector<Imagen> _imagenesACalcular;
	vector<int> _imagenesAUsar;
	MatrizDouble _matrizLucesAUsar;
	vector<Vector3D> _luces;
	MatrizInt _tablaIndicesNumeroDePixel;
	int _cantidadDePixelsDeImagen;
	MatrizDouble _M;
	MatrizDouble _V;
};

#endif // !_FOTOMETRIAESTEREO_H_
