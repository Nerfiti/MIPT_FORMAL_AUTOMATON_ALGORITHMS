#pragma once

#include "automaton.hpp"

namespace AutomatonTransformer
{
    void RemoveEpsilonTransitions(Automaton &automaton);
    void InverseCompleteDFA(Automaton &automaton);
    void MinimizeCompleteDFA(Automaton &automaton);
    void MakeDFAComplete(Automaton &automaton);

    Automaton GetDFAFromNFA(const Automaton &automaton);
    Automaton GetCompleteDFAFromDFA(const Automaton &automaton);
    Automaton GetComplementFromCompleteDFA(const Automaton &automaton);
    Automaton GetMinimalCompleteDFAFromCompleteDFA(const Automaton &automaton);
};