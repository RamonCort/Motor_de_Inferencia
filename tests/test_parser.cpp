#include "NaturalLanguageParser.hpp"

#include <cassert>

void test_parser() {
    using inferencias::Connective;
    inferencias::NaturalLanguageParser parser;

    const auto atom = parser.parse("Está lloviendo");
    assert(atom.ok());
    assert(atom.original == "Está lloviendo");
    assert(atom.formula->connective() == Connective::Atom);
    assert(atom.formula->symbol() == "P");
    assert(atom.atoms.size() == 1);
    assert(atom.atoms[0].sentence == "Está lloviendo");

    const auto negation = parser.parse("No está lloviendo");
    assert(negation.ok());
    assert(negation.formula->connective() == Connective::Not);
    assert(negation.formula->left()->symbol() == "P");

    const auto conjunction = parser.parse(
        "Está lloviendo y llevo paraguas");
    assert(conjunction.ok());
    assert(conjunction.formula->connective() == Connective::And);
    assert(conjunction.formula->toSymbolic() == "P \xE2\x88\xA7 Q");

    const auto disjunction = parser.parse(
        "Está lloviendo o llevo paraguas");
    assert(disjunction.ok());
    assert(disjunction.formula->connective() == Connective::Or);

    const auto implication = parser.parse(
        "Si llueve, entonces llevo paraguas");
    assert(implication.ok());
    assert(implication.formula->connective() == Connective::Implies);

    const auto biconditional = parser.parse(
        "Llevo paraguas si y solo si llueve");
    assert(biconditional.ok());
    assert(biconditional.formula->connective() == Connective::Iff);

    const auto nested = parser.parse(
        "Si está lloviendo o nieva, entonces llevo paraguas");
    assert(nested.ok());
    assert(nested.formula->connective() == Connective::Implies);
    assert(nested.formula->left()->connective() == Connective::Or);

    const auto repeated = parser.parse("ESTÁ lloviendo.");
    assert(repeated.ok());
    assert(repeated.formula->symbol() == "P");
    assert(parser.atomExtractor().atoms().size() == 3);

    const auto empty = parser.parse("");
    assert(!empty.ok());
    assert(!empty.error.empty());

    const auto malformedImplication = parser.parse("Si llueve llevo paraguas");
    assert(!malformedImplication.ok());
    assert(!malformedImplication.error.empty());

    const auto malformedBiconditional = parser.parse(
        "Llevo paraguas si solo si llueve");
    assert(!malformedBiconditional.ok());
    assert(!malformedBiconditional.error.empty());

    const auto dangling = parser.parse("Está lloviendo y");
    assert(!dangling.ok());
    assert(!dangling.error.empty());

    const auto unsupported = parser.parse(
        "Está lloviendo pero llevo paraguas");
    assert(!unsupported.ok());
    assert(!unsupported.error.empty());
}
