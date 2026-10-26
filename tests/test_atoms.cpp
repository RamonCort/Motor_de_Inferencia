#include "AtomExtractor.hpp"

#include <cassert>
#include <stdexcept>

void test_atoms() {
    inferencias::AtomExtractor extractor;

    const auto rain = extractor.registerAtom("Está lloviendo.");
    const auto umbrella = extractor.registerAtom("Llevo paraguas.");
    const auto repeated = extractor.registerAtom("  está   LLOVIENDO! ");

    assert(rain.symbol == "P");
    assert(rain.sentence == "Está lloviendo.");
    assert(rain.normalized == "está lloviendo");
    assert(rain.id == 1);
    assert(umbrella.symbol == "Q");
    assert(umbrella.id == 2);
    assert(repeated.symbol == "P");
    assert(repeated.id == rain.id);
    assert(extractor.atoms().size() == 2);

    assert(extractor.contains("LLEVO PARAGUAS"));
    assert(extractor.symbolFor("Está lloviendo...") == "P");
    assert(extractor.sentenceFor("Q") == "Llevo paraguas.");
    assert(extractor.findBySymbol("R") == nullptr);
    assert(extractor.symbolFor("Una proposición diferente") == "");

    const auto extracted = extractor.extract(
        "El cielo está despejado.\n"
        "el cielo está despejado\n"
        "La biblioteca está abierta?");
    assert(extracted.size() == 2);
    assert(extracted[0].symbol == "R");
    assert(extracted[1].symbol == "S");
    assert(extractor.atoms().size() == 4);

    bool rejected = false;
    try {
        extractor.registerAtom("   ... !!! ");
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
}
