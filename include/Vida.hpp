#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Vida {
public:
    Vida();    void EstablecerVidas(int vidas);
    void Dibujar(sf::RenderWindow& window);
private:
    sf::RectangleShape box;
    sf::Font font;
    sf::Text texto;
    float width;
    float height;
};
