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
    automaton.AddEdge(0, 2, 'b');
    automaton.AddEdge(2, 3, 'a');
    automaton.AddEdge(1, 4, 0);

    automaton.MarkStateAsFinal(0);
    automaton.MarkStateAsFinal(3);

    AutomatonDrawer::GenerateImage(automaton);

    return 0;
}