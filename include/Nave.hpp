#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// Estructura para las partículas de rastro
struct ParticulaRastro {
    sf::Vector2f posicion;
    float vida;
    sf::Uint8 alpha;
    
    ParticulaRastro(sf::Vector2f pos) : posicion(pos), vida(1.0f), alpha(255) {}
};

class Nave {
public:
    Nave(float x, float y, const std::string& ruta = "assets/images/nave.png");
    void Dibujar(sf::RenderWindow& window);
    void Mover(const sf::RenderWindow& window);
    void ApuntarHaciaMouse(const sf::RenderWindow& window); // Nueva función para apuntar al mouse
    sf::Sprite& ObtenerSprite(); // <-- Añadido para Margen
    float ObtenerVelocidad() const { return velocidad; }
    void EstablecerVelocidad(float v) { velocidad = v; }
    float ObtenerAngulo() const { return sprite.getRotation(); } // Para obtener el ángulo de disparo
    void ActualizarRastro(); // Actualizar las partículas de rastro
    void DibujarRastro(sf::RenderWindow& window); // Dibujar el rastro
    
protected:
    sf::Texture texture;
    sf::Sprite sprite;
    float velocidad = 5.0f;
      // Sistema de rastro
    std::vector<ParticulaRastro> particulas;
    sf::Vector2f posicionAnterior;
    bool seEstaMoviendo = false;
    bool tieneRastro = false; // Solo la nave de la izquierda tiene rastro
};
