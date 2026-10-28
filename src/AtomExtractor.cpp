#include "AtomExtractor.hpp"

#include <algorithm>
#include <array>
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
    struct EquivalentPhrase {
        std::string_view canonical;
        std::string_view alternative;
    };
    static constexpr std::array aliases{
        EquivalentPhrase{"camino", "estoy caminando"},
        EquivalentPhrase{"corro", "estoy corriendo"},
        EquivalentPhrase{"manejo", "estoy conduciendo"},
        EquivalentPhrase{"viajo", "estoy viajando"},
        EquivalentPhrase{"manejo moto", "voy en motocicleta"},
        EquivalentPhrase{"tomo el bus", "voy en transporte público"},
        EquivalentPhrase{"camino", "voy a pie"},
        EquivalentPhrase{"vuelo", "estoy viajando en avión"},
        EquivalentPhrase{"estudio", "estoy estudiando"},
        EquivalentPhrase{"leo", "estoy leyendo"},
        EquivalentPhrase{"escribo", "estoy escribiendo"},
        EquivalentPhrase{"asisto a clase", "estoy en clase"},
        EquivalentPhrase{"tengo junta", "estoy en una reunión"},
        EquivalentPhrase{"renuncio", "dejo mi empleo"},
        EquivalentPhrase{"busco chamba", "busco trabajo"},
        EquivalentPhrase{"desayuno", "estoy desayunando"},
        EquivalentPhrase{"almuerzo", "estoy almorzando"},
        EquivalentPhrase{"ceno", "estoy cenando"},
        EquivalentPhrase{"cocino", "estoy cocinando"},
        EquivalentPhrase{"duermo", "estoy durmiendo"},
        EquivalentPhrase{"descanso", "estoy descansando"},
        EquivalentPhrase{"tomo una siesta", "estoy durmiendo un rato"},
        EquivalentPhrase{"tengo insomnio", "no puedo dormir"},
        EquivalentPhrase{"trasnocho", "me quedo despierto tarde"},
        EquivalentPhrase{"está lloviendo", "llueve"},
        EquivalentPhrase{"neva", "está nevando"},
        EquivalentPhrase{"hace frío", "está frío"},
        EquivalentPhrase{"hace calor", "está caluroso"},
        EquivalentPhrase{"hace viento", "está ventoso"},
        EquivalentPhrase{"está granizando", "cae granizo"},
        EquivalentPhrase{"hace un calorón", "está extremadamente caluroso"},
        EquivalentPhrase{"hago ejercicio", "entreno"},
        EquivalentPhrase{"juego", "estoy jugando"},
        EquivalentPhrase{"escucho música", "oigo música"},
        EquivalentPhrase{"veo tele", "estoy viendo televisión"},
        EquivalentPhrase{"limpio", "estoy limpiando"},
        EquivalentPhrase{"lavo la ropa", "estoy lavando"},
        EquivalentPhrase{"compro", "estoy comprando"},
        EquivalentPhrase{"pago", "estoy pagando"},
        EquivalentPhrase{"hago el super", "estoy comprando despensa"},
        EquivalentPhrase{"saco al perro", "estoy paseando a mi mascota"},
        EquivalentPhrase{"estoy feliz", "estoy contento"},
        EquivalentPhrase{"estoy triste", "estoy de bajón"},
        EquivalentPhrase{"estoy libre", "tengo tiempo"},
        EquivalentPhrase{"estoy enojado", "estoy molesto"},
        EquivalentPhrase{"tengo miedo", "estoy asustado"},
        EquivalentPhrase{"hola", "saludo"},
        EquivalentPhrase{"adiós", "me despido"},
        EquivalentPhrase{"gracias", "agradezco"},
        EquivalentPhrase{"de nada", "respondo agradecimiento"},
        EquivalentPhrase{"qué onda", "saludo informal"},
        EquivalentPhrase{"nos vemos", "me despido temporalmente"},
        EquivalentPhrase{"disculpa", "pido perdón"},
        EquivalentPhrase{"chateo", "estoy mandando mensajes"},
        EquivalentPhrase{"llamo por cel", "estoy hablando por teléfono"},
        EquivalentPhrase{"reviso redes", "estoy viendo publicaciones"},
        EquivalentPhrase{"se cayó el wifi", "no tengo internet"},
        EquivalentPhrase{"mando un mail", "estoy enviando un correo"},
        EquivalentPhrase{"ahorro", "estoy guardando dinero"},
        EquivalentPhrase{"cobro mi sueldo", "recibo mi pago"},
        EquivalentPhrase{"estoy quebrado", "no tengo dinero"},
        EquivalentPhrase{"hago una transferencia", "estoy enviando dinero digital"},
        EquivalentPhrase{"debo dinero", "tengo una deuda"},
        EquivalentPhrase{"compro acciones", "estoy invirtiendo"},
        EquivalentPhrase{"voy al doctor", "asisto a consulta médica"},
        EquivalentPhrase{"tomo medicina", "estoy bajo tratamiento"},
        EquivalentPhrase{"me rompí un hueso", "tengo una fractura"},
        EquivalentPhrase{"tengo fiebre", "mi temperatura está alta"},
        EquivalentPhrase{"estoy a dieta", "estoy cuidando mi alimentación"},
        EquivalentPhrase{"me vacuno", "recibo una dosis médica"},
        EquivalentPhrase{"estoy crudo", "tengo resaca"},
        EquivalentPhrase{"me baño", "me estoy aseando"},
        EquivalentPhrase{"me peino", "estoy arreglando mi cabello"},
        EquivalentPhrase{"me cepillo los dientes", "estoy lavando mi boca"},
        EquivalentPhrase{"me cambio de ropa", "estoy vistiéndome"},
        EquivalentPhrase{"me maquillo", "estoy arreglando mi rostro"},
        EquivalentPhrase{"me afeito", "estoy rasurándome"},
    };
    for (const auto& alias : aliases) {
        if (result == alias.alternative) {
            return std::string(alias.canonical);
        }
    }
    return result;
}

std::string symbolForIndex(std::size_t index) {
    if (index < 11) {
        return std::string(1, static_cast<char>('P' + index));
    }

    std::size_t value = index - 11;
    std::string symbol;
    do {
        symbol.insert(symbol.begin(),
                      static_cast<char>('A' + (value % 26)));
        if (value < 26) {
            break;
        }
        value = value / 26 - 1;
    } while (true);

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
