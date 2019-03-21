#include "../Include/manejadorImagenes.h"
#include "../Include/imagen.h"
#include "../Include/vector.h"
#include "../Include/fotometriaEstereo.h"

#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <Windows.h>

using namespace std;

void escribirResultados(const vector<MN::FotometriaEstereo::Vertice> &vertices, string nombreImagenesProcesadas, int luz1, int luz2, int luz3);
void escribirFormaDeEjecucion();

int main(int argc, char *argv[]) {
	if(argc < 6){
		escribirFormaDeEjecucion();
		return 1;
	}
	
	string carpetaEsferaMate = argv[1];
	string carpetaImagenesAProcesar = argv[2];
	
	vector<int> imagenesAUsar = vector<int>(3);
	imagenesAUsar[0] = atoi(argv[3]);
	imagenesAUsar[1] = atoi(argv[4]);
	imagenesAUsar[2] = atoi(argv[5]);
	
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	string path = string(buffer).substr(0, pos);

	ManejadorImagenes mi = ManejadorImagenes();
	Imagen circuloMascara = mi.abrirImagenPPM(path + "/" + carpetaEsferaMate + "/" + carpetaEsferaMate + ".mask.ppm");

	vector<Imagen> circulos = vector<Imagen>();
	stringstream ss;
	for (int i = 0; i < 12; i++) {
		ss.str("");
		ss.clear();

		ss << path + "/" + carpetaEsferaMate << "/" << carpetaEsferaMate << "." << i << ".ppm";

		circulos.push_back(mi.abrirImagenPPM(ss.str()));
	}

	Imagen imagenMascara = mi.abrirImagenPPM(path + "/" + carpetaImagenesAProcesar + "/" + carpetaImagenesAProcesar + ".mask.ppm");

	vector<Imagen> imagenes = vector<Imagen>();
	for (int i = 0; i < 12; i++) {
		ss.str("");
		ss.clear();
		ss << path + "/" + carpetaImagenesAProcesar << "/" << carpetaImagenesAProcesar << "." << i << ".ppm";

		imagenes.push_back(mi.abrirImagenPPM(ss.str()));
	}

	MN::FotometriaEstereo fe(circuloMascara, circulos, imagenMascara, imagenes, imagenesAUsar);
	vector<MN::FotometriaEstereo::Vertice> vertices = fe.resolverNormalesYProfundidades();

	escribirResultados(vertices, carpetaImagenesAProcesar, imagenesAUsar[0], imagenesAUsar[1], imagenesAUsar[2]);

	return 0;
}

void escribirResultados(const vector<MN::FotometriaEstereo::Vertice> &vertices, string nombreImagenesProcesadas, int luz1, int luz2, int luz3) {
	cout << "Imagen: " << nombreImagenesProcesadas << endl;
	cout << "Luz 1: " << luz1 << " Luz 2: " << luz2 << " Luz 3: " << luz3 <<  endl;
	int n = vertices.size();
	cout << "Cantidad de vectices" << endl;
	cout << n << endl;

	cout << "Posicion x, y, z de cada vertice" << endl;
	for (vector<MN::FotometriaEstereo::Vertice>::const_iterator it = vertices.begin(); it != vertices.end(); it++) {
		cout << it->x << " " << it->y << " " << it->z << endl;
	}
	cout << endl;
	cout << "x, y, z de vector normal de cada vertice" << endl;
	for (vector<MN::FotometriaEstereo::Vertice>::const_iterator it = vertices.begin(); it != vertices.end(); it++) {
		cout << it->normal[0] << " " << it->normal[1] << " " << it->normal[2] << endl;
	}
}

void escribirFormaDeEjecucion(){
	cout << "Para ejecutar hacer: " << endl;
	cout << "./TP1 nombre_carpeta_de_esfera_mate nombre_carpeta_de_imagen_a_procesar indice_luz_1 indice_luz_2 indice_luz_3" << endl;
	cout << "Donde: " << endl;
	cout << "nombre_carpeta_de_esfera_mate es el nombre de la carpeta con las imagenes de la esfera mate para calibrar." << endl;
	cout << "indice_luz_1 es un numero entre 0 y 11 que indica la primera luz a usar." << endl;
	cout << "indice_luz_2 es un numero entre 0 y 11 que indica la segunda luz a usar." << endl;
	cout << "indice_luz_3 es un numero entre 0 y 11 que indica la tercera luz a usar." << endl;
}
