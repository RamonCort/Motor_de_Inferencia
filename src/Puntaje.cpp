#include "../include/Puntaje.hpp"

Puntaje::Puntaje(float windowWidth) : width(250), height(60) {
    box.setSize(sf::Vector2f(width, height));
    box.setFillColor(sf::Color(30,30,30,200));
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(3);
    box.setPosition(windowWidth - width - 20, 20);

    // Cargar fuente retro del proyecto
    font.loadFromFile("assets/fonts/Retro Gaming.ttf");
    texto.setFont(font);
    texto.setString("Puntaje : 0");
    texto.setCharacterSize(32); // Más grande y legible
    texto.setFillColor(sf::Color::White);
    texto.setOutlineColor(sf::Color::Black);
    texto.setOutlineThickness(3.f);
    // Centrado vertical respecto al box
    sf::FloatRect textBounds = texto.getLocalBounds();
    texto.setOrigin(textBounds.left, textBounds.top);
    texto.setPosition(box.getPosition().x + 20, box.getPosition().y + (box.getSize().y - textBounds.height) / 2 - 5);
}

void Puntaje::EstablecerPuntos(int puntos) {
    texto.setString("Puntaje : " + std::to_string(puntos));
}

void Puntaje::Dibujar(sf::RenderWindow& window) {
    window.draw(box);
    window.draw(texto);
}
