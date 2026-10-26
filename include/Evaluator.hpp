#ifndef MOTOR_INFERENCIAS_EVALUATOR_HPP
#define MOTOR_INFERENCIAS_EVALUATOR_HPP

#include "Formula.hpp"
#include "AtomExtractor.hpp"

#include <map>
#include <cstddef>
#include <string>
#include <vector>

namespace inferencias {

using TruthAssignment = std::map<std::string, bool>;

struct ModelSearchResult {
    std::vector<std::string> atomSymbols;
    std::vector<TruthAssignment> models;
    std::size_t assignmentsEvaluated{0};
    std::size_t maxAssignments{0};
    bool completed{true};

    bool satisfiable() const noexcept {
        return completed && !models.empty();
    }

    bool interrupted() const noexcept {
        return !completed;
    }
};

enum class QueryStatus {
    NotEvaluated,
    Deduced,
    NotDeduced,
    PremisesInconsistent,
    Interrupted
};

struct EntailmentResult {
    ModelSearchResult premiseSearch;
    ModelSearchResult countermodelSearch;
    QueryStatus status{QueryStatus::NotEvaluated};
    std::vector<bool> queryValues;
};

class Evaluator {
public:
    bool evaluate(const Formula& formula, const TruthAssignment& assignment) const;
    std::vector<std::string> relevantAtoms(
        const std::vector<Formula::Ptr>& premises) const;
    ModelSearchResult enumerateModels(
        const std::vector<Formula::Ptr>& premises,
        std::size_t maxAssignments = 1048576) const;
    EntailmentResult checkEntailment(
        const std::vector<Formula::Ptr>& premises,
        const Formula& query,
        std::size_t maxAssignments = 1048576) const;
};

} // namespace inferencias

#endif
