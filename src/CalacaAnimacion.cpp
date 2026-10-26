#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(612, 490), "Animación Calaca Sprite");
    sf::Texture textura;
    if (!textura.loadFromFile("assets/images/Calaca_Sprite.png")) {
        std::cerr << "No se pudo cargar la imagen Calaca_Sprite.png" << std::endl;
        return 1;
    }
    sf::Sprite sprite(textura);
    int frameWidth = 153;
    int frameHeight = 245;
    int totalFrames = 8;
    int framesPorFila = 4;
    float tiempoPorFrame = 0.12f;
    int frameActual = 0;
    sf::Clock relojAnimacion;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (relojAnimacion.getElapsedTime().asSeconds() > tiempoPorFrame) {
            frameActual = (frameActual + 1) % totalFrames;
            int fila = frameActual / framesPorFila;
            int columna = frameActual % framesPorFila;
            sprite.setTextureRect(sf::IntRect(columna * frameWidth, fila * frameHeight, frameWidth, frameHeight));
            relojAnimacion.restart();
        }
        sprite.setPosition((612-frameWidth)/2, (490-frameHeight)/2);
        window.clear(sf::Color::Black);
        window.draw(sprite);
        window.display();
    }
    return 0;
}
