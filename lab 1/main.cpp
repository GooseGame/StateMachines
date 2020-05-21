//Visual Studio 19, Tokarev Andrey, PS 31
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>

using namespace std;

const string VERTEX_LABEL = "q";
const string EDGE_LABEL = "x";
const string MOORE_TYPE = "moore";
const string MEALY_TYPE = "mealy";



struct Moore
{
	vector<int> inputs;
	vector<string> outputs;
	vector<vector<int>> graph;
};

struct Mealy
{
	vector<vector<string>> graph;
	vector<string> states;
};

struct VertexProps
{
	string label;
};

struct EdgeProps
{
	string label;
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProps, EdgeProps>;

void graphOutput(Graph graph, string type) {
	boost::dynamic_properties dp;
	dp.property("label", boost::get(&VertexProps::label, graph));
	dp.property("label", boost::get(&EdgeProps::label, graph));
	dp.property("node_id", boost::get(boost::vertex_index, graph));
	ofstream ofs;
	if (type == MEALY_TYPE) {
		ofs.open("mealy.dot");
	}
	else if (type == MOORE_TYPE) {
		ofs.open("moore.dot");
	}
	boost::write_graphviz_dp(ofs, graph, dp);
}

void CreateMoore(Moore& moore, map<string, string>& automatStates)
{
	Graph graph;
	vector<Graph::vertex_descriptor> vertices;
	map<string, string>::iterator it = automatStates.begin();
	for (size_t i = 0; i < moore.graph[0].size(); ++i)
	{
		string vertexLabel = VERTEX_LABEL + to_string(moore.inputs.at(i)) + moore.outputs[i];
		map<string, string>::iterator it = automatStates.find(vertexLabel);
		vertexLabel = it->second + (moore.outputs[i]);
		vertices.push_back(boost::add_vertex({ vertexLabel }, graph));
	}

	for (size_t i = 0; i < moore.graph.size(); ++i)
	{
		for (size_t j = 0; j < moore.graph[0].size(); ++j)
		{
			string edgeLabel = EDGE_LABEL + to_string(i + 1);
			boost::add_edge(vertices[j], vertices[moore.graph[i][j]], { edgeLabel }, graph);
		}
	}
	graphOutput(graph, MOORE_TYPE);
}

void CreateMealy(Mealy& mealy, map<string, string>& automatStates)
{
	Graph graph;
	vector<Graph::vertex_descriptor> vertices;
	map<string, string>::iterator it = automatStates.begin();
	for (size_t i = 0; i < mealy.graph[0].size(); ++i)
	{
		string vertexLabel = VERTEX_LABEL + to_string(i);
		map<string, string>::iterator it = automatStates.find(vertexLabel);
		vertices.push_back(boost::add_vertex({ vertexLabel }, graph));
	}

	for (size_t i = 0; i < mealy.graph.size(); ++i)
	{
		for (size_t j = 0; j < mealy.graph[0].size(); ++j)
		{
			string edgeLabel = EDGE_LABEL + to_string(i + 1) + "/" + mealy.graph[i][j].substr(2, 3);
			boost::add_edge(vertices[j], vertices[stoi(mealy.graph[i][j].substr(1, 1))], { edgeLabel }, graph);
		}
	}
	graphOutput(graph, MEALY_TYPE);
}
using namespace std;

const string MOORE_VERTEX_CHAR = "q";
const string MEALY_VERTEX_CHAR = "Z";
const string MOORE_AUTOMAT = "moore";
const string MEALY_AUTOMAT = "mealy";

void createMoore(int inputCharsCount, int vertexCount, ifstream& inputFile, ofstream& outputAutomat, string** outputTable)
{
	map<string, string> states;
	map<string, string>::iterator iteratorOfStatesMap = states.begin();
	Moore moore;
	vector<string> vertexes;
	string vertexValue;

	for (int i = 0; i < inputCharsCount; i++) //fill source table
	{
		for (int j = 0; j < vertexCount; j++)
		{
			inputFile >> outputTable[i][j];
			vertexes.push_back(outputTable[i][j]);
		}
	}

	int iteratorOfNewValue = 0;
	for (int i = 0; i < inputCharsCount; i++)
	{
		for (int j = 0; j < vertexCount; j++)
		{
			iteratorOfStatesMap = states.find(outputTable[i][j]);
			if (iteratorOfStatesMap == states.end())
			{
				states.insert(make_pair(outputTable[i][j], MEALY_VERTEX_CHAR + to_string(iteratorOfNewValue)));
				iteratorOfNewValue++;
			}
		}
	}
	//
	iteratorOfStatesMap = states.begin();
	for (int i = 0; iteratorOfStatesMap != states.end(); iteratorOfStatesMap++, i++) {
		iteratorOfStatesMap->second = MEALY_VERTEX_CHAR + to_string(i);
		moore.inputs.push_back(stoi(iteratorOfStatesMap->first.substr(1, 1)));
		moore.outputs.push_back(iteratorOfStatesMap->first.substr(2, 3));
		cout << i << ") Key " << iteratorOfStatesMap->first << ", value " << iteratorOfStatesMap->second << endl;

	}
	
	int index = 0;

	for (int i = 0; i <= inputCharsCount; i++)
	{
		iteratorOfStatesMap = states.begin();
		if (i != 0)
		{
			moore.graph.push_back({});
		}
		for (int j = 0; j < states.size(); j++)
		{
			if (i == 0)
			{
				outputAutomat << iteratorOfStatesMap->first.substr(iteratorOfStatesMap->first.find('y')) << ' ';
				iteratorOfStatesMap++;
			}
			else if (i == 1)
			{
				index = stoi(iteratorOfStatesMap->first.substr(iteratorOfStatesMap->first.find('q')+1, 1));
				outputAutomat << states[vertexes.at(index)] << ' ';
				moore.graph[0].push_back(stoi(states[vertexes.at(index)].substr(1, 1)));
				iteratorOfStatesMap++;
			}
			else
			{
				index = stoi(iteratorOfStatesMap->first.substr(iteratorOfStatesMap->first.find('q')+1, 1));
				outputAutomat << states[vertexes.at(index + vertexCount * (i - 1))] << ' ';
				moore.graph[i - 1].push_back(stoi(states[vertexes.at(index + vertexCount * (i - 1))].substr(1, 1)));
				iteratorOfStatesMap++;
			}
		}
		outputAutomat << endl;
	}
	CreateMoore(moore, states);
}
void createMealy(int inputCharsCount, int vertexCount, ifstream& inputFile, ofstream& outputAutomat, string** outputTable)
{
	string vertexValue;
	string outputChar;
	string inputChar;
	map<string, string> states;
	Mealy mealy;
	for (int i = 0; i < vertexCount; i++)
	{
		vertexValue = MOORE_VERTEX_CHAR;
		vertexValue += to_string(i);
		states.insert(make_pair(vertexValue, ""));
	}

	map<string, string>::iterator it = states.begin();
	for (int i = 0; i < vertexCount; i++, it++) // add input symbols to vertexes
	{
		inputFile >> outputChar;
		it->second = outputChar;
	}

	for (int i = 0; i < inputCharsCount; i++) //fill output
	{
		mealy.graph.push_back({});
		for (int j = 0; j < vertexCount; j++)
		{
			inputFile >> inputChar;
			it = states.find(inputChar);
			outputTable[i][j] = it->first + it->second;
			mealy.graph[i].push_back(outputTable[i][j]);
			outputAutomat << outputTable[i][j] << ' ';
		}
		outputAutomat << endl;
	}
	CreateMealy(mealy, states);
}


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << "Usage: enter 'minimization.exe inputFile.txt outputFile.txt'" << endl;
		return 0;
	}
	int inputCharsNum = 0, outputCharsNum = 0, vertexNum = 0;
	string inputChar = "", outputChar = "", vertexValue = "", automatType = "";

	ifstream input(argv[1]);
	ofstream output(argv[2]);
	if (!input || !output)
	{
		cout << "input or output file not found" << endl;
		return 0;
	}

	input >> inputCharsNum >> outputCharsNum >> vertexNum >> automatType;

	string** outputTable = new string * [inputCharsNum];
	for (int i = 0; i < inputCharsNum; i++)
	{
		outputTable[i] = new string[vertexNum];
	}

	if (automatType == MOORE_AUTOMAT)
	{
		createMealy(inputCharsNum, vertexNum, inputFile, output, outputTable);
	}
	else if (automatType == MEALY_AUTOMAT)
	{
		createMoore(inputCharsNum, vertexNum, inputFile, output, outputTable);
	}
	else
	{
		output << "Unknown automat" << endl;
	}
}
