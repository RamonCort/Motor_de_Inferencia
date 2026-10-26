#pragma once
#include <SFML/Graphics.hpp>

class Misil {
public:
    Misil(float posicionX, float posicionY, float angulo = -90.f); // Nuevo: ángulo de disparo
    void Mover();
    void Dibujar(sf::RenderWindow& window);
    bool VerificarSiEstaFueraDePantalla(float altoVentana) const;
    sf::FloatRect ObtenerLimites() const;
private:
    sf::RectangleShape shape;
    static sf::Texture texturaDisparo;
    sf::Sprite spriteDisparo;
    float velocidad;
    float anguloDisparo = -90.f; // Nuevo: dirección del disparo en grados
    sf::Vector2f direccion;      // Nuevo: vector de dirección

    // Variables para animación de sprite
    int frameActual = 0;
    int totalFrames = 5; // Solo la primera fila
    int framesPorFila = 5;
    int frameWidth = 125;
    int frameHeight = 313;
    float tiempoPorFrame = 0.08f;
    sf::Clock relojAnimacion;
};
