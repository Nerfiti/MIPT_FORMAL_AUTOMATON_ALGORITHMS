#include <iostream>

template <class alpha_t>
GenericAutomaton<alpha_t>::GenericAutomaton(const std::set<alpha_t> &alphabet, size_t number_of_states):
    states_(),
    number_of_states_(0),
    start_state_(0),
    final_states_(),
    existent_states_(),
    alphabet_(alphabet),
    optimize_epsilons_(false)
{
    if (number_of_states == 0)
    {
        number_of_states = 1;
        std::cerr << "Can't create an automation with 0 states. Number of states was set on 1.\n";
    }

    SetStates(number_of_states);
}

template <class alpha_t>
GenericAutomaton<alpha_t>::GenericAutomaton(std::set<alpha_t> &&alphabet, size_t number_of_states):
    states_(),
    number_of_states_(0),
    start_state_(0),
    final_states_(),
    existent_states_(),
    alphabet_(std::move(alphabet)),
    optimize_epsilons_(false)
{
    if (number_of_states == 0)
    {
        number_of_states = 1;
        std::cerr << "Can't create an automation with 0 states. Number of states was set on 1.\n";
    }

    SetStates(number_of_states); 
}

template <class alpha_type>
template <class other_alpha>
GenericAutomaton<alpha_type>::GenericAutomaton(const GenericAutomaton<other_alpha> &other):
    states_(),
    number_of_states_(other.number_of_states_),
    start_state_(other.start_state_),
    final_states_(other.final_states_),
    existent_states_(other.existent_states_),
    alphabet_(),
    optimize_epsilons_(false)
{
    for (auto &old_state : other.states_)
    {
        state_t new_state = {};
        for (auto alpha_neigh : other.GetNeighbours(old_state.first))
        {
            if constexpr (std::is_same_v<alpha_type, std::string>)
                new_state[std::isalpha(alpha_neigh.first)
                            ? std::string(1, static_cast<char>(alpha_neigh.first))
                            : std::string("[") + std::to_string(alpha_neigh.first) + "]"
                         ] = alpha_neigh.second; 
            else
                new_state[static_cast<alpha_type>(alpha_neigh.first)] = alpha_neigh.second;
        }
        states_.insert({old_state.first, new_state});
    }

    for (auto alpha : other.alphabet_)
    {
        if constexpr (std::is_same_v<alpha_type, std::string>)
            alphabet_.insert(std::isalpha(alpha)
                                ? std::string(1, static_cast<char>(alpha))
                                : std::string("[") + std::to_string(alpha) + "]"
                            );
        else
            alphabet_.insert(static_cast<alpha_type>(alpha));
    }
}

template <class alpha_t>
Automaton GenericAutomaton<alpha_t>::buildFromAnother(const Automaton &GenericAutomaton, bool optimize_epsilons)
{
    Automaton result;
    result.SetAlphabet(GenericAutomaton.alphabet_);
    result.SetOptimizeEpsilonsFlag(optimize_epsilons);

    for (auto state : GenericAutomaton.GetStateNumbers())
    {
        if (!result.DoesStateExist(state))
        {
            result.AddState(state);
            result.SetFinal(state, GenericAutomaton.IsStateFinal(state));
        }
        for (auto alpha : GenericAutomaton.GetAlphabet())
        {
            if (!GenericAutomaton.CanTransit(state, alpha))
                continue;

            for (auto neighbour : GenericAutomaton.GetNeighbours(state).at(alpha))
            {
                if (!result.DoesStateExist(neighbour))
                {
                    result.AddState(neighbour);
                    result.SetFinal(neighbour, GenericAutomaton.IsStateFinal(neighbour));
                }

                result.AddEdge(state, neighbour, alpha);
            }
        }
    }

    for (auto state : GenericAutomaton.GetStateNumbers())
    {
        if (!GenericAutomaton.CanTransit(state, Epsilon))
            continue;

        for (auto neighbour : GenericAutomaton.GetNeighbours(state).at(Epsilon))
            result.AddEdge(state, neighbour, Epsilon);
    }

    return result;
}

template <class alpha_t>
size_t GenericAutomaton<alpha_t>::AddState(size_t state_number)
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

template <class alpha_t>
void GenericAutomaton<alpha_t>::SetStates(size_t numberOfStates)
{
    states_.clear();
    existent_states_.clear();
    number_of_states_ = 0;

    for (size_t i = 0; i < numberOfStates; ++i)
        AddState(i);
}

template <class alpha_t>
void GenericAutomaton<alpha_t>::RemoveState(size_t state_number)
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

    --number_of_states_;
}

template <class alpha_t>
size_t GenericAutomaton<alpha_t>::GetNumberOfStates() const { return number_of_states_; }

template <class alpha_t>
bool GenericAutomaton<alpha_t>::AddEdge(size_t from, size_t to, alpha_t alpha)
{
    if (!DoesStateExist(from) || !DoesStateExist(to) || DoesEdgeExist(from, to, alpha))
        return false;

    if (alpha == Epsilon && optimize_epsilons_)
    {
        for (auto &neigh_alpha : GetNeighbours(to))
        {
            for (auto new_neigh : neigh_alpha.second)
                AddEdge(from, new_neigh, neigh_alpha.first);
        }

        if (IsStateFinal(to))
            SetFinal(from);

        return true;
    }

    state_t& state_from = states_[from];

    if (!state_from.contains(alpha))
        state_from[alpha] = {};

    state_from[alpha].insert(to);

    return true;
}

template <class alpha_t>
bool GenericAutomaton<alpha_t>::RemoveEdge(size_t from, size_t to, alpha_t alpha)
{
    if (!DoesEdgeExist(from, to, alpha))
        return false;

    states_[from][alpha].erase(to);

    return true;
}

template <class alpha_t>
bool GenericAutomaton<alpha_t>::DoesEdgeExist(size_t from, size_t to, alpha_t alpha) const
{
    if (!DoesStateExist(from) || !DoesStateExist(to))
        return false;

    auto& state_from = states_.at(from);

    if (!state_from.contains(alpha))
        return false;

    auto &neighbours = state_from.at(alpha);
    return neighbours.contains(to);
}

template <class alpha_t>
bool GenericAutomaton<alpha_t>::DoesStateExist(size_t state_number) const { return states_.contains(state_number); }

template <class alpha_t>
bool GenericAutomaton<alpha_t>::CanTransit(size_t state, alpha_t alpha) const
{
    if (!DoesStateExist(state))
        return false;

    auto& state_from = states_.at(state);

    return state_from.contains(alpha) && !state_from.at(alpha).empty();
}

template <class alpha_t>
const std::unordered_map<alpha_t, std::set<size_t>>& GenericAutomaton<alpha_t>::GetNeighbours(size_t vertex_number) const
{
    return states_.at(vertex_number);
}

template <class alpha_t>
void GenericAutomaton<alpha_t>::SetStartState(size_t start_state) { start_state_ = start_state; }

template <class alpha_t>
size_t GenericAutomaton<alpha_t>::GetStartState() const { return start_state_; }

template <class alpha_t>
void GenericAutomaton<alpha_t>::SetFinal(size_t state, bool is_final)
{
    if (is_final)
        final_states_.insert(state);
    else
        final_states_.erase(state);
}

template <class alpha_t>
bool GenericAutomaton<alpha_t>::IsStateFinal(size_t state) const
{
    return final_states_.contains(state);
}

template <class alpha_t>
const std::set<size_t>& GenericAutomaton<alpha_t>::GetFinalStates() const
{
    return final_states_;
}

template <class alpha_t>
const std::set<size_t>& GenericAutomaton<alpha_t>::GetStateNumbers() const { return existent_states_; }

template <class alpha_t>
void GenericAutomaton<alpha_t>::SetOptimizeEpsilonsFlag(bool optimize_epsilons)
{
    optimize_epsilons_ = optimize_epsilons;
}

template <class alpha_t>
bool GenericAutomaton<alpha_t>::GetOptimizeEpsilonsFlag() { return optimize_epsilons_; }

template <class alpha_t>
void GenericAutomaton<alpha_t>::AddCharToAlphabet(GenericAutomaton::alpha_t alpha) { alphabet_.insert(alpha); }

template <class alpha_t>
void GenericAutomaton<alpha_t>::SetAlphabet(const std::set<GenericAutomaton::alpha_t>& alphabet) { alphabet_ = alphabet; }

template <class alpha_t>
const std::set<alpha_t>& GenericAutomaton<alpha_t>::GetAlphabet() const { return alphabet_; }