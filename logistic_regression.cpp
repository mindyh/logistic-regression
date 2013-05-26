#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include<math.h>
using namespace std;

#define NUM_CLASSIFICATIONS 2
#define NUM_INDICATOR_OUTCOMES 2
#define NUM_EPOCHS 10000
#define ETA .0001

int getResult(ifstream &file){
	string line;
	getline(file, line);
	cout << line << endl;
	return line[line.length() - 1] - '0'; // last character on the line, guaranteed. Convert from char to int
}
void initArray(double *arr, int numElem, int value)
{
	for(int i = 0; i < numElem; i++) arr[i] = value;
}

double calcZ(double *betaVector, int *currVector, int numVariables)
{
	double z = betaVector[0]; 
	for(int i = 0; i <= numVariables; i++) 
		z += betaVector[i]*currVector[i];

	return z;
}


void trainModel(double *betaVector, int numVariables, ifstream &file)
{
	int numVectors;
	file >> numVectors;
	double *gradient = new double[numVariables + 1];
	int *currVector = new int[numVariables + 1];
	int *resultsVector = new int[numVectors];

	for (int i = 0; i < NUM_EPOCHS; i++) {
		initArray(gradient, numVariables + 1, 0);
		// Compute "batch" gradient vector
		for(int j = 0; j < numVectors; j++)
		{
			/* get the next vector */
			currVector[0] = 1;
			for(int k = 1; k <= numVariables; k++)
				file >> currVector[k];

			/* get the binary output of the vector */
			int currBin = getResult(file);
				
			// Add contribution to gradient for each data point
			for (int k = 0; k <= numVariables; k++) {
				double z = calcZ(betaVector, currVector, numVariables);
				gradient[k] += currVector[k]*(currBin - 1.0/(1 + exp(-z)));			
			}
		}
		// Update all Beta[j]
		for(int j = 0; j <= numVariables; j++)
			betaVector[j] += ETA * gradient[j];	
	}

	cout << "got here w/o segfault" << endl;

	delete []gradient;
	delete []currVector;
}

double calcProbability(int * currVector, double *betaVector, int numVariables)
{
	double z = calcZ(betaVector, currVector, numVariables);
	return 1.0 / (1.0 + exp(-z));
}

void testModel(ifstream &file, double *betaVector, double *numTestedPerClass, double *numCorrectPerClass)
{
	int numVariables, numVectors; 
	file >> numVariables >> numVectors;
	
	int numCorrect = 0;
	int *currVector = new int[numVariables + 1];

	for(int i = 0; i < numVectors; i++) // for each vector
	{
		/* read in the vector */
		currVector[0] = 1;
		for(int j = 1; j <= numVariables; j++)
			file >> currVector[j];
		int actualResult = getResult(file);
		numTestedPerClass[actualResult]++;

		/* calculate and test result */
		int myResult = calcProbability(currVector, betaVector, numVariables) > 0.5 ? 1 : 0;
		if(myResult == actualResult) numCorrectPerClass[actualResult]++;
	}

	delete []currVector;
}
void printResults(double *numTestedPerClass, double *numCorrectPerClass)
{
	double totalTested = 0;
	double totalCorrect = 0;
	
	for (int i = 0; i < NUM_CLASSIFICATIONS; i++)
	{
		cout << "Class " << i << ": tested " << numTestedPerClass[i] << ", correctly classified " << numCorrectPerClass[i] << "." << endl;
		
		totalTested += numTestedPerClass[i];
		totalCorrect += numCorrectPerClass[i];
	}
	
	cout << "Overall: tested " << totalTested << ", correctly classified " << totalCorrect << "." << endl;
	cout << "Accuracy = " << totalCorrect/totalTested << endl << endl;
}

void runTest(const char *train, const char *test)
{
	ifstream trainFile(train);
	ifstream testFile(test);

	/* setup */
	double numTestedPerClass[NUM_CLASSIFICATIONS], numCorrectPerClass[NUM_CLASSIFICATIONS];
	initArray(numTestedPerClass, NUM_CLASSIFICATIONS, 0); initArray(numCorrectPerClass, NUM_CLASSIFICATIONS, 0);
	int numVariables;
	trainFile >> numVariables;
	double *betaVector = new double [numVariables + 1];
	initArray(betaVector, numVariables + 1, 0);
	
	/* train and test model */
	trainModel(betaVector, numVariables, trainFile);
	testModel(testFile, betaVector, numTestedPerClass, numCorrectPerClass);

	
	/* print out results */	
	cout << "For " << test << ":" << endl;
	printResults(numTestedPerClass, numCorrectPerClass);

	 /* cleanup */
	delete []betaVector;
	 trainFile.close();
	 testFile.close();
}



int main()
{
	/* simple on MLE */
	runTest("datasets/simple-train.txt", "datasets/simple-test.txt");
	///* simple on Laplace */
	//runTest("datasets/simple-train.txt", "datasets/simple-test.txt", IS_MAP);

	///* vote on MLE */
	//runTest("datasets/vote-train.txt", "datasets/vote-test.txt", IS_MLE);
	///* vote on Laplace */
	//runTest("datasets/vote-train.txt", "datasets/vote-test.txt", IS_MAP);

	///* heart on MLE */
	//runTest("datasets/heart-train.txt", "datasets/heart-test.txt", IS_MLE);
	///* heart on Laplace */
	//runTest("datasets/heart-train.txt", "datasets/heart-test.txt", IS_MAP);


	getchar(); // prevent command window from closing
	
	 return 0;
}
