#include <iostream>
#include <stdio.h>
#include <vector>
#include "../Include/matriz.h"
#include "../Include/matrizFactory.h"
#include "../Include/sistemaDeEcuaciones.h"

using namespace std;

int main(){
	MatrizDouble m1 = MatrizDouble(3,4, 0);
	
	for (int i = 0; i < 3; i++){
		for(int j = 0; j < 4; j++){
			m1[i][j] = 50*i*j;
		}
	}

	MatrizDouble m2 = m1 + m1;
	
	//MatrizInt m3 = MatrizInt(3,5,0);
	//debe abortar
	//m3 = m3 + m1;
	
	m1.mostrar();
	cout << endl;
	m2.mostrar();
	
	//m2 = m2/3;
	MatrizUChar m3 = m2.saturar();
	//m2 = m2.saturarAMatrizChar();
	cout << endl;
	m3.mostrar();
	
	m2 = m2 - 2* m2;
	m3 = m2.saturar();
	
	cout << endl;
	m2.mostrar();
	
	cout << endl;
	m3.mostrar();

	Matriz<double> i = MatrizFactory::identidad<double>(4);
	i.mostrar();

	m2.submatriz(1, 2, 2, 2).mostrar();

	vector<int> v = vector<int>(3);
	v[0] = 1;
	v[1] = 2;
	v[2] = 0;
	cout << endl;
	MatrizFactory::vectorDeParticion(4).mostrar();
	cout << endl;
	MatrizFactory::vectorDeParticion(v).mostrar();
	cout << endl;
	MatrizFactory::matrizDeParticion(4).mostrar();
	cout << endl;
	MatrizFactory::matrizDeParticion(MatrizFactory::vectorDeParticion(v)).mostrar();
	cout << endl;

	//9*x1 2*x2 3*x3 = 6 => x1 = (6 - 2*(-0,0833333325) - 3 * 0,6666666666) / 9 = 0,462962963
	//0*x1 4*x2 5*x3 = 3 => x2 = (3 - 5*0,6666666)/4 = -0,0833333325
	//0*x1 0*x2 6*x3 = 4 => x3 = 4/6 = 0,6666666666

	MatrizDouble m = MatrizDouble(3, 3, 0);
	m[0][0] = 9; m[0][1] = 2; m[0][2] = 3;
	m[1][0] = 0; m[1][1] = 4; m[1][2] = 5;
	m[2][0] = 0; m[2][1] = 0; m[2][2] = 6;

	MatrizDouble b = MatrizDouble(3, 1, 0);
	b[0][0] = 6;
	b[1][0] = 3;
	b[2][0] = 4;

	cout << endl;
	m.mostrar();
	cout << endl;
	b.mostrar();
	cout << endl;
	SistemaDeEcuaciones sde = SistemaDeEcuaciones();
	MatrizDouble x = sde.resolverAPartirDeTriangularSuperior<double>(m, b);
	x.mostrar();
	cout << endl;

	//6*x1 + 0*x2 + 0*x3 = 4 => x1 = 4/6 = 0,6666666666
	//5*x1 + 4*x2 + 0*x3 = 3 => x2 = (3 - 5*0,6666666)/4 = -0,0833333325
	//3*x1 + 2*x2 + 9*x3 = 6 => x3 = (6 - 2*(-0,0833333325) - 3 * 0,6666666666) / 9 = 0,462962963

	MatrizDouble mm = MatrizDouble(3, 3, 0);
	mm[2][2] = 9;	mm[2][1] = 2;	mm[2][0] = 3;
					mm[1][1] = 4;	mm[1][0] = 5;
									mm[0][0] = 6;

	MatrizDouble bb = MatrizDouble(3, 1, 0);
	bb[2][0] = 6;
	bb[1][0] = 3;
	bb[0][0] = 4;

	cout << endl;
	mm.mostrar();
	cout << endl;
	bb.mostrar();
	cout << endl;

	x = sde.resolverAPartirDeTriangularInferior<double>(mm, bb);
	x.mostrar();
	cout << endl;

	/*

	2x1 + 3x2 + 4x3 = 5
	1x1 - 2x2 + 2x3 = 6
	3x1 + 4x2 + 2x3 = 7

	P=(0,1,2)

	3x1 + 4x2 + 2x3 = 7					3x1	+ 4x2		+ 2x3	= 7
	1x1 - 2x2 + 2x3 = 6		m21 = 1/3	0x1	+ -10/3x2	+ 4/3x3	= 11/3
	2x1 + 3x2 + 4x3 = 5		m31 = 2/3	0x1	+ 1/3x2		+ 8/3x3	= 1/3

	P=(2,1,0)

	3x1	+ 4x2		+ 2x3	= 7							3x1	+ 4x2		+ 2x3	= 7
	0x1	+ 1/3x2		+ 8/3x3	= 1/3						0x1	+ 1/3x2		+ 8/3x3	= 1/3
	0x1	+ -10/3x2	+ 4/3x3	= 11/3		m32 = -10		0x1	+ 0x2		+ 84/3x3	= 21/3

	P=(2,0,1)


		 0 0 1
	MP = 1 0 0
		 0 1 0

		1		0		0
	L =	-1/3	1		0
		-2/3	10	1

				3	4		2			3	4			2
	PA = U =	0	1/3		8/3		=	0	0,333333	2,666666
				0	0		84/3		0	0			28

			7			7
	Pb =	1/3		=	0,333333
			21/3		7
	*/
	
	MatrizDouble A = MatrizDouble(3, 3, 0);
	A[0][0] = 2;	A[0][1] = 3;	A[0][2] = 4;
	A[1][0] = 1;	A[1][1] = -2;	A[1][2] = 2;
	A[2][0] = 3;	A[2][1] = 4;	A[2][2] = 2;

	b = MatrizDouble(3, 1, 0);
	b[0][0] = 5;
	b[1][0] = 6;
	b[2][0] = 7;

	cout << "A" << endl;
	A.mostrar();
	cout << endl;
	cout << "b" << endl;
	b.mostrar();
	cout << endl;
	SistemaDeEcuaciones::ResultadosDeEliminacionGausseana<double> res = sde.eliminacionGausseana<double>(A, b);
	cout << "A final" << endl;
	res.matrizLuegoDeEliminacionGausseana.mostrar();
	cout << endl;
	cout << "Pb final" << endl;
	res.resultadosLuegoDeEliminacionGausseana.mostrar();
	cout << endl;
	cout << "MP final" << endl;
	res.matrizDeParticion.mostrar();
	cout << endl;
	cout << "L" << endl;
	res.L.mostrar();
	cout << endl;
	cout << "U" << endl;
	res.U.mostrar();
	cout << endl;
}
