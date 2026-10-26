#include "../include/EscudoItem.hpp"
#include <cstdlib>

EscudoItem::EscudoItem(float posicionX, float posicionY) : x(posicionX), y(posicionY) {
    shape.setRadius(18.f);
    shape.setPointCount(20);
    shape.setFillColor(sf::Color::Red);
    shape.setOutlineColor(sf::Color::White);
    shape.setOutlineThickness(3);
    shape.setPosition(x, y);
    // Cargar sprite del escudo
    spriteTexture.loadFromFile("assets/images/Escudo.item.png");
    spriteEscudo.setTexture(spriteTexture);
    spriteEscudo.setOrigin(spriteTexture.getSize().x/2, spriteTexture.getSize().y/2);
    spriteEscudo.setScale(0.55f, 0.55f); // Un poco más grande
    spriteEscudo.setPosition(x, y);
}

void EscudoItem::Mover(float limiteY, float limiteX, float velocidadY) {
    y += velocidadY;
    if (y > limiteY) {
        Reiniciar(limiteX);
    }
    shape.setPosition(x, y);
    spriteEscudo.setPosition(x, y);
}

void EscudoItem::Dibujar(sf::RenderWindow& window) {
    spriteEscudo.setPosition(x, y);
    window.draw(spriteEscudo);
}

bool EscudoItem::VerificarColision(Nave& nave) {
    if (spriteEscudo.getGlobalBounds().intersects(nave.ObtenerSprite().getGlobalBounds())) {
        return true;
    }
    return false;
}

void EscudoItem::Reiniciar(float limiteX) {
    x = static_cast<float>(rand() % static_cast<int>(limiteX - 40) + 20);
    y = 0;
    shape.setPosition(x, y);
    spriteEscudo.setPosition(x, y);
}

float EscudoItem::ObtenerY() const {
    return y;
}
