#define _CRT_SECURE_NO_WARNINGS

#include "../Include/manejadorImagenes.h"
#include "../Include/ppmloader.h"

#include <iostream>
#include <memory>

using namespace std;

ManejadorImagenes::ManejadorImagenes() {

}

ManejadorImagenes::~ManejadorImagenes() {

}

Imagen ManejadorImagenes::abrirImagenPPM(string nombreArchivo) const {
	uchar* data = NULL;
	int width = 0, height = 0;
	PPM_LOADER_PIXEL_TYPE pt = PPM_LOADER_PIXEL_TYPE_INVALID;

	bool ret = LoadPPMFile(&data, &width, &height, &pt, nombreArchivo.c_str());
	if (!ret || width == 0 || height == 0 || pt != PPM_LOADER_PIXEL_TYPE_RGB_8B)
	{
		throw std::runtime_error("test_load failed");
	}

	Imagen img = Imagen(height, width, data, 3);

	delete[] data;

	return img;
}

void ManejadorImagenes::guardarImagenPPM(const Imagen &imagen, string nombreArchivo) const {
	char comments[100];
	sprintf(comments, "%s", "Hello world");

	int width = imagen.ancho(), height = imagen.alto();
	unique_ptr<uchar> data = imagen.datos().convertToPointer();

	bool ret = SavePPMFile(nombreArchivo.c_str(), data.get(), width, height, PPM_LOADER_PIXEL_TYPE_RGB_8B, comments);
	if (!ret)
	{
		std::cout << "ERROR: couldn't save Image to ppm file" << std::endl;
	}
}