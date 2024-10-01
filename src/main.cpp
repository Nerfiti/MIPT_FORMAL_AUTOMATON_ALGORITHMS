#include <iostream>

#include "automaton_drawer.hpp"

#define TEST(cond) \
    if (!cond) std::cerr << #cond << " is incorrect.\n"

int main()
{
    Automaton automaton;
    automaton.SetStates(14);
    automaton.SetStartState(0);
    automaton.AddEdge(0, 1, 'a');
    automaton.AddEdge(1, 2, 'b');
    automaton.AddEdge(2, 3, 'a');
    automaton.AddEdge(3, 4, 'a');
    automaton.AddEdge(0, 5, 'b');
    automaton.AddEdge(5, 6, 'b');
    automaton.AddEdge(6, 7, 'b');
    automaton.AddEdge(6, 8, 'a');
    automaton.AddEdge(7, 9, 'a');
    automaton.AddEdge(7, 10, 'a');
    automaton.AddEdge(8, 11, 'a');
    automaton.AddEdge(8, 12, 'b');
    automaton.AddEdge(8, 13, 'b');

    automaton.MarkStateAsFinal(0);
    automaton.MarkStateAsFinal(12);
    automaton.MarkStateAsFinal(9);
    automaton.MarkStateAsFinal(3);

    AutomatonDrawer::GenerateImage(automaton);

    return 0;
}