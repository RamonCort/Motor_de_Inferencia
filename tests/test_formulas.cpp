#include "Formula.hpp"

#include <cassert>
#include <stdexcept>

void test_formulas() {
    using inferencias::Connective;
    using inferencias::Formula;

    const auto p = Formula::atom("P");
    const auto q = Formula::atom("Q");
    assert(p->connective() == Connective::Atom);
    assert(p->symbol() == "P");
    assert(p->toString() == "P");
    assert(p->toSymbolic() == "P");

    const auto notP = Formula::unary(Connective::Not, p);
    const auto conjunction = Formula::binary(Connective::And, p, q);
    const auto disjunction = Formula::binary(Connective::Or, p, q);
    const auto implication = Formula::binary(Connective::Implies, p, q);
    const auto biconditional = Formula::binary(Connective::Iff, p, q);

    assert(notP->toSymbolic() == "\xC2\xACP");
    assert(conjunction->toSymbolic() == "P \xE2\x88\xA7 Q");
    assert(disjunction->toSymbolic() == "P \xE2\x88\xA8 Q");
    assert(implication->toSymbolic() == "P \xE2\x86\x92 Q");
    assert(biconditional->toSymbolic() == "P \xE2\x86\x94 Q");
    assert(conjunction->toString() == "P AND Q");

    const auto nested = Formula::binary(
        Connective::And, p, Formula::binary(Connective::Or, q, p));
    assert(nested->toSymbolic() == "P \xE2\x88\xA7 (Q \xE2\x88\xA8 P)");

    const auto atoms = nested->atoms();
    assert(atoms.size() == 2);
    assert(atoms[0] == "P");
    assert(atoms[1] == "Q");

    bool rejected = false;
    try {
        Formula::unary(Connective::And, p);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);

    rejected = false;
    try {
        Formula::atom("");
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
}
