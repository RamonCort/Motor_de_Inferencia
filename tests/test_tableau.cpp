#include "SemanticTableau.hpp"

#include <cassert>

void test_tableau() {
    using inferencias::Connective;
    using inferencias::Formula;

    inferencias::SemanticTableau tableau;
    const auto p = Formula::atom("P");
    const auto q = Formula::atom("Q");

    tableau.addPremise(p);
    tableau.build();
    assert(tableau.premises().size() == 1);
    assert(tableau.isBuilt());
    assert(tableau.isSatisfiable());
    assert(tableau.branches().size() == 1);
    assert(tableau.branches()[0].open);
    assert(!tableau.branches()[0].history.empty());
    assert(tableau.toDot().find("digraph") != std::string::npos);
    assert(tableau.toDot().find("P") != std::string::npos);

    inferencias::SemanticTableau contradiction;
    contradiction.addPremise(p);
    contradiction.addPremise(Formula::unary(Connective::Not, p));
    contradiction.build();
    assert(!contradiction.isSatisfiable());
    assert(contradiction.branches().size() == 1);
    assert(!contradiction.branches()[0].open);
    assert(contradiction.toText().find("cerrada") != std::string::npos);

    inferencias::SemanticTableau implication;
    implication.addPremise(Formula::binary(Connective::Implies, p, q));
    implication.addPremise(p);
    implication.build();
    assert(implication.isSatisfiable());
    assert(implication.branches().size() == 2);
    assert(!implication.branches()[0].open ||
           !implication.branches()[1].open);
    assert(implication.toText().find("beta") != std::string::npos);

    inferencias::SemanticTableau disjunction;
    disjunction.addPremise(Formula::binary(Connective::Or, p, q));
    disjunction.build();
    assert(disjunction.isSatisfiable());
    assert(disjunction.branches().size() == 2);
    assert(disjunction.branches()[0].open);
    assert(disjunction.branches()[1].open);

    inferencias::SemanticTableau repeated;
    repeated.addPremise(Formula::binary(Connective::And, p, p));
    repeated.build();
    assert(repeated.isSatisfiable());
    assert(repeated.branches().size() == 1);
    assert(repeated.branches()[0].formulas.size() == 1);

    inferencias::SemanticTableau negatedConjunction;
    negatedConjunction.addPremise(Formula::unary(
        Connective::Not, Formula::binary(Connective::And, p, q)));
    negatedConjunction.build();
    assert(negatedConjunction.isSatisfiable());
    assert(negatedConjunction.branches().size() == 2);

    inferencias::SemanticTableau biconditional;
    biconditional.addPremise(Formula::binary(Connective::Iff, p, q));
    biconditional.build();
    assert(biconditional.isSatisfiable());
    assert(biconditional.branches().size() == 2);
    assert(biconditional.toDot().find("->") != std::string::npos);
}
