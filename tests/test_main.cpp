#include <iostream>

void test_formulas();
void test_atoms();
void test_parser();
void test_evaluator();
void test_tableau();

int main() {
    test_formulas();
    test_atoms();
    test_parser();
    test_evaluator();
    test_tableau();
    std::cout << "Todas las pruebas iniciales pasaron.\n";
    return 0;
}
