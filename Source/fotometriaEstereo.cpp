#include "../Include/fotometriaEstereo.h"
#include "../Include/rectangulo.h"
#include "../Include/circulo.h"
#include "../Include/sistemaDeEcuaciones.h"

FotometriaEstereo::FotometriaEstereo(const Imagen &mascaraCirculo, const vector<Imagen> &imagenesCirculos, const Imagen &mascaraImagenesACalcular, const vector<Imagen> &imagenesACalcular, const vector<int> &imagenesAUsar)
	:_mascaraCirculo(mascaraCirculo), _mascaraImagenesACalcular(mascaraImagenesACalcular), _matrizLucesAUsar(MatrizDouble(0, 0, 0.0)) {
	_imagenesCirculos = imagenesCirculos;
	//_imagenesACalcular = imagenesACalcular;
	_imagenesAUsar = imagenesAUsar;

	Rectangulo regionDeImagenesAUsar = mascaraImagenesACalcular.subRectanguloConImagenReal();

	_mascaraImagenesACalcular = mascaraImagenesACalcular.subImagen(regionDeImagenesAUsar);
	_imagenesACalcular = vector<Imagen>();
	for (int i = 0; i < imagenesACalcular.size(); i++) {
		_imagenesACalcular.push_back(imagenesACalcular[i].subImagen(regionDeImagenesAUsar));
	}
}

FotometriaEstereo::~FotometriaEstereo() {

}

void FotometriaEstereo::calibrar() {
	_luces = vector<Vector3D>();
	int n = _imagenesCirculos.size();

	Rectangulo circuloBoundingRect = _mascaraCirculo.subRectanguloConImagenReal();
	double radio = circuloBoundingRect.ancho() / 2;

	Circulo circ = Circulo(0, 0, radio);
	const int offset = 3;
	Rectangulo regionMasBrillante = Rectangulo(0, 0, 0, 0);
	pair<int, int> pos = pair<int, int>(0, 0);
	Vector3D luz = Vector3D(0.0, 0.0, 0.0);
	for (int i = 0; i < n; i++) {
		Imagen img = _imagenesCirculos[i].subImagen(circuloBoundingRect);
		regionMasBrillante = img.regionMasBrillante(offset);
		pos = img.posicionPixelMasBrillanteDentroDeRegion(regionMasBrillante);
		luz = circ.normalEnPuntoXY(pos.first - radio, -pos.second + radio);
		_luces.push_back(luz.normalizar());
	}
}

Matriz<Vector3D> FotometriaEstereo::obtenerNormales() {
	int ancho = _mascaraImagenesACalcular.ancho();
	int alto = _mascaraImagenesACalcular.alto();
	Matriz<Vector3D> normales = Matriz<Vector3D>(alto, ancho, Vector3D(0.0, 0.0, 0.0));
	SistemaDeEcuaciones se = SistemaDeEcuaciones();
	MatrizDouble lucesAUsar = obtenerLucesAUsar();
	MatrizDouble intensidades = intensidadesEnPixelXY(0, 0);
	SistemaDeEcuaciones::ResultadosDeEliminacionGausseana<double> res = se.eliminacionGausseana(lucesAUsar, intensidades);
	//lucesAUsar * iRoN = intencidades
	MatrizDouble iRoN = MatrizDouble(lucesAUsar.ancho(), 1, 0.0);
	double normaAux = 0.0;
	for (int y = 0; y < alto; y++) {
		for (int x = 0; x < ancho; x++) {
			iRoN = obtenerIRoNEnPixelXYAPartirDeSistemaPLU(x, y, res.matrizDeParticion, res.L, res.U);
			normaAux = iRoN.norma();
			normales[y][x] = Vector3D(iRoN[0][0] / normaAux, iRoN[1][0] / normaAux, iRoN[2][0] / normaAux);
		}
	}

	return normales;
}

MatrizDouble FotometriaEstereo::obtenerLucesAUsar() const {
	vector<Vector3D> filas = vector<Vector3D>();
	int n = _imagenesAUsar.size();
	for (int i = 0; i < n; i++) {
		filas.push_back(_luces[_imagenesAUsar[i]]);
	}

	return MatrizDouble(filas);
}

MatrizDouble FotometriaEstereo::obtenerIRoNEnPixelXYAPartirDeSistemaPLU(int x, int y, const MatrizDouble &P, const MatrizDouble &L, const MatrizDouble &U) {
	MatrizDouble intensidades = intensidadesEnPixelXY(x, y);
	SistemaDeEcuaciones se = SistemaDeEcuaciones();

	return se.resolverAPartirDePLU(intensidades, P, L, U);
}

MatrizDouble FotometriaEstereo::intensidadesEnPixelXY(int x, int y) {
	int n = _imagenesAUsar.size();

	MatrizDouble intensidades = MatrizDouble(n, 1, 0.0);

	for (int i = 0; i < n; i++) {
		intensidades[i][0] = _imagenesACalcular[i].brilloDelPixel(x, y);
	}

	return intensidades;
}