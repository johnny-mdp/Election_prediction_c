#include <vector> 
#include <random>
#include "Datos.h"
using namespace std;



vector <float> Sigmoid( vector <float> aux);



class Cost_function{
public:
	float delta(float a, float y){
		if (a<0 || a> 100)
			return abs(a-y)*100;	
		// Heavily penalize predictions outise the 0-1 range
		return abs(a-y);
	}
};


class Neural_Network : public Cost_function
{	
	vector < vector < vector <float> > > weights;	
	//The NN's weights, a vector of matrices
	vector < vector <float> > bias; // The NN's biases
	public:
		float total_error;	// Cost parameter that evaluates the goodness of fit
		vector <int> sizes;	// Number of neurons per layer

		Neural_Network(){ total_error = 0;	}
		Neural_Network(  vector <int> sizes){
			total_error = 0;
			sizes = sizes;
		}
		~Neural_Network(){}

		void InitilizeParameters();	// Initializes the weights and biases of each NN
		void Train(vector <Election> training);	// Trains the NN 1 epoch
		double FeedForward(Candidate candidate, Pais country);	
		float GetTotalError() { return total_error;}	// Gets NN's total error
		
		float GetWeigth(int i, int j, int k){ return weights[i][j][k];}		
		void SetWeigth(int i, int j, int k, float value){ weights[i][j][k] = value;}
		float GetBias(int i, int j){ return bias[i][j];}
		void SetBias(int i, int j, float value){ bias[i][j] = value;}
		void Predict();		// Once trained the NN can predic elections
		void CalculateResults( Election & elect, double & norm);


		friend void SaveNet(Neural_Network final_net);		// Saves weights and biases of the NN to a .txt file
		friend void LoadNet(Neural_Network & final_net);	// Reads weights and biases of the NN from a .txt file
		friend Neural_Network Son(Neural_Network father, Neural_Network mother); // Creates the next generation of NN
};



void Neural_Network::InitilizeParameters(){
	weights.resize( sizes.size()-1 );
	bias.resize( sizes.size()-1 ); 
	auto start = high_resolution_clock::now(); 

	std::random_device rd;
	std::default_random_engine generator;
	generator.seed( rd() );
	std::normal_distribution<double> distribution(0,1);
	for (int i = 0; i < (sizes.size()-1); ++i){
		int T = sizes[i+1];
		int S = sizes[i];
		float aux[T][S];
		float aux_bias[T];
		weights[i].resize( T );
		bias[i].resize( T );
		for (int j = 0; j < T; ++j){
			weights[i][j].resize( S );
			for (int k = 0; k < S; ++k){
				weights[i][j][k] = distribution(generator);
			}
		}

		for (int k = 0; k < T; ++k){
			bias[i][k] = distribution(generator);
		}
	}
}



void Neural_Network::Train( vector <Election> training){
	double result_pred; 
	total_error = 0;
	for (int i = 0; i < entrenamiento.size(); ++i){
		for (int j = 0; j < entrenamiento[i].candidates.size(); ++j){	
			// Calculates the election's result and the error compared to the actual result
			result_pred = Feed_forward(training[i].candidates[j], training[i].country);
			total_error += delta(training[i].results[j],result_pred);	
		}
	}
}



double Neural_Network::FeedForward( Candidate candidate, Country country){
	vector <float> input; input.resize(tamanos[0]);
	// Input, 1 candidate and the country's economic status
	input[0] = candidate.ideology;	input[1] = candidate.power;
	input[2] = candidate.polling;	input[3] = candidate.image;
	input[4] = country.poverty[0];	input[5] = country.poverty[1];	
	input[6] = country.unemployment[0];	input[7] = country.unemployment[1];
	input[8] = country.gdp_growth[0];	input[9] = country.gdp_growth[1];	
	input[10] = country.inflation[0];	input[11] = country.inflation[1];
	
	for (int i = 0; i < sizes.size()-1; i++){
		vector <float> aux; aux.resize(sizes[i+1], 0);
			
		for (int j = 0; j < sizes[i+1]; ++j){	
			for (int k = 0; k < sizes[i]; ++k){
				aux[j] += weights[i][j][k]*input[k];
			}
			aux[j] += bias[i][j];
		}
		input.clear();
		input = Sigmoid(aux);
	}
	// Returns an estimate of the candidate's vote share
	return input[0]; 
}


void Neural_Network::Predict(){
	Election elect;
	double norm = 0;	// (projects undecided voters
	char c;
	cout << "Do you wish to simulate an election? y/n" << endl;
	cin >> c;

	fstream myresults;	// Guardará el resultado de la simulacion de la eleccion
	char name[] = "Simulation_results.txt";
	myresults.open(name,fstream::out);
	const auto begin = myresults.tellg();// servirá para decir si el archivo de resultados está vacio

	c = std::tolower(c);
	while (c == 'y' ){	
		elect = ReadData();

		CalculateResults(elect, norm);

		for (int i = 0; i < elect.country.cand_per_year; ++i){
			elect.results[i] = elect.results[i]/ norm;//Distributes undecided voters
			cout << "Candidate " << i+1 << "  would get " << elect.resultados[i]*100 << "% of the votes."<< endl;
		}

		cout <<"Save results? y/n" << endl;
		char guardar = 'y';
		cin >> guardar;
		guardar = std::tolower(guardar);
		while ( guardar != 'y' && guardar != 'n'){
			cout << "Incorrect, try again." << endl;
			cin >> guardar;
		}
		if( guardar == 'y'){
			SaveElection(elect, myresults);
		}

		cout <<"Simulate another election? y/n" << endl;
		cin >> c;
		c = std::tolower(c);
		while ( c != 'y' && c != 'n'){
			cout << "Incorrect, try again." << endl;
			cin >> c;
		}
		norm = 0;
	}
	// Si el archivo donde se guardarían resultados de las simulaciones esta vacio, lo borra
	myresults.seekg (0, ios::end);	const auto end = myresults.tellg(); const auto fsize = (end-begin);
	if ( fsize == 0){
		int i = remove(name);
		if (i == 0){
			cout << "Unused file succesfully deleted.\n";
		}
		else{
			cout << "ERROR. Could not delete file. \n";
		}
	}
	myresults.close();
}


void Neural_Network::CalculateResults(Election & elect, double & norm){
	for (int i = 0; i < elect.country.cand_per_year; ++i){
		elect.results[i] = FeedForward(elect.candidates[i], elect.country);// calcula el resultado del candidato
		norm += elect.results[i];
	}
}



void SaveNet(Neural_Network final_net){
	ofstream mysave;
	mysave.open("weights_and_biases.txt");

	mysave << final_net.sizes.size(); // How many layers does the NN have
	mysave << endl <<endl;
	for (int i = 0; i < final_net.sizes.size(); ++i){
		// The number of neurons on each layer
		mysave << final_net.sizes[i] << "	";
	}
	mysave << endl << endl;

	for (int i = 0; i < (final_net.sizes.size()-1); ++i){
		int T = final_net.sizes[i+1];
		int S = final_net.sizes[i];
		for (int j = 0; j < T; ++j){
			for (int k = 0; k < S; ++k){
				mysave << final_net.weights[i][j][k] << "	";
			}
			mysave << endl;
		}
	}

	for (int i = 0; i < (final_net.sizes.size()-1); ++i){
		int T = final_net.sizes[i+1];
		int S = final_net.sizes[i];
		for (int k = 0; k < T; ++k){
			mysave << final_net.bias[i][k]<< "	";
		}
	}
	mysave.close();
}



void LoadNed(Neural_Network & final_net){
	fstream myload;
	string path_arch;
	cout << "Enter path to the file with the weights and biases of a trained NN:" << endl;
	cin >> path_arch;
	myload.open(path_arch);
	while (myload.fail()){	// Sino encuentra el archivo inisiste
		cout << "Try again: \n";
		cin >> path_arch;
		myload.open(path_arch);
	}

	std::string line;
	int a;
	double b;

	myload >> a;
	std::getline(myload, line);
	int t = a; // Ahora se sabe el nro de capas

	for (int i = 0; i < t; i++){
		myload >> a;
		final_net.sizes.push_back(a);
	}
	cout << "Read sizes \n";
	std::getline(myload, line);
	final_net.weights.resize(t-1);
	final_net.bias.resize(t-1);

	//ARead weigths
	for (int i = 0; i < (final_net.sizes.size()-1); ++i){
		int T = final_net.sizes[i+1];
		int S = final_net.sizes[i];
		
		final_net.weights[i].resize( T );
		for (int j = 0; j < T; ++j){
			final_net.weights[i][j].resize( S );
			for (int k = 0; k < S; ++k){
				myload >> b;
				final_net.weights[i][j][k] = b;
			}
		}
	}
	cout << "Read weights.\n";
	std::getline(myload, line);

	for (int i = 0; i < (final_net.sizes.size()-1); ++i){
		int T = final_net.sizes[i+1];
		float aux_bias[T];
		final_net.bias[i].resize( T );

		for (int k = 0; k < T; ++k){
			myload >> b;
			final_net.bias[i][k] = b;
		}
	}
	cout << "Read biases \n";
	myload.close();
}



Neural_Network Son(Neural_Network father, Neural_Network mother){
	std::random_device rd;
	std::default_random_engine generator;
	generator.seed( rd() ); //Now this is seeded differently each time.
	std::normal_distribution<double> distribution(0,1);	
	
	father.total_error = 0;
	for (int layer = 0; layher < father.sizes.size()-1; ++layer){
		for (int j = 0; j < father.sizes[capa+1]; ++j){
			if ( distribution(generator) < 0 ){
				// A weigth is inherited from each parent with a 50% chance
				father.bias[capa][j] = mother.bias[capa][j];
			}	
			if  ( abs( distribution(generator) ) < 0.001){
				// Random mutations
				father.bias[capa][j] = distribution(generator);
			}

			for ( int k=0; k<father.tamanos[capa] ; k++ ){
				if ( distribution(generator) < 0){
					father.weights[capa][j][k] = mother.weights[capa][j][k];
				}	
				if  ( abs(distribution(generator)) < 0.001){
					//cout <<"Bias evoluciona al azar" << endl;
					father.weights[capa][j][k] = distribution(generator);
				}
			}
		}
	}
	return father;
}


vector <float> Sigmoid( vector <float> aux){
	for (int i = 0; i < aux.size(); ++i)	{
		aux[i] =  1./(1. + pow(2.71828, -aux[i]) ) ;
	}
	return aux;
}
