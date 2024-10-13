#include <iostream>

#include "automaton_algorithms.hpp"
#include "automaton_drawer.hpp"

int main()
{
    Automaton automaton({'a', 'b'}, 2);
    automaton.SetFinal(1);

    automaton.AddEdge(0, 1, 'a');
    automaton.AddEdge(0, 1, 'b');
    automaton.AddEdge(0, 1, Automaton::Epsilon);
    automaton.AddEdge(1, 1, 'a');

    AutomatonDrawer::GenerateImage(automaton);

    AutomatonTransformer::RemoveEpsTransitions(automaton);
    auto MCDFA = AutomatonTransformer::MCDFAFromCDFA
                (
                    AutomatonTransformer::CDFAFromDFA
                    (
                        AutomatonTransformer::DFAFromNFA(automaton)
                    )
                );

    AutomatonDrawer::GenerateImage(MCDFA, true);

    std::cout << AutomatonTransformer::RegExpr(MCDFA) << "\n";

    return 0;
}