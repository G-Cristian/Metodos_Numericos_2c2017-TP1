#include "../Include/fotometriaEstereo.h"
#include "../Include/rectangulo.h"
#include "../Include/circulo.h"
#include "../Include/sistemaDeEcuaciones.h"

FotometriaEstereo::FotometriaEstereo(const Imagen &mascaraCirculo, const vector<Imagen> &imagenesCirculos, const Imagen &mascaraImagenesACalcular, const vector<Imagen> &imagenesACalcular, const vector<int> &imagenesAUsar)
	:_mascaraCirculo(mascaraCirculo), _mascaraImagenesACalcular(mascaraImagenesACalcular), _matrizLucesAUsar(MatrizDouble(0, 0, 0.0)), _M(MatrizEsparsa(1,1)), _V(MatrizEsparsa(1,1)),
	_tablaIndicesNumeroDePixel(MatrizInt(1,1,(int)-1)), _cantidadDePixelsDeImagen(0){
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

vector<FotometriaEstereo::Vertice> FotometriaEstereo::resolverNormalesYProfundidades() {
	calibrar();
	Matriz<Vector3D> normales = obtenerNormales();
	MatrizDouble profundidades = obtenerProfundidades(normales);

	return obtenerVertices(normales, profundidades);
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
	_tablaIndicesNumeroDePixel = MatrizInt(alto, ancho, (int)-1);
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
	_M = MatrizEsparsa(_cantidadDePixelsDeImagen * 2, _cantidadDePixelsDeImagen);
	_V = MatrizEsparsa(_cantidadDePixelsDeImagen * 2, 1);
	int alto = _mascaraImagenesACalcular.alto();
	int ancho = _mascaraImagenesACalcular.ancho();
	int indexActual = 0;
	for (int i = 0; i < alto; i++) {
		for (int j = 0; j < ancho; j++) {
			if (_tablaIndicesNumeroDePixel[i][j] >= 0) {
				double nx = normales[i][j].x();
				double ny = normales[i][j].y();
				double nz = normales[i][j].z();
				if (j + 1 < ancho && i + 1 < alto && _tablaIndicesNumeroDePixel[i][j + 1] >= 0 && _tablaIndicesNumeroDePixel[i + 1][j] >= 0) {
					//(X+1,Y) (X,Y+1) estan en la imagen
					_M.insertarEnYX(2 * indexActual, _tablaIndicesNumeroDePixel[i][j], 1);
					_M.insertarEnYX(2 * indexActual, _tablaIndicesNumeroDePixel[i][j + 1], -1);
					_V.insertarEnYX(2 * indexActual, 0, nx / nz);

					_M.insertarEnYX(2 * indexActual + 1, _tablaIndicesNumeroDePixel[i][j], 1);
					_M.insertarEnYX(2 * indexActual + 1, _tablaIndicesNumeroDePixel[i + 1][j], -1);
					_V.insertarEnYX(2 * indexActual + 1, 0, ny / nz);

					indexActual++;
				}
				else if (i + 1 < alto && _tablaIndicesNumeroDePixel[i + 1][j] >= 0) {
					if (j - 1 >= 0 && _tablaIndicesNumeroDePixel[i][j - 1] >= 0) {
						_M.insertarEnYX(2 * indexActual, _tablaIndicesNumeroDePixel[i][j], 1);
						_M.insertarEnYX(2 * indexActual, _tablaIndicesNumeroDePixel[i][j - 1], -1);
						_V.insertarEnYX(2 * indexActual, 0, -nx / nz);
					}
					_M.insertarEnYX(2 * indexActual + 1, _tablaIndicesNumeroDePixel[i][j], 1);
					_M.insertarEnYX(2 * indexActual + 1, _tablaIndicesNumeroDePixel[i + 1][j], -1);
					_V.insertarEnYX(2 * indexActual + 1, 0, ny / nz);

					indexActual++;
				}
				else if (j + 1 < ancho && _tablaIndicesNumeroDePixel[i][j + 1] >= 0) {
					if (i - 1 >= 0 && _tablaIndicesNumeroDePixel[i - 1][j] >= 0) {
						_M.insertarEnYX(2 * indexActual + 1, _tablaIndicesNumeroDePixel[i][j], 1);
						_M.insertarEnYX(2 * indexActual + 1, _tablaIndicesNumeroDePixel[i - 1][j], -1);
						_V.insertarEnYX(2 * indexActual + 1, 0, -ny / nz);
					}

					_M.insertarEnYX(2 * indexActual, _tablaIndicesNumeroDePixel[i][j], 1);
					_M.insertarEnYX(2 * indexActual, _tablaIndicesNumeroDePixel[i][j + 1], -1);
					_V.insertarEnYX(2 * indexActual, 0, nx / nz);

					indexActual++;
				}
				else {
					if (j - 1 >= 0 && _tablaIndicesNumeroDePixel[i][j - 1] >= 0) {
						_M.insertarEnYX(2 * indexActual, _tablaIndicesNumeroDePixel[i][j], 1);
						_M.insertarEnYX(2 * indexActual, _tablaIndicesNumeroDePixel[i][j - 1], -1);
						_V.insertarEnYX(2 * indexActual, 0, -nx / nz);
					}

					if (i - 1 >= 0 && _tablaIndicesNumeroDePixel[i - 1][j] >= 0) {
						_M.insertarEnYX(2 * indexActual + 1, _tablaIndicesNumeroDePixel[i][j], 1);
						_M.insertarEnYX(2 * indexActual + 1, _tablaIndicesNumeroDePixel[i - 1][j], -1);
						_V.insertarEnYX(2 * indexActual + 1, 0, -ny / nz);
					}

					indexActual++;
				}
			}
		}
	}
}

MatrizDouble FotometriaEstereo::obtenerProfundidades(const Matriz<Vector3D> &normales) {
	obtenerMV(normales);
	//MatrizEsparsa Mt = _M.transpuesta();
	//MatrizDouble MtM = MatrizDouble(Mt*_M);
	//MatrizDouble MtV = MatrizDouble(Mt*_V);

	MatrizDouble MtM = MatrizDouble(_M.transpuestaPorOtra(_M));
	MatrizDouble MtV = MatrizDouble(_M.transpuestaPorOtra(_V));
	SistemaDeEcuaciones se = SistemaDeEcuaciones();
	MatrizDouble L = se.cholesky(MtM);
	MatrizDouble aux = se.resolverAPartirDeLLt(MtV, L);


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

vector<FotometriaEstereo::Vertice> FotometriaEstereo::obtenerVertices(const Matriz<Vector3D> &normales, const MatrizDouble &profundidades) const {
	vector<FotometriaEstereo::Vertice> vertices = vector<FotometriaEstereo::Vertice>();
	vertices.reserve(_cantidadDePixelsDeImagen);

	int alto = _mascaraImagenesACalcular.alto();
	int ancho = _mascaraImagenesACalcular.ancho();

	for (int i = 0; i < alto; i++) {
		for (int j = 0; j < ancho; j++) {
			if (pixelEstaEnImagen(j, i, _mascaraImagenesACalcular)) {
				FotometriaEstereo::Vertice vertice = { j, 
													   i,
													   profundidades[i][j],
													   normales[i][j]
													 };
				vertices.push_back(vertice);
			}
		}
	}

	return vertices;
}