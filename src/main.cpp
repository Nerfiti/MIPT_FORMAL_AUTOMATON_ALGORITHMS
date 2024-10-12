#include <iostream>

#include "automaton_algorithms.hpp"
#include "automaton_drawer.hpp"

int main()
{
    Automaton automaton({'a', 'b'}, 5);
    automaton.SetFinal(1);
    automaton.SetFinal(4);

    automaton.AddEdge(0, 1, 'a');
    automaton.AddEdge(2, 2, 'b');
    automaton.AddEdge(0, 2, 0);
    automaton.AddEdge(2, 3, 'b');
    automaton.AddEdge(3, 4, 0);

    AutomatonDrawer::GenerateImage(automaton);

    AutomatonTransformer::RemoveEpsilonTransitions(automaton);
    AutomatonDrawer::GenerateImage(automaton);

    auto MCDFA = AutomatonTransformer::GetMinimalCompleteDFAFromCompleteDFA
                (
                    AutomatonTransformer::GetCompleteDFAFromDFA
                    (
                        AutomatonTransformer::GetDFAFromNFA(automaton)
                    )
                );

    AutomatonDrawer::GenerateImage(MCDFA, true);

    return 0;
}