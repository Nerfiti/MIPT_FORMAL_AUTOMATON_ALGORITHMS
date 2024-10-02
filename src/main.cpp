#include <iostream>

#include "automaton_algorithms.hpp"
#include "automaton_drawer.hpp"

int main()
{
    Automaton automaton;
    automaton.SetAlphabet({'a', 'b'});

    automaton.SetStates(7);
    automaton.SetStartState(0);

    automaton.SetFinal(0);
    automaton.SetFinal(1);
    automaton.SetFinal(2);
    automaton.SetFinal(6);

    automaton.AddEdge(0, 1, 'a');
    automaton.AddEdge(1, 2, 'a');
    automaton.AddEdge(2, 2, 'a');
    automaton.AddEdge(1, 3, 'b');
    automaton.AddEdge(3, 1, 'a');
    automaton.AddEdge(0, 4, 'a');
    automaton.AddEdge(0, 4, 'b');
    automaton.AddEdge(4, 5, 'a');
    automaton.AddEdge(5, 6, 'b');
    automaton.AddEdge(4, 6, 'b');
    automaton.AddEdge(6, 4, 'a');
    automaton.AddEdge(6, 4, 'b');

    auto CDFA = AutomatonTransformer::GetDFAFromNFA(automaton);
    AutomatonTransformer::MakeDFAComplete(CDFA);
    CDFA = AutomatonTransformer::GetComplementFromCompleteDFA(CDFA);

    auto MCDFA = AutomatonTransformer::GetMinimalCompleteDFAFromCompleteDFA(CDFA);

    AutomatonDrawer::GenerateImage(automaton);
    AutomatonDrawer::GenerateImage(CDFA);
    AutomatonDrawer::GenerateImage(MCDFA);

    return 0;
}