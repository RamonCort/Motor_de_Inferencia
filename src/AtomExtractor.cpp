#include "AtomExtractor.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string_view>

namespace inferencias {

namespace {

bool isAsciiSpace(unsigned char character) {
    return std::isspace(character) != 0;
}

bool isFinalPunctuation(unsigned char character) {
    return character == '.' || character == ',' || character == ';' ||
           character == ':' || character == '!' || character == '?';
}

std::string trimAndCollapseSpaces(const std::string& text) {
    std::string result;
    bool pendingSpace = false;

    for (unsigned char character : text) {
        if (isAsciiSpace(character)) {
            pendingSpace = !result.empty();
            continue;
        }
        if (pendingSpace) {
            result.push_back(' ');
            pendingSpace = false;
        }
        result.push_back(static_cast<char>(character));
    }

    bool removedTrailingCharacter = true;
    while (!result.empty() && removedTrailingCharacter) {
        removedTrailingCharacter = false;
        while (!result.empty() &&
               isAsciiSpace(static_cast<unsigned char>(result.back()))) {
            result.pop_back();
            removedTrailingCharacter = true;
        }
        while (!result.empty() &&
               isFinalPunctuation(static_cast<unsigned char>(result.back()))) {
            result.pop_back();
            removedTrailingCharacter = true;
        }
    }
    return result;
}

void replaceAll(std::string& text, std::string_view from,
                std::string_view to) {
    std::size_t position = 0;
    while ((position = text.find(from, position)) != std::string::npos) {
        text.replace(position, from.size(), to);
        position += to.size();
    }
}

std::string normalize(const std::string& sentence) {
    std::string result = trimAndCollapseSpaces(sentence);

    for (char& character : result) {
        const auto value = static_cast<unsigned char>(character);
        if (value >= 'A' && value <= 'Z') {
            character = static_cast<char>(value - 'A' + 'a');
        }
    }

    // UTF-8 no tiene un case-folding completo en la biblioteca estandar.
    // Estas sustituciones cubren las mayusculas espanolas mas habituales.
    replaceAll(result, "\xC3\x81", "\xC3\xA1"); // A con tilde
    replaceAll(result, "\xC3\x89", "\xC3\xA9"); // E con tilde
    replaceAll(result, "\xC3\x8D", "\xC3\xAD"); // I con tilde
    replaceAll(result, "\xC3\x93", "\xC3\xB3"); // O con tilde
    replaceAll(result, "\xC3\x9A", "\xC3\xBA"); // U con tilde
    replaceAll(result, "\xC3\x91", "\xC3\xB1"); // N con tilde
    replaceAll(result, "\xC2\xBF", "");          // ¿
    replaceAll(result, "\xC2\xA1", "");          // ¡
    return result;
}

std::string symbolForIndex(std::size_t index) {
    if (index < 11) {
        return std::string(1, static_cast<char>('P' + index));
    }

    std::size_t value = index - 11 + 26;
    std::string symbol;
    do {
        symbol.insert(symbol.begin(),
                      static_cast<char>('A' + (value % 26)));
        value = value / 26;
        if (value != 0) {
            --value;
        }
    } while (value != 0);

    return symbol;
}

} // namespace

Atom AtomExtractor::registerAtom(const std::string& sentence) {
    const std::string normalizedSentence = normalize(sentence);
    if (normalizedSentence.empty()) {
        throw std::invalid_argument(
            "No se puede registrar una proposicion vacia.");
    }

    const Atom* existing = find(sentence);
    if (existing != nullptr) {
        return *existing;
    }

    const std::size_t index = atoms_.size();
    Atom atom;
    atom.symbol = symbolForIndex(index);
    atom.sentence = sentence;
    atom.normalized = normalizedSentence;
    atom.id = static_cast<std::uint64_t>(index + 1);
    atoms_.push_back(atom);
    return atom;
}

bool AtomExtractor::contains(const std::string& sentence) const {
    return find(sentence) != nullptr;
}

const Atom* AtomExtractor::find(const std::string& sentence) const noexcept {
    const std::string normalizedSentence = normalize(sentence);
    const auto match = std::find_if(
        atoms_.begin(), atoms_.end(),
        [&normalizedSentence](const Atom& atom) {
            return atom.normalized == normalizedSentence;
        });
    return match == atoms_.end() ? nullptr : &*match;
}

const Atom* AtomExtractor::findBySymbol(
    const std::string& symbol) const noexcept {
    const auto match = std::find_if(
        atoms_.begin(), atoms_.end(),
        [&symbol](const Atom& atom) { return atom.symbol == symbol; });
    return match == atoms_.end() ? nullptr : &*match;
}

std::string AtomExtractor::symbolFor(const std::string& sentence) const {
    const Atom* atom = find(sentence);
    return atom == nullptr ? std::string{} : atom->symbol;
}

std::string AtomExtractor::sentenceFor(const std::string& symbol) const {
    const Atom* atom = findBySymbol(symbol);
    return atom == nullptr ? std::string{} : atom->sentence;
}

const std::vector<Atom>& AtomExtractor::atoms() const noexcept {
    return atoms_;
}

std::vector<Atom> AtomExtractor::extract(const std::string& text) {
    std::vector<Atom> extracted;
    std::size_t lineStart = 0;

    while (lineStart <= text.size()) {
        const std::size_t lineEnd = text.find('\n', lineStart);
        const std::size_t length = lineEnd == std::string::npos
            ? text.size() - lineStart
            : lineEnd - lineStart;
        const std::string line = text.substr(lineStart, length);
        if (!normalize(line).empty()) {
            const Atom* existing = find(line);
            if (existing == nullptr) {
                extracted.push_back(registerAtom(line));
            }
        }
        if (lineEnd == std::string::npos) {
            break;
        }
        lineStart = lineEnd + 1;
    }
    return extracted;
}

} // namespace inferencias
