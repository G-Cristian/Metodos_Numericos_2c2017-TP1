#ifndef _FOTOMETRIAESTEREO_H_
#define _FOTOMETRIAESTEREO_H_

#include "AbstractMatrix.h"
#include "imagen.h"
#include "Matrix.h"
#include "MatrizEsparsa.h"
#include <memory>
#include "vector.h"
#include <vector>

namespace MN {
	class FotometriaEstereo {
	public:
		struct Vertice {
			double x;
			double y;
			double z;
			Vector3D normal;
		};

		FotometriaEstereo(const Imagen &mascaraCirculo, const std::vector<Imagen> &imagenesCirculos, const Imagen &mascaraImagenesACalcular, const std::vector<Imagen> &imagenesACalcular, const std::vector<int> &imagenesAUsar);
		~FotometriaEstereo();

		inline std::vector<Vector3D> luces() const { return _luces; }

		std::vector<FotometriaEstereo::Vertice> resolverNormalesYProfundidades();
	private:
		void calibrar();
		std::vector<std::vector<Vector3D> > obtenerNormales();
		Matrix<double> obtenerLucesAUsar() const;
		//Obtiene M = I_0 * Ro * N en pixel x,y
		Matrix<double> obtenerIRoNEnPixelXYAPartirDeSistemaPLU(int x, int y, const Matrix<double> &P, const Matrix<double> &L, const Matrix<double> &U);
		Matrix<double> intensidadesEnPixelXY(int x, int y);
		//'mascara' representa una imagen mascara (blanco donde esta el objeto de la imagen y negro en el resto)
		bool pixelEstaEnImagen(int x, int y, const Imagen &mascara) const;
		void obtenerTablaIndicesNumeroDePixelYCantidad();
		void obtenerMV(const std::vector<std::vector<Vector3D> > &normales);
		Matrix<double> obtenerProfundidades(const std::vector<std::vector<Vector3D> > &normales);
		vector<Vertice> obtenerVertices(const std::vector<std::vector<Vector3D> > &normales, const Matrix<double> &profundidades) const;

		Imagen _mascaraCirculo;
		vector<Imagen> _imagenesCirculos;
		Imagen _mascaraImagenesACalcular;
		vector<Imagen> _imagenesACalcular;
		vector<int> _imagenesAUsar;
		Matrix<double> _matrizLucesAUsar;
		vector<Vector3D> _luces;
		Matrix<int> _tablaIndicesNumeroDePixel;
		int _cantidadDePixelsDeImagen;
		std::unique_ptr<AbstractMatrix<double> > _M;
		std::unique_ptr<AbstractMatrix<double> >_V;
	};
}
#endif // !_FOTOMETRIAESTEREO_H_
