#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Puntaje {
public:
    Puntaje(float windowWidth);    void EstablecerPuntos(int puntos);
    void Dibujar(sf::RenderWindow& window);
private:
    sf::RectangleShape box;
    sf::Font font;
    sf::Text texto;
    float width;
    float height;
};
