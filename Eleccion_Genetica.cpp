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

/*
COMPILAR CON ESTE COMANDO. EL OTRO ESTA DPRECADO POR VIEJO

g++ -std=c++11 Eleccion_Genetica.cpp
*/


// Función de comparación para ordenar la población de redes neuronales
// en función de su error
bool Orden(Neural_Network arg1, Neural_Network arg2)
{
	if ( arg1.Get_Total_Error() > arg2.Get_Total_Error() )
		return false;

	return true;
}



int main()
{
	char l;
	Neural_Network Red_lista; // Red neuronal con la que se predicirá el resultado de la elecciones
	cout << "¿Tiene ud. un archivo de texto con los pesos y bias de una red neuroanal ya entrenada que desee cargar? S/N" << endl;
	cin >> l;		// Si se tiene un archivo que se quiere usar no tiene sentido volver a entrenar otro.
	while ( l != 'S' && l != 's' && l != 'N' && l != 'n')
	{
		cout << "Caracter ingresado incorrecto. Intente nuevamente." << endl;
		cin >> l;
	}
	
	if ( l == 'S' || l == 's') //
	{
		Load_red(Red_lista);	// Carga los pesos y biases  en la red neuronal que se usara
		Red_lista.Predict();	// Se pasa el control a la función que predice elecciones en base a un red entrenada
		return 0;
	}

	fstream mycandidate, mycountry;
	string path_arch;
	cout << "Ingrese el path del archivo con los datos de los candidatos:" << endl;
	// Este archivo contendrá los datos de los candidatos que son:
	// puntaje ideológico, si estaban en el gobierno o no, su promedio en encuestas, el resultado, y su imagen positiva
	cin >> path_arch;
	mycandidate.open(path_arch);	
	while (mycandidate.fail())
	{
		cout << "Intente nuevamente ingresar el path: \n";
		cin >> path_arch;
		mycandidate.open(path_arch);
	}

	cout << "Ingrese el path del archivo con los datos de la economia:" << endl;
	// Este archivo contendrá los datos de la economia que son:
	// desempleo el año de la eleccion y el anterior, pobreza el año de la eleccion y el anterior, 
	// variación del pbi el año de la eleccion y el anterior,
	// inflación el año de la eleccion y el anterior, nro de candidatos 
	cin >> path_arch;
	mycountry.open(path_arch);
	while (mycountry.fail())
	{
		cout << "Intente nuevamente ingresar el path: \n";
		cin >> path_arch;
		mycountry.open(path_arch);
	}


	vector < Eleccion > entrenamiento;	// este vector guardará el set de datos del entrenamiento
	Cargar_datos(entrenamiento, mycandidate, mycountry); // función para inicializar el set de datos
	if (entrenamiento.size()==0)
	{// Si no cargó nada no se puede entrenar. 
	 //	Pero si cargo algo se puede usar hasta la parte donde no hubo problemas
		return 1;
	}
	//Expandir_data_set( entrenamiento ); // Genera nuevas elecciones al azar
	mycandidate.close(); mycountry.close();

	int Poblacion_total = 6;		// Nro de redes neuronales con las que se trabajará
	int Generaciones = 100000;		// Nro de iteraciones máximo que se entrenará
	vector < Neural_Network > poblacion; // Vector que contendrá a todas la redes neuronales
	vector <int> tamanos;			// Este vector tiene el nro de neuronas en cada capa de la red neuronal.
									// Tienen que ser todas iguales para que tenga sentido la comparación
	tamanos.push_back(12); tamanos.push_back(5); tamanos.push_back(5); tamanos.push_back(1);

	for (int i = 0; i < Poblacion_total; ++i) // Este ciclo for inicializa la primer generación de redes neuronales.
	{
		Neural_Network red( tamanos );
		cout <<"Individuo " << i+1<< endl;
		red.Inicia_pesos_bias();
		poblacion.push_back( red );
	}


	Neural_Network padre, madre, hijo;	// Estas redes darán origen a la siguiente generación de redes
	ofstream myerror;		// Este archivo contrendrá el error medio de padre y madre en cada iteración
	myerror.open("/Users/macbookair/Documents/PHD/MATERIAS/c++/Eleccion/Error.txt");
	for (int x = 0; x < Generaciones; ++x)
	{
		cout << "Generacion nro " << x << endl;		// Para que el usuario sepa por donde va el programa
		for (int j = 0; j < poblacion.size(); ++j)	// Este ciclo for entrena a toda la población 1 iteración
		{
			poblacion[j].Entrenar(entrenamiento);
		}
		sort(poblacion.begin(), poblacion.end(), Orden);	// Ordena pa población de menor a mayor error
		padre= poblacion[ 0 ];	// Seleccionando a los padres de la siguiente generación
		madre= poblacion[ 1 ];
		poblacion.clear();		// Eliminando a la generación anteriror

		Guardar_Error( myerror, x, padre.Get_Total_Error()/entrenamiento.size(), madre.Get_Total_Error()/entrenamiento.size());

		if (padre.Get_Total_Error()/entrenamiento.size() < 0.12) 
		{// Si la generación actual es lo bastante precisa no tiene sentido seguir entrenando
			break;
		}
		for (int i = 0; i < Poblacion_total; ++i)
		{// Creando y guardando a la nueva generación
			hijo = Hijo( padre, madre );
			poblacion.push_back( hijo );
		}
	}
	myerror.close();

	// Eligiendo a la red más precisa como la red que se guardará y usara para predecir elecciones.
	Red_lista = padre;	
	cout<< "¿Desea guardar los pesos y bias de la red neuronal? S/N" << endl;
	char c;
	cin >> c;
	while ( c != 'S' && c != 's' && c != 'N' && c != 'n')
	{
		cout << "Caracter ingresado incorrecto. Intente nuevamente." << endl;
		cin >> c;
	}
	if ( c== 'S' || c== 's' ) // Si se la quiere guardar Save_red la escribe en un archivo .txt
		Save_red(Red_lista);

	Red_lista.Predict();	// Se pasa el control a la función que predice elecciones en base a un red entrenada


	return 0;
}