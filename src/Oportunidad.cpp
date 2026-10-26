// Agrega el include correcto para el método miembro
#include "../include/Oportunidad.hpp"

void Oportunidad::SumarVida() {
    if (vidas < vidasPorOportunidad) {
        vidas++;
    }
}

Oportunidad::Oportunidad(int oportunidades, int vidasPorOportunidad)
    : oportunidades(oportunidades), vidas(vidasPorOportunidad), vidasPorOportunidad(vidasPorOportunidad) {}

void Oportunidad::PerderVida() {
    vidas--;
    if (vidas <= 0) {
        oportunidades--;
        if (oportunidades > 0) {
            vidas = vidasPorOportunidad;
        }
    }
}

int Oportunidad::ObtenerVidas() const {
    return vidas;
}

int Oportunidad::ObtenerOportunidades() const {
    return oportunidades;
}

bool Oportunidad::VerificarSiSinOportunidades() const {
    return oportunidades <= 0;
}

void Oportunidad::Reiniciar() {
    oportunidades = 3;
    vidas = vidasPorOportunidad;
}
