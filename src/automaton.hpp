#include <cstddef>
#include <set>
#include <unordered_map>

class Automaton
{
    public:
        using alpha_t = short int;
        using state_t = std::unordered_map<alpha_t, std::set<size_t>>;

        bool AddState(size_t state_number);
        void SetStates(size_t number_of_states);
        size_t GetNumberOfStates() const;

        bool AddEdge(size_t from, size_t to, alpha_t alpha);
        bool DoesEdgeExist(size_t from, size_t to, alpha_t alpha) const;
        bool DoesStateExist(size_t state_number) const;
        bool RemoveEdge(size_t from, size_t to, alpha_t alpha);

        const std::unordered_map<alpha_t, std::set<size_t>>& GetNeighbours(size_t vertex_number) const;

        void SetStartState(size_t start_state);
        size_t GetStartState() const;

        void MarkStateAsFinal(size_t state);
        bool RemoveStateFromFinals(size_t state);
        bool IsStateFinal(size_t state) const;
        const std::set<size_t>& GetFinalStates() const;

    private:
        std::unordered_map<size_t, state_t> states_;
        size_t number_of_states_ = 0;

        size_t start_state_ = 0;
        std::set<size_t> final_states_;
};