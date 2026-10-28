#include "SemanticTableau.hpp"

#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace inferencias {

namespace {

using FormulaList = std::vector<Formula::Ptr>;

Formula::Ptr negate(Formula::Ptr formula) {
    if (formula->connective() == Connective::Not) {
        return formula->left();
    }
    return Formula::unary(Connective::Not, std::move(formula));
}

Formula::Ptr toNnf(const Formula::Ptr& formula, bool positive) {
    if (formula->connective() == Connective::Atom) {
        return positive ? formula : negate(formula);
    }

    if (formula->connective() == Connective::Not) {
        return toNnf(formula->left(), !positive);
    }

    const auto left = formula->left();
    const auto right = formula->right();
    if (positive) {
        switch (formula->connective()) {
        case Connective::And:
            return Formula::binary(
                Connective::And, toNnf(left, true), toNnf(right, true));
        case Connective::Or:
            return Formula::binary(
                Connective::Or, toNnf(left, true), toNnf(right, true));
        case Connective::Implies:
            return Formula::binary(
                Connective::Or, toNnf(left, false), toNnf(right, true));
        case Connective::Iff:
            return Formula::binary(
                Connective::Or,
                Formula::binary(Connective::And,
                                toNnf(left, true), toNnf(right, true)),
                Formula::binary(Connective::And,
                                toNnf(left, false), toNnf(right, false)));
        case Connective::Atom:
        case Connective::Not:
            break;
        }
    } else {
        switch (formula->connective()) {
        case Connective::And:
            return Formula::binary(
                Connective::Or, toNnf(left, false), toNnf(right, false));
        case Connective::Or:
            return Formula::binary(
                Connective::And, toNnf(left, false), toNnf(right, false));
        case Connective::Implies:
            return Formula::binary(
                Connective::And, toNnf(left, true), toNnf(right, false));
        case Connective::Iff:
            return Formula::binary(
                Connective::Or,
                Formula::binary(Connective::And,
                                toNnf(left, true), toNnf(right, false)),
                Formula::binary(Connective::And,
                                toNnf(left, false), toNnf(right, true)));
        case Connective::Atom:
        case Connective::Not:
            break;
        }
    }
    throw std::logic_error("No se pudo convertir la formula a FNN.");
}

bool containsFormula(const FormulaList& formulas, const Formula::Ptr& formula) {
    return std::any_of(
        formulas.begin(), formulas.end(),
        [&formula](const Formula::Ptr& candidate) {
            return candidate->toSymbolic() == formula->toSymbolic();
        });
}

void appendUnique(FormulaList& formulas, const Formula::Ptr& formula) {
    if (!containsFormula(formulas, formula)) {
        formulas.push_back(formula);
    }
}

bool isLiteral(const Formula::Ptr& formula) {
    return formula->connective() == Connective::Atom ||
           (formula->connective() == Connective::Not &&
            formula->left()->connective() == Connective::Atom);
}

std::string literalKey(const Formula::Ptr& formula) {
    const std::string symbol = formula->connective() == Connective::Atom
        ? formula->symbol()
        : formula->left()->symbol();
    return (formula->connective() == Connective::Atom ? "+" : "-") + symbol;
}

std::map<std::string, bool> assignmentFor(const FormulaList& formulas) {
    std::map<std::string, bool> assignment;
    for (const auto& formula : formulas) {
        if (!isLiteral(formula)) {
            continue;
        }
        const std::string symbol = formula->connective() == Connective::Atom
            ? formula->symbol()
            : formula->left()->symbol();
        assignment[symbol] = formula->connective() == Connective::Atom;
    }
    return assignment;
}

bool isClosed(const FormulaList& formulas, std::string& reason) {
    std::set<std::string> positive;
    std::set<std::string> negative;
    for (const auto& formula : formulas) {
        if (!isLiteral(formula)) {
            continue;
        }
        const std::string key = literalKey(formula);
        const std::string symbol = key.substr(1);
        if (key.front() == '+') {
            positive.insert(symbol);
        } else {
            negative.insert(symbol);
        }
    }
    for (const std::string& symbol : positive) {
        if (negative.count(symbol) != 0) {
            reason = "Contradiccion: " + symbol + " y NOT " + symbol;
            return true;
        }
    }
    return false;
}

std::string stateName(TableauNodeState state) {
    switch (state) {
    case TableauNodeState::Pending:
        return "pendiente";
    case TableauNodeState::Expanded:
        return "expandido";
    case TableauNodeState::Open:
        return "abierta";
    case TableauNodeState::Closed:
        return "cerrada";
    }
    return "desconocido";
}

std::string escapeDot(const std::string& text) {
    std::string result;
    for (char character : text) {
        if (character == '"' || character == '\\') {
            result.push_back('\\');
        }
        if (character == '\n') {
            result += "\\n";
        } else {
            result.push_back(character);
        }
    }
    return result;
}

void renderText(const std::shared_ptr<TableauNode>& node, std::string prefix,
                std::ostringstream& output) {
    output << prefix << "Nodo " << node->id << " ["
           << stateName(node->state) << "]\n";
    for (const auto& formula : node->formulas) {
        output << prefix << "  " << formula->toString() << "\n";
    }
    if (!node->appliedFormula.empty()) {
        output << prefix << "  Aplicada: " << node->appliedFormula
               << " [OK]\n";
    }
    if (!node->expansion.empty()) {
        output << prefix << "  Regla: " << node->expansion << "\n";
    }
    if (!node->closureReason.empty()) {
        output << prefix << "  " << node->closureReason << "\n";
    }
    for (const auto& child : node->children) {
        renderText(child, prefix + "  ", output);
    }
}

void renderDot(const std::shared_ptr<TableauNode>& node, std::ostringstream& out) {
    std::ostringstream label;
    label << "Nodo " << node->id << " [" << stateName(node->state) << "]";
    for (const auto& formula : node->formulas) {
        label << "\\n" << formula->toString();
    }
    if (!node->expansion.empty()) {
        label << "\\n" << node->expansion;
    }
    out << "  n" << node->id << " [label=\""
        << escapeDot(label.str()) << "\"];\n";
    for (const auto& child : node->children) {
        out << "  n" << node->id << " -> n" << child->id << ";\n";
        renderDot(child, out);
    }
}

} // namespace

void SemanticTableau::addPremise(Formula::Ptr premise) {
    if (premise == nullptr) {
        throw std::invalid_argument("Una premisa no puede ser nula.");
    }
    premises_.push_back(std::move(premise));
    built_ = false;
}

void SemanticTableau::clear() {
    premises_.clear();
    root_.reset();
    branches_.clear();
    built_ = false;
    nextNodeId_ = 1;
}

const std::vector<Formula::Ptr>& SemanticTableau::premises() const noexcept {
    return premises_;
}

const std::shared_ptr<TableauNode>& SemanticTableau::root() const noexcept {
    return root_;
}

const std::vector<TableauBranch>& SemanticTableau::branches() const noexcept {
    return branches_;
}

bool SemanticTableau::isSatisfiable() const noexcept {
    return std::any_of(
        branches_.begin(), branches_.end(),
        [](const TableauBranch& branch) { return branch.open; });
}

bool SemanticTableau::isBuilt() const noexcept {
    return built_;
}

void SemanticTableau::build() {
    root_ = std::make_shared<TableauNode>();
    root_->id = nextNodeId_++;
    for (const auto& premise : premises_) {
        appendUnique(root_->formulas, toNnf(premise, true));
    }
    branches_.clear();

    std::function<void(const std::shared_ptr<TableauNode>&,
                       std::vector<std::string>)> expand;
    expand = [&](const std::shared_ptr<TableauNode>& node,
                 std::vector<std::string> history) {
        std::string closureReason;
        if (isClosed(node->formulas, closureReason)) {
            node->state = TableauNodeState::Closed;
            node->closureReason = closureReason;
            history.push_back(closureReason);
            TableauBranch branch;
            branch.formulas = node->formulas;
            branch.open = false;
            branch.history = std::move(history);
            branch.assignment = assignmentFor(node->formulas);
            branches_.push_back(std::move(branch));
            return;
        }

        auto candidate = std::find_if(
            node->formulas.begin(), node->formulas.end(),
            [](const Formula::Ptr& formula) { return !isLiteral(formula); });
        if (candidate == node->formulas.end()) {
            node->state = TableauNodeState::Open;
            history.push_back("Rama abierta: no quedan expansiones.");
            TableauBranch branch;
            branch.formulas = node->formulas;
            branch.open = true;
            branch.history = std::move(history);
            branch.assignment = assignmentFor(node->formulas);
            branches_.push_back(std::move(branch));
            return;
        }

        const Formula::Ptr selected = *candidate;
        FormulaList base;
        for (const auto& formula : node->formulas) {
            if (formula != selected) {
                appendUnique(base, formula);
            }
        }

        std::vector<FormulaList> alternatives;
        std::string rule;
        const Connective connective = selected->connective();
        if (connective == Connective::And) {
            base.push_back(selected->left());
            appendUnique(base, selected->right());
            alternatives.push_back(std::move(base));
            rule = "alfa: " + selected->toString() + " agrega ambos operandos";
        } else if (connective == Connective::Or) {
            alternatives.push_back(base);
            appendUnique(alternatives.back(), selected->left());
            FormulaList second = base;
            appendUnique(second, selected->right());
            alternatives.push_back(std::move(second));
            rule = "beta: " + selected->toString() + " divide la rama";
        } else {
            throw std::logic_error("La FNN contiene un conector no permitido.");
        }

        node->state = TableauNodeState::Expanded;
        node->appliedFormula = selected->toString();
        node->expansion = rule;
        history.push_back(rule);
        for (auto& formulas : alternatives) {
            auto child = std::make_shared<TableauNode>();
            child->id = nextNodeId_++;
            child->formulas = std::move(formulas);
            node->children.push_back(child);
            expand(child, history);
        }
    };

    expand(root_, {});
    built_ = true;
}

std::string SemanticTableau::toText() const {
    if (!root_) {
        return "Tableau no construido.\n";
    }
    std::ostringstream output;
    renderText(root_, {}, output);
    output << "Ramas y asignaciones:\n";
    for (std::size_t index = 0; index < branches_.size(); ++index) {
        output << "  Rama " << index + 1 << (branches_[index].open
            ? " [abierta]: " : " [cerrada]: ");
        bool first = true;
        for (const auto& [symbol, value] : branches_[index].assignment) {
            if (!first) {
                output << ", ";
            }
            first = false;
            output << symbol << "=" << (value ? "V" : "F");
        }
        output << "\n";
    }
    return output.str();
}

std::string SemanticTableau::toDot() const {
    std::ostringstream output;
    output << "digraph SemanticTableau {\n";
    if (root_) {
        renderDot(root_, output);
    }
    output << "}\n";
    return output.str();
}

} // namespace inferencias
