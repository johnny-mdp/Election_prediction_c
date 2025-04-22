#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;
using namespace std::chrono;

# include <math.h>
#include <vector>
# include <stdlib.h>
# include <stdio.h>
# include <time.h>
#include <new>
#include <ctime>
#include <random>
#include "Red_Neuronal_Genetica.h"

//g++ -std=c++11 Eleccion_Genetica.o Eleccion_Genetica.cpp



// Sorting function to compare performance of different NNs
bool Orden(Neural_Network arg1, Neural_Network arg2){
	if ( arg1.Get_Total_Error() > arg2.Get_Total_Error() )
		return false;

	return true;
}



int main(){
	char l;
	Neural_Network Red_lista; // Red neuronal con la que se predicirá el resultado de la elecciones
	cout << "¿Tiene ud. un archivo de texto con los pesos y bias de una red neuroanal ya entrenada que desee cargar? S/N" << endl;
	cin >> l;		// In case wealready have a trained NN
	l = std::tolower(l);
	while ( l != 's' && l != 'n'){
		cout << "Caracter ingresado incorrecto. Intente nuevamente." << endl;
		cin >> l;
	}
	
	if ( l == 's'){
		Load_red(Red_lista);	// Load a NN's parameters
		Red_lista.Predict();	// Function that predicts an election
		return 0;
	}

	fstream mycandidate, mycountry;
	string path_arch;
	cout << "Ingrese el path del archivo con los datos de los candidatos:" << endl;
	
	cin >> path_arch;
	mycandidate.open(path_arch);	
	while (mycandidate.fail()){
		cout << "Intente nuevamente ingresar el path: \n";
		cin >> path_arch;
		mycandidate.open(path_arch);
	}

	cout << "Ingrese el path del archivo con los datos de la economia:" << endl;
	
	cin >> path_arch;
	mycountry.open(path_arch);
	while (mycountry.fail()){
		cout << "Intente nuevamente ingresar el path: \n";
		cin >> path_arch;
		mycountry.open(path_arch);
	}


	vector < Eleccion > entrenamiento;	// training data set
	Cargar_datos(entrenamiento, mycandidate, mycountry); 
	if (entrenamiento.size()==0){return 1;}

	mycandidate.close(); mycountry.close();

	int Poblacion_total = 6;		// Population size
	int Generaciones = 100000;		// Max number of iterations
	vector < Neural_Network > poblacion;  
	vector <int> tamanos;	// Number of neurons on each layer of the NN
	
	tamanos.push_back(12); tamanos.push_back(5); tamanos.push_back(5); tamanos.push_back(1);

	for (int i = 0; i < Poblacion_total; ++i) {
		//creates first generation of NN
		Neural_Network red( tamanos );
		cout <<"Individuo " << i+1<< endl;
		red.Inicia_pesos_bias();
		poblacion.push_back( red );
	}


	Neural_Network padre, madre, hijo;	// These NNs produce the next generation
	ofstream myerror;
	myerror.open("Error.txt");
	for (int x = 0; x < Generaciones; ++x){
		cout << "Generacion nro " << x << endl;		// So the user can track progress
		for (int j = 0; j < poblacion.size(); ++j){
			poblacion[j].Entrenar(entrenamiento);
		}
		sort(poblacion.begin(), poblacion.end(), Orden);	// Sorts population by error
		padre= poblacion[ 0 ];
		madre= poblacion[ 1 ];
		poblacion.clear();
		
		Guardar_Error( myerror, x, padre.Get_Total_Error()/entrenamiento.size(), madre.Get_Total_Error()/entrenamiento.size());

		if (padre.Get_Total_Error()/entrenamiento.size() < 0.12){
			// if we attain the desired precission, training stops
			break;
		}
		
		for (int i = 0; i < Poblacion_total; ++i){
			hijo = Hijo( padre, madre );
			poblacion.push_back( hijo );
		}
	}
	myerror.close();

	
	Red_lista = padre;	
	cout<< "¿Desea guardar los pesos y bias de la red neuronal? S/N" << endl;
	char c;
	cin >> c; c = std::tolower(c);
	while ( c != 's' && c != 'n'){
		cout << "Caracter ingresado incorrecto. Intente nuevamente." << endl;
		cin >> c;
	}
	if ( c== 's' ) Save_red(Red_lista);

	Red_lista.Predict();	// Se pasa el control a la función que predice elecciones en base a un red entrenada
	return 0;
}
