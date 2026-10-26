#include "NaturalLanguageParser.hpp"

#include <cctype>
#include <stdexcept>
#include <utility>

namespace inferencias {

bool ParseResult::ok() const noexcept {
    return formula != nullptr && error.empty();
}

namespace {

std::string trim(const std::string& text) {
    std::size_t first = 0;
    while (first < text.size() &&
           std::isspace(static_cast<unsigned char>(text[first])) != 0) {
        ++first;
    }
    std::size_t last = text.size();
    while (last > first &&
           std::isspace(static_cast<unsigned char>(text[last - 1])) != 0) {
        --last;
    }
    return text.substr(first, last - first);
}

std::string lowerAscii(std::string text) {
    for (char& character : text) {
        const auto value = static_cast<unsigned char>(character);
        if (value >= 'A' && value <= 'Z') {
            character = static_cast<char>(value - 'A' + 'a');
        }
    }
    return text;
}

std::string canonicalAtomText(const std::string& text) {
    std::string value = trim(text);
    while (!value.empty() &&
           (value.back() == '.' || value.back() == '!' ||
            value.back() == '?')) {
        value.pop_back();
        value = trim(value);
    }
    std::string lower = lowerAscii(value);
    if (lower == "llueve" || lower == "esta lloviendo" ||
        lower == "está lloviendo" || lower == "Está lloviendo") {
        return "Está lloviendo";
    }
    return value;
}

bool hasOuterParentheses(const std::string& text) {
    if (text.size() < 2 || text.front() != '(' || text.back() != ')') {
        return false;
    }
    int depth = 0;
    for (std::size_t index = 0; index < text.size(); ++index) {
        if (text[index] == '(') {
            ++depth;
        } else if (text[index] == ')') {
            --depth;
            if (depth == 0 && index + 1 != text.size()) {
                return false;
            }
            if (depth < 0) {
                return false;
            }
        }
    }
    return depth == 0;
}

bool hasBalancedParentheses(const std::string& text) {
    int depth = 0;
    for (char character : text) {
        if (character == '(') {
            ++depth;
        } else if (character == ')' && --depth < 0) {
            return false;
        }
    }
    return depth == 0;
}

bool isBoundary(const std::string& text, std::size_t position) {
    return position >= text.size() ||
           std::isspace(static_cast<unsigned char>(text[position])) != 0 ||
           text[position] == '(' || text[position] == ')' ||
           text[position] == ',';
}

std::size_t findKeyword(const std::string& text, const std::string& keyword) {
    int depth = 0;
    for (std::size_t index = 0; index + keyword.size() <= text.size();
         ++index) {
        if (text[index] == '(') {
            ++depth;
        } else if (text[index] == ')') {
            --depth;
        }
        if (depth != 0 || text.compare(index, keyword.size(), keyword) != 0) {
            continue;
        }
        const bool left = index == 0 || isBoundary(text, index - 1);
        const bool right = isBoundary(text, index + keyword.size());
        if (left && right) {
            return index;
        }
    }
    return std::string::npos;
}

struct Parser {
    const NaturalLanguageParser& owner;

    Formula::Ptr parseExpression(const std::string& text) const {
        const std::string value = trim(text);
        if (value.empty()) {
            throw std::invalid_argument("Se esperaba una proposicion.");
        }
        if (!hasBalancedParentheses(value)) {
            throw std::invalid_argument("Los parentesis no estan balanceados.");
        }
        if (hasOuterParentheses(value)) {
            return parseExpression(value.substr(1, value.size() - 2));
        }

        const std::string lower = lowerAscii(value);
        std::size_t position = findKeyword(lower, "si y solo si");
        if (position != std::string::npos) {
            return binary(value, position, "si y solo si",
                          Connective::Iff);
        }

        if (lower.rfind("si ", 0) == 0) {
            const std::size_t thenPosition = findKeyword(lower, "entonces");
            if (thenPosition == std::string::npos) {
                throw std::invalid_argument(
                    "La implicacion requiere 'entonces'.");
            }
            const std::size_t comma = value.find(',', 3);
            const std::size_t split = comma != std::string::npos &&
                    comma < thenPosition ? comma : thenPosition;
            std::string left = value.substr(3, split - 3);
            if (!left.empty() && left.back() == ',') {
                left.pop_back();
            }
            const std::size_t rightStart = thenPosition +
                std::string("entonces").size();
            return Formula::binary(
                Connective::Implies, parseExpression(left),
                parseExpression(value.substr(rightStart)));
        }

        position = findKeyword(lower, "implica");
        if (position != std::string::npos) {
            return binary(value, position, "implica", Connective::Implies);
        }
        position = findKeyword(lower, "o");
        if (position != std::string::npos) {
            return binary(value, position, "o", Connective::Or);
        }
        position = findKeyword(lower, "y");
        if (position != std::string::npos) {
            return binary(value, position, "y", Connective::And);
        }
        if (lower.rfind("no ", 0) == 0) {
            return Formula::unary(
                Connective::Not, parseExpression(value.substr(3)));
        }
        if (lower.rfind("no(", 0) == 0 && value.back() == ')') {
            return Formula::unary(
                Connective::Not, parseExpression(value.substr(2)));
        }

        if (findKeyword(lower, "entonces") != std::string::npos ||
            findKeyword(lower, "si") != std::string::npos) {
            throw std::invalid_argument(
                "Se encontro un conector de implicacion mal formado.");
        }
        if (findKeyword(lower, "solo") != std::string::npos) {
            throw std::invalid_argument(
                "El bicondicional requiere 'si y solo si'.");
        }
        for (const char* unsupported : {
                 "pero", "aunque", "porque", "cuando", "ni"}) {
            if (findKeyword(lower, unsupported) != std::string::npos) {
                throw std::invalid_argument(
                    std::string("El conector '") + unsupported +
                    "' no forma parte de la gramatica aceptada.");
            }
        }
        return atom(value);
    }

    Formula::Ptr atom(const std::string& text) const {
        try {
            const Atom registered = owner.registerAtom(canonicalAtomText(text));
            return Formula::atom(registered.symbol);
        } catch (const std::invalid_argument& error) {
            throw std::invalid_argument(error.what());
        }
    }

    Formula::Ptr binary(const std::string& text, std::size_t position,
                        const std::string& keyword,
                        Connective connective) const {
        const std::string left = text.substr(0, position);
        const std::string right = text.substr(position + keyword.size());
        return Formula::binary(
            connective, parseExpression(left), parseExpression(right));
    }
};

} // namespace

ParseResult NaturalLanguageParser::parse(const std::string& text) const {
    ParseResult result;
    result.original = text;
    try {
        const std::string cleaned = trim(text);
        Parser parser{*this};
        result.formula = parser.parseExpression(cleaned);
        for (const std::string& symbol : result.formula->atoms()) {
            const Atom* atom = atomExtractor_.findBySymbol(symbol);
            if (atom != nullptr) {
                result.atoms.push_back(*atom);
            }
        }
    } catch (const std::invalid_argument& error) {
        result.error = error.what();
    }
    return result;
}

const AtomExtractor& NaturalLanguageParser::atomExtractor() const noexcept {
    return atomExtractor_;
}

Atom NaturalLanguageParser::registerAtom(const std::string& sentence) const {
    return atomExtractor_.registerAtom(sentence);
}

} // namespace inferencias
