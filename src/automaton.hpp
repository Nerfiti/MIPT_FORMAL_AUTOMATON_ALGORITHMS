#pragma once

#include <cstddef>
#include <limits>
#include <set>
#include <string>
#include <unordered_map>

template <class alpha_type>
class GenericAutomaton;

using Automaton = GenericAutomaton<short int>;
using RegularAutomaton = GenericAutomaton<std::string>;

template <class alpha_type>
class GenericAutomaton
{
    public:
        using alpha_t = alpha_type;
        using state_t = std::unordered_map<alpha_t, std::set<size_t>>;

        static const alpha_t Epsilon;

        GenericAutomaton(const std::set<alpha_t> &alphabet, size_t number_of_states = 1);
        GenericAutomaton(std::set<alpha_t> &&alphabet, size_t number_of_states = 1);

        template <class other_alpha>
        GenericAutomaton(const GenericAutomaton<other_alpha> &other);

        static Automaton buildFromAnother(const Automaton &automaton, bool optimize_epsilons);

        size_t AddState(size_t state_number = std::numeric_limits<size_t>::max());
        void SetStates(size_t number_of_states);
        void RemoveState(size_t state_number);
        size_t GetNumberOfStates() const;

        bool AddEdge(size_t from, size_t to, alpha_t alpha);
        bool RemoveEdge(size_t from, size_t to, alpha_t alpha);
        bool DoesEdgeExist(size_t from, size_t to, alpha_t alpha) const;
        bool DoesStateExist(size_t state_number) const;

        bool CanTransit(size_t state, alpha_t alpha) const;

        const std::unordered_map<alpha_t, std::set<size_t>>& GetNeighbours(size_t vertex_number) const;

        void SetStartState(size_t start_state);
        size_t GetStartState() const;

        void SetFinal(size_t state, bool is_final = true);
        bool IsStateFinal(size_t state) const;
        const std::set<size_t>& GetFinalStates() const;

        void SetOptimizeEpsilonsFlag(bool optimize_epsilons);
        bool GetOptimizeEpsilonsFlag();

        const std::set<size_t>& GetStateNumbers() const;

        void AddCharToAlphabet(alpha_t alpha);
        void SetAlphabet(const std::set<alpha_t>& alphabet);
        const std::set<alpha_t>& GetAlphabet() const;

        template <class> friend class GenericAutomaton;

    private:
        std::unordered_map<size_t, state_t> states_;
        size_t number_of_states_ = 0;

        size_t start_state_ = 0;
        std::set<size_t> final_states_;

        std::set<size_t> existent_states_;

        std::set<alpha_t> alphabet_;

        bool optimize_epsilons_ = false;

        GenericAutomaton() = default;
};

#include "automaton_implementation.cpp"