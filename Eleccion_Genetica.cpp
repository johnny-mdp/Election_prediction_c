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

//g++ -std=c++11 Genetic_Election.o Genetic_Election.cpp



// Sorting function to compare performance of different NNs
bool Order(Neural_Network arg1, Neural_Network arg2){
	if ( arg1.GetTotalError() > arg2.GetTotalError() )
		return false;
	return true;
}



int main(){
	char l;
	Neural_Network final_net; // Red neuronal con la que se predicirá el resultado de la Electiones
	cout << "Do you have an already trained NN? y/n" << endl;
	cin >> l;		// In case wealready have a trained NN
	l = std::tolower(l);
	while ( l != 'y' && l != 'n'){
		cout << "Incorrect, try again." << endl;
		cin >> l;
	}
	
	if ( l == 'y'){
		LoadNet(final_net);	// Load a NN's parameters
		final_net.Predict();	// Function that predicts an election
		return 0;
	}

	fstream mycandidate, mycountry;
	string path_arch;
	cout << "Enter file path to the candidate data:" << endl;
	
	cin >> path_arch;
	mycandidate.open(path_arch);	
	while (mycandidate.fail()){
		cout << "Error, try again: \n";
		cin >> path_arch;
		mycandidate.open(path_arch);
	}

	cout << "Enter file path to the economic data:" << endl;
	
	cin >> path_arch;
	mycountry.open(path_arch);
	while (mycountry.fail()){
		cout << "Error, try again:\n";
		cin >> path_arch;
		mycountry.open(path_arch);
	}


	vector < Election > traning;	// training data set
	LoadData(traning, mycandidate, mycountry); 
	if (traning.size()==0){return 1;}

	mycandidate.close(); mycountry.close();

	int total_population = 6;		// Population size
	int generations = 100000;		// Max number of iterations
	vector < Neural_Network > population;  
	vector <int> sizes;	// Number of neurons on each layer of the NN
	
	sizes.push_back(12); sizes.push_back(5); sizes.push_back(5); sizes.push_back(1);

	for (int i = 0; i < total_population; ++i) {
		//creates first generation of NN
		Neural_Network aux_net( sizes );
		cout <<"Individual " << i+1<< endl;
		aux_net.InitializeParameters();
		population.push_back( red );
	}


	Neural_Network father, mother, son;	// These NNs produce the next generation
	ofstream myerror;
	myerror.open("Error.txt");
	for (int x = 0; x < generations; ++x){
		cout << "Generation num " << x << endl;		// So the user can track progress
		for (int j = 0; j < population.size(); ++j){
			population[j].Train(entrenamiento);
		}
		sort(population.begin(), population.end(), Order);	// Sorts population by error
		father = population[ 0 ];
		mother = population[ 1 ];
		population.clear();
		
		SaveError( myerror, x, father.GetTotalError()/traning.size(), mother.GetTotalError()/traning.size());

		if (father.Get_Total_Error()/traning.size() < 0.12){
			// if we attain the desired precission, training stops
			break;
		}
		
		for (int i = 0; i < total_population; ++i){
			son = Son( father, mother );
			population.push_back( son );
		}
	}
	myerror.close();

	
	final_net = father;	
	cout<< "Do you wish to save this trained NN? y/n" << endl;
	char c;
	cin >> c; c = std::tolower(c);
	while ( c != 'y' && c != 'n'){
		cout << "Incorrect, try again." << endl;
		cin >> c;
	}
	if ( c== 's' ) SaveNet(final_net);

	final_net.Predict();	// Se pasa el control a la función que predice Electiones en base a un red entrenada
	return 0;
}
