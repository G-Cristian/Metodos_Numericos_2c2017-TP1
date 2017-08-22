#include iostream
#include <stdio.h>
#include <stdlib.h>
#include "ppmloader.h"


using namespace std;


void read_image(std::string filename, uchar** data, int* width, int* height){
  *data = NULL;
  *width = 0;
  *height = 0;
  PPM_LOADER_PIXEL_TYPE pt = PPM_LOADER_PIXEL_TYPE_INVALID;

  bool ret = LoadPPMFile(data, width, height, &pt, filename.c_str());
  if (!ret || width == 0|| height == 0|| pt!=PPM_LOADER_PIXEL_TYPE_RGB_8B){
    throw std::exception();//("Fallo al leer la imagen.");
  }
}
//lee una imagen

void write_image(const char *filename, const void *src, int width, int height, PPM_LOADER_PIXEL_TYPE pt, const char* comments){
	bool ret = SavePPMFile(filename, src, width, height, pt, comments);
	if (!ret || width == 0|| height == 0|| pt!=PPM_LOADER_PIXEL_TYPE_RGB_8B){
    	throw std::exception();//("Fallo al leer la imagen.");
  	}
}


void test_image(){
  uchar* data = NULL;
  int width = 0, height = 0;
  std::string filename = "ppmImagenes/buda/buda.0.ppm";
  read_image(filename, &data, &width, &height); // Ejemplo de llamada

  for (int h = 0; h < height; ++h){
    for (int w = 0; w < width; ++w){
      cout << get_pixel_average(data, h, w, height, width) << " "; // Ejemplo de lectura de un pixel
    }
    cout << endl;
  }
  delete [] data;
}

int main(){

uchar* data = NULL;
int width = 0, height = 0;
std:: string filename;
cin >> filename;

read_image(filename, &data, &width; &height);//en data me queda la imagen

for(int h=0; h < height; ++h){
	for(int w = 0; w < width; ++w){
		cout << 
	}
}



for(int h =0; h < height; ++h){
	for (int w =0; w < width; ++w){

	}	
}
	
//recibo la  matriz mate.
//Hallo el punto mas brillante 
//esa es la direccion de la intensidad de la luz
//imprimo la direccion de la luz	
//repito



return 0;
}

/*

void test_image(){
  uchar* data = NULL;
  int width = 0, height = 0;
  std::string filename = "ppmImagenes/buda/buda.0.ppm";
  read_image(filename, &data, &width, &height); // Ejemplo de llamada

  for (int h = 0; h < height; ++h){
    for (int w = 0; w < width; ++w){
      cout << get_pixel_average(data, h, w, height, width) << " "; // Ejemplo de lectura de un pixel
    }
    cout << endl;
  }
  delete [] data;
}

void test_load(){

  uchar* data = NULL;
  int width = 0, height = 0;
  PPM_LOADER_PIXEL_TYPE pt = PPM_LOADER_PIXEL_TYPE_INVALID;
//  std::string filename = "buda.0.ppm";
  std::string filename = "ppmImagenes/buda/buda.0.ppm";

  bool ret = LoadPPMFile(&data, &width, &height, &pt, filename.c_str());
  if (!ret || width == 0|| height == 0|| pt!=PPM_LOADER_PIXEL_TYPE_RGB_8B)
  {
    throw std::exception();//("test_load failed");
  }

  delete [] data;
}

void test_save(){

  char comments[100];
  sprintf(comments, "%s", "Hello world");

  int width = 3, height =1;
  uchar* data = new uchar[width*height*3];
  data[0] = data[1] = data[2] = 100; // RGB
  data[3] = data[4] = data[5] = 150; // RGB
  data[6] = data[7] = data[8] = 245; // RGB
  std::string filename = "ppmImagenes/buda/buda.0.ppm";

  bool ret = SavePPMFile(filename.c_str(),data,width,height,PPM_LOADER_PIXEL_TYPE_RGB_8B, comments);
  if (!ret)
  {
    std::cout << "ERROR: couldn't save Image to ppm file" << std::endl;
  }
}
*/