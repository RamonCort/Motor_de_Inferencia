#pragma once

#include <vector>
#include <string>

class TablaDePuntaje {
public:
    struct Entrada {
        std::string nombre;
        int puntaje;
    };
    TablaDePuntaje(const std::string& archivo);    void Agregar(const std::string& nombre, int puntaje);
    const std::vector<Entrada>& Obtener() const;
private:
    std::string archivo_;
    std::vector<Entrada> entradas;    void Cargar();
    void Guardar();
};
