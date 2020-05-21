#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <algorithm>

#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/iteration_macros.hpp>

using namespace std;

const string OUTPUT_STATE_CHAR = "A";

struct StateTypeMachine
{
    vector<int> inputs;
    vector<string> outputs;
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

void CreateMooreGraph(StateTypeMachine& mooreMachine)
{
    Graph graph;
    vector<Graph::vertex_descriptor> vertices;
    for (size_t i = 0; i < mooreMachine.graph.size(); i++)
    {
        string vertexLabel = "A" + to_string(i) + mooreMachine.graph.at(i).at(1);
        vertices.push_back(boost::add_vertex({ vertexLabel }, graph));
    }
    for (size_t i = 0; i < mooreMachine.graph.size(); i++)
    {
        for (size_t j = 2; j < mooreMachine.graph.at(0).size(); j++)
        {
            string edgeLabel = 'x' + to_string(j - 1);
            boost::add_edge(vertices[i], vertices[stoi(mooreMachine.graph[i][j].substr(1, 1))], { edgeLabel }, graph);
        }
    }

    boost::dynamic_properties dp;
    dp.property("label", boost::get(&VertexProps::label, graph));
    dp.property("label", boost::get(&EdgeProps::label, graph));
    dp.property("node_id", boost::get(boost::vertex_index, graph));
    ofstream ofs("moore.dot");
    boost::write_graphviz_dp(ofs, graph, dp);

}

void CreateMealyGraph(StateTypeMachine& mealyMachine)
{
    Graph graph;
    vector<Graph::vertex_descriptor> vertices;
    for (size_t i = 0; i < mealyMachine.graph[0].size() - 1; ++i)
    {
        string vertexLabel = "A" + to_string(i);
        vertices.push_back(boost::add_vertex({ vertexLabel }, graph));
    }

    for (size_t i = 1; i < mealyMachine.graph[0].size(); i++)
    {
        for (size_t j = 0; j < mealyMachine.graph.size(); j++)
        {
            string edgeLabel = 'x' + to_string(i) + "/" + mealyMachine.graph[j][i].substr(2);
            boost::add_edge(vertices[j], vertices[stoi(mealyMachine.graph[j][i].substr(1, 1))], { edgeLabel }, graph);
        }
    }

    boost::dynamic_properties dp;
    dp.property("label", boost::get(&EdgeProps::label, graph));
    dp.property("label", boost::get(&VertexProps::label, graph));
    dp.property("node_id", boost::get(boost::vertex_index, graph));
    ofstream ofs("mealy.dot");
    boost::write_graphviz_dp(ofs, graph, dp);
}

class MealyAutomate
{
public:
    vector<vector<string>> sourceGraph;
	vector<vector<string>> OutSignals;
	map<string, vector<vector<string>>> classes;
	map<string, vector<vector<string>>> sourceClasses;
	map<string, vector<vector<string>>> modifiedClasses;
	map<string, vector<vector<string>>> finalClasses;
	vector<vector<string>> output;
};

class MooreAutomate
{
public:
	vector<vector<string>> sourceGraph;
	map<string, vector<vector<string>>> classes;
	map<string, vector<vector<string>>> finalClasses;
	vector<vector<string>> output;
};

vector<string> getVectorWithOutputSignals(vector<string> sourceVector, int begIndex)
{
	vector<string> result = {};
	for (int i = begIndex; i < sourceVector.size(); i++)
	{
		result.push_back(sourceVector.at(i).substr(2));
	}

	return result;
}

vector<string> getVectorWithClassNames(vector<string> sourceVector, int begIndex)
{
	vector<string> result = {};
	for (int i = begIndex; i < sourceVector.size(); i++)
	{
		result.push_back(sourceVector.at(i));
	}

	return result;
}

void mealyMachineMinimizer(ifstream& inputFile, ofstream& outputFile, int inputAlphabetCount, int vertexCount)
{
	string state;
	MealyAutomate MealyAutomate;
	vector<vector<string>> graph;
	for (int i = 0; i <= inputAlphabetCount; i++)
	{
		graph.emplace_back();
		MealyAutomate.OutSignals.emplace_back();
		for (int j = 0; j < vertexCount; j++)
		{
			if (i != 0)
			{
				inputFile >> state;
				graph.at(i).push_back(state);
				MealyAutomate.OutSignals.at(i).push_back(state.substr(2));
			}
			else
			{
				MealyAutomate.OutSignals.at(i).push_back("q" + to_string(j));
				graph.at(i).push_back("q" + to_string(j));
			}
		}
	}

	vector<string> reachableGraphLine;

	bool endDeleting = false;
	while (!endDeleting)
	{
		if (reachableGraphLine.empty())
		{
			for (auto & i : graph)
			{
				string element = i.at(0).substr(0, 2);
				auto search = find(reachableGraphLine.begin(), reachableGraphLine.end(), element);
				if (search == reachableGraphLine.end())
				{
					reachableGraphLine.push_back(i.at(0).substr(0, 2));
				}
			}
		}
		else
		{
			for (int i = 1; i < reachableGraphLine.size(); i++)
			{
				for (int j = 1; j < graph.size(); j++)
				{
					string element = graph.at(j).at(stoi(reachableGraphLine.at(i).substr(1, 2))).substr(0, 2);
					auto it = find(reachableGraphLine.begin(), reachableGraphLine.end(), element);
					if (it == reachableGraphLine.end())
					{
						reachableGraphLine.push_back(element);
					}
				}
			}
			endDeleting = true;
		}
	}

	for (int i = 0; i < graph.at(0).size(); i++)
	{
		bool found = false;
		for (auto & j : reachableGraphLine)
		{
			if (graph.at(0).at(i).substr(0, 2) == j)
			{
                found = true;
			}
		}

		if (!found)
		{
			for (int k = 0; k < inputAlphabetCount + 1; k++)
			{
				auto delElement = graph.at(k).begin();
				graph.at(k).erase(delElement + i);
			}
			i--;
		}
	}
	sort(reachableGraphLine.begin(), reachableGraphLine.end());

	for (int i = 0; i < graph.at(0).size(); i++)
	{
		graph.at(0).at(i) = reachableGraphLine.at(i);
	}

	MealyAutomate.sourceGraph = graph;
	int classCount = 0;

	for (int i = 0; i < MealyAutomate.sourceGraph.at(0).size(); i++)
	{
		bool foundClass = false;
		vector<string> signals;
		for (auto & j : MealyAutomate.sourceGraph)
		{
			signals.push_back(j.at(i));
		}

		auto it = MealyAutomate.classes.begin();
		if (i == 0)
		{
			vector<vector<string>> classes = { signals };
			MealyAutomate.classes.insert(make_pair(OUTPUT_STATE_CHAR + to_string(classCount), classes));
			classCount++;
		}
		else
		{
			for (int k = 0; k < it->second.size(); k++)
			{
				for (auto & classes : MealyAutomate.classes)
				{
					if (getVectorWithOutputSignals(signals, 1) == getVectorWithOutputSignals(classes.second.at(0), 1))
					{
                        foundClass = true;
					}
				}
			}
			if (!foundClass)
			{
				vector<vector<string>> classes = { signals };
				MealyAutomate.classes.insert(make_pair(OUTPUT_STATE_CHAR + to_string(classCount), classes));
				classCount++;
			}
			else
			{
				for (int l = 0; l <= classCount; l++)
				{
					auto search = MealyAutomate.classes.find(OUTPUT_STATE_CHAR + to_string(l));
					if (search != MealyAutomate.classes.end() && getVectorWithOutputSignals(search->second.at(0), 1) == getVectorWithOutputSignals(signals, 1))
					{
						search->second.push_back(signals);
					}
				}
			}
		}
	}
	
	MealyAutomate.modifiedClasses = MealyAutomate.classes;
	MealyAutomate.sourceClasses = MealyAutomate.classes;
	for (auto it = MealyAutomate.modifiedClasses.begin(); it != MealyAutomate.modifiedClasses.end(); it++)
	{
		for (int i = 0; i < it->second.size(); i++)
		{
			for (int j = 1; j < it->second.at(0).size(); j++)
			{
				for (auto & modifiedClasses : MealyAutomate.modifiedClasses)
				{
					for (int k = 0; k < modifiedClasses.second.size(); k++)
					{
						if (it->second.at(i).at(j).substr(0, 2) == modifiedClasses.second.at(k).at(0).substr(0, 2))
						{
							it->second.at(i).at(j) = modifiedClasses.first;
						}
					}
				}
			}
		}
	}
	MealyAutomate.classes.size();

	bool done = false;
	while (!done)
	{
		classCount = 0;
		int index = 0;
		vector<string> signals;
		for (auto iter = MealyAutomate.modifiedClasses.begin(); iter != MealyAutomate.modifiedClasses.end(); iter++, index++)
		{
			if (MealyAutomate.finalClasses.empty())
			{
				for (int j = 0; j < iter->second.size(); j++)
				{
					bool found = false;
					for (auto & finalClasses : MealyAutomate.finalClasses)
					{
						if (getVectorWithClassNames(iter->second.at(j), 1) == getVectorWithClassNames(finalClasses.second.at(0), 1) && j != 0)
						{
							finalClasses.second.push_back(iter->second.at(j));
                            found = true;
						}
					}
					if (!found)
					{
						vector<vector<string>> classes = { iter->second.at(j) };
						MealyAutomate.finalClasses.insert(make_pair(OUTPUT_STATE_CHAR + to_string(classCount), classes));
						classCount++;
					}
				}
			}
			else
			{
				bool found = false;
				for (int i = 0; i < iter->second.size(); i++)
				{
					found = false;
					for (auto & finalClasses : MealyAutomate.finalClasses)
					{
						if (getVectorWithClassNames(iter->second.at(i), 1) == getVectorWithClassNames(finalClasses.second.at(0), 1) && i != 0)
						{
							finalClasses.second.push_back(iter->second.at(i));
                            found = true;
						}
					}
					if (!found)
					{
						vector<vector<string>> classes = { iter->second.at(i) };
						MealyAutomate.finalClasses.insert(make_pair(OUTPUT_STATE_CHAR + to_string(classCount), classes));
						classCount++;
					}
				}

				for (auto it = MealyAutomate.finalClasses.begin(); it != MealyAutomate.finalClasses.end(); it++)
				{
					for (int i = 0; i < it->second.size(); i++)
					{
						for (int j = 0; j < MealyAutomate.sourceGraph.at(0).size(); j++)
						{
							if (it->second.at(i).at(0) == MealyAutomate.sourceGraph.at(0).at(j))
							{
								for (int k = 1; k < MealyAutomate.sourceGraph.size(); k++)
								{
									for (auto iter = MealyAutomate.finalClasses.begin(); iter != MealyAutomate.finalClasses.end(); iter++)
									{
										for (int l = 0; l < iter->second.size(); l++)
										{
											if (MealyAutomate.sourceGraph.at(k).at(j).substr(0, 2) == iter->second.at(l).at(0))
											{
												it->second.at(i).at(k) = iter->first + MealyAutomate.sourceGraph.at(k).at(j).substr(2);
											}
										}
									}

								}
							}
						}
					}
				}

			}
		}
		if (MealyAutomate.finalClasses.size() == MealyAutomate.modifiedClasses.size())
		{
			done = true;
		}
		else
		{
			MealyAutomate.modifiedClasses = MealyAutomate.finalClasses;
			MealyAutomate.finalClasses.clear();
		}
	}

	for (auto it = MealyAutomate.finalClasses.begin(); it != MealyAutomate.finalClasses.end(); it++)
	{
		MealyAutomate.output.push_back(it->second.at(0));
	}
	for (int i = 1; i < MealyAutomate.output.at(0).size(); i++)
	{
		for (auto & j : MealyAutomate.output)
		{
			outputFile << j.at(i) << ' ';
		}
		outputFile << endl;
	}
    StateTypeMachine mealyMachine;
	mealyMachine.graph = MealyAutomate.output;
	CreateMealyGraph(mealyMachine);
	cout << "mealy: done" << endl;
}
void mooreMachineMinimizer(ifstream& inputFile, ofstream& outputFile, int inputAlphabetCount, int vertexCount)
{
	int classCount = 0;
	string state;
	MooreAutomate MooreAutomate;
	vector<vector<string>> sourceGraph;

	for (int i = 0; i < inputAlphabetCount + 2; i++)
	{
		sourceGraph.emplace_back();
		for (int j = 0; j < vertexCount; j++)
		{
			if (i == 0)
			{
				sourceGraph.at(i).push_back("q" + to_string(j));
			}
			else
			{
				inputFile >> state;
				sourceGraph.at(i).push_back(state);
			}
		}
	}
	vector<string> reachableGraphLine;

	bool endDeleting = false;
	while (!endDeleting)
	{
		if (reachableGraphLine.empty())
		{
			for (int i = 0; i < sourceGraph.size(); i++)
			{
				string element = sourceGraph.at(i).at(0);
				auto search = find(reachableGraphLine.begin(), reachableGraphLine.end(), element);
				if (i != 1 || search == reachableGraphLine.end())
				{
					reachableGraphLine.push_back(sourceGraph.at(i).at(0));
				}
			}
		}
		else
		{
			for (int i = 1; i < reachableGraphLine.size(); i++)
			{
				for (int j = 2; j < sourceGraph.size(); j++)
				{
					string element = sourceGraph.at(j).at(stoi(reachableGraphLine.at(i).substr(1)));
					auto it = find(reachableGraphLine.begin(), reachableGraphLine.end(), element);
					if (it == reachableGraphLine.end())
					{
						reachableGraphLine.push_back(element);
					}
				}
			}
			endDeleting = true;
		}
	}

	for (int i = 0; i < sourceGraph.at(0).size(); i++)
	{
		bool found = false;
		for (auto & j : reachableGraphLine)
		{
			if (sourceGraph.at(0).at(i) == j)
			{
                found = true;
			}
		}

		if (!found)
		{
			for (int k = 0; k < inputAlphabetCount + 2; k++)
			{
				auto delElement = sourceGraph.at(k).begin();
				sourceGraph.at(k).erase(delElement + i);
			}
			i--;
		}
	}
	sort(reachableGraphLine.begin(), reachableGraphLine.end());

	for (int i = 0; i < sourceGraph.at(0).size(); i++)
	{
		sourceGraph.at(0).at(i) = reachableGraphLine.at(i);
	}

	MooreAutomate.sourceGraph = sourceGraph;
	for (int i = 0; i < MooreAutomate.sourceGraph.at(0).size(); i++)
	{
		vector<string> verticalElements;
		for (auto & j : MooreAutomate.sourceGraph)
		{
			verticalElements.push_back(j.at(i));
		}

		if (MooreAutomate.classes.empty())
		{
			vector<vector<string>> classes = { verticalElements };
			MooreAutomate.classes.insert(make_pair(OUTPUT_STATE_CHAR + to_string(classCount), classes));
			classCount++;
		}
		else
		{
			bool found = false;
			map<string, vector<vector<string>>>::iterator foundIteration;
			for (auto it = MooreAutomate.classes.begin(); it != MooreAutomate.classes.end(); it++)
			{
				if (it->second.at(0).at(1) == verticalElements.at(1))
				{
                    found = true;
                    foundIteration = it;
				}
			}
			if (found)
			{
                foundIteration->second.push_back(verticalElements);
			}
			else
			{
				vector<vector<string>> classes = { verticalElements };
				MooreAutomate.classes.insert(make_pair(OUTPUT_STATE_CHAR + to_string(classCount), classes));
				classCount++;
			}
		}
	}

	for (auto it = MooreAutomate.classes.begin(); it != MooreAutomate.classes.end(); it++)
	{
		for (int i = 0; i < it->second.size(); i++)
		{
			for (int j = 1; j < it->second.at(0).size(); j++)
			{
				for (auto & classes : MooreAutomate.classes)
				{
					for (int k = 0; k < classes.second.size(); k++)
					{
						if (it->second.at(i).at(j) == classes.second.at(k).at(0))
						{
							it->second.at(i).at(j) = classes.first;
						}
					}
				}
			}
		}
	}

	bool done = false;
	while (!done)
	{
		classCount = 0;
		int index = 0;
		vector<string> signals;
		for (auto iter = MooreAutomate.classes.begin(); iter != MooreAutomate.classes.end(); iter++, index++)
		{
			if (MooreAutomate.finalClasses.empty())
			{
				for (int j = 0; j < iter->second.size(); j++)
				{
					bool found = false;
					for (auto & finalClasses : MooreAutomate.finalClasses)
					{
						if (getVectorWithClassNames(iter->second.at(j), 2) == getVectorWithClassNames(finalClasses.second.at(0), 2) && j != 0)
						{
							finalClasses.second.push_back(iter->second.at(j));
							found = true;
						}
					}
					if (!found)
					{
						vector<vector<string>> classes = { iter->second.at(j) };
						MooreAutomate.finalClasses.insert(make_pair(OUTPUT_STATE_CHAR + to_string(classCount), classes));
						classCount++;
					}
				}
			}
			else
			{
				for (int i = 0; i < iter->second.size(); i++)
				{
					bool found = false;
					for (auto & finalClasses : MooreAutomate.finalClasses)
					{
						if (getVectorWithClassNames(iter->second.at(i), 2) == getVectorWithClassNames(finalClasses.second.at(0), 2) && i != 0)
						{
							finalClasses.second.push_back(iter->second.at(i));
                            found = true;
						}
					}
					if (!found)
					{
						vector<vector<string>> classes = { iter->second.at(i) };
						MooreAutomate.finalClasses.insert(make_pair(OUTPUT_STATE_CHAR + to_string(classCount), classes));
						classCount++;
					}
				}

				for (auto it = MooreAutomate.finalClasses.begin(); it != MooreAutomate.finalClasses.end(); it++)
				{
					for (int i = 0; i < it->second.size(); i++)
					{
						for (int j = 0; j < MooreAutomate.sourceGraph.at(0).size(); j++)
						{
							if (it->second.at(i).at(0) == MooreAutomate.sourceGraph.at(0).at(j))
							{
								for (int k = 2; k < MooreAutomate.sourceGraph.size(); k++)
								{
									for (auto iter = MooreAutomate.finalClasses.begin(); iter != MooreAutomate.finalClasses.end(); iter++)
									{
										for (int l = 0; l < iter->second.size(); l++)
										{
											if (MooreAutomate.sourceGraph.at(k).at(j) == iter->second.at(l).at(0))
											{
												it->second.at(i).at(k) = iter->first;
											}
										}
									}

								}
							}
						}
					}
				}

			}
		}
		if (MooreAutomate.finalClasses.size() == MooreAutomate.classes.size())
		{
			done = true;
		}
		else
		{
			MooreAutomate.classes = MooreAutomate.finalClasses;
			MooreAutomate.finalClasses.clear();
		}
	}

	for (auto it = MooreAutomate.finalClasses.begin(); it != MooreAutomate.finalClasses.end(); it++)
	{
		MooreAutomate.output.push_back(it->second.at(0));
	}

	for (int i = 1; i < MooreAutomate.output.at(0).size(); i++)
	{
		for (auto & j : MooreAutomate.output)
		{
			outputFile << j.at(i) << ' ';
		}
		outputFile << endl;
	}
    StateTypeMachine mooreMachine;
	mooreMachine.graph = MooreAutomate.output;

	CreateMooreGraph(mooreMachine);
	cout << "moore: done" << endl;
}


