#pragma once
#include <SFML/Graphics.hpp>

class DobleDisparoItem {
public:
    DobleDisparoItem(float posicionX, float posicionY);    void Mover(float limiteY, float limiteX, float velocidadY = 1.0f);
    void Dibujar(sf::RenderWindow& window);
    bool VerificarColision(sf::Sprite& naveSprite);
    void Reiniciar(float limiteX);    float ObtenerY() const;
    float ObtenerX() const { return x; }
    void EstablecerPosicion(float nx, float ny) { x = nx; y = ny; sprite.setPosition(x, y); }
private:
    float x, y;
    sf::Texture textura;
    sf::Sprite sprite;
    float angulo;
    float velocidadRotacion;
    int direccionRotacion;
    sf::Clock relojCambioDireccion;
};
