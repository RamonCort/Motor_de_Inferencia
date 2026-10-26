#pragma once

class Punto {
public:
    Punto();    void Sumar();
    void Sumar(int valor); // Nueva función para sumar puntos variables
    int ObtenerPuntos() const;
private:
    int puntos;
};
