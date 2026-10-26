#pragma once
#include <SFML/Graphics.hpp>
#include "Nave.hpp"

class Margen {
public:    Margen(float anchoVentana, float altoVentana, float grosor = 2.f);
    void Dibujar(sf::RenderWindow& window);
    void Limitar(Nave& nave); // Declaración de la función limitar
private:
    sf::RectangleShape lineaSuperior;
    sf::RectangleShape lineaInferior;
    sf::RectangleShape lineaIzquierda;
    sf::RectangleShape lineaDerecha;
    sf::CircleShape esquinaSupIzq;
    sf::CircleShape esquinaSupDer;
    sf::CircleShape esquinaInfIzq;
    sf::CircleShape esquinaInfDer;
    float ancho, alto, grosor; // Guardar dimensiones para limitar
};
