#ifndef MOTOR_INFERENCIAS_NATURAL_LANGUAGE_PARSER_HPP
#define MOTOR_INFERENCIAS_NATURAL_LANGUAGE_PARSER_HPP

#include "Formula.hpp"
#include "AtomExtractor.hpp"

#include <string>
#include <vector>

namespace inferencias {

struct ParseResult {
    std::string original;
    Formula::Ptr formula;
    std::vector<Atom> atoms;
    std::string error;

    bool ok() const noexcept;
};

class NaturalLanguageParser {
public:
    ParseResult parse(const std::string& text) const;
    const AtomExtractor& atomExtractor() const noexcept;
    Atom registerAtom(const std::string& sentence) const;

private:
    mutable AtomExtractor atomExtractor_;
};

} // namespace inferencias

#endif
