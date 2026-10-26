#pragma once
#include <SFML/Graphics.hpp>
#include "Nave.hpp"

class EscudoItem {
public:
    EscudoItem(float posicionX, float posicionY);
    void Mover(float limiteY, float limiteX, float velocidadY = 1.0f);
    void Dibujar(sf::RenderWindow& window);
    bool VerificarColision(Nave& nave);
    void Reiniciar(float limiteX);
    float ObtenerY() const;
    float ObtenerX() const { return x; }
    void EstablecerX(float nx) { x = nx; shape.setPosition(x, y); }
    void EstablecerY(float ny) { y = ny; shape.setPosition(x, y); }
    void EstablecerPos(float nx, float ny) { x = nx; y = ny; shape.setPosition(x, y); }
private:
    float x, y;
    sf::CircleShape shape;
    sf::Texture spriteTexture; // Textura del escudo
    sf::Sprite spriteEscudo;   // Sprite del escudo
};
