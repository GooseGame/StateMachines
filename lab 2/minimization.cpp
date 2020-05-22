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
    if (!input || !output)
    {
        cout << "input or output file not found" << endl;
        return 0;
    }
    int inputAlphabetCount = 0, outputAlphabetCount = 0, vertexCount = 0;
    string automate;
    input >> inputAlphabetCount >> outputAlphabetCount >> vertexCount >> automate;

    if (automate == "mealy")
    {
        mealyMachineMinimizer(input, output, inputAlphabetCount, vertexCount);
    }
    else if (automate == "moore")
    {
        mooreMachineMinimizer(input, output, inputAlphabetCount, vertexCount);
    }
    else
    {
        cout << "Unknown automate. Try again" << endl;
    }

}
