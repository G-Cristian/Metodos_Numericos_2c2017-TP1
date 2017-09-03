#ifndef _MANEJADORIMAGENES_H_
#define _MANEJADORIMAGENES_H_

#include "imagen.h"
#include <string>

using namespace std;

class ManejadorImagenes {
public:
	ManejadorImagenes();
	~ManejadorImagenes();
	
	Imagen abrirImagenPPM(string nombreArchivo) const;
	void guardarImagenPPM(const Imagen &imagen, string nombreArchivo) const;
};

#endif // !_MANEJADORIMAGENES_H_
