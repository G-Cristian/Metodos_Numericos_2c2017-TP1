#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "../Include/circulo.h"
#include "../Include/rectangulo.h"
#include "../Include/imagen.h"
#include "../ppmloader/ppmloader/ppmloader.h"


using namespace std;

Imagen cargarImagen(string fileName);
void grabarImagen(const Imagen &img, string fileName);

int main() {
	Imagen circuloMascara = cargarImagen("mate.mask.ppm");
	vector<Imagen> circulos = vector<Imagen>();
	stringstream ss;
	for (int i = 0; i < 12; i++) {
		ss.str("");
		ss.clear();
		ss << "mate." << i << ".ppm";
		Imagen circ = cargarImagen(ss.str());

		circulos.push_back(circ);
	}
	grabarImagen(circuloMascara, "out.ppm");
	
	Rectangulo circuloBoundingRect = circuloMascara.subRectanguloConImagenReal();

	double radio = circuloBoundingRect.ancho() / 2;
	Circulo circ = Circulo(0, 0, radio);
	
	vector<Vector3D> luces = vector<Vector3D>();
	for (int i = 0; i < 12; i++) {
		pair<int, int> pos = circulos[i].subImagen(circuloBoundingRect).posicionPixelMasBrillante();
		Vector3D l = circ.normalEnPuntoXY(pos.first - radio, -pos.second + radio);
		luces.push_back(Vector3D(-l.x(), -l.y(), -l.z()).normalizar());
	}
	for (int i = 0; i < 12; i++) {
		cout << luces[i].x() << " " << luces[i].y() << " " << luces[i].z() << endl;
	}
	/*
	grabarImagen(circuloMascara.subImagen(circuloBoundingRect), "a.ppm");
	for (int i = 0; i < 12; i++) {
		stringstream ss;
		ss.str("");
		ss.clear();
		ss << "a." << i << ".ppm";
		grabarImagen(circulos[i].subImagen(circuloBoundingRect), ss.str());
	}
	*/
	
	return 0;
}


Imagen cargarImagen(string fileName) {
	uchar* data = NULL;
	int width = 0, height = 0;
	PPM_LOADER_PIXEL_TYPE pt = PPM_LOADER_PIXEL_TYPE_INVALID;

	bool ret = LoadPPMFile(&data, &width, &height, &pt, fileName.c_str());
	if (!ret || width == 0 || height == 0 || pt != PPM_LOADER_PIXEL_TYPE_RGB_8B)
	{
		throw std::runtime_error("test_load failed");
	}

	Imagen img = Imagen(height, width * 3, data, 3);

	delete[] data;

	return img;
}

void grabarImagen(const Imagen &img, string fileName) {
	char comments[100];
	sprintf(comments, "%s", "Hello world");

	int width = img.ancho(), height = img.alto();
	uchar* data = img.datos().comoPuntero();

	bool ret = SavePPMFile(fileName.c_str(), data, width, height, PPM_LOADER_PIXEL_TYPE_RGB_8B, comments);
	delete[] data;
	if (!ret)
	{
		std::cout << "ERROR: couldn't save Image to ppm file" << std::endl;
	}
}