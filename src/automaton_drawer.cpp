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

    const char *Image_path = "./graph/Automaton";
    const char *Image_type = "svg";
    const char *Dot_file_path = "./graph/automaton";
    const char *Dot_file_type = "dot";

    size_t number_of_images = 0;

    std::string GetImagePath()
    {
        return std::string(Image_path) + "_" + std::to_string(number_of_images) + "." + Image_type;
    }

    std::string GetDotFilePath()
    {
        return std::string(Dot_file_path) + "_" + std::to_string(number_of_images) + "." + Dot_file_type;
    }

    std::string GetDrawCommand()
    {
        return std::string("dot -v -T") + Image_type + " -o" + GetImagePath() + " " + GetDotFilePath() + " > /dev/null 2>&1";
    };
};

static int Command_execution_failure = -1;

void DrawAutomaton(const Automaton& automaton, std::ofstream& dot_file);
void RecursiveDraw(const Automaton& automaton, size_t vertex, std::ofstream& dot_file, std::set<size_t>& processed_vertices);

void AutomatonDrawer::GenerateImage(const Automaton& automaton)
{
    std::ofstream dot_file(GetDotFilePath());
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
        system((std::string("rm ") + GetDotFilePath()).c_str());
        return;
    }

    ++number_of_images;
}

void DrawAutomaton(const Automaton& automaton, std::ofstream& dot_file)
{
    auto start_state = automaton.GetStartState();
    dot_file << "fictitious -> " << start_state << std::endl;

    for (auto& state : automaton.GetStateNumbers())
    {
        dot_file << state 
                 << (automaton.IsStateFinal(state) ? " [fillcolor=red, style=filled]" : "") 
                 << "\n";
    }
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

            if (alpha_neighbours.first == 0)
                label = "\u03B5";

            dot_file << vertex << " -> " << neighbour << " [label=" << label << "]" << std::endl;
            RecursiveDraw(automaton, neighbour, dot_file, processed_vertices);
        }
    }
}