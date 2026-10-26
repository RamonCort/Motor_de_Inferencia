#include "Evaluator.hpp"
#include "ModelReporter.hpp"
#include "NaturalLanguageParser.hpp"
#include "SemanticTableau.hpp"

#include <fstream>
#include <exception>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

constexpr std::size_t kMaxAssignments = 1048576;

bool readLine(const std::string& prompt, std::string& value) {
    std::cout << prompt;
    return static_cast<bool>(std::getline(std::cin, value));
}

bool parseCount(const std::string& text, std::size_t& count) {
    try {
        std::size_t parsed = 0;
        const unsigned long long value = std::stoull(text, &parsed);
        if (parsed != text.size() ||
            value > std::numeric_limits<std::size_t>::max()) {
            return false;
        }
        count = static_cast<std::size_t>(value);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void printGrammar() {
    std::cout
        << "\nGRAMATICA LIMITADA\n"
        << "  expresion       := bicondicional\n"
        << "  bicondicional   := implicacion [\"si y solo si\" implicacion]\n"
        << "  implicacion     := disyuncion | \"si\" disyuncion "
           "[\",\"] \"entonces\" disyuncion\n"
        << "  disyuncion      := conjuncion { \"o\" conjuncion }\n"
        << "  conjuncion      := negacion { \"y\" negacion }\n"
        << "  negacion        := \"no\" negacion | primaria\n"
        << "  primaria        := proposicion | \"(\" expresion \")\"\n\n"
        << "Ejemplos:\n"
        << "  Esta lloviendo\n"
        << "  No esta lloviendo\n"
        << "  Esta lloviendo y llevo paraguas\n"
        << "  Esta lloviendo o llevo paraguas\n"
        << "  Si llueve, entonces llevo paraguas\n"
        << "  Llevo paraguas si y solo si llueve\n"
        << "Las formas 'llueve', 'esta lloviendo' y 'está lloviendo'\n"
           "se reconocen como la misma expresion canonica limitada.\n"
        << "La gramatica no representa lenguaje natural libre ni reconoce todos\n"
        << "los conectores del espanol.\n\n";
}

bool writeDot(const std::string& path, const std::string& dot) {
    std::ofstream output(path);
    if (!output) {
        std::cerr << "Error: no se pudo crear el archivo DOT '" << path
                  << "'.\n";
        return false;
    }
    output << dot;
    if (!output) {
        std::cerr << "Error: no se pudo escribir el archivo DOT '" << path
                  << "'.\n";
        return false;
    }
    return true;
}

void printAtoms(const inferencias::AtomExtractor& extractor) {
    std::cout << "\nPASO 2. ATOMOS DETECTADOS\n";
    if (extractor.atoms().empty()) {
        std::cout << "No se detectaron atomos.\n";
        return;
    }
    for (const auto& atom : extractor.atoms()) {
        std::cout << "  " << atom.symbol << " = " << atom.sentence
                  << " (id " << atom.id << ")\n";
    }
}

void printFormulas(const std::vector<std::string>& texts,
                   const std::vector<inferencias::Formula::Ptr>& formulas) {
    std::cout << "\nPASO 3. FORMULAS LOGICAS\n";
    for (std::size_t index = 0; index < formulas.size(); ++index) {
        std::cout << "  " << index + 1 << ". " << texts[index] << "\n"
                  << "     " << formulas[index]->toString() << "\n";
    }
}

bool parseCase(
    std::vector<std::string>& premiseTexts,
    std::string& queryText,
    std::vector<inferencias::Formula::Ptr>& premises,
    inferencias::Formula::Ptr& query,
    inferencias::NaturalLanguageParser& parser) {
    while (true) {
        parser = inferencias::NaturalLanguageParser{};
        premises.clear();
        bool valid = true;
        for (std::size_t index = 0; index < premiseTexts.size(); ++index) {
            const auto result = parser.parse(premiseTexts[index]);
            if (!result.ok()) {
                std::cout << "\nError en la premisa " << index + 1 << ": "
                          << result.error << "\n";
                if (!readLine("Corrija la premisa: ", premiseTexts[index])) {
                    return false;
                }
                valid = false;
                break;
            }
            premises.push_back(result.formula);
        }
        if (!valid) {
            continue;
        }

        query.reset();
        if (!queryText.empty()) {
            const auto result = parser.parse(queryText);
            if (!result.ok()) {
                std::cout << "\nError en la consulta: " << result.error << "\n";
                if (!readLine("Corrija la consulta (vacia para quitarla): ",
                              queryText)) {
                    return false;
                }
                continue;
            }
            query = result.formula;
        }
        return true;
    }
}

void analyzeCase() {
    std::string countText;
    std::size_t count = 0;
    while (true) {
        if (!readLine("\nCantidad de premisas: ", countText)) {
            return;
        }
        if (parseCount(countText, count)) {
            break;
        }
        std::cout << "Introduzca un numero entero no negativo.\n";
    }

    std::vector<std::string> premiseTexts(count);
    for (std::size_t index = 0; index < count; ++index) {
        if (!readLine("Premisa " + std::to_string(index + 1) + ": ",
                      premiseTexts[index])) {
            return;
        }
    }

    std::string queryText;
    if (!readLine("Consulta opcional (deje vacio para omitirla): ",
                  queryText)) {
        return;
    }

    std::vector<inferencias::Formula::Ptr> premises;
    inferencias::Formula::Ptr query;
    inferencias::NaturalLanguageParser parser;
    if (!parseCase(premiseTexts, queryText, premises, query, parser)) {
        return;
    }

    printAtoms(parser.atomExtractor());
    printFormulas(premiseTexts, premises);
    if (query != nullptr) {
        std::cout << "  Consulta: " << queryText << "\n"
                  << "     " << query->toString() << "\n";
    }

    std::cout << "\nPASO 4. ARBOL SEMANTICO\n";
    inferencias::SemanticTableau tableau;
    for (const auto& premise : premises) {
        tableau.addPremise(premise);
    }
    tableau.build();
    std::cout << tableau.toText();
    if (writeDot("tableau_premisas.dot", tableau.toDot())) {
        std::cout << "DOT guardado en tableau_premisas.dot\n";
    }

    if (query != nullptr) {
        inferencias::SemanticTableau queryTableau;
        for (const auto& premise : premises) {
            queryTableau.addPremise(premise);
        }
        queryTableau.addPremise(
            inferencias::Formula::unary(inferencias::Connective::Not, query));
        queryTableau.build();
        std::cout << "\nTableau de premisas y negacion de la consulta:\n";
        std::cout << queryTableau.toText();
        if (writeDot("tableau_consulta.dot", queryTableau.toDot())) {
            std::cout << "DOT guardado en tableau_consulta.dot\n";
        }
    }

    std::cout << "\nPASO 5. REPORTE DE MODELOS\n";
    const inferencias::Evaluator evaluator;
    const auto modelSearch = evaluator.enumerateModels(
        premises, kMaxAssignments);
    const inferencias::ModelReporter reporter;
    std::cout << reporter.formatModels(
        modelSearch, parser.atomExtractor().atoms());

    if (query != nullptr) {
        const auto entailment = evaluator.checkEntailment(
            premises, *query, kMaxAssignments);
        std::cout << reporter.formatEntailment(
            entailment, parser.atomExtractor().atoms());
    }
}

} // namespace

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::cout << "MOTOR DE INFERENCIAS BASADO EN LOGICA PROPOSICIONAL\n";
    while (true) {
        std::cout << "\nMENU PRINCIPAL\n"
                  << "1. Analizar un nuevo caso\n"
                  << "2. Consultar la gramatica y ejemplos\n"
                  << "3. Salir\n";

        std::string option;
        if (!readLine("Seleccione una opcion: ", option)) {
            std::cout << "\nSalida.\n";
            return 0;
        }
        if (option == "1") {
            analyzeCase();
        } else if (option == "2") {
            printGrammar();
        } else if (option == "3") {
            std::cout << "Hasta luego.\n";
            return 0;
        } else {
            std::cout << "Opcion no valida.\n";
        }
    }
}
