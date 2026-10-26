#include "Evaluator.hpp"
#include "ModelReporter.hpp"
#include "SemanticTableau.hpp"

#include <cassert>
#include <stdexcept>

void test_evaluator() {
    using inferencias::Connective;
    using inferencias::Formula;

    const auto p = Formula::atom("P");
    const auto q = Formula::atom("Q");
    const inferencias::Evaluator evaluator;

    assert(evaluator.evaluate(*p, {{"P", true}}));
    assert(!evaluator.evaluate(*p, {{"P", false}}));
    assert(evaluator.evaluate(
        *Formula::unary(Connective::Not, p), {{"P", false}}));
    assert(evaluator.evaluate(
        *Formula::binary(Connective::And, p, q), {{"P", true}, {"Q", true}}));
    assert(!evaluator.evaluate(
        *Formula::binary(Connective::And, p, q), {{"P", true}, {"Q", false}}));
    assert(evaluator.evaluate(
        *Formula::binary(Connective::Or, p, q), {{"P", false}, {"Q", true}}));
    assert(!evaluator.evaluate(
        *Formula::binary(Connective::Or, p, q), {{"P", false}, {"Q", false}}));
    assert(evaluator.evaluate(
        *Formula::binary(Connective::Implies, p, q),
        {{"P", false}, {"Q", false}}));
    assert(!evaluator.evaluate(
        *Formula::binary(Connective::Implies, p, q),
        {{"P", true}, {"Q", false}}));
    assert(evaluator.evaluate(
        *Formula::binary(Connective::Iff, p, q), {{"P", true}, {"Q", true}}));
    assert(!evaluator.evaluate(
        *Formula::binary(Connective::Iff, p, q), {{"P", true}, {"Q", false}}));

    bool rejected = false;
    try {
        evaluator.evaluate(*p, {});
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);

    const std::vector<inferencias::Formula::Ptr> premises = {
        Formula::binary(Connective::Implies, p, q), p};
    const auto models = evaluator.enumerateModels(premises);
    assert(models.satisfiable());
    assert(models.models.size() == 1);
    assert(models.models[0].at("P"));
    assert(models.models[0].at("Q"));
    assert(models.atomSymbols.size() == 2);

    const auto deduced = evaluator.checkEntailment(premises, *q);
    assert(deduced.status == inferencias::QueryStatus::Deduced);
    assert(deduced.queryValues.size() == 1);
    assert(deduced.queryValues[0]);

    const auto notDeduced = evaluator.checkEntailment({p}, *q);
    assert(notDeduced.status == inferencias::QueryStatus::NotDeduced);
    assert(!notDeduced.countermodelSearch.models.empty());
    assert(notDeduced.queryValues.size() == 2);

    const auto inconsistent = evaluator.checkEntailment(
        {p, Formula::unary(Connective::Not, p)}, *q);
    assert(inconsistent.status ==
           inferencias::QueryStatus::PremisesInconsistent);
    assert(!inconsistent.premiseSearch.satisfiable());

    const auto compoundQuery = evaluator.checkEntailment(
        {p}, *Formula::binary(Connective::Or, p, q));
    assert(compoundQuery.status == inferencias::QueryStatus::Deduced);

    inferencias::ModelReporter reporter;
    const auto limited = evaluator.enumerateModels({p, q}, 2);
    assert(limited.interrupted());
    const std::string limitedReport = reporter.formatModels(limited);
    assert(limitedReport.find("NO DETERMINADA") != std::string::npos);

    inferencias::SemanticTableau tableau;
    for (const auto& premise : premises) {
        tableau.addPremise(premise);
    }
    tableau.build();
    assert(tableau.isSatisfiable() == models.satisfiable());

    const auto disjunction = Formula::binary(Connective::Or, p, q);
    const auto disjunctionModels = evaluator.enumerateModels({disjunction});
    inferencias::SemanticTableau disjunctionTableau;
    disjunctionTableau.addPremise(disjunction);
    disjunctionTableau.build();
    assert(disjunctionTableau.isSatisfiable() ==
           disjunctionModels.satisfiable());
    assert(disjunctionModels.models.size() == 3);

    const auto contradiction = Formula::unary(Connective::Not, p);
    const auto contradictionModels = evaluator.enumerateModels(
        {p, contradiction});
    inferencias::SemanticTableau contradictionTableau;
    contradictionTableau.addPremise(p);
    contradictionTableau.addPremise(contradiction);
    contradictionTableau.build();
    assert(contradictionTableau.isSatisfiable() ==
           contradictionModels.satisfiable());
    assert(!contradictionModels.satisfiable());
    assert(contradictionTableau.branches().size() == 1);
    assert(!contradictionTableau.branches()[0].open);

    const auto biconditional = Formula::binary(Connective::Iff, p, q);
    const auto biconditionalModels = evaluator.enumerateModels(
        {biconditional});
    inferencias::SemanticTableau biconditionalTableau;
    biconditionalTableau.addPremise(biconditional);
    biconditionalTableau.build();
    assert(biconditionalTableau.isSatisfiable() ==
           biconditionalModels.satisfiable());
    assert(biconditionalModels.models.size() == 2);

    const auto controlLeft = Formula::atom("P");
    const auto controlRight = Formula::atom("Q");
    const auto controlPremise = Formula::binary(
        Connective::Or, controlLeft, controlRight);
    const auto controlModels = evaluator.enumerateModels({controlPremise});
    assert(controlModels.models.size() == 3);
    const auto controlQuery = evaluator.checkEntailment(
        {controlPremise}, *controlLeft);
    assert(controlQuery.status == inferencias::QueryStatus::NotDeduced);
    assert(!controlQuery.countermodelSearch.models.empty());

    const inferencias::Atom atomP{"P", "Está lloviendo", "está lloviendo", 1};
    const inferencias::Atom atomQ{"Q", "Llevo paraguas", "llevo paraguas", 2};
    const std::string report = reporter.formatEntailment(
        deduced, {atomP, atomQ});
    assert(report.find("DEDUCIDA") != std::string::npos);
    assert(report.find("Está lloviendo") != std::string::npos);

    const std::string inconsistentReport = reporter.formatEntailment(
        inconsistent, {atomP, atomQ});
    assert(inconsistentReport.find("NO EVALUADA") != std::string::npos);
    assert(inconsistentReport.find("vacuamente") != std::string::npos);
}
