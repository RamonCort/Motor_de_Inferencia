#include "../include/VidaExtra.hpp"
#include <cstdlib>

VidaExtra::VidaExtra(float posicionX, float posicionY) : x(posicionX), y(posicionY) {
    shape.setRadius(18.f);
    shape.setPointCount(20);
    shape.setFillColor(sf::Color::Green);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(3);
    shape.setPosition(x, y);
    // Cargar sprite animado
    spriteTexture.loadFromFile("assets/images/sprite.png");
    spriteAnimado.setTexture(spriteTexture);
    spriteAnimado.setOrigin(spriteTexture.getSize().x/2, spriteTexture.getSize().y/2);
    spriteAnimado.setScale(0.8f, 0.8f); // Más pequeño que el original
    spriteAnimado.setPosition(x, y);
    angulo = 0.f;
    velocidadRotacion = 2.f;
    direccionRotacion = 1;
    relojCambioDireccion.restart();
}

void VidaExtra::Mover(float limiteY, float limiteX, float velocidadY) {
    y += velocidadY;
    if (y > limiteY) {
        Reiniciar(limiteX);
    }
    shape.setPosition(x, y);
    spriteAnimado.setPosition(x, y);
    // Animar rotación y cambio de dirección
    if (relojCambioDireccion.getElapsedTime().asSeconds() > 0.7f) {
        direccionRotacion = (rand() % 2 == 0) ? 1 : -1;
        velocidadRotacion = 1.5f + static_cast<float>(rand() % 30) / 10.f;
        relojCambioDireccion.restart();
    }
    angulo += velocidadRotacion * direccionRotacion;
    spriteAnimado.setRotation(angulo);
}

void VidaExtra::Dibujar(sf::RenderWindow& window) {
    spriteAnimado.setPosition(x, y);
    window.draw(spriteAnimado);
}

bool VidaExtra::VerificarColision(Nave& nave) {
    if (spriteAnimado.getGlobalBounds().intersects(nave.ObtenerSprite().getGlobalBounds())) {
        return true;
    }
    return false;
}

void VidaExtra::Reiniciar(float limiteX) {
    x = static_cast<float>(rand() % static_cast<int>(limiteX - 40) + 20);
    y = 0;
    shape.setPosition(x, y);
    spriteAnimado.setPosition(x, y);
}

float VidaExtra::ObtenerY() const { return y; }
