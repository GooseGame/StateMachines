//Tokarev PS 31
#include <iostream>
#include <string>
#include "functions.h"

using namespace std;


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Usage: enter 'minimization.exe inputFile.txt outputFile.txt'" << endl;
        return 0;
    }
    ifstream input(argv[1]);
    ofstream output(argv[2]);
    if (!input) {
        cout << "input file not found" << endl;
        return 0;
    }
    if (!output) {
        cout << "output file not found" << endl;
        return 0;
    }
    int inputCharsCount, outputCharsCount, vertexCount;
    string machineType;
    input >> inputCharsCount >> outputCharsCount >> vertexCount >> machineType;

    if (machineType == "mealy")
    {
        mealyMachineMinimizer(input, output, inputCharsCount, vertexCount);
    }
    else if (machineType == "moore")
    {
        mooreMachineMinimizer(input, output, inputCharsCount, vertexCount);
    }
    else
    {
        cout << "Unknown machine" << endl;
    }

}