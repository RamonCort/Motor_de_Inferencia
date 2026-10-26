#include <SFML/Graphics.hpp>
#include <cstdlib>
#include "../include/DobleDisparoItem.hpp"

DobleDisparoItem::DobleDisparoItem(float posicionX, float posicionY) : x(posicionX), y(posicionY) {
    textura.loadFromFile("assets/images/Doble_Disparo_Item.png");
    sprite.setTexture(textura);
    sprite.setOrigin(textura.getSize().x/2, textura.getSize().y/2);
    sprite.setPosition(x, y);
    sprite.setScale(0.12f, 0.12f); // Más pequeño que el escudo
    angulo = 0.f;
    velocidadRotacion = 2.f;
    direccionRotacion = 1;
    relojCambioDireccion.restart();
}

void DobleDisparoItem::Mover(float limiteY, float limiteX, float velocidadY) {
    y += velocidadY;
    if (y > limiteY) {
        Reiniciar(limiteX);
    }
    sprite.setPosition(x, y);
    // Animar rotación y cambio de dirección
    if (relojCambioDireccion.getElapsedTime().asSeconds() > 0.7f) {
        direccionRotacion = (rand() % 2 == 0) ? 1 : -1;
        velocidadRotacion = 1.5f + static_cast<float>(rand() % 30) / 10.f;
        relojCambioDireccion.restart();
    }
    angulo += velocidadRotacion * direccionRotacion;
    sprite.setRotation(angulo);
}

void DobleDisparoItem::Dibujar(sf::RenderWindow& window) {
    sprite.setPosition(x, y);
    window.draw(sprite);
}

bool DobleDisparoItem::VerificarColision(sf::Sprite& naveSprite) {
    return sprite.getGlobalBounds().intersects(naveSprite.getGlobalBounds());
}

void DobleDisparoItem::Reiniciar(float limiteX) {
    x = static_cast<float>(rand() % static_cast<int>(limiteX - 40) + 20);
    y = 0;
    sprite.setPosition(x, y);
}

float DobleDisparoItem::ObtenerY() const { return y; }
