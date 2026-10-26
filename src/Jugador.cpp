#include "../include/Jugador.hpp"

// Constructor
Jugador::Jugador(float x, float y, float velocidad)
    : x(x), y(y), velocidad(velocidad) {}

// Mover el jugador
void Jugador::mover(float dx, float dy) {
    x += dx * velocidad;
    y += dy * velocidad;
}

// Getters
float Jugador::getX() const {
    return x;
}

float Jugador::getY() const {
    return y;
}

float Jugador::getVelocidad() const {
    return velocidad;
}

// Setters
void Jugador::setPosicion(float x, float y) {
    this->x = x;
    this->y = y;
}

void Jugador::setVelocidad(float velocidad) {
    this->velocidad = velocidad;
}
