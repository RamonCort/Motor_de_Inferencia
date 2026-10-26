#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp> // <-- Incluye el encabezado de Audio
#include <string>

class Ventana {
public:    Ventana(int width = 400, int height = 300); // Cambia el constructor
    void Mostrar();
    void MostrarInicio(); // <-- Nuevo método para la pantalla de inicio
private:
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;    sf::Font font;
    sf::Text errorText;
    bool fontLoaded;
    
    sf::Music music; // Música de fondo del juego
    sf::Music musicaInicio; // Música de los menús
    sf::Music musicaMuerte; // Música de Game Over
    
    // Sonidos globales
    sf::SoundBuffer bufferGameStart;
    sf::Sound sonidoGameStart;
    sf::SoundBuffer bufferObtenerItem;
    sf::Sound sonidoObtenerItem;
    sf::SoundBuffer bufferSonidoNave;
    sf::Sound sonidoNave;
    
    // Variables para el control de cámara con mouse
    sf::View camara;
    sf::Vector2f ultimaPosicionMouse;
    bool arrastandoCamara;
    float velocidadCamara;
};
