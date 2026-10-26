#ifndef MOTOR_INFERENCIAS_MODEL_REPORTER_HPP
#define MOTOR_INFERENCIAS_MODEL_REPORTER_HPP

#include "Evaluator.hpp"

#include <string>
#include <vector>

namespace inferencias {

class ModelReporter {
public:
    std::string formatModels(const std::vector<TruthAssignment>& models) const;
    std::string formatModels(
        const ModelSearchResult& result,
        const std::vector<Atom>& atoms = {}) const;
    std::string formatEntailment(
        const EntailmentResult& result,
        const std::vector<Atom>& atoms = {}) const;
};

} // namespace inferencias

#endif
