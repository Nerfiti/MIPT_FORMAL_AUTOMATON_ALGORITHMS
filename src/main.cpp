#include <iostream>

#include "automaton_algorithms.hpp"
#include "automaton_drawer.hpp"

int main()
{
    Automaton automaton;
    automaton.SetAlphabet({'a', 'b'});

    automaton.SetStates(5);
    automaton.SetStartState(0);

    automaton.AddEdge(0, 0, 'a');
    automaton.AddEdge(0, 1, 'a');
    automaton.AddEdge(1, 0, 'b');
    automaton.AddEdge(0, 2, 'b');
    automaton.AddEdge(2, 3, 'a');

    automaton.SetFinal(0);
    automaton.SetFinal(2);
    automaton.SetFinal(3);
    AutomatonDrawer::GenerateImage(automaton);

    auto DFA = AutomatonTransformer::GetDFAFromNFA(automaton);
    AutomatonTransformer::MakeDFAComplete(DFA);
    AutomatonDrawer::GenerateImage(DFA);

    return 0;
}