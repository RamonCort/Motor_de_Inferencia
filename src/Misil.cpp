#include "../include/Misil.hpp"
#include <cmath>

sf::Texture Misil::texturaDisparo; // Definición de la textura estática

Misil::Misil(float posicionX, float posicionY, float angulo) : velocidad(7.0f), anguloDisparo(angulo) {
    if (texturaDisparo.getSize().x == 0) {
        texturaDisparo.loadFromFile("assets/images/Disparo.png");
    }
    if (texturaDisparo.getSize().x > 0) {
        frameActual = 0;
        totalFrames = 5;
        framesPorFila = 5;
        frameWidth = 125;
        frameHeight = 313;
        tiempoPorFrame = 0.08f;
        spriteDisparo.setTexture(texturaDisparo);
        spriteDisparo.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));
        spriteDisparo.setOrigin(frameWidth / 2, frameHeight);        spriteDisparo.setPosition(posicionX, posicionY);
        spriteDisparo.setScale(0.15f, 0.15f);
        relojAnimacion.restart();
        spriteDisparo.setRotation(anguloDisparo);
    } else {
        shape.setSize(sf::Vector2f(8, 24));
        shape.setFillColor(sf::Color::Yellow);
        shape.setOrigin(shape.getSize().x / 2, shape.getSize().y);
        shape.setPosition(posicionX, posicionY);
        shape.setRotation(anguloDisparo);
    }
    // Calcular vector de dirección
    float rad = (anguloDisparo - 90.f) * 3.14159265f / 180.f;
    direccion = sf::Vector2f(std::cos(rad), std::sin(rad));
}

void Misil::Mover() {
    if (texturaDisparo.getSize().x > 0) {
        spriteDisparo.move(direccion * velocidad);
        // Animación de frames (solo primera fila)
        if (relojAnimacion.getElapsedTime().asSeconds() > tiempoPorFrame) {
            frameActual = (frameActual + 1) % totalFrames;
            int columna = frameActual % framesPorFila;
            int fila = 0; // Solo la primera fila
            spriteDisparo.setTextureRect(sf::IntRect(columna * frameWidth, fila * frameHeight, frameWidth, frameHeight));
            relojAnimacion.restart();
        }
    } else {
        shape.move(direccion * velocidad);
    }
}

void Misil::Dibujar(sf::RenderWindow& window) {
    if (texturaDisparo.getSize().x > 0)
        window.draw(spriteDisparo);
    else
        window.draw(shape);
}

bool Misil::VerificarSiEstaFueraDePantalla(float altoVentana) const {
    if (texturaDisparo.getSize().x > 0)
        return spriteDisparo.getPosition().y + frameHeight < 0;
    else
        return shape.getPosition().y + shape.getSize().y < 0;
}

sf::FloatRect Misil::ObtenerLimites() const {
    if (texturaDisparo.getSize().x > 0)
        return spriteDisparo.getGlobalBounds();
    else
        return shape.getGlobalBounds();
}
