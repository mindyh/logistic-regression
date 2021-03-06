#include<string>
#include<iostream>
#include<fstream>
#include<math.h>
using namespace std;

#define NUM_CLASSIFICATIONS 2
#define NUM_INDICATOR_OUTCOMES 2
#define NUM_EPOCHS 10000
#define ETA .000001

int getResult(ifstream &file){
	string line;
	getline(file, line);
	return line[line.length() - 1] - '0'; // last character on the line, guaranteed. Convert from char to int
}
void initArray(double *arr, int numElem, int value)
{
	for(int i = 0; i < numElem; i++) arr[i] = value;
}

double calcZ(double *betaVector, int *currVector, int numVariables)
{
	double z = 0;
	for(int i = 0; i <= numVariables; i++)
		z += betaVector[i]*currVector[i];

	return z;
}

/* create a 2d array from the data. arr[vector][data point in vector] */
int ** readData(int *resultsVector, int numVariables, int numVectors, ifstream &file)
{
	/* declare */
	int **data = new int*[numVectors];
	data[0] = new int[numVectors*(numVariables+1)];
	for(int i = 0; i < numVectors; i++)
		data[i] = data[0] + i * (numVariables+1);

	/* initialize */
	for(int i = 0; i < numVectors; i++)
	{
		data[i][0] = 1; //choose x_0 to be 1
		for(int j = 1; j <= numVariables; j++)
			file >> data[i][j];
		
		resultsVector[i] = getResult(file);
	}

	return data;
}

void trainModel(double *betaVector, int numVariables, ifstream &file)
{
	int numVectors; file >> numVectors;
	double *gradient = new double[numVariables + 1];
	int *resultsVector = new int[numVectors];
	int **dataArray = readData(resultsVector, numVariables, numVectors, file);

	/* compute beta */
	for (int i = 0; i < NUM_EPOCHS; i++) {
		initArray(gradient, numVariables + 1, 0);
		// Compute "batch" gradient vector
		for(int j = 0; j < numVectors; j++)
		{			
			// Add contribution to gradient for each data point
			double z = calcZ(betaVector, dataArray[j], numVariables);
			for (int k = 0; k <= numVariables; k++)
				gradient[k] += dataArray[j][k]*(resultsVector[j] - 1.0/(1.0 + exp(-z)));
		}
		// Update all Beta[j]
		for(int j = 0; j <= numVariables; j++)
			betaVector[j] += ETA * gradient[j];	
	}

	/* cleanup */
	delete []resultsVector;
	delete []dataArray;
	delete []gradient;
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
	cout << "On " << test << ":" << endl;
	printResults(numTestedPerClass, numCorrectPerClass);

	 /* cleanup */
	delete []betaVector;
	 trainFile.close();
	 testFile.close();
}



int main()
{
	/* simple */
	runTest("datasets/simple-train.txt", "datasets/simple-test.txt");

	/* vote */
	runTest("datasets/vote-train.txt", "datasets/vote-test.txt");

	/* heart */
	runTest("datasets/heart-train.txt", "datasets/heart-test.txt");

	getchar(); // prevent command window from closing
	return 0;
}
