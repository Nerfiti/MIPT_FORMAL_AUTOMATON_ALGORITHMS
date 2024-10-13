#include <iostream>
#include <map>
#include <queue>
#include <vector>

#include "automaton_algorithms.hpp"

void AutomatonTransformer::RemoveEpsTransitions(Automaton &automaton)
{
    bool old_flag = automaton.GetOptimizeEpsilonsFlag();
    automaton = Automaton::buildFromAnother(automaton, true);
    automaton.SetOptimizeEpsilonsFlag(old_flag);
}

void AutomatonTransformer::InverseCDFA(Automaton &automaton)
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

Automaton AutomatonTransformer::DFAFromNFA(const Automaton &automaton)
{
    Automaton DFA(automaton.GetAlphabet());
    DFA.SetFinal(0, automaton.IsStateFinal(0));

    size_t cur_max_index = 0;

    std::unordered_map<std::set<size_t>, size_t, decltype([](const std::set<size_t> &) { return 0; })> new_indices;
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

Automaton AutomatonTransformer::CDFAFromDFA(const Automaton &automaton)
{
    Automaton result = automaton;
    MakeDFAComplete(result);
    return result;
}

Automaton AutomatonTransformer::ComplementOfCDFA(const Automaton &automaton)
{
    Automaton result = automaton;
    InverseCDFA(result);
    return result;
}

Automaton AutomatonTransformer::MCDFAFromCDFA(const Automaton &automaton)
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
        // std::cout << "Start new step.\n";
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

        // for (size_t vertex = 0; vertex < number_of_states; ++vertex)
        // {
        //     for (size_t i = 0; i <= automaton.GetAlphabet().size(); ++i)
        //         std::cout << factor_set[vertex][i] << " ";
        //     std::cout << "\n";
        // }
        // std::cout << "\n";

        cur_classes = 0;
        new_classes.resize(number_of_states, std::numeric_limits<size_t>::max());

        // std::cout << "NEW CLASSES:\n";
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
                // std::cout << current_state << "\n";
                new_classes[current_state] = cur_classes++;
            }
        }

        for (size_t i = 0; i < number_of_states; ++i)
        {
            factor_set[i].resize(automaton.GetAlphabet().size() + 1, std::numeric_limits<size_t>::max());
            factor_set[i][0] = new_classes[i];
        }
        // std::cout << "Number of classes = " << cur_classes << '\n';
        // std::cout << "Old number of classes = " << old_number_of_classes << '\n';
    }

    Automaton MDFA(automaton.GetAlphabet(), cur_classes);
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

std::string AutomatonTransformer::RegExpr(const Automaton &automaton)
{
    static const char *EmptyLanguage = "[Empty language]";

    auto &finals = automaton.GetFinalStates();
    if (finals.size() == 0)
        return EmptyLanguage;

    RegularAutomaton regauto = automaton;
    auto new_final = regauto.AddState();
    regauto.SetFinal(new_final);

    for (auto final : automaton.GetFinalStates())
    {
        regauto.AddEdge(final, new_final, RegularAutomaton::Epsilon);
        regauto.SetFinal(final, false);
    }

    for (auto state : regauto.GetStateNumbers())
    {
        std::map<size_t, RegularAutomaton::alpha_t> transitions;
        for (auto &alpha_neigh : regauto.GetNeighbours(state))
        {
            auto &alpha = alpha_neigh.first;

            std::set<size_t> neighbours = {};
            for (auto neigh : alpha_neigh.second)
            {
                if (transitions.contains(neigh))
                    transitions[neigh] += std::string(" + ") + alpha;
                else
                    transitions[neigh] = alpha;

                neighbours.insert(neigh);
            }

            for (auto neigh : neighbours)
                regauto.RemoveEdge(state, neigh, alpha);
        }

        for (auto &[neigh, alpha] : transitions)
            regauto.AddEdge(state, neigh, alpha);
    }

    while (regauto.GetNumberOfStates() > 2) // 2 = start and end
    {
        auto processing_state = regauto.GetStartState();

        auto states_set = regauto.GetStateNumbers();
        std::vector states(states_set.begin(), states_set.end());
        std::random_shuffle(states.begin(), states.end());
        for (auto state : states)
        {
            if (state != processing_state && !regauto.IsStateFinal(state))
            {
                processing_state = state;

                break;
            }
        }
        if (processing_state == regauto.GetStartState())
            return "Error in start or final state notation.";

        std::map<size_t, std::string> incoming_edges = {};
        std::string loop = "";
        for (auto state : regauto.GetStateNumbers())
        {
            for (auto &alpha_neigh : regauto.GetNeighbours(state))
            {
                if (alpha_neigh.second.contains(processing_state))
                {
                    if (state != processing_state)
                    {
                        incoming_edges.insert({state, alpha_neigh.first});
                    }
                    else
                    {
                        if (loop != "")
                            return "Two or more loops in one vertex";

                        loop = alpha_neigh.first;
                    }
                }
            }
        }

        for (auto &income : incoming_edges)
        {
            for (auto &alpha_neigh : regauto.GetNeighbours(processing_state))
            {
                for (auto &neigh : alpha_neigh.second)
                {
                    auto outcome = std::make_pair(neigh, alpha_neigh.first);

                    std::string new_string = "";
                    bool empty_income = income.second == RegularAutomaton::Epsilon;
                    bool empty_outcome = outcome.second == RegularAutomaton::Epsilon;
                    bool empty_loop = loop == "" || loop == RegularAutomaton::Epsilon;

                    if (empty_income && empty_outcome && empty_loop)
                    {
                        new_string = RegularAutomaton::Epsilon;
                    }
                    else 
                    {
                        if (!empty_income)
                        {
                            bool need_brackets = income.second.length() > 1 &&
                                               income.second.find('+') != std::string::npos &&
                                               (empty_outcome && empty_loop);

                            if (need_brackets)
                                new_string += "(";

                            new_string += income.second;

                            if (need_brackets)
                                new_string += ")";
                        }
                        if (!empty_loop)
                        {
                            bool need_repeat = !(loop.length() == 2 && loop[1] == '*'); //TODO: think about smart check
                            bool need_brackets = need_repeat && loop.length() > 1;

                            if (need_brackets)
                                new_string += "(";

                            new_string += loop;

                            if (need_brackets)
                                new_string += ")";

                            if (need_repeat)
                                new_string += "*";
                        }
                        if (!empty_outcome)
                        {
                            bool need_brackets = outcome.second.length() > 1 &&
                                               outcome.second.find('+') != std::string::npos &&
                                               !(empty_income && empty_loop);
                            if (need_brackets)
                                new_string += "(";

                            new_string += outcome.second;

                            if (need_brackets) 
                                new_string += ")";
                        }
                    }

                    std::string existent_string = "";
                    for (auto &tmp_alpha_neigh : regauto.GetNeighbours(income.first))
                    {
                        if (tmp_alpha_neigh.second.contains(outcome.first))
                        {
                            if (existent_string != "")
                                return "Error: two or more edges between two vertices.";

                            existent_string = tmp_alpha_neigh.first;
                        }
                    }

                    if (existent_string == "")
                    {
                        regauto.RemoveEdge(income.first, outcome.first, existent_string);
                        regauto.AddEdge(income.first, outcome.first, new_string);
                    }
                    else if (new_string != RegularAutomaton::Epsilon)
                    {
                        regauto.RemoveEdge(income.first, outcome.first, existent_string);
                        regauto.AddEdge(income.first, outcome.first, existent_string + " + " + new_string);
                    }
                }
            }
        }

        regauto.RemoveState(processing_state);
    }

    size_t start = regauto.GetStartState();
    size_t end = *regauto.GetFinalStates().begin();
    std::string reg_expr = "";
    std::string start_loop = "";
    std::string end_expr = "";

    for (auto &alpha_neigh : regauto.GetNeighbours(start))
    {
        if (alpha_neigh.second.contains(start))
        {
            if (start_loop != "")
                return "Unexpected error";

            start_loop = alpha_neigh.first;
        }
        if (alpha_neigh.second.contains(end))
        {
            if (end_expr != "")
                return "Unexpected error";

            end_expr = alpha_neigh.first;
        }
    }

    if (end_expr == "")
        return EmptyLanguage;

    if (start_loop != "" && start_loop != RegularAutomaton::Epsilon)
    {
        bool need_repeat = !(start_loop.length() == 2 && start_loop[1] == '*'); //TODO: think about smart check
        bool need_brackets = need_repeat && start_loop.length() > 1;

        if (need_brackets)
            reg_expr += "(";

        reg_expr += start_loop;

        if (need_brackets)
            reg_expr += ")";

        if (need_repeat)
            reg_expr += "*";
    }

    if (end_expr != RegularAutomaton::Epsilon || reg_expr == "")
    {
        bool need_brackets = end_expr.length() > 1 && end_expr.find('+') != std::string::npos;

        if (need_brackets)
            reg_expr += "(" + end_expr + ")";
        else
            reg_expr += end_expr;
    }
    return reg_expr;
}