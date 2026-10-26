#pragma once
#include <SFML/Graphics.hpp>
#include "Nave.hpp"
#include "Misil.hpp"
#include <vector>

enum class TamanoAsteroide {
    GRANDE = 0,    // Tamaño inicial (4x más grande)
    PEQUENO = 1    // Después del primer impacto
};

class Asteroide {
public:
    Asteroide(float posicionX, float posicionY, TamanoAsteroide tam = TamanoAsteroide::GRANDE, float targetX = 0, float targetY = 0);
    float ObtenerX() const;
    float ObtenerY() const;
    void Dibujar(sf::RenderWindow& window);
    void Mover(float limiteY, float limiteX, float velocidadBase = 3.0f);
    void MoverHaciaObjetivo(const sf::Vector2f& posicionNave, float limiteY, float limiteX, float velocidadBase = 3.0f);
    void Colisionar(Nave& nave);
    bool VerificarColisionConNave(Nave& nave);
    bool VerificarColisionConMisil(const Misil& misil);
    void ReiniciarPosicion(float limiteX, float limiteY);
    std::vector<Asteroide> Dividir() const;
    TamanoAsteroide ObtenerTamano() const { return tamano; }
    bool VerificarSiPuedeDestruirse() const { return tamano == TamanoAsteroide::PEQUENO; }
    
    sf::CircleShape shape; // <-- Fallback si no hay textura
    float x, y; // <-- Posición
    float velocidadX, velocidadY; // Velocidad en cada eje
    TamanoAsteroide tamano; // Tamaño del asteroide
    static sf::Texture textureAsteroide;
    static bool textureCargada;
    sf::Sprite sprite;
};
