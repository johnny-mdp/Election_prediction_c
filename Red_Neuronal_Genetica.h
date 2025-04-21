#include <vector> 
#include <random>
#include "Datos.h"
using namespace std;



vector <float> Sigmoide( vector <float> aux);



class Cost_function
{
public:
	float delta(float a, float y)
	{
		if (a<0 || a> 100)
			return abs(a-y)*100;	
		// Si por algun tema numérico arroja una fracción de votos negativa quiero castigar fuertemente eso
		// Idem para fracciones mayores a 1.
		return abs(a-y);
	}
};


class Neural_Network : public Cost_function
{	
	vector < vector < vector <float> > > pesos;	
	//Los pesos de la red neuronal son un vector de matrices. Cada matriz conecta una capa con la siguiente
	vector < vector <float> > bias;
	//	Los bias de la res son un vector de vectores. Cada bias es una característica de una neurona.
	public:
		float total_error;		// Parametro de costo que me dice que tan buen o malo es el ajuste de mi red al set de datos.
		vector <int> tamanos;	// Nro de neuronas en cada capa

		Neural_Network(){ total_error = 0;	}
		Neural_Network(  vector <int> sizes)
		{
			total_error = 0;
			tamanos = sizes;
		}
		~Neural_Network(){}

		void Inicia_pesos_bias();	// Inicial los pesos y bias de cada red
		void Entrenar(vector <Eleccion> entrenamiento);	// Entrena la red 1 iteracion sobre el set de datos
		double Feed_forward(Candidato candidate, Pais country);	// Hace multiplicacion de matrices
		float Get_Total_Error() { return total_error;}	// Getter del error de la red
		
		float Get_Peso(int i, int j, int k){ return pesos[i][j][k];}				// Getter del peso
		void Set_Peso(int i, int j, int k, float value){ pesos[i][j][k] = value;}	// Setter del peso
		float Get_Bias(int i, int j){ return bias[i][j];}						// Getter del peso
		void Set_Bias(int i, int j, float value){ bias[i][j] = value;}				// Setter del bias
		void Predict();		// Una vez que la red ya esta entrenada permite predecir elecciones y guardar el resultado
		void Calcula_porcentajes(Eleccion & elect, double & norm);


		friend void Save_red(Neural_Network Red_final);		// Escribe pesos y bias de la red en un archivo .txt
		friend void Load_red(Neural_Network & Red_final);	// Lee pesos y bias de la red de un archivo .txt
		friend Neural_Network Hijo(Neural_Network padre, Neural_Network madre); // Crea la sgte generacion de redes neuronales
};



void Neural_Network::Inicia_pesos_bias()
{
	pesos.resize( tamanos.size()-1 );
	bias.resize( tamanos.size()-1 ); 
	auto start = high_resolution_clock::now(); 

	std::random_device rd;
	std::default_random_engine generator;
	generator.seed( rd() ); //La semilla debe ser diferente cada vez para tener variabilidad en la poblacion
	std::normal_distribution<double> distribution(0,1);
	for (int i = 0; i < (tamanos.size()-1); ++i)
	{
		int T = tamanos[i+1];
		int S = tamanos[i];
		float aux[T][S];
		float aux_bias[T];
		pesos[i].resize( T );
		bias[i].resize( T );
		for (int j = 0; j < T; ++j)
		{
			pesos[i][j].resize( S );
			for (int k = 0; k < S; ++k)
			{
				pesos[i][j][k] = distribution(generator);
			}
		}

		for (int k = 0; k < T; ++k)
		{
			bias[i][k] = distribution(generator);
			
		}
	}

}



void Neural_Network::Entrenar( vector <Eleccion> entrenamiento)
{
	double resultado_corregido; 
	total_error = 0;
	for (int i = 0; i < entrenamiento.size(); ++i)	// Calc
	{
		for (int j = 0; j < entrenamiento[i].candidates.size(); ++j) 
		{	// Calcula los porcentajes de los candidatos y el error respecto al resultado real
			resultado_corregido = Feed_forward(entrenamiento[i].candidates[j], entrenamiento[i].country);
			total_error += delta(entrenamiento[i].resultados[j],resultado_corregido);	
		}
	}
	
}



double Neural_Network::Feed_forward( Candidato candidate, Pais country)
{
	vector <float> input; input.resize(tamanos[0]);
	
	// Toma como input 1 candidato y la economia del pais
	input[0] = candidate.puntaje;	input[1] = candidate.poder;	input[2] = candidate.encuestas;	input[3] = candidate.imagen;
	input[4] = country.pobreza[0];	input[5] = country.pobreza[1];	input[6] = country.desempleo[0];	input[7] = country.desempleo[1];
	input[8] = country.pbi_crec[0];	input[9] = country.pbi_crec[1];	input[10] = country.inflac[0];	input[11] = country.inflac[1];
	
	for (int i = 0; i < tamanos.size()-1; i++)	// Realiza la multiplicación de matrices
	{
		vector <float> aux; aux.resize(tamanos[i+1], 0);
			
		for (int j = 0; j < tamanos[i+1]; ++j)
		{	
			for (int k = 0; k < tamanos[i]; ++k)
			{
				aux[j] += pesos[i][j][k]*input[k];
			}
			
			aux[j] += bias[i][j];
		}
		input.clear();
		// Utilizo neuronas sigmoides. Esto me debería asugar que al final obtengo un resultado entre 0 y 1.
		input = Sigmoide(aux);
		
	}
	// A la salida input es un vector de 1 componente que tiene la estimacion del porcentaje del candidato
	return input[0]; 
}


void Neural_Network::Predict()
{
	Eleccion elect;
	double norm = 0;	// norm me sirve para redondear los porcentaje a 1 (proyecció de indecisos)
	char c;
	cout << "¿Desea ud. simular una eleccion? S/N" << endl;
	cin >> c;

	fstream myresults;	// Guardará el resultado de la simulacion de la eleccion
	char name[] = "Resultados_de_la_simulacion.txt";
	myresults.open(name,fstream::out);
	const auto begin = myresults.tellg();// servirá para decir si el archivo de resultados está vacio

	while (c == 's' || c == 'S')
	{	
		elect = Leer_datos();

		Calcula_porcentajes(elect, norm);

		for (int i = 0; i < elect.country.cand_por_ano; ++i)
		{
			elect.resultados[i] = elect.resultados[i]/ norm;	//Proyecta indecisos
			cout << "El candidato " << i+1 << " obtendría " << elect.resultados[i]*100 << "% de los votos."<< endl;
		}

		cout <<"¿Desea guardar los resultados de esta simulacion? S/N" << endl;
		char guardar = 's';
		cin >> guardar;
		while ( guardar != 'S' && guardar != 's' && guardar != 'N' && guardar != 'n')
		{
			cout << "Caracter ingresado incorrecto. Intente nuevamente." << endl;
			cin >> guardar;
		}
		if( guardar == 's' || guardar == 'S')
		{
			Guardar_Eleccion(elect, myresults);
		}

		cout <<"¿Desea simular otra eleccion? S/N" << endl;
		cin >> c;
		while ( c != 'S' && c != 's' && c != 'N' && c != 'n')
		{
			cout << "Caracter ingresado incorrecto. Intente nuevamente." << endl;
			cin >> c;
		}
		norm = 0;
	}
	// Si el archivo donde se guardarían resultados de las simulaciones esta vacio, lo borra
	myresults.seekg (0, ios::end);	const auto end = myresults.tellg(); const auto fsize = (end-begin);
	if ( fsize == 0)
	{
		int i = remove(name);
		if (i == 0)
		{
			cout << "Archivo sin usar borrado con éxito. \n";
		}
		else
		{
			cout << "ERROR. No se pudo borrar el archivo de salida sin uso. \n";
		}
	}
	myresults.close();
}


void Neural_Network::Calcula_porcentajes(Eleccion & elect, double & norm)
{
	for (int i = 0; i < elect.country.cand_por_ano; ++i)
	{
		elect.resultados[i] = Feed_forward(elect.candidates[i], elect.country);// calcula el resultado del candidato
		norm += elect.resultados[i];
	}
}



void Save_red(Neural_Network Red_final)
{
	ofstream mysave;
	mysave.open("Pesos_y_bias.txt");

	mysave << Red_final.tamanos.size(); // Para saber cuantas capas tiene la red neuronal
	mysave << endl <<endl;
	for (int i = 0; i < Red_final.tamanos.size(); ++i)
	{// Para saber el tamaño de cada capa
		mysave << Red_final.tamanos[i] << "	";
	}
	mysave << endl << endl;

	// Escribimos los pesos y despues dejamos una linea vacía
	for (int i = 0; i < (Red_final.tamanos.size()-1); ++i)
	{
		int T = Red_final.tamanos[i+1];
		int S = Red_final.tamanos[i];
		for (int j = 0; j < T; ++j)
		{
			for (int k = 0; k < S; ++k)
			{
				mysave << Red_final.pesos[i][j][k] << "	";
			}
			mysave << endl;
		}
	}

	//Escribimos los biases
	for (int i = 0; i < (Red_final.tamanos.size()-1); ++i)
	{
		int T = Red_final.tamanos[i+1];
		int S = Red_final.tamanos[i];
		for (int k = 0; k < T; ++k)
		{
			mysave << Red_final.bias[i][k]<< "	";
			
		}
	}
	mysave.close();
}



void Load_red(Neural_Network & Red_final)
{
	fstream myload;
	string path_arch;
	cout << "Ingrese el path del archivo con los pesos y biases de la red ya entrenada:" << endl;
	cin >> path_arch;
	myload.open(path_arch);
	while (myload.fail())
	{	// Sino encuentra el archivo inisiste
		cout << "Intente nuevamente ingresar el path: \n";
		cin >> path_arch;
		myload.open(path_arch);
	}

	std::string line;
	int a;
	double b;

	myload >> a;
	std::getline(myload, line);
	int t = a; // Ahora se sabe el nro de capas

	for (int i = 0; i < t; i++)
	{//Ahoa se sabe el tamaño de cada capa
			myload >> a;
			Red_final.tamanos.push_back(a);
	}
	cout << "Tamaños leidos \n";
	std::getline(myload, line);
	Red_final.pesos.resize(t-1);
	Red_final.bias.resize(t-1);

	//Ahora leimos los pesos
	for (int i = 0; i < (Red_final.tamanos.size()-1); ++i)
	{
		int T = Red_final.tamanos[i+1];
		int S = Red_final.tamanos[i];
		
		Red_final.pesos[i].resize( T );
		for (int j = 0; j < T; ++j)
		{
			Red_final.pesos[i][j].resize( S );
			for (int k = 0; k < S; ++k)
			{

				myload >> b;
				Red_final.pesos[i][j][k] = b;
			}
		}
	}
	cout << "Pesos leidos \n";
	std::getline(myload, line);


	//Ahora leimos los biases
	for (int i = 0; i < (Red_final.tamanos.size()-1); ++i)
	{
		int T = Red_final.tamanos[i+1];
		float aux_bias[T];
		Red_final.bias[i].resize( T );

		for (int k = 0; k < T; ++k)
		{
			myload >> b;
			Red_final.bias[i][k] = b;
			
		}
	}
	cout << "Biases leidos \n";
	myload.close();
}



Neural_Network Hijo(Neural_Network padre, Neural_Network madre)
{
	std::random_device rd;
	std::default_random_engine generator;
	generator.seed( rd() ); //Now this is seeded differently each time.
	std::normal_distribution<double> distribution(0,1);	
	padre.total_error = 0;
	for (int capa = 0; capa < padre.tamanos.size()-1; ++capa)
	{
		for (int j = 0; j < padre.tamanos[capa+1]; ++j)
		{
			if ( distribution(generator) < 0 )
			{// Se hereda cada pesos o bias de un dado padre con un 50% de chances
				padre.bias[capa][j] = madre.bias[capa][j];
			}	
			if  ( abs( distribution(generator) ) < 0.001)
			{// Las mutaciones cambian los pesos o biases al azar
				padre.bias[capa][j] = distribution(generator);
			}

			for ( int k=0; k<padre.tamanos[capa] ; k++ )
			{
				if ( distribution(generator) < 0)
				{
					padre.pesos[capa][j][k] = madre.pesos[capa][j][k];
				}	
				if  ( abs(distribution(generator)) < 0.001)
				{
					//cout <<"Bias evoluciona al azar" << endl;
					padre.pesos[capa][j][k] = distribution(generator);
				}
			}
		}
	}

	return padre;
}


vector <float> Sigmoide( vector <float> aux)
{
	for (int i = 0; i < aux.size(); ++i)
	{
		aux[i] =  1./(1. + pow(2.71828, -aux[i]) ) ;//( pow(2.71828, aux[i]) - pow(2.71828, -aux[i]) )/( pow(2.71828, aux[i]) + pow(2.71828, -aux[i]) );
	}

	return aux;
}
