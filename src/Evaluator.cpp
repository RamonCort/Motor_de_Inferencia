#include "Evaluator.hpp"

#include <algorithm>
#include <limits>
#include <set>
#include <utility>

namespace inferencias {

namespace {

void evaluateExtensions(const Formula& query, const std::vector<std::string>&
                        missing, std::size_t index, TruthAssignment assignment,
                        std::vector<bool>& values) {
    if (index == missing.size()) {
        values.push_back(query.evaluate(assignment));
        return;
    }
    assignment[missing[index]] = false;
    evaluateExtensions(query, missing, index + 1, assignment, values);
    assignment[missing[index]] = true;
    evaluateExtensions(query, missing, index + 1, assignment, values);
}

} // namespace

bool Evaluator::evaluate(
    const Formula& formula, const TruthAssignment& assignment) const {
    return formula.evaluate(assignment);
}

std::vector<std::string> Evaluator::relevantAtoms(
    const std::vector<Formula::Ptr>& premises) const {
    std::vector<std::string> result;
    std::set<std::string> seen;
    for (const auto& premise : premises) {
        if (premise == nullptr) {
            continue;
        }
        for (const auto& symbol : premise->atoms()) {
            if (seen.insert(symbol).second) {
                result.push_back(symbol);
            }
        }
    }
    return result;
}

ModelSearchResult Evaluator::enumerateModels(
    const std::vector<Formula::Ptr>& premises,
    std::size_t maxAssignments) const {
    ModelSearchResult result;
    result.atomSymbols = relevantAtoms(premises);
    result.maxAssignments = maxAssignments;

    if (result.atomSymbols.size() >= sizeof(std::size_t) * 8 ||
        result.atomSymbols.size() >= 63) {
        result.completed = false;
        return result;
    }

    const std::size_t totalAssignments =
        std::size_t{1} << result.atomSymbols.size();
    if (totalAssignments > maxAssignments) {
        result.completed = false;
        return result;
    }

    for (std::size_t mask = 0; mask < totalAssignments; ++mask) {
        TruthAssignment assignment;
        for (std::size_t index = 0; index < result.atomSymbols.size(); ++index) {
            assignment[result.atomSymbols[index]] =
                (mask & (std::size_t{1} << index)) != 0;
        }
        ++result.assignmentsEvaluated;

        bool satisfies = true;
        for (const auto& premise : premises) {
            if (premise == nullptr || !evaluate(*premise, assignment)) {
                satisfies = false;
                break;
            }
        }
        if (satisfies) {
            result.models.push_back(std::move(assignment));
        }
    }
    return result;
}

EntailmentResult Evaluator::checkEntailment(
    const std::vector<Formula::Ptr>& premises,
    const Formula& query,
    std::size_t maxAssignments) const {
    EntailmentResult result;
    result.premiseSearch = enumerateModels(premises, maxAssignments);
    if (result.premiseSearch.interrupted()) {
        result.status = QueryStatus::Interrupted;
        return result;
    }
    for (const auto& model : result.premiseSearch.models) {
        std::vector<std::string> missing;
        for (const auto& symbol : query.atoms()) {
            if (model.find(symbol) == model.end()) {
                missing.push_back(symbol);
            }
        }
        if (missing.empty()) {
            result.queryValues.push_back(evaluate(query, model));
        } else {
            evaluateExtensions(
                query, missing, 0, model, result.queryValues);
        }
    }
    if (result.premiseSearch.models.empty()) {
        result.status = QueryStatus::PremisesInconsistent;
        return result;
    }

    result.countermodelSearch.atomSymbols = result.premiseSearch.atomSymbols;
    for (const auto& symbol : query.atoms()) {
        if (std::find(result.countermodelSearch.atomSymbols.begin(),
                      result.countermodelSearch.atomSymbols.end(), symbol) ==
            result.countermodelSearch.atomSymbols.end()) {
            result.countermodelSearch.atomSymbols.push_back(symbol);
        }
    }
    result.countermodelSearch.maxAssignments = maxAssignments;
    if (result.countermodelSearch.atomSymbols.size() >=
        sizeof(std::size_t) * 8 ||
        result.countermodelSearch.atomSymbols.size() >= 63) {
        result.countermodelSearch.completed = false;
        result.status = QueryStatus::Interrupted;
        return result;
    }
    const std::size_t total =
        std::size_t{1} << result.countermodelSearch.atomSymbols.size();
    if (total > maxAssignments) {
        result.countermodelSearch.completed = false;
        result.status = QueryStatus::Interrupted;
        return result;
    }
    for (std::size_t mask = 0; mask < total; ++mask) {
        TruthAssignment assignment;
        for (std::size_t index = 0;
             index < result.countermodelSearch.atomSymbols.size(); ++index) {
            assignment[result.countermodelSearch.atomSymbols[index]] =
                (mask & (std::size_t{1} << index)) != 0;
        }
        ++result.countermodelSearch.assignmentsEvaluated;
        bool premisesHold = true;
        for (const auto& premise : premises) {
            if (!evaluate(*premise, assignment)) {
                premisesHold = false;
                break;
            }
        }
        if (premisesHold && !evaluate(query, assignment)) {
            result.countermodelSearch.models.push_back(std::move(assignment));
        }
    }
    result.status = result.countermodelSearch.models.empty()
        ? QueryStatus::Deduced
        : QueryStatus::NotDeduced;
    return result;
}

} // namespace inferencias
