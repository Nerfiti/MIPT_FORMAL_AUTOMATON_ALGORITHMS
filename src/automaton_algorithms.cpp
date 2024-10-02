#include <iostream>
#include <queue>

#include "automaton_algorithms.hpp"

void AutomatonTransformer::InverseCompleteDFA(Automaton &automaton)
{
    for (auto vertex : automaton.GetStateNumbers())
        automaton.SetFinal(vertex, !automaton.IsStateFinal(vertex));
}

void AutomatonTransformer::MinimizeCompleteDFA(Automaton &automaton)
{

}

void AutomatonTransformer::MakeDFAComplete(Automaton &automaton)
{
    size_t garbage_state = automaton.AddState();

    for (auto vertex : automaton.GetStateNumbers())
    {
        for (auto alpha : automaton.GetAlphabet())
        {
            if (!automaton.CanTransit(vertex, alpha))
                automaton.AddEdge(vertex, garbage_state, alpha);
        }
    }
}

Automaton AutomatonTransformer::GetDFAFromNFA(const Automaton &automaton)
{
    Automaton DFA;
    DFA.SetAlphabet(automaton.GetAlphabet());
    DFA.AddState(0);
    DFA.SetStartState(0);
    DFA.SetFinal(0, automaton.IsStateFinal(0));

    size_t cur_max_index = 0;

    std::unordered_map<std::set<size_t>, size_t, decltype([](const std::set<size_t>&) { return 0; })> new_indices;
    new_indices.insert({{0}, cur_max_index++});
    std::vector<std::set<size_t>> old_indices = {{0}};

    std::queue<size_t> states_queue;
    states_queue.push(0);

    while (!states_queue.empty())
    {
        size_t state = states_queue.front();
        states_queue.pop();
        for (auto alpha : automaton.GetAlphabet())
        {
            std::set<size_t> new_state = {};
            bool is_final = false;
            for (auto &state_in_new_state : old_indices[state])
            {
                auto &letters_transitions = automaton.GetNeighbours(state_in_new_state);
                if (!letters_transitions.contains(alpha))
                    continue;

                for (auto neighbour : automaton.GetNeighbours(state_in_new_state).at(alpha))
                {
                    new_state.insert(neighbour);
                    is_final = is_final || automaton.IsStateFinal(neighbour);
                }
            }

            if (!new_indices.contains(new_state))
            {
                new_indices.insert({new_state, cur_max_index});
                old_indices.push_back(new_state);
                DFA.AddState(cur_max_index);
                DFA.SetFinal(cur_max_index, is_final);
                DFA.AddEdge(state, cur_max_index, alpha);

                states_queue.push(cur_max_index);
                ++cur_max_index;
            }
            else
            {
                size_t target = new_indices.at(new_state);
                DFA.AddEdge(state, target, alpha);
            }
        }
    }

    return DFA;
}

Automaton AutomatonTransformer::GetCompleteDFAFromDFA(const Automaton &automaton)
{
    Automaton result = automaton;
    MakeDFAComplete(result);
    return result;
}

Automaton AutomatonTransformer::GetComplementFromCompleteDFA(const Automaton &automaton)
{
    Automaton result = automaton;
    InverseCompleteDFA(result);
    return result;
}

Automaton AutomatonTransformer::GetMinimalCompleteDFAFromCompleteDFA(const Automaton &automaton)
{
    Automaton result = automaton;
    MinimizeCompleteDFA(result);
    return result;
}