#include <vector> 
# include <time.h>
#include <new>
#include <ctime>
#include <random>
#include <stdexcept>
using namespace std;
using namespace std::chrono;

struct Candidate{
	int year;	// A candidate compites a given year
	float ideology;	// A candidate a given ideology
	bool power;	// A candidate's party is or is not in power
	float polling;// Candidate's polling average
	float image; // Cadidate's  possitive+regular favourability rating

	Candidate(){}
	Candidate (int a, float pun, int pod, float ima, float encu){
		year = a;
		ideology = pun;
		power = pod;
		image = ima;
		polling = encu;
	}
};

ostream &operator<<(ostream &os, Candidate cand){
	//Overloding  << to write a Candidate variable
	os << cand.year << "	" << cand.ideology << "	" << cand.power << "	"\
		<< cand.image << "	" << cand.polling;
	return os;
}



struct Country{
	// Country's economic variables
	int year;	
	float unemployment[2];
	float poverty[2];
	float gdp_growth[2];
	float inflation[2];
	int cand_por_ano; // Number of candidates per election

	Pais(){}
	Pais(int a, float unemp, float prev_unemp, float pov, float prev_pov, float gdp_gro, float prev_gdp_gro, float infl, float prev_infl){
		year = a;
		unemployment[0] = unemp;	unemployment[1] = prev_unemp;
		poverty[0] = pov;	poverty[1] = prev_pov ;
		gdp_growth[0] = gdp_gro; gdp_growth[1] = prev_gdp_gro;
		inflation[0] = infl; inflation[1] = prev_infl;
	}
};

ostream &operator<<(ostream &os, Country country){	
	//Overloding << to write the country's economic state
	os << country.year << "	" << country.unemployment[0]  << "	" << country.unemployment[1] << "	" \
		<< country.poverty[0] << "	" << country.poverty[1] << "	"\
		<< country.gdp_growth[0] << "	" << country.gdp_growth[1] << "	"\
		<< country.inflation[0] << "	" << country.inflation[1] << "	" << country.cand_per_year; 
	return os;
}



struct Election{
	// An election has a set of candidates and a economic context
	vector <Candidate> candidates;
	vector <float> results;
	Country country;

	Election(){}
	Election( Country argentina, vector<Candidate> candi, vector <float> results){
		country = argentina;
		candidates = candi;
		results = results;
	}
};

void LoadData( vector <Election> & training, fstream & mycandidate, fstream  & mycountry ); // Load the training data set
void DataAugmentation( vector <Election> & training );// Data aumentation
void SaveElection(Election elect, fstream & myresults); // Saves the results of a simulated election to a txt file
void SaveError(ofstream & myfile, int i, float a, float b);	// Tracks at each iteration the error and saves it to a .txt file
void ReadData( Country & argentina, vector<Candidate> & candis); // Uses console I/O to read an election input data



void LoadData( vector <Election> & training, fstream & mycandidate, fstream & mycountry ){
	Country argentina;
	std::string line;
	std::getline(mycountry, line);
	std::getline(mycandidate, line);

	while (!mycandidate.eof() && !mycountry.eof()){
	// First loads economic data and number of candidates
		mycountry >> argentina.year;	

		mycountry >> argentina.unemployment[0];
		mycountry >> argentina.unemployment[1];
		
		mycountry >> argentina.poverty[0];	
		mycountry >> argentina.poverty[1];

		mycountry >> argentina.gdp_growth[0];
		mycountry >> argentina.gdp_growth[1];
		
		mycountry >> argentina.inflation[0];
		mycountry >> argentina.inflation[1];	

		mycountry >> argentina.cand_per_year;

		if (argentina.cand_per_year <0 ){cout << "Error. Corrupted data base.\n"; return;}

		if (argentina.unemployment[0] <0 || argentina.unemployment[0] >1 \
			|| argentina.unemployment[1] <0 || argentina.unemployment[1] >1 \
			|| argentina.poverty[0] <0 || argentina.poverty[0] >1 \
			|| argentina.poverty[1] <0 || argentina.poverty[1] >1 \
			|| argentina.gdp_growth[0] <-1 || argentina.gdp_growth[1] <-1 \
			|| argentina.inflation[0] <-1  || argentina.inflation[1] <-1){
			cout << "Error reading economic data. Corrupted data base.\n"; return;
		}
		vector <Candidate> candi; candi.resize(argentina.cand_per_year);
		vector <float> results;	results.resize(argentina.cand_per_year);
		
		for (int i = 0; i < argentina.cand_por_ano; ++i){
		// Loads candidates data
			Candidate aux;	
			
			mycandidate >> candi[i].year;
			mycandidate >> candi[i].ideology;
			mycandidate >> candi[i].power;
			mycandidate >> candi[i].image;
			mycandidate >> candi[i].polling;
			mycandidate >> results[i];

			if ( candi[i].year != argentina.year \
				|| candi[i].ideology <0 || candi[i].ideology >1 \
				|| (candi[i].power !=0 && candi[i].poder != 1) \
				|| candi[i].power <0 || candi[i].power >1  \
				|| candi[i].polling <0 || candi[i].polling >1 \
				|| results[i] <0 || results[i] >1 ){
				cout << argentina.ano<<"Error reading cadidates. Corrupted data base.\n"; return;
			}
		}

		Election aux(argentina, candi, results);
		training.push_back(aux); // Guarda en el set de entrenamieto la eleccion
		candi.clear();
		std::getline(mycandidate, line);
	}
}



void DataAugmentation( vector< Election > & training){ 
	//Data aumentation
	float r,s;
	std::random_device rd;
	std::default_random_engine generator;
	generator.seed( rd() ); //Now this is seeded differently each time.
	std::normal_distribution<double> distribution(0,0.1);
	int T = entrenamiento.size();
	
	for (int j = 0; j < 5	; ++j){
		Election aux;
		for (int i = 0; i < T; ++i){
			r = distribution(generator);
			s = distribution(generator);
			aux = training[i];
			aux.country.gdp_growth[0] *= (1. + r);	aux.country.gdp_growth[1] *= (1. + s);
			aux.country.unemployment[0] *= (1. - r/2);	aux.country.unemployment[1] *= (1. - s/2);
			aux.country.poverty[0] *= (1. - r/3);	aux.country.poverty[1] *= (1. - s/3);
			// Economic correlations show that  for each point in GDP growth
			// poverty is reduce 0.3% and unemployment 0.5%
			float total=0;
			for (int k = 0; k < aux.candidates.size(); ++k){
				if (aux.candidates[k].power){
					aux.candidates[k].polling *= (1. + r);
					aux.candidates[k].image *= (1. + r); 
					aux.results[k] *= (1. + r);
				}
				else{
					aux.candidates[k].polling *= (1. - r);
					aux.candidates[k].image *= (1. - r);
					aux.results[k] *= (1. - r);			
				}
				total += aux.results[k];
			}

			for (int k = 0; k < aux.candidates.size(); ++k){
				aux.results[k] /= total;
			}
			training.push_back(aux);
		}
	}
}


void SaveElection(Election elect, fstream & myresults){
// Saves a simulated election's results to a .txt file
	myresults << "Year	Unemploy	Prev_unemp	Poverty	Prev_poverty	GDP_gro	Prev_GDP_gro	Inf	Prev_In	Num_Cands \n";
	myresults << elect.country << endl << endl;
	myresults << "Year	Ideology	Poeer	Image	Polling	Result \n";
	for (int i = 0; i < elect.country.cand_per_year; ++i){
		myresults << elect.candidates[i] << "	" << elect.results[i] << endl;
	}
	myresults << endl << endl;
}


void SaveError(ofstream & myfile, int i, float a, float b){
	// Tracks error 
	myfile << i << "	" << (a + b)/2. << "	" << endl;
}



Election ReadData(){
	Election elect;
	// Uses console I/O to read a election data
	cout << "Input election year:" << endl;
	cin >> elect.country.year;
	// Loads economic data
	cout << "Enter election's year unemployment rate (normalized to 1):" << endl;
	cin >> elect.country.unemployment[0];
	while ( elect.country.unemployment[0]< 0 || elect.country.unemployment[0]> 1){
		cout << "Impossible. Try again." << endl;
		cin >> elect.country.unemployment[0];
	}
	cout << "Enter unemployment rate (normalized to 1) on the previous year:" << endl;
	cin >> elect.country.desempleo[1];
	while ( elect.country.unemployment[1]< 0 || elect.country.unemployment[1]> 1){
		cout << "Impossible. Try again." << endl;
		cin >> elect.country.unemployment[1];
	}
	
	cout << "Enter election's year poverty rate (normalized to 1):" << endl;
	cin >> elect.country.poverty[0];
	while (  elect.country.poverty[0]< 0 || elect.country.poverty[0]> 1 ){
		cout << "Impossible. Try again." << endl;
		cin >> elect.country.poverty[0];
	}
	cout << "Enter poverty rate (normalized to 1) on the previous year:" << endl;
	cin >> elect.country.poverty[1];
	while ( elect.country.poverty[1]< 0 || elect.country.poverty[1]> 1){
		cout << "Impossible. Try again." << endl;
		cin >> elect.country.poverty[1];
	}
	
	cout << "Enter election's year GDP growth rate (normalized to 1):" << endl;
	cin >> elect.country.gdp_growth[0];
	while ( elect.country.gdp_growth[0]< -1 ){
		cout << "Impossible. Try again." << endl;
		cin >> elect.country.gdp_growth[0];
	}
	cout << "Enter GDP growth rate (normalized to 1) on the previous year:" << endl;
	cin >> elect.country.gdp_growth[1];
	while ( elect.country.gdp_growth[1]< -1 ){
		cout << "Impossible. Try again." << endl;
		cin >> elect.country.gdp_growth[1];
	}
	
	cout << "Enter election's year inflation rate (normalized to 1):" << endl;
	cin >> elect.country.inflation[0];
	while ( elect.country.inflation[0]< -1 ){
		cout << "Impossible. Try again." << endl;
		cin >> elect.country.inflac[0];
	}
	cout << "Enter inflation rate (normalized to 1) on the previous year:" << endl;
	cin >> elect.country.inflation[1];
	while ( elect.country.inflation[1]< -1 ){
		cout << "Impossible. Try again." << endl;
		cin >> elect.country.inflation[1];
	}
	
	cout << "Please, enter the number of candidates competing on this election:" << endl;
	cin >> elect.country.cand_por_ano;
	
	elect.candidates.resize( elect.country.cand_per_year );
	elect.resultados.resize (elect.country.cand_per_year);
	for (int i = 0; i < elect.country.cand_per_year; ++i){	
		//Load each candidate's data
		elect.candidates[i].year = elect.country.year;
		cout << "Enter the candidates ideological score (0 left, 1 right):" << endl;
		cin >> elect.candidates[i].ideology;
		while ( elect.candidates[i].ideology < 0 || elect.candidates[i].ideology>1 ){
			cout << "Impossible. Try again." << endl;
			cin >> elect.candidates[i].ideology;
		}
	
		cout << "Is the candidate's party in power? 1 Yes, 0 NO:" << endl;
		cin >> elect.candidates[i].power;
	
		cout << "What is the candidate's favourability rating?" << endl;
		cin >> elect.candidates[i].image;
		while ( elect.candidates[i].image < 0 || elect.candidates[i].image > 1 ){
			cout << "Impossible. Try again." << endl;
			cin >> elect.candidates[i].image;
		}
	
		cout << "Enter the candidates polling numbers (normalized to 1):" << endl;
		cin >> elect.candidates[i].polling;
		while ( elect.candidates[i].polling < 0 || elect.candidates[i].polling> 1 ){
			cout << "Impossible. Try again." << endl;
			cin >> elect.candidates[i].polling;
		}
	}
	return elect;
}



