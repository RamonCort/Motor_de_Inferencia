#include "../include/Explosion.hpp"

sf::Texture Explosion::texture;
bool Explosion::textureLoaded = false;

// 2 filas x 4 columnas, cada frame de 156.5x185.5 (redondeado a enteros para píxeles)
static const sf::IntRect explosionFrames[8] = {
    sf::IntRect(0,     0,   156, 186), // Frame 1
    sf::IntRect(156,   0,   156, 186), // Frame 2
    sf::IntRect(313,   0,   156, 186), // Frame 3
    sf::IntRect(469,   0,   156, 186), // Frame 4
    sf::IntRect(0,   186,   156, 186), // Frame 5
    sf::IntRect(156, 186,   156, 186), // Frame 6
    sf::IntRect(313, 186,   156, 186), // Frame 7
    sf::IntRect(469, 186,   156, 186)  // Frame 8
};

Explosion::Explosion(float x, float y)
    : frameActual(0), totalFrames(8), tiempoPorFrame(0.08f), finished(false) {
    if (!textureLoaded) {
        texture.loadFromFile("assets/images/Explocion.png");
        textureLoaded = true;
    }
    sprite.setTexture(texture);
    sprite.setTextureRect(explosionFrames[0]);
    sprite.setOrigin(explosionFrames[0].width / 2, explosionFrames[0].height / 2);
    sprite.setPosition(x, y);
    sprite.setScale(1.f, 1.f);
    relojAnimacion.restart();
}

void Explosion::Actualizar() {
    if (finished) return;
    if (relojAnimacion.getElapsedTime().asSeconds() > tiempoPorFrame) {
        frameActual++;
        if (frameActual >= totalFrames) {
            finished = true;
            return;
        }
        sprite.setTextureRect(explosionFrames[frameActual]);
        sprite.setOrigin(explosionFrames[frameActual].width / 2, explosionFrames[frameActual].height / 2);
        relojAnimacion.restart();
    }
}

void Explosion::Dibujar(sf::RenderWindow& window) {
    if (!finished) window.draw(sprite);
}

bool Explosion::VerificarSiEstaTerminada() const {
    return finished;
}
