#include "../Include/AbstractMatrix.h"
#include "../Include/Matrix.h"
#include "../Include/vector.h"
#include "../Include/VectorVecrorImp.h"
#include "../Include/circulo.h"
#include "../Include/fotometriaEstereo.h"
#include "../Include/rectangulo.h"
#include "../Include/sistemaDeEcuaciones.h"
#include <memory>
#include <vector>

namespace MN {
	FotometriaEstereo::FotometriaEstereo(const Imagen &mascaraCirculo, const std::vector<Imagen> &imagenesCirculos, const Imagen &mascaraImagenesACalcular, const std::vector<Imagen> &imagenesACalcular, const std::vector<int> &imagenesAUsar)
		:_mascaraCirculo(mascaraCirculo), _mascaraImagenesACalcular(mascaraImagenesACalcular), _matrizLucesAUsar(Matrix<double>(0, 0, 0.0)), _M(new SparceMatrix<double>(1, 1)), _V(new SparceMatrix<double>(1, 1)),
		_tablaIndicesNumeroDePixel(Matrix<int>(1, 1, (int)-1)), _cantidadDePixelsDeImagen(0) {
		_imagenesCirculos = imagenesCirculos;
		//_imagenesACalcular = imagenesACalcular;
		_imagenesAUsar = imagenesAUsar;

		Rectangulo regionDeImagenesAUsar = mascaraImagenesACalcular.subRectanguloConImagenReal();

		_mascaraImagenesACalcular = std::move(mascaraImagenesACalcular.subImagen(regionDeImagenesAUsar));
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
		std::vector<std::vector<Vector3D> > normales = obtenerNormales();
		Matrix<double> profundidades(obtenerProfundidades(normales));

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
		Vector<double> luz(new VectorVectorImp<double>({ 0.0, 0.0, 0.0 }));
		for (int i = 0; i < n; i++) {
			Imagen img = _imagenesCirculos[i].subImagen(circuloBoundingRect);
			regionMasBrillante = img.regionMasBrillante(offset);
			pos = img.posicionPixelMasBrillanteDentroDeRegion(regionMasBrillante);
			luz = circ.normalEnPuntoXY(pos.first - radio, -pos.second + radio);
			_luces.push_back(luz.normailize());
		}
	}

	std::vector<std::vector<Vector3D> > FotometriaEstereo::obtenerNormales() {
		int ancho = _mascaraImagenesACalcular.ancho();
		int alto = _mascaraImagenesACalcular.alto();
		std::vector<std::vector<Vector3D> > normales = std::vector<std::vector<Vector3D> >(alto, std::vector<Vector3D>(ancho, Vector3D(new VectorVectorImp<double>({ 0.0, 0.0, 0.0 }))));
		SistemaDeEcuaciones se = SistemaDeEcuaciones();
		Matrix<double> lucesAUsar = obtenerLucesAUsar();
		Matrix<double> intensidades = intensidadesEnPixelXY(0, 0);
		SistemaDeEcuaciones::ResultadosDeEliminacionGausseana<double> res = se.eliminacionGausseana<double>(lucesAUsar, intensidades);
		//lucesAUsar * iRoN = intencidades
		Matrix<double> iRoN = Matrix<double>(lucesAUsar.width(), 1, 0.0);
		double normaAux = 0.0;
		for (int y = 0; y < alto; y++) {
			for (int x = 0; x < ancho; x++) {
				iRoN = obtenerIRoNEnPixelXYAPartirDeSistemaPLU(x, y, res.matrizDeParticion, res.L, res.U);
				normaAux = iRoN.norm();
				normales[y][x]=(Vector<double>(new VectorVectorImp<double>({ iRoN[0][0] / normaAux, iRoN[1][0] / normaAux, iRoN[2][0] / normaAux })));
			}
		}

		return normales;
	}

	Matrix<double> FotometriaEstereo::obtenerLucesAUsar() const {
		vector<Vector3D> filas = vector<Vector3D>();
		int n = _imagenesAUsar.size();
		for (int i = 0; i < n; i++) {
			filas.push_back(_luces[_imagenesAUsar[i]]);
		}

		return Matrix<double>(filas);
	}

	Matrix<double> FotometriaEstereo::obtenerIRoNEnPixelXYAPartirDeSistemaPLU(int x, int y, const Matrix<double> &P, const Matrix<double> &L, const Matrix<double> &U) {
		Matrix<double> intensidades = intensidadesEnPixelXY(x, y);
		SistemaDeEcuaciones se = SistemaDeEcuaciones();

		return se.resolverAPartirDePLU(intensidades, P, L, U);
	}

	Matrix<double> FotometriaEstereo::intensidadesEnPixelXY(int x, int y) {
		int n = _imagenesAUsar.size();

		Matrix<double> intensidades = Matrix<double>(n, 1, 0.0);

		for (int i = 0; i < n; i++) {
			intensidades.setValueInRowColumn(_imagenesACalcular[i].brilloDelPixel(x, y), i, 0);
		}

		return intensidades;
	}

	bool FotometriaEstereo::pixelEstaEnImagen(int x, int y, const Imagen &mascara) const {
		if (x < 0 || x >= mascara.ancho() || y < 0 || y >= mascara.alto())
			return false;

		Vector<unsigned char> pixel(mascara.pixelEnXY(x, y));

		return (pixel[0] + pixel[1] + pixel[2]) == (255 * mascara.canales());
	}

	void FotometriaEstereo::obtenerTablaIndicesNumeroDePixelYCantidad() {
		int alto = _mascaraImagenesACalcular.alto();
		int ancho = _mascaraImagenesACalcular.ancho();
		_tablaIndicesNumeroDePixel = Matrix<int>(alto, ancho, (int)-1);
		_cantidadDePixelsDeImagen = 0;

		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < ancho; j++) {
				if (pixelEstaEnImagen(j, i, _mascaraImagenesACalcular)) {
					_tablaIndicesNumeroDePixel.setValueInRowColumn(_cantidadDePixelsDeImagen, i, j);
					_cantidadDePixelsDeImagen++;
				}
			}
		}
	}

	void FotometriaEstereo::obtenerMV(const std::vector<std::vector<Vector3D> > &normales) {
		_M.reset(new SparceMatrix<double>(_cantidadDePixelsDeImagen * 2, _cantidadDePixelsDeImagen));
		_V.reset(new SparceMatrix<double>(_cantidadDePixelsDeImagen * 2, 1));
		int alto = _mascaraImagenesACalcular.alto();
		int ancho = _mascaraImagenesACalcular.ancho();
		int indexActual = 0;
		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < ancho; j++) {
				if (_tablaIndicesNumeroDePixel[i][j] >= 0) {
					double nx(normales[i][j][0]);
					double ny(normales[i][j][1]);
					double nz(normales[i][j][2]);
					if (j + 1 < ancho && i + 1 < alto && _tablaIndicesNumeroDePixel[i][j + 1] >= 0 && _tablaIndicesNumeroDePixel[i + 1][j] >= 0) {
						//(X+1,Y) (X,Y+1) estan en la imagen
						_M->setValueInRowColumn(1, 2 * indexActual, _tablaIndicesNumeroDePixel[i][j]);
						_M->setValueInRowColumn(-1, 2 * indexActual, _tablaIndicesNumeroDePixel[i][j + 1]);
						_V->setValueInRowColumn(nx / nz, 2 * indexActual, 0);

						_M->setValueInRowColumn(1, 2 * indexActual + 1, _tablaIndicesNumeroDePixel[i][j]);
						_M->setValueInRowColumn(-1, 2 * indexActual + 1, _tablaIndicesNumeroDePixel[i + 1][j]);
						_V->setValueInRowColumn(ny / nz, 2 * indexActual + 1, 0);

						indexActual++;
					}
					else if (i + 1 < alto && _tablaIndicesNumeroDePixel[i + 1][j] >= 0) {
						if (j - 1 >= 0 && _tablaIndicesNumeroDePixel[i][j - 1] >= 0) {
							_M->setValueInRowColumn(1, 2 * indexActual, _tablaIndicesNumeroDePixel[i][j]);
							_M->setValueInRowColumn(-1, 2 * indexActual, _tablaIndicesNumeroDePixel[i][j - 1]);
							_V->setValueInRowColumn(-nx / nz, 2 * indexActual, 0);
						}
						_M->setValueInRowColumn(1, 2 * indexActual + 1, _tablaIndicesNumeroDePixel[i][j]);
						_M->setValueInRowColumn(-1, 2 * indexActual + 1, _tablaIndicesNumeroDePixel[i + 1][j]);
						_V->setValueInRowColumn(ny / nz, 2 * indexActual + 1, 0);

						indexActual++;
					}
					else if (j + 1 < ancho && _tablaIndicesNumeroDePixel[i][j + 1] >= 0) {
						if (i - 1 >= 0 && _tablaIndicesNumeroDePixel[i - 1][j] >= 0) {
							_M->setValueInRowColumn(1, 2 * indexActual + 1, _tablaIndicesNumeroDePixel[i][j]);
							_M->setValueInRowColumn(-1, 2 * indexActual + 1, _tablaIndicesNumeroDePixel[i - 1][j]);
							_V->setValueInRowColumn(-ny / nz, 2 * indexActual + 1, 0);
						}

						_M->setValueInRowColumn(1, 2 * indexActual, _tablaIndicesNumeroDePixel[i][j]);
						_M->setValueInRowColumn(-1, 2 * indexActual, _tablaIndicesNumeroDePixel[i][j + 1]);
						_V->setValueInRowColumn(nx / nz, 2 * indexActual, 0);

						indexActual++;
					}
					else {
						if (j - 1 >= 0 && _tablaIndicesNumeroDePixel[i][j - 1] >= 0) {
							_M->setValueInRowColumn(1, 2 * indexActual, _tablaIndicesNumeroDePixel[i][j]);
							_M->setValueInRowColumn(-1, 2 * indexActual, _tablaIndicesNumeroDePixel[i][j - 1]);
							_V->setValueInRowColumn(-nx / nz, 2 * indexActual, 0);
						}

						if (i - 1 >= 0 && _tablaIndicesNumeroDePixel[i - 1][j] >= 0) {
							_M->setValueInRowColumn(1, 2 * indexActual + 1, _tablaIndicesNumeroDePixel[i][j]);
							_M->setValueInRowColumn(-1, 2 * indexActual + 1, _tablaIndicesNumeroDePixel[i - 1][j]);
							_V->setValueInRowColumn(-ny / nz, 2 * indexActual + 1, 0);
						}

						indexActual++;
					}
				}
			}
		}
	}

	Matrix<double> FotometriaEstereo::obtenerProfundidades(const std::vector<std::vector<Vector3D> > &normales) {
		obtenerMV(normales);
		unique_ptr<AbstractMatrix<double> > Mt = _M->transpose();
		unique_ptr<AbstractMatrix<double> > MtM = (*Mt)*(*_M);
		unique_ptr<AbstractMatrix<double> > MtV = (*Mt)*(*_V);

		//Matrix<double> MtM = Matrix<double>(_M.transpuestaPorOtra(_M));
		//Matrix<double> MtV = Matrix<double>(_M.transpuestaPorOtra(_V));
		SistemaDeEcuaciones se = SistemaDeEcuaciones();
		unique_ptr<AbstractMatrix<double> > L = se.cholesky(MtM);
		Matrix<double> aux = se.resolverAPartirDeLLt<double>(*MtV, *L);


		int alto = _mascaraImagenesACalcular.alto();
		int ancho = _mascaraImagenesACalcular.ancho();
		Matrix<double> z = Matrix<double>(alto, ancho, -1.0);
		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < ancho; j++) {
				if (_tablaIndicesNumeroDePixel[i][j] >= 0) {
					//(X,Y) está en la imagen
					z.setValueInRowColumn(aux[_tablaIndicesNumeroDePixel[i][j]][0], i, j);
				}
			}
		}

		return z;
	}

	vector<FotometriaEstereo::Vertice> FotometriaEstereo::obtenerVertices(const std::vector<std::vector<Vector3D> > &normales, const Matrix<double> &profundidades) const {
		vector<FotometriaEstereo::Vertice> vertices = vector<FotometriaEstereo::Vertice>();
		vertices.reserve(_cantidadDePixelsDeImagen);

		int alto = _mascaraImagenesACalcular.alto();
		int ancho = _mascaraImagenesACalcular.ancho();

		for (int i = 0; i < alto; i++) {
			for (int j = 0; j < ancho; j++) {
				if (pixelEstaEnImagen(j, i, _mascaraImagenesACalcular)) {
					FotometriaEstereo::Vertice vertice = { (double)j,
														   (double)i,
														   profundidades[i][j],
														   normales[i][j]
					};
					vertices.push_back(vertice);
				}
			}
		}

		return vertices;
	}
}