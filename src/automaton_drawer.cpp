#include <iostream>
#include <fstream>
#include <string>

#include "automaton_drawer.hpp"

namespace
{
    const char *Graph_start =   "digraph {\n"
                                "rankdir=LR\n"
                                "fictitious [shape=point, label=\"\", width=0, height=0]\n\n";

    const char *Graph_end = "}";

    const char *Image_path = "./graph/Automaton.svg";
    const char *Image_type = "svg";
    const char *Dot_file_path = "./graph/automaton.dot";

    std::string GetDrawCommand()
    {
        return std::string("dot -v -T") + Image_type + " -o" + Image_path + " " + Dot_file_path + " > /dev/null 2>&1";
    };
};

static int Command_execution_failure = -1;

void DrawAutomaton(const Automaton& automaton, std::ofstream& dot_file);
void RecursiveDraw(const Automaton& automaton, size_t vertex, std::ofstream& dot_file, std::set<size_t>& processed_vertices);

void AutomatonDrawer::GenerateImage(const Automaton& automaton)
{
    std::ofstream dot_file(Dot_file_path);
    if (!dot_file)
    {
        std::cout << "Can't open file for graph description: \"" << Dot_file_path << "\"\n";
        return;
    }

    dot_file << Graph_start;

    DrawAutomaton(automaton, dot_file);

    dot_file << Graph_end;
    dot_file.close();

    if (system(GetDrawCommand().c_str()) == Command_execution_failure)
    {
        std::cout << "Can't open file for graph image: \"" << Image_path << "\". File type: \"" << Image_type << "\".\n";
        return;
    }
}

void DrawAutomaton(const Automaton& automaton, std::ofstream& dot_file)
{
    auto start_state = automaton.GetStartState();
    dot_file << "fictitious -> " << start_state << std::endl;

    for (auto& final_state : automaton.GetFinalStates())
        dot_file << final_state << " [fillcolor=red, style=filled]\n";

    std::set<size_t> processed_vertices;

    RecursiveDraw(automaton, start_state, dot_file, processed_vertices);
}

void RecursiveDraw(const Automaton& automaton, size_t vertex, std::ofstream& dot_file, std::set<size_t>& processed_vertices)
{
    if (processed_vertices.contains(vertex))
        return;
    processed_vertices.insert(vertex);

    for (auto& alpha_neighbours : automaton.GetNeighbours(vertex))
    {
        for (auto& neighbour : alpha_neighbours.second)
        {
            std::string label = std::isalpha(alpha_neighbours.first)
                                ? std::string(1, alpha_neighbours.first)
                                : std::string("\"[") + std::to_string(alpha_neighbours.first) + "]\"";

            dot_file << vertex << " -> " << neighbour << " [label=" << label << "]" << std::endl;
            RecursiveDraw(automaton, neighbour, dot_file, processed_vertices);
        }
    }
}