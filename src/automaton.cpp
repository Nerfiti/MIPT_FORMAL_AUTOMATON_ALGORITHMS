#include <iostream>

#include "automaton.hpp"

size_t Automaton::AddState(size_t state_number)
{
    if (state_number == std::numeric_limits<size_t>::max())
    {
        if (existent_states_.empty())
            state_number = 0;
        else
            state_number = *existent_states_.rbegin() + 1;
    }

    existent_states_.insert(state_number);
    states_[state_number] = state_t();
    ++number_of_states_;

    return state_number;
}

void Automaton::SetStates(size_t numberOfStates)
{
    states_.clear();
    existent_states_.clear();
    number_of_states_ = 0;

    for (size_t i = 0; i < numberOfStates; ++i)
        AddState(i);
}

void Automaton::RemoveState(size_t state_number)
{
    if (state_number == start_state_)
    {
        std::cout << "Can't remove start state. Please, change other state as start and try again";
        return;
    }

    SetFinal(state_number, false);
    existent_states_.erase(state_number);
    states_.erase(state_number);

    for (auto& state_transit : states_)
        for (auto& state : state_transit.second)
            state.second.erase(state_number);
}

size_t Automaton::GetNumberOfStates() const { return number_of_states_; }

bool Automaton::AddEdge(size_t from, size_t to, alpha_t alpha)
{
    if (!DoesStateExist(from) || !DoesStateExist(to) || DoesEdgeExist(from, to, alpha))
        return false;

    state_t& state_from = states_[from];

    if (!state_from.contains(alpha))
        state_from[alpha] = {};

    state_from[alpha].insert(to);

    return true;
}

bool Automaton::RemoveEdge(size_t from, size_t to, alpha_t alpha)
{
    if (!DoesEdgeExist(from, to, alpha))
        return false;

    states_[from][alpha].erase(to);

    return true;
}

bool Automaton::DoesEdgeExist(size_t from, size_t to, alpha_t alpha) const
{
    if (!DoesStateExist(from) || !DoesStateExist(to))
        return false;

    auto& state_from = states_.at(from);

    if (!state_from.contains(alpha))
        return false;

    auto &neighbours = state_from.at(alpha);
    return neighbours.contains(to);
}

bool Automaton::DoesStateExist(size_t state_number) const { return states_.contains(state_number); }

bool Automaton::CanTransit(size_t state, alpha_t alpha) const
{
    if (!DoesStateExist(state))
        return false;

    auto& state_from = states_.at(state);

    return state_from.contains(alpha) && !state_from.at(alpha).empty();
}

const std::unordered_map<Automaton::alpha_t, std::set<size_t>>& Automaton::GetNeighbours(size_t vertex_number) const
{
    return states_.at(vertex_number);
}

void Automaton::SetStartState(size_t start_state) { start_state_ = start_state; }

size_t Automaton::GetStartState() const { return start_state_; }

void Automaton::SetFinal(size_t state, bool is_final)
{
    if (is_final)
        final_states_.insert(state);
    else
        final_states_.erase(state);
}

bool Automaton::IsStateFinal(size_t state) const
{
    return final_states_.contains(state);
}

const std::set<size_t>& Automaton::GetFinalStates() const
{
    return final_states_;
}

const std::set<size_t>& Automaton::GetStateNumbers() const { return existent_states_; }

void Automaton::AddCharToAlphabet(Automaton::alpha_t alpha) { alphabet_.insert(alpha); }

void Automaton::SetAlphabet(const std::set<Automaton::alpha_t>& alphabet) { alphabet_ = alphabet; }

const std::set<Automaton::alpha_t>& Automaton::GetAlphabet() const { return alphabet_; }