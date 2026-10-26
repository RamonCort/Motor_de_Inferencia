#ifndef MOTOR_INFERENCIAS_ATOM_EXTRACTOR_HPP
#define MOTOR_INFERENCIAS_ATOM_EXTRACTOR_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace inferencias {

struct Atom {
    std::string symbol;
    std::string sentence;
    std::string normalized;
    std::uint64_t id{0};
};

class AtomExtractor {
public:
    Atom registerAtom(const std::string& sentence);
    bool contains(const std::string& sentence) const;
    const Atom* find(const std::string& sentence) const noexcept;
    const Atom* findBySymbol(const std::string& symbol) const noexcept;
    std::string symbolFor(const std::string& sentence) const;
    std::string sentenceFor(const std::string& symbol) const;
    const std::vector<Atom>& atoms() const noexcept;

    // Cada linea no vacia se registra como una proposicion independiente.
    std::vector<Atom> extract(const std::string& text);

private:
    std::vector<Atom> atoms_;
};

} // namespace inferencias

#endif
