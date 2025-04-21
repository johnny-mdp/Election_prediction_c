#include <vector> 
# include <time.h>
#include <new>
#include <ctime>
#include <random>
#include <stdexcept>
using namespace std;
using namespace std::chrono;

struct Candidato
{
	int ano;	// Un candidato compite un dado año
	float puntaje;	// Un candidato tiene una ideologia
	bool poder;	// Un candidato viene de un partido en el poder o no
	float encuestas;	// Su promedio en encuestas
	float imagen; // Su imagen positiva+regular

	Candidato(){}
	Candidato (int a, float pun, int pod, float ima, float encu)
	{
		ano = a;
		puntaje = pun;
		poder = pod;
		imagen = ima;
		encuestas = encu;
	}
};

ostream &operator<<(ostream &os, Candidato cand)
{//Overlodeo << para poder escribir un Candidato simplemente poniendo en nombre de la variable de tipo Candidato
	os << cand.ano << "	" << cand.puntaje << "	" << cand.poder << "	"\
		<< cand.imagen << "	" << cand.encuestas;
	return os;
}



struct Pais
{	// Las variables elegidas son algunas de las más importantes de la economía
	int ano;	
	float desempleo[2];
	float pobreza[2];
	float pbi_crec[2];
	float inflac[2];
	int cand_por_ano; // Para saber cuantos candidatos hay en una eleccion

	Pais(){}
	Pais(int a, float des, float des_ant, float pob, float pob_ant, float gdp_crec,float gdp_crec_ant, float infl, float infl_ant)
	{
		ano = a;
		desempleo[0] = des;	desempleo[1] = des_ant;
		pobreza[0] = pob;	pobreza[1] = pob_ant ;
		pbi_crec[0] = gdp_crec; pbi_crec[1] = gdp_crec_ant;
		inflac[0] = infl; inflac[1] = infl_ant;
	}
};

ostream &operator<<(ostream &os, Pais country)
{	//Overlodeo << para poder escribir un pais simplemente poniendo en nombre de la variable de tipo pais
	os << country.ano << "	" << country.desempleo[0]  << "	" << country.desempleo[1] << "	" \
		<< country.pobreza[0] << "	" << country.pobreza[1] << "	"\
		<< country.pbi_crec[0] << "	" << country.pbi_crec[1] << "	"\
		<< country.inflac[0] << "	" << country.inflac[1] << "	" << country.cand_por_ano; 
	return os;
}



struct Eleccion
{
	// Una eleccion tiene candidaton, un resultado y se da en un contexto economico.
	// De ahí los miembro de esta estructura
	vector <Candidato> candidates;
	vector <float> resultados;
	Pais country;

	Eleccion(){}
	Eleccion( Pais argentina, vector<Candidato> candi, vector <float> results)
	{
		country = argentina;
		candidates = candi;
		resultados = results;
	}
};

void Cargar_datos( vector <Eleccion> & entrenamiento, fstream & mycandidate, fstream  & mycountry ); // Lee el set de datos de entrenamiento de archivos .txt
void Expandir_data_set( vector <Eleccion> & entrenamiento );// Generar elecciones nuevas variado levemente (al azar) los parametros de elecciones reales
void Guardar_Eleccion(Eleccion elect, fstream & myresults); // Guarda en un archivo .txt los resultados de una simulacion
void Guardar_Error(ofstream & myfile, int i, float a, float b);	// Guarda en un archivo .txt el error al finald e cada iteración
void Leer_datos( Pais & argentina, vector<Candidato> & candis); // Lee por pantalla los datos de una elección



void Cargar_datos( vector <Eleccion> & entrenamiento, fstream & mycandidate, fstream & mycountry )
{
	Pais argentina;
	std::string line;
	std::getline(mycountry, line);
	std::getline(mycandidate, line);

	while (!mycandidate.eof() && !mycountry.eof())
	{// Primero lee del archivo con los datos de la economia cual era el estado del país y el nro de candidatos
		mycountry >> argentina.ano;	

		mycountry >> argentina.desempleo[0];
		mycountry >> argentina.desempleo[1];
		
		mycountry >> argentina.pobreza[0];	
		mycountry >> argentina.pobreza[1];

		mycountry >> argentina.pbi_crec[0];
		mycountry >> argentina.pbi_crec[1];
		
		mycountry >> argentina.inflac[0];
		mycountry >> argentina.inflac[1];	

		mycountry >> argentina.cand_por_ano;

		if (argentina.cand_por_ano <0 ){cout << "Error. Base de datos corrupta.\n"; return;}

		if (argentina.desempleo[0] <0 || argentina.desempleo[0] >1 \
			|| argentina.desempleo[1] <0 || argentina.desempleo[1] >1 \
			|| argentina.pobreza[0] <0 || argentina.pobreza[0] >1 \
			|| argentina.pobreza[1] <0 || argentina.pobreza[1] >1 \
			|| argentina.pbi_crec[0] <-1 || argentina.pbi_crec[1] <-1 \
			|| argentina.inflac[0] <-1  || argentina.inflac[1] <-1 \
			|| argentina.cand_por_ano <0)
		{
			cout << "Error. Base de datos corrupta.\n"; return;
		}
		vector <Candidato> candi; candi.resize(argentina.cand_por_ano);
		vector <float> resultados;	resultados.resize(argentina.cand_por_ano);
		
		for (int i = 0; i < argentina.cand_por_ano; ++i)
		{// Lee los parametros de cada candidato del archivo adecuado
			Candidato aux;	
			
			mycandidate >> candi[i].ano;
			mycandidate >> candi[i].puntaje;
			mycandidate >> candi[i].poder;
			mycandidate >> candi[i].imagen;
			mycandidate >> candi[i].encuestas;
			mycandidate >> resultados[i];

			if ( candi[i].ano != argentina.ano \
				|| candi[i].puntaje <0 || candi[i].puntaje >1 \
				|| (candi[i].poder !=0 && candi[i].poder != 1) \
				|| candi[i].imagen <0 || candi[i].imagen >1  \
				|| candi[i].encuestas <0 || candi[i].encuestas >1 \
				|| resultados[i] <0 || resultados[i] >1 )
			{
				cout << argentina.ano<<"Error. Base de datos corrupta.\n"; return;
			}
		}

		Eleccion aux(argentina, candi, resultados);
		entrenamiento.push_back(aux); // Guarda en el set de entrenamieto la eleccion
		candi.clear();
		std::getline(mycandidate, line);
	}
}



void Expandir_data_set( vector< Eleccion > & entrenamiento)
{ // Generan nuevas elecciones variando (al azar) los parametros de elecciones reales
	float r,s;
	std::random_device rd;
	std::default_random_engine generator;
	generator.seed( rd() ); //Now this is seeded differently each time.
	std::normal_distribution<double> distribution(0,0.1);
	int T = entrenamiento.size();
	
	for (int j = 0; j < 5	; ++j)
	{
		Eleccion aux;
		for (int i = 0; i < T; ++i)
		{
			r = distribution(generator);
			s = distribution(generator);
			aux = entrenamiento[i];
			aux.country.pbi_crec[0] *= (1. + r);	aux.country.pbi_crec[1] *= (1. + s);
			aux.country.desempleo[0] *= (1. - r/2);	aux.country.desempleo[1] *= (1. - s/2);
			aux.country.pobreza[0] *= (1. - r/3);	aux.country.pobreza[1] *= (1. - s/3);
			// Los nro de 1/2 y 1/3 surgen de correlaciones que muestran que por
			// cada punto que crezca la economia el desempleo se reduce en 0.5 y la poblreza en 0.3
			// y similar por cada punto de caída en el pbi
			float total=0;
			for (int k = 0; k < aux.candidates.size(); ++k)
			{
				if (aux.candidates[k].poder)
				{
					aux.candidates[k].encuestas *= (1. + r);
					aux.candidates[k].imagen *= (1. + r); 
					aux.resultados[k] *= (1. + r);
				}
				else
				{
					aux.candidates[k].encuestas *= (1. - r);
					aux.candidates[k].imagen *= (1. - r);
					aux.resultados[k] *= (1. - r);			
				}
				total += aux.resultados[k];
			}

			for (int k = 0; k < aux.candidates.size(); ++k)
			{
				aux.resultados[k] /= total;
			}
			
			entrenamiento.push_back(aux);
		}

		
	}

}


void Guardar_Eleccion(Eleccion elect, fstream & myresults)
{
// Guarda los resultados de la simulacion en un archivo .txt
	myresults << "Año	Desem	D_ant	Pobr	P_ant	Pbi_cre	PBI_ant	Inf	Inf_ant	Nro Cands \n";
	myresults << elect.country << endl << endl;
	myresults << "Año	Punt	Poder	Imagen	Encues	Result \n";
	for (int i = 0; i < elect.country.cand_por_ano; ++i)
	{
		myresults << elect.candidates[i] << "	" << elect.resultados[i] << endl;
	}

	myresults << endl << endl;
}


void Guardar_Error(ofstream & myfile, int i, float a, float b)
{
	// Guarda el error medio de los padres de cada generación en un archivo .txt
	myfile << i << "	" << (a + b)/2. << "	" << endl;
}



Eleccion Leer_datos( )
{
	Eleccion elect;
	// Ingresa los datos de la economía del pais y nro decandidatos
	cout << "Ingrese el año de la eleccion:" << endl;
	cin >> elect.country.ano;
	cout << "Ingrese tasa de desempleo (normalizada a 1) el año de la eleccion:" << endl;
	cin >> elect.country.desempleo[0];
	while ( elect.country.desempleo[0]< 0 || elect.country.desempleo[0]> 1){
		cout << "Valor incorrecto. Intente nuevamente." << endl;
		cin >> elect.country.desempleo[0];
	}
	cout << "Ingrese tasa de desempleo (normalizada a 1) el año anterior a la eleccion :" << endl;
	cin >> elect.country.desempleo[1];
	while ( elect.country.desempleo[1]< 0 || elect.country.desempleo[1]> 1){
		cout << "Valor incorrecto. Intente nuevamente." << endl;
		cin >> elect.country.desempleo[1];
	}
	
	cout << "Ingrese tasa de pobreza (normalizada a 1) el año de la eleccion:" << endl;
	cin >> elect.country.pobreza[0];
	while (  elect.country.pobreza[0]< 0 || elect.country.pobreza[0]> 1 ){
		cout << "Valor incorrecto. Intente nuevamente." << endl;
		cin >> elect.country.pobreza[0];
	}
	cout << "Ingrese tasa de pobreza (normalizada a 1) el año anterior a la eleccion :" << endl;
	cin >> elect.country.pobreza[1];
	while ( elect.country.pobreza[1]< 0 || elect.country.pobreza[1]> 1){
		cout << "Valor incorrecto. Intente nuevamente." << endl;
		cin >> elect.country.pobreza[1];
	}
	
	cout << "Ingrese tasa de crecimiento del PBI (normalizada a 1) el año de la eleccion:" << endl;
	cin >> elect.country.pbi_crec[0];
	while ( elect.country.pbi_crec[0]< -1 ){
		cout << "Valor incorrecto. Intente nuevamente." << endl;
		cin >> elect.country.pbi_crec[0];
	}
	cout << "Ingrese tasa de crecimiento del PBI  (normalizada a 1) el año anterior a la eleccion :" << endl;
	cin >> elect.country.pbi_crec[1];
	while ( elect.country.pbi_crec[1]< -1 ){
		cout << "Valor incorrecto. Intente nuevamente." << endl;
		cin >> elect.country.pbi_crec[1];
	}
	
	cout << "Ingrese tasa de inflacion (normalizada a 1) el año de la eleccion:" << endl;
	cin >> elect.country.inflac[0];
	while ( elect.country.inflac[0]< -1 ){
		cout << "Valor incorrecto. Intente nuevamente." << endl;
		cin >> elect.country.inflac[0];
	}
	cout << "Ingrese tasa de inflacion  (normalizada a 1) el año anterior a la eleccion :" << endl;
	cin >> elect.country.inflac[1];
	while ( elect.country.inflac[1]< -1 ){
		cout << "Valor incorrecto. Intente nuevamente." << endl;
		cin >> elect.country.inflac[1];
	}
	
	cout << "Por favor, ingrese el número de candidatos en la elección:" << endl;
	cin >> elect.country.cand_por_ano;
	
	elect.candidates.resize( elect.country.cand_por_ano );
	elect.resultados.resize (elect.country.cand_por_ano);
	for (int i = 0; i < elect.country.cand_por_ano; ++i)
	{	//Ingresa los datos de cada candidato
		elect.candidates[i].ano = elect.country.ano;
		cout << "Ingrese el puntaje ideologico del candidato (0 izquierda, 1 derecha):" << endl;
		cin >> elect.candidates[i].puntaje;
		while ( elect.candidates[i].puntaje < 0 || elect.candidates[i].puntaje>1 ){
			cout << "Valor incorrecto. Intente nuevamente." << endl;
			cin >> elect.candidates[i].puntaje;
		}
	
		cout << "¿El candidato forma parte del partido de gobierno? 1 SÍ, 0 NO:" << endl;
		cin >> elect.candidates[i].poder;
	
		cout << "¿Cual es su imágen positiva + regular?" << endl;
		cin >> elect.candidates[i].imagen;
		while ( elect.candidates[i].imagen < 0 || elect.candidates[i].imagen > 1 ){
			cout << "Valor incorrecto. Intente nuevamente." << endl;
			cin >> elect.candidates[i].imagen;
		}
	
		cout << "¿Cual es su intención de voto normalizada a 1?" << endl;
		cin >> elect.candidates[i].encuestas;
		while ( elect.candidates[i].encuestas < 0 || elect.candidates[i].encuestas> 1 ){
			cout << "Valor incorrecto. Intente nuevamente." << endl;
			cin >> elect.candidates[i].encuestas;
		}
	}

	return elect;
}



