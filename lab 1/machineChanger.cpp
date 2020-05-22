#include <iostream>
#include <string>
#include <map>
#include <vector>

#include <boost/graph/graphviz.hpp>

using namespace std;

struct StateMachine
{
    vector<int> inputValues;
    vector<string> outputValues;
    vector<vector<string>> graph;
};

struct VertexProps
{
    string label;
};

struct EdgeProps
{
    string label;
};

const string MOORE_VERTEX_CHAR = "q";
const string MEALY_VERTEX_CHAR = "Z";
const string MOORE_TYPE = "moore";
const string MEALY_TYPE = "mealy";

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProps, EdgeProps>;

void visualize(Graph graph) {
    boost::dynamic_properties dp;
    dp.property("label", boost::get(&VertexProps::label, graph));
    dp.property("label", boost::get(&EdgeProps::label, graph));
    dp.property("node_id", boost::get(boost::vertex_index, graph));
    ofstream ofs("moore.dot");
    boost::write_graphviz_dp(ofs, graph, dp);
}

void CreateMooreGraph(StateMachine& moore, map<string, string> &states)
{
    Graph graph;
    vector<Graph::vertex_descriptor> vertices;
    auto it = states.begin();
    for (size_t i = 0; i < moore.graph[0].size(); ++i)
    {
        string vertexLabel = 'q' + to_string(moore.inputValues.at(i)) + moore.outputValues[i];
        it = states.find(vertexLabel);
        vertexLabel = it->second + (moore.outputValues[i]);
        vertices.push_back(boost::add_vertex({ vertexLabel }, graph));
    }

    for (size_t i = 0; i < moore.graph.size(); ++i)
    {
        for (size_t j = 0; j < moore.graph[0].size(); ++j)
        {
            string edgeLabel = 'x' + to_string(i + 1);
            boost::add_edge(vertices[j], vertices[stoi(moore.graph[i][j])], { edgeLabel }, graph);
        }
    }

    visualize(graph);
}

void CreateMealyGraph(StateMachine& mealy, map<string, string> &states)
{
    Graph graph;
    vector<Graph::vertex_descriptor> vertices;
    auto it = states.begin();
    for (size_t i = 0; i < mealy.graph[0].size(); ++i)
    {
        string vertexLabel = "q" + to_string(i);
        it = states.find(vertexLabel);
        vertices.push_back(boost::add_vertex({ vertexLabel }, graph));
    }

    for (size_t i = 0; i < mealy.graph.size(); ++i)
    {
        for (size_t j = 0; j < mealy.graph[0].size(); ++j)
        {
            string edgeLabel = 'x' + to_string(i + 1) + "/" + mealy.graph[i][j].substr(2, 3);
            boost::add_edge(vertices[j], vertices[stoi(mealy.graph[i][j].substr(1, 1))], { edgeLabel }, graph);
        }
    }

    visualize(graph);
}

void createMooreMachine(int inputCharsCount, int vertexCount, ifstream &input, ofstream &output, string** outputTable)
{
    map<string, string> states;
    StateMachine moore;
    vector<string> vertexes;
    for (int i = 0; i < inputCharsCount; i++)
    {
        for (int j = 0; j < vertexCount; j++)
        {
            input >> outputTable[i][j];
            vertexes.push_back(outputTable[i][j]);
        }
    }

    auto it = states.begin();
    int newValue = 0;
    for (int i = 0; i < inputCharsCount; i++)
    {
        for (int j = 0; j < vertexCount; j++)
        {
            it = states.find(outputTable[i][j]);
            if (it == states.end())
            {
                states.insert(make_pair(outputTable[i][j], MEALY_VERTEX_CHAR + to_string(newValue)));
                newValue++;
            }
        }
    }
    it = states.begin();
    for (int i = 0; it != states.end(); it++, i++) {
        it->second = MEALY_VERTEX_CHAR + to_string(i);
        moore.inputValues.push_back(stoi(it->first.substr(1, 1)));
        moore.outputValues.push_back(it->first.substr(2, 3));
    }
    it = states.begin();

    int index = 0;

    for (int i = 0; i < inputCharsCount + 1; i++)
    {
        it = states.begin();
        if (i != 0)
        {
            moore.graph.emplace_back();
        }
        for (int j = 0; j < states.size(); j++)
        {
            if (i == 0)
            {
                output << it->first.substr(2) << ' ';
                it++;
            }
            else if (i == 1)
            {
                index = stoi(it->first.substr(1, 1));
                output << states[vertexes.at(index)] << ' ';
                moore.graph[0].push_back(states[vertexes.at(index)].substr(1, 1));
                it++;
            }
            else
            {
                index = stoi(it->first.substr(1, 1));
                output << states[vertexes.at(index + vertexCount * (i - 1))] << ' ';
                moore.graph[i - 1].push_back(states[vertexes.at(index + vertexCount * (i - 1))].substr(1, 1));
                it++;
            }
        }
        output << endl;
    }
    CreateMooreGraph(moore, states);
}
void createMealyMachine(int inputCharsCount, int vertexCount, ifstream &input, ofstream &output, string** outputTable)
{
    string vertexValue;
    char inputChar;
    map<string, string> states;
    StateMachine mealy;
    for (int i = 0; i < vertexCount; i++)
    {
        vertexValue = MOORE_VERTEX_CHAR + to_string(i);
        states.insert(make_pair(vertexValue, ""));
    }

    auto it = states.begin();
    for (int i = 0; i < vertexCount; i++, it++)
    {
        input >> it->second;
    }

    for (int i = 0; i < inputCharsCount; i++)
    {
        mealy.graph.emplace_back();
        for (int j = 0; j < vertexCount; j++)
        {
            input >> inputChar;
            it = states.find(to_string(inputChar));
            outputTable[i][j] = it->first + it->second;
            mealy.graph[i].push_back(outputTable[i][j]);
            output << outputTable[i][j] << ' ';
        }
        output << endl;
    }
    CreateMealyGraph(mealy, states);
}


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Usage: enter machineChanger.exe inputFile.txt outputFile.txt" << endl;
        return 0;
    }
    int inputCharsCount, outputCharsCount, vertexCount;
    string vertexValue, machineType;

    ifstream input(argv[1]);
    ofstream output(argv[2]);

    if (!input) {
        cout << "input not found" << endl;
        return 0;
    }
    if (!output) {
        cout << "output not found" << endl;
        return 0;
    }


    input >> inputCharsCount >> outputCharsCount >> vertexCount >> machineType;

    auto** outputTable = new string*[inputCharsCount];
    for (int i = 0; i < inputCharsCount; i++)
    {
        outputTable[i] = new string[vertexCount];
    }

    if (machineType == MOORE_TYPE)
    {
        createMealyMachine(inputCharsCount, vertexCount, input, output, outputTable);
    }
    else if (machineType == MEALY_TYPE)
    {
        createMooreMachine(inputCharsCount, vertexCount, input, output, outputTable);
    }
    else
    {
        output << "Unknown machine" << endl;
    }
}