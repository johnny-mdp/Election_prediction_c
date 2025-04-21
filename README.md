# Election_prediction_c
A code that implements the genetic algorithm to train neural networks for predicting election results in Argentina using economic and political data 


Genetic Algorithm

The genetic algorithm is the method used to train the neural network on the dataset.

This method consists of creating “N” (the population) neural networks with their parameters (weights and biases) initialized randomly. The cost function of all networks in the population is then calculated using the training dataset. After that, the two networks with the lowest cost are selected as the “parents”.

A new generation of N networks is created as follows:

    For setting a given weight or bias, the parent it is inherited from is selected with a 50% probability.

    With a low probability (in our case, 0.1%), the weight or bias is replaced with a random value.

The process described in the previous two paragraphs is repeated until a maximum number of iterations is reached or a sufficiently low cost is achieved. These stopping criteria are defined at compile time—in our case, 20,000 iterations and a cost function value of 0.012 (which, given the dataset, corresponds to an average error of 3 points in the estimation of each candidate's percentage).

As a result, we obtain a neural network that can incorporate economic data and other candidate-related variables in addition to their average polling score. This allows for a more accurate estimate of an election result than using polling averages alone.
Program Usage

The purpose of the program is to predict the percentages obtained by a group of candidates competing in the same election, based on their polling average and the country's economic status.

Upon running the program, the user has two options:

    Load the weights and biases of an already trained neural network: If this option is chosen, the program proceeds to the next stage.

    Load a database: The program will ask the user to provide a dataset using two files. One contains data on the country’s economy and the other contains data on the candidates. The database must follow a defined format, which will be explained later.

If the second option is chosen, the program will load the dataset on which the neural network will be trained. To do this, it will prompt the user to input the paths of the required files.

The neural network will then be trained using the genetic algorithm described in the previous section. Afterwards, the program will ask the user whether they want to save the weights and biases of the trained network. This is done to avoid having to retrain the network every time the program is run.

In either case, the program will then offer the user the option to simulate an election. If accepted, the program will ask the user to input the country’s economic data for the election year and the previous year:

    Unemployment: fraction of the population without work (a number between 0 and 1),

    Poverty: fraction of the population that is poor (a number between 0 and 1),

    GDP growth: a number greater than -1,

    Inflation: a number greater than -1,

    Number of candidates.

Then, the user will be asked to input candidate data:

    Score: a number between 0 and 1 representing ideology (0 = far left, 1 = far right),

    Power: a boolean indicating whether the candidate’s party is in power,

    Image: the sum of positive and regular image perception of the candidate (a number between 0 and 1),

    Polling average: the candidate’s average in the polls (a number between 0 and 1).

The program will then output the predicted vote percentages for each candidate by projecting undecided voters (normalizing the percentages to 100%). It will also offer the option to save the simulation results. If chosen, the result is written to a .txt file.

Finally, the program will ask whether the user wants to simulate another election. The program continues until the user chooses to stop.
