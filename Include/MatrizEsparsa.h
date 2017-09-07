#ifndef _MATRIZESPARSA_H_
#define _MATRIZESPARSA_H_

#include <map>
#include <utility>

using namespace std;

typedef pair<int, int> tipoClave;
typedef double tipoElementos;

class MatrizEsparsa {
public:
	MatrizEsparsa(int alto, int ancho);
	~MatrizEsparsa();

	tipoElementos enYX(int y, int x) const;
	void insertarEnYX(int y, int x, tipoElementos elemento);
	MatrizEsparsa operator*(const MatrizEsparsa &otra) const;
	MatrizEsparsa transpuesta() const;
private:
	map<tipoClave, tipoElementos> _elementos;
	int _alto;
	int _ancho;
};

#endif // !_MATRIZESPARSA_H_
