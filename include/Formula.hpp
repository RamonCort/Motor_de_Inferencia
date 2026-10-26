#ifndef MOTOR_INFERENCIAS_FORMULA_HPP
#define MOTOR_INFERENCIAS_FORMULA_HPP

#include <memory>
#include <map>
#include <string>
#include <vector>

namespace inferencias {

enum class Connective {
    Atom,
    Not,
    And,
    Or,
    Implies,
    Iff
};

class Formula {
public:
    using Ptr = std::shared_ptr<const Formula>;

    static Ptr atom(std::string symbol);
    static Ptr unary(Connective connective, Ptr operand);
    static Ptr binary(Connective connective, Ptr left, Ptr right);

    Connective connective() const noexcept;
    const std::string& symbol() const noexcept;
    const Ptr& left() const noexcept;
    const Ptr& right() const noexcept;

    std::string toString() const;
    std::string toSymbolic() const;
    std::vector<std::string> atoms() const;
    bool evaluate(const std::map<std::string, bool>& assignment) const;

private:
    Formula(Connective connective, std::string symbol, Ptr left, Ptr right);

    Connective connective_;
    std::string symbol_;
    Ptr left_;
    Ptr right_;
};

} // namespace inferencias

#endif
