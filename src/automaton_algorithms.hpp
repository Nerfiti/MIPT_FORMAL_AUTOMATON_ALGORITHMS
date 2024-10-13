#pragma once

#include "automaton.hpp"

namespace AutomatonTransformer
{
    void RemoveEpsTransitions(Automaton &automaton);
    void InverseCDFA(Automaton &automaton);
    void MinimizeCDFA(Automaton &automaton);
    void MakeDFAComplete(Automaton &automaton);

    Automaton DFAFromNFA(const Automaton &automaton);
    Automaton CDFAFromDFA(const Automaton &automaton);
    Automaton ComplementOfCDFA(const Automaton &automaton);
    Automaton MCDFAFromCDFA(const Automaton &automaton);

    std::string RegExpr(const Automaton &automaton);
};