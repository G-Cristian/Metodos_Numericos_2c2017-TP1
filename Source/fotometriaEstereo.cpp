#include "../Include/fotometriaEstereo.h"
#include "../Include/rectangulo.h"
#include "../Include/circulo.h"
#include "../Include/sistemaDeEcuaciones.h"

FotometriaEstereo::FotometriaEstereo(const Imagen &mascaraCirculo, const vector<Imagen> &imagenesCirculos, const Imagen &mascaraImagenesACalcular, const vector<Imagen> &imagenesACalcular, const vector<int> &imagenesAUsar)
	:_mascaraCirculo(mascaraCirculo), _mascaraImagenesACalcular(mascaraImagenesACalcular), _matrizLucesAUsar(MatrizDouble(0, 0, 0.0)), _M(MatrizDouble(1,1,0.0)), _V(MatrizDouble(1,1,0.0)),
	_tablaIndicesNumeroDePixel(MatrizInt(1,1,(int)-1)){
	_imagenesCirculos = imagenesCirculos;
	//_imagenesACalcular = imagenesACalcular;
	_imagenesAUsar = imagenesAUsar;

	Rectangulo regionDeImagenesAUsar = mascaraImagenesACalcular.subRectanguloConImagenReal();

	_mascaraImagenesACalcular = mascaraImagenesACalcular.subImagen(regionDeImagenesAUsar);
	_imagenesACalcular = vector<Imagen>();
	for (int i = 0; i < imagenesACalcular.size(); i++) {
		_imagenesACalcular.push_back(imagenesACalcular[i].subImagen(regionDeImagenesAUsar));
	}

	obtenerTablaIndicesNumeroDePixelYCantidad();
}

FotometriaEstereo::~FotometriaEstereo() {

}

pair<Matriz<Vector3D>, MatrizDouble> FotometriaEstereo::resolverNormalesYProfundidades() {
	Matriz<Vector3D> normales = obtenerNormales();
	MatrizDouble profundidades = obtenerProfundidades(normales);

	return pair<Matriz<Vector3D>, MatrizDouble>(normales, profundidades);
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

bool FotometriaEstereo::pixelEstaEnImagen(int x, int y, const Imagen &mascara) const {
	if (x < 0 || x >= mascara.ancho() || y < 0 || y >= mascara.alto())
		return false;

	Vector3D pixel = mascara.pixelEnXY(x, y);

	return (pixel.x() + pixel.y() + pixel.z()) == (255 * mascara.canales());
}

void FotometriaEstereo::obtenerTablaIndicesNumeroDePixelYCantidad() {
	int alto = _mascaraImagenesACalcular.alto();
	int ancho = _mascaraImagenesACalcular.ancho();
	MatrizInt _tablaIndicesNumeroDePixel = MatrizInt(alto, ancho, (int)-1);
	_cantidadDePixelsDeImagen = 0;

	for (int i = 0; i < alto; i++) {
		for (int j = 0; j < ancho; j++) {
			if (pixelEstaEnImagen(j, i, _mascaraImagenesACalcular)) {
				_tablaIndicesNumeroDePixel[i][j] = _cantidadDePixelsDeImagen;
				_cantidadDePixelsDeImagen++;
			}
		}
	}
}

void FotometriaEstereo::obtenerMV(const Matriz<Vector3D> &normales) {
	_M = MatrizDouble(_cantidadDePixelsDeImagen * 2, _cantidadDePixelsDeImagen, 0.0);
	_V = MatrizDouble(_cantidadDePixelsDeImagen * 2, 1, 0.0);
	int alto = _mascaraImagenesACalcular.alto();
	int ancho = _mascaraImagenesACalcular.ancho();
	int indexActual = 0;
	for (int i = 0; i < alto; i++) {
		for (int j = 0; j < ancho; j++) {
			double nx = normales[i][j].x();
			double ny = normales[i][j].y();
			double nz = normales[i][j].z();
			if (_tablaIndicesNumeroDePixel[i][j+1] >= 0 && _tablaIndicesNumeroDePixel[i+1][j] >= 0) {
				//(X+1,Y) (X,Y+1) estan en la imagen
				_M[2 * indexActual][_tablaIndicesNumeroDePixel[i][j]] = 1;
				_M[2 * indexActual][_tablaIndicesNumeroDePixel[i][j+1]] = -1;
				_V[2 * indexActual][0] = nx/nz;

				_M[2 * indexActual + 1][_tablaIndicesNumeroDePixel[i][j]] = 1;
				_M[2 * indexActual + 1][_tablaIndicesNumeroDePixel[i+1][j]] = -1;
				_V[2 * indexActual + 1][0] = ny / nz;
			}
			else if (_tablaIndicesNumeroDePixel[i + 1][j] >= 0) {
				if (_tablaIndicesNumeroDePixel[i][j - 1] >= 0) {
					_M[2 * indexActual][_tablaIndicesNumeroDePixel[i][j]] = 1;
					_M[2 * indexActual][_tablaIndicesNumeroDePixel[i][j - 1]] = -1;
					_V[2 * indexActual][0] = -nx / nz;
				}
				_M[2 * indexActual + 1][_tablaIndicesNumeroDePixel[i][j]] = 1;
				_M[2 * indexActual + 1][_tablaIndicesNumeroDePixel[i + 1][j]] = -1;
				_V[2 * indexActual + 1][0] = ny / nz;
			}
			else if (_tablaIndicesNumeroDePixel[i][j + 1] >= 0) {
				if (_tablaIndicesNumeroDePixel[i - 1][j] >= 0) {
					_M[2 * indexActual + 1][_tablaIndicesNumeroDePixel[i][j]] = 1;
					_M[2 * indexActual + 1][_tablaIndicesNumeroDePixel[i - 1][j]] = -1;
					_V[2 * indexActual + 1][0] = -ny / nz;
				}

				_M[2 * indexActual][_tablaIndicesNumeroDePixel[i][j]] = 1;
				_M[2 * indexActual][_tablaIndicesNumeroDePixel[i][j + 1]] = -1;
				_V[2 * indexActual][0] = nx / nz;
			}
			else {
				if (_tablaIndicesNumeroDePixel[i][j - 1] >= 0) {
					_M[2 * indexActual][_tablaIndicesNumeroDePixel[i][j]] = 1;
					_M[2 * indexActual][_tablaIndicesNumeroDePixel[i][j - 1]] = -1;
					_V[2 * indexActual][0] = -nx / nz;
				}

				if (_tablaIndicesNumeroDePixel[i - 1][j] >= 0) {
					_M[2 * indexActual + 1][_tablaIndicesNumeroDePixel[i][j]] = 1;
					_M[2 * indexActual + 1][_tablaIndicesNumeroDePixel[i - 1][j]] = -1;
					_V[2 * indexActual + 1][0] = -ny / nz;
				}
			}
		}
	}
}

MatrizDouble FotometriaEstereo::obtenerProfundidades(const Matriz<Vector3D> &normales) {
	obtenerMV(normales);
	MatrizDouble Mt = _M.transpuesta();
	MatrizDouble MMt = _M*Mt;
	MatrizDouble MtV = Mt*_V;
	SistemaDeEcuaciones se = SistemaDeEcuaciones();
	SistemaDeEcuaciones::ResultadosDeEliminacionGausseana<double> res = se.eliminacionGausseana(MMt,MtV);
	MatrizDouble aux = se.resolverAPartirDePLU(MtV, res.matrizDeParticion, res.L, res.U);

	int alto = _mascaraImagenesACalcular.alto();
	int ancho = _mascaraImagenesACalcular.ancho();
	MatrizDouble z = MatrizDouble(alto, ancho, -1.0);
	for (int i = 0; i < alto; i++) {
		for (int j = 0; j < ancho; j++) {
			if (_tablaIndicesNumeroDePixel[i][j] >= 0) {
				//(X,Y) está en la imagen
				z[i][j] = aux[_tablaIndicesNumeroDePixel[i][j]][0];
			}
		}
	}

	return z;
}