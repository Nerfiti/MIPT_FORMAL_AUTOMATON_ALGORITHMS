#include "automaton.hpp"

template<>
const Automaton::alpha_t Automaton::Epsilon = 1;

template<>
const RegularAutomaton::alpha_t RegularAutomaton::Epsilon = "1";