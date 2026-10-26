#include "../include/Vida.hpp"

Vida::Vida() : width(200), height(60) {
    box.setSize(sf::Vector2f(width, height));
    box.setFillColor(sf::Color(30,30,30,200));
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(3);
    box.setPosition(20, 20);

    if (font.loadFromFile("assets/fonts/Retro Gaming.ttf")) {
        texto.setFont(font);
    }
    texto.setCharacterSize(28);
    texto.setFillColor(sf::Color::White);
    texto.setPosition(30, 30);
    texto.setString("Vidas : 3");
}

void Vida::EstablecerVidas(int vidas) {
    texto.setString("Vidas : " + std::to_string(vidas));
}

void Vida::Dibujar(sf::RenderWindow& window) {
    window.draw(box);
    // Dibujar corazones como vidas
    static sf::Texture corazonTexture;
    static bool corazonCargado = false;
    if (!corazonCargado) {
        corazonCargado = corazonTexture.loadFromFile("assets/images/Corazon.png");
    }
    int vidas = 3;
    try {
        // Extraer número de vidas del texto
        std::string str = texto.getString();
        size_t pos = str.find(":");
        if (pos != std::string::npos) {
            vidas = std::stoi(str.substr(pos + 1));
        }
    } catch (...) {}

    float x = 30.f;
    float y = 38.f; // Más arriba que antes (antes 65.f)
    float separacion = 45.f;
    for (int i = 0; i < vidas; ++i) {
        if (corazonCargado) {
            sf::Sprite corazon(corazonTexture);
            corazon.setScale(0.08f, 0.08f);
            corazon.setPosition(x + i * separacion, y);
            window.draw(corazon);
        }
    }
    window.draw(texto);
}
