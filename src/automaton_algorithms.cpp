#include <iostream>
#include <queue>
#include <vector>

#include "automaton_algorithms.hpp"

void AutomatonTransformer::InverseCompleteDFA(Automaton &automaton)
{
    for (auto vertex : automaton.GetStateNumbers())
        automaton.SetFinal(vertex, !automaton.IsStateFinal(vertex));
}

void AutomatonTransformer::MakeDFAComplete(Automaton &automaton)
{
    bool need_garbage = true;
    size_t garbage_state = 0;

    auto vertices = automaton.GetStateNumbers();
    for (auto vertex : vertices)
    {
        for (auto alpha : automaton.GetAlphabet())
        {
            if (!automaton.CanTransit(vertex, alpha))
            {
                if (need_garbage)
                {
                    garbage_state = automaton.AddState();
                    need_garbage = false;
                }
                automaton.AddEdge(vertex, garbage_state, alpha);
            }
        }
    }

    if (need_garbage == false)
        for (auto alpha : automaton.GetAlphabet())
            automaton.AddEdge(garbage_state, garbage_state, alpha);
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

            if (new_state.empty())
                continue;

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
    std::unordered_map<size_t, size_t> to_vertex_order;
    std::vector<size_t> to_vertex_number(automaton.GetNumberOfStates(), std::numeric_limits<size_t>::max());

    size_t number_of_states = 0;
    for (auto vertex_num : automaton.GetStateNumbers())
    {
        to_vertex_number[number_of_states] = vertex_num;
        to_vertex_order[vertex_num] = number_of_states++;
    }

    std::vector<std::vector<size_t>> factor_set(number_of_states, 
                                                std::vector<size_t>(automaton.GetAlphabet().size() + 1, 
                                                                    std::numeric_limits<size_t>::max()));

    for (size_t i = 0; i < number_of_states; ++i)
        factor_set[i][0] = automaton.IsStateFinal(to_vertex_number[i]);

    size_t old_number_of_classes = 0;
    size_t cur_classes = 2;
    std::vector<size_t> new_classes;

    while (cur_classes != old_number_of_classes)
    {
        std::cout << "Start new step.\n";
        old_number_of_classes = cur_classes;

        for (size_t i = 0; i < number_of_states; ++i)
        {
            size_t alpha_num = 1;
            for (auto alpha : automaton.GetAlphabet())
            {
                factor_set[i][alpha_num] = factor_set[*(automaton.GetNeighbours(to_vertex_number[i]).at(alpha).begin())][0];
                ++alpha_num;
            }
        }

        for (size_t vertex = 0; vertex < number_of_states; ++vertex)
        {
            for (size_t i = 0; i <= automaton.GetAlphabet().size(); ++i)
                std::cout << factor_set[vertex][i] << " ";
            std::cout << "\n";
        }
        std::cout << "\n";

        cur_classes = 0;
        new_classes.resize(number_of_states, std::numeric_limits<size_t>::max());

        std::cout << "NEW CLASSES:\n";
        for (size_t current_state = 0; current_state < factor_set.size(); ++current_state)
        {
            bool new_class = true;
            for (size_t to_check_state = 0; to_check_state < current_state; ++to_check_state)
            {
                if (factor_set[current_state] == factor_set[to_check_state])
                {
                    new_class = false;
                    new_classes[current_state] = new_classes[to_check_state];
                }
            }

            if (new_class)
            {
                std::cout << current_state << "\n";
                new_classes[current_state] = cur_classes++;
            }
        }


        for (size_t i = 0; i < number_of_states; ++i)
        {
            factor_set[i].resize(automaton.GetAlphabet().size() + 1, std::numeric_limits<size_t>::max());
            factor_set[i][0] = new_classes[i];
        }
        std::cout << "Number of classes = " << cur_classes << '\n';
        std::cout << "Old number of classes = " << old_number_of_classes << '\n';
    }

    Automaton MDFA;
    MDFA.SetAlphabet(automaton.GetAlphabet());
    MDFA.SetStates(cur_classes);
    MDFA.SetStartState(new_classes[to_vertex_order[automaton.GetStartState()]]);

    for (auto final : automaton.GetFinalStates())
        MDFA.SetFinal(new_classes[to_vertex_order[final]]);

    for (auto vertex : automaton.GetStateNumbers())
    {
        size_t new_vertex_notation = new_classes[to_vertex_order[vertex]];
        auto neighbours = automaton.GetNeighbours(vertex);
        for (auto alpha : MDFA.GetAlphabet())
        {
            for (auto vertex_to : neighbours.at(alpha))
                MDFA.AddEdge(new_vertex_notation, new_classes[to_vertex_order[vertex_to]], alpha);
        }
    }

    return MDFA;
}