#include "Formula.hpp"

#include <algorithm>
#include <set>
#include <stdexcept>
#include <utility>

namespace inferencias {

namespace {

bool isBinary(Connective connective) {
    return connective == Connective::And ||
           connective == Connective::Or ||
           connective == Connective::Implies ||
           connective == Connective::Iff;
}

int precedence(Connective connective) {
    switch (connective) {
    case Connective::Atom:
        return 4;
    case Connective::Not:
        return 3;
    case Connective::And:
        return 2;
    case Connective::Or:
        return 1;
    case Connective::Implies:
        return 0;
    case Connective::Iff:
        return -1;
    }
    return -1;
}

std::string symbolicOperator(Connective connective) {
    switch (connective) {
    case Connective::Not:
        return "\xC2\xAC";
    case Connective::And:
        return "\xE2\x88\xA7";
    case Connective::Or:
        return "\xE2\x88\xA8";
    case Connective::Implies:
        return "\xE2\x86\x92";
    case Connective::Iff:
        return "\xE2\x86\x94";
    case Connective::Atom:
        break;
    }
    return {};
}

std::string textualOperator(Connective connective) {
    switch (connective) {
    case Connective::Not:
        return "NOT";
    case Connective::And:
        return "AND";
    case Connective::Or:
        return "OR";
    case Connective::Implies:
        return "->";
    case Connective::Iff:
        return "<->";
    case Connective::Atom:
        break;
    }
    return {};
}

bool needsParentheses(const Formula& child, Connective parent, bool right) {
    const int childPrecedence = precedence(child.connective());
    const int parentPrecedence = precedence(parent);

    if (childPrecedence < parentPrecedence) {
        return true;
    }
    if (right && childPrecedence == parentPrecedence &&
        (parent == Connective::Implies || parent == Connective::Iff)) {
        return true;
    }
    return false;
}

std::string render(const Formula& formula, bool symbolic) {
    if (formula.connective() == Connective::Atom) {
        return formula.symbol();
    }

    const std::string operation = symbolic
        ? symbolicOperator(formula.connective())
        : textualOperator(formula.connective());

    if (formula.connective() == Connective::Not) {
        std::string operand = render(*formula.left(), symbolic);
        if (needsParentheses(*formula.left(), formula.connective(), false)) {
            operand = "(" + operand + ")";
        }
        return symbolic ? operation + operand : operation + " " + operand;
    }

    std::string left = render(*formula.left(), symbolic);
    std::string right = render(*formula.right(), symbolic);
    if (needsParentheses(*formula.left(), formula.connective(), false)) {
        left = "(" + left + ")";
    }
    if (needsParentheses(*formula.right(), formula.connective(), true)) {
        right = "(" + right + ")";
    }

    const std::string separator = symbolic ? " " + operation + " " :
                                              " " + operation + " ";
    return left + separator + right;
}

void collectAtoms(const Formula& formula, std::vector<std::string>& result,
                  std::set<std::string>& seen) {
    if (formula.connective() == Connective::Atom) {
        if (seen.insert(formula.symbol()).second) {
            result.push_back(formula.symbol());
        }
        return;
    }
    collectAtoms(*formula.left(), result, seen);
    if (formula.right() != nullptr) {
        collectAtoms(*formula.right(), result, seen);
    }
}

} // namespace

Formula::Formula(Connective connective, std::string symbol, Ptr left, Ptr right)
    : connective_(connective),
      symbol_(std::move(symbol)),
      left_(std::move(left)),
      right_(std::move(right)) {}

Formula::Ptr Formula::atom(std::string symbol) {
    if (symbol.empty()) {
        throw std::invalid_argument("Un atomo debe tener un simbolo.");
    }
    return Formula::Ptr(new Formula(
        Connective::Atom, std::move(symbol), nullptr, nullptr));
}

Formula::Ptr Formula::unary(Connective connective, Ptr operand) {
    if (connective != Connective::Not) {
        throw std::invalid_argument("Una formula unaria solo puede usar negacion.");
    }
    if (operand == nullptr) {
        throw std::invalid_argument("La negacion requiere un operando.");
    }
    return Formula::Ptr(new Formula(
        connective, {}, std::move(operand), nullptr));
}

Formula::Ptr Formula::binary(Connective connective, Ptr left, Ptr right) {
    if (!isBinary(connective)) {
        throw std::invalid_argument(
            "Una formula binaria requiere un conector binario.");
    }
    if (left == nullptr || right == nullptr) {
        throw std::invalid_argument(
            "Una formula binaria requiere dos operandos.");
    }
    return Formula::Ptr(new Formula(
        connective, {}, std::move(left), std::move(right)));
}

Connective Formula::connective() const noexcept {
    return connective_;
}

const std::string& Formula::symbol() const noexcept {
    return symbol_;
}

const Formula::Ptr& Formula::left() const noexcept {
    return left_;
}

const Formula::Ptr& Formula::right() const noexcept {
    return right_;
}

std::string Formula::toString() const {
    return render(*this, false);
}

std::string Formula::toSymbolic() const {
    return render(*this, true);
}

std::vector<std::string> Formula::atoms() const {
    std::vector<std::string> result;
    std::set<std::string> seen;
    collectAtoms(*this, result, seen);
    return result;
}

bool Formula::evaluate(
    const std::map<std::string, bool>& assignment) const {
    switch (connective_) {
    case Connective::Atom: {
        const auto value = assignment.find(symbol_);
        if (value == assignment.end()) {
            throw std::invalid_argument(
                "Falta un valor de verdad para el atomo: " + symbol_);
        }
        return value->second;
    }
    case Connective::Not:
        return !left_->evaluate(assignment);
    case Connective::And:
        return left_->evaluate(assignment) && right_->evaluate(assignment);
    case Connective::Or:
        return left_->evaluate(assignment) || right_->evaluate(assignment);
    case Connective::Implies:
        return !left_->evaluate(assignment) || right_->evaluate(assignment);
    case Connective::Iff:
        return left_->evaluate(assignment) == right_->evaluate(assignment);
    }
    throw std::logic_error("Tipo de formula desconocido.");
}

} // namespace inferencias
