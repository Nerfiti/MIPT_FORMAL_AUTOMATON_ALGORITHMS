#include "automaton.hpp"

bool Automaton::AddState(size_t state_number)
{
    if (DoesStateExist(state_number))
        return false;

    states_[state_number] = state_t();
    ++number_of_states_;
    return true;
}

void Automaton::SetStates(size_t numberOfStates)
{
    states_.clear();

    number_of_states_ = numberOfStates;
    for (size_t i = 0; i < numberOfStates; ++i)
        AddState(i);
}

size_t Automaton::GetNumberOfStates() const { return number_of_states_; }

bool Automaton::AddEdge(size_t from, size_t to, alpha_t alpha)
{
    if (!DoesStateExist(from) || !DoesStateExist(to) || DoesEdgeExist(from, to, alpha))
        return false;

    state_t state_from = states_[from];
    state_from[alpha].insert(to);

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

bool Automaton::RemoveEdge(size_t from, size_t to, alpha_t alpha)
{
    if (!DoesEdgeExist(from, to, alpha))
        return false;

    states_[from][alpha].erase(to);

    return true;
}

const std::unordered_map<Automaton::alpha_t, std::set<size_t>>& Automaton::GetNeighbours(size_t vertex_number) const
{
    return states_.at(vertex_number);
}

void Automaton::SetStartState(size_t start_state) { start_state_ = start_state; }

size_t Automaton::GetStartState() const { return start_state_; }

void Automaton::MarkStateAsFinal(size_t state)
{
    final_states_.insert(state);
}

bool Automaton::RemoveStateFromFinals(size_t state)
{
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