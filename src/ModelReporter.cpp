#include "ModelReporter.hpp"

#include <map>
#include <sstream>

namespace inferencias {

std::string ModelReporter::formatModels(
    const std::vector<TruthAssignment>& models) const {
    ModelSearchResult result;
    result.models = models;
    result.completed = true;
    return formatModels(result);
}

std::string ModelReporter::formatModels(
    const ModelSearchResult& result, const std::vector<Atom>& atoms) const {
    std::map<std::string, std::string> names;
    for (const auto& atom : atoms) {
        names[atom.symbol] = atom.sentence;
    }

    std::ostringstream output;
    output << "Premisas: ";
    if (result.interrupted()) {
        output << "NO DETERMINADA\n";
        output << "Advertencia: se supero el limite de asignaciones ("
               << result.maxAssignments << ").\n";
        return output.str();
    }
    output << (result.satisfiable() ? "SATISFACIBLES" : "INSATISFACIBLES")
           << "\n";
    output << "Modelos validos: " << result.models.size() << "\n";
    for (std::size_t index = 0; index < result.models.size(); ++index) {
        output << "Modelo " << index + 1 << ": ";
        bool first = true;
        for (const auto& symbol : result.atomSymbols) {
            if (!first) {
                output << ", ";
            }
            first = false;
            output << symbol;
            const auto name = names.find(symbol);
            if (name != names.end() && !name->second.empty()) {
                output << " (" << name->second << ")";
            }
            output << "=" << (result.models[index].at(symbol) ? "V" : "F");
        }
        output << "\n";
    }
    if (!result.satisfiable()) {
        output << "Advertencia: las premisas son contradictorias.\n";
    }
    return output.str();
}

std::string ModelReporter::formatEntailment(
    const EntailmentResult& result, const std::vector<Atom>& atoms) const {
    std::ostringstream output;
    output << formatModels(result.premiseSearch, atoms);
    output << "Consulta: ";
    switch (result.status) {
    case QueryStatus::Deduced:
        output << "DEDUCIDA\n";
        output << "Explicacion: no existe un modelo de las premisas en el "
                   "que la consulta sea falsa.\n";
        break;
    case QueryStatus::NotDeduced:
        output << "NO DEDUCIDA\n";
        output << "Explicacion: existe al menos un contra-modelo.\n";
        break;
    case QueryStatus::PremisesInconsistent:
        output << "NO EVALUADA\n";
        output << "Advertencia: las premisas son insatisfacibles; la "
                   "implicacion clasica seria vacuamente verdadera, pero no "
                   "se considera una demostracion consistente.\n";
        break;
    case QueryStatus::Interrupted:
        output << "NO EVALUADA\n";
        output << "Advertencia: el calculo fue interrumpido por el limite.\n";
        break;
    case QueryStatus::NotEvaluated:
        output << "NO EVALUADA\n";
        break;
    }
    return output.str();
}

} // namespace inferencias
