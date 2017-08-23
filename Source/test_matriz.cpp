#include <iostream>
#include <stdio.h>
#include "../Include/matriz.h"

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
	
	for (int i = 0; i < 3; i++){
		for(int j = 0; j < 4; j++){
			cout << m1[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < 3; i++){
		for(int j = 0; j < 4; j++){
			cout << m2[i][j] << " ";
		}
		cout << endl;
	}
	
	//m2 = m2/3;
	MatrizUChar m3 = m2.saturar();
	//m2 = m2.saturarAMatrizChar();
	cout << endl;
	for (int i = 0; i < 3; i++){
		for(int j = 0; j < 4; j++){
			cout << (int)(m3[i][j]) << " ";
		}
		cout << endl;
	}
	
	m2 = m2 - 2* m2;
	m3 = m2.saturar();
	
	cout << endl;
	for (int i = 0; i < 3; i++){
		for(int j = 0; j < 4; j++){
			cout << m2[i][j] << " ";
		}
		cout << endl;
	}
	
	cout << endl;
	for (int i = 0; i < 3; i++){
		for(int j = 0; j < 4; j++){
			cout << (int)(m3[i][j]) << " ";
		}
		cout << endl;
	}
}
