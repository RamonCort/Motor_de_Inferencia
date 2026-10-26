#include "../include/Punto.hpp"

Punto::Punto() : puntos(0) {}

void Punto::Sumar() {
    puntos += 10;
}

void Punto::Sumar(int valor) {
    puntos += valor;
}

int Punto::ObtenerPuntos() const {
    return puntos;
}
