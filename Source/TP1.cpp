#include "../Include/manejadorImagenes.h"
#include "../Include/imagen.h"
#include "../Include/vector.h"
#include "../Include/fotometriaEstereo.h"

#include <utility>
#include <vector>
#include <sstream>

using namespace std;

void escribirResultados(const vector<FotometriaEstereo::Vertice> &vertices);

int main() {
	ManejadorImagenes mi = ManejadorImagenes();
	Imagen circuloMascara = mi.abrirImagenPPM("mate.mask.ppm");

	vector<Imagen> circulos = vector<Imagen>();
	stringstream ss;
	for (int i = 0; i < 12; i++) {
		ss.str("");
		ss.clear();
		ss << "mate." << i << ".ppm";
		Imagen circ = mi.abrirImagenPPM(ss.str());

		circulos.push_back(circ);
	}

	Imagen imagenMascara = mi.abrirImagenPPM("buda.mask.ppm");

	vector<Imagen> imagenes = vector<Imagen>();
	for (int i = 0; i < 12; i++) {
		ss.str("");
		ss.clear();
		ss << "buda." << i << ".ppm";
		Imagen circ = mi.abrirImagenPPM(ss.str());

		imagenes.push_back(circ);
	}

	vector<int> imagenesAUsar = vector<int>(3);
	imagenesAUsar[0] = 0;
	imagenesAUsar[1] = 2;
	imagenesAUsar[2] = 5;

	FotometriaEstereo fe = FotometriaEstereo(circuloMascara, circulos, imagenMascara, imagenes, imagenesAUsar);
	vector<FotometriaEstereo::Vertice> vertices = fe.resolverNormalesYProfundidades();

	escribirResultados(vertices);

	return 0;
}

void escribirResultados(const vector<FotometriaEstereo::Vertice> &vertices) {
	int n = vertices.size();

	cout << n << endl;

	for (vector<FotometriaEstereo::Vertice>::const_iterator it = vertices.begin(); it != vertices.end(); it++) {
		cout << it->x << " " << it->y << " " << it->z << endl;
		cout << it->normal.x() << " " << it->normal.y() << " " << it->normal.z() << endl;
	}
}