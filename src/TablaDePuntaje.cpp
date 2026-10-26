#include "../include/TablaDePuntaje.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

TablaDePuntaje::TablaDePuntaje(const std::string& archivo) : archivo_(archivo) {
    Cargar();
}

void TablaDePuntaje::Agregar(const std::string& nombre, int puntaje) {
    entradas.push_back({nombre, puntaje});
    std::sort(entradas.begin(), entradas.end(), [](const Entrada& a, const Entrada& b) {
        return b.puntaje < a.puntaje;
    });
    if (entradas.size() > 5) entradas.resize(5);
    Guardar();
}

const std::vector<TablaDePuntaje::Entrada>& TablaDePuntaje::Obtener() const {
    return entradas;
}

void TablaDePuntaje::Cargar() {
    entradas.clear();
    std::ifstream f(archivo_);
    std::string nombre;
    int puntaje;
    while (f >> nombre >> puntaje) {
        entradas.push_back({nombre, puntaje});
    }
}

void TablaDePuntaje::Guardar() {
    std::ofstream f(archivo_);
    for (const auto& e : entradas) {
        f << e.nombre << " " << e.puntaje << "\n";
    }
}
