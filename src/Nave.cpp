#include "../include/Nave.hpp"
#include <iostream>
#include <cmath> // Incluir cmath para std::atan2

Nave::Nave(float x, float y, const std::string& ruta) {
    if (!texture.loadFromFile(ruta)) {
        std::cerr << "No se pudo cargar la imagen: " << ruta << std::endl;
    }    sprite.setTexture(texture);
    sf::FloatRect bounds = sprite.getLocalBounds();
    // Hacer la nave más delgada y pequeña
    sprite.setScale(1.0f, 1.5f); // Ancho normal, altura aumentada para hacerla más delgada
    sprite.setOrigin(bounds.width / 2, bounds.height / 2);
    sprite.setPosition(x, y);
    velocidad = 0.8f; // Velocidad reducida
    
    // Inicializar posición anterior para el rastro
    posicionAnterior = sf::Vector2f(x, y);
    
    // Solo la nave de la izquierda (AstroNave_pixil.png) tiene rastro
    tieneRastro = (ruta.find("AstroNave_pixil.png") != std::string::npos);
}

void Nave::Mover(const sf::RenderWindow& window) {
    sf::Vector2f posicionActual = sprite.getPosition();
    sf::Vector2f movimiento(0.f, 0.f);
      // Rotación con teclas de cursor izquierda/derecha
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        sprite.rotate(-2.0f); // Rotación más lenta hacia la izquierda
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        sprite.rotate(2.0f);  // Rotación más lenta hacia la derecha
    
    // Movimiento hacia adelante con flecha arriba o W
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        // Calcular la dirección basada en la rotación actual de la nave
        float angulo = sprite.getRotation() - 90.f; // -90 porque la nave apunta hacia arriba por defecto
        float rad = angulo * 3.14159265f / 180.f;
        
        // Mover en la dirección hacia donde apunta la nave
        movimiento.x = std::cos(rad) * velocidad;
        movimiento.y = std::sin(rad) * velocidad;
    }
    
    // Determinar si la nave se está moviendo
    seEstaMoviendo = (movimiento.x != 0.f || movimiento.y != 0.f);
      // Crear partículas de rastro si se está moviendo y la nave tiene rastro
    if (seEstaMoviendo && tieneRastro) {
        // Calcular la posición trasera de la nave basada en su rotación
        float angulo = sprite.getRotation() - 90.f; // -90 porque la nave apunta hacia arriba por defecto
        float rad = angulo * 3.14159265f / 180.f;
        
        // Obtener el tamaño de la nave
        sf::FloatRect bounds = sprite.getGlobalBounds();
        float distanciaTrasera = bounds.height * 0.4f; // Distancia desde el centro hacia la parte trasera
        
        // Calcular la posición trasera
        sf::Vector2f posicionTrasera;
        posicionTrasera.x = posicionActual.x - std::cos(rad) * distanciaTrasera;
        posicionTrasera.y = posicionActual.y - std::sin(rad) * distanciaTrasera;
        
        // Agregar nueva partícula de rastro
        particulas.emplace_back(posicionTrasera);
    }
      sprite.move(movimiento);
    
    // Solo apuntar hacia el mouse si no se están usando las teclas de rotación
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        ApuntarHaciaMouse(window);
    }
    
    // Actualizar el rastro
    ActualizarRastro();
    
    // Actualizar posición anterior
    posicionAnterior = sprite.getPosition();
}

void Nave::ApuntarHaciaMouse(const sf::RenderWindow& window) {
    // Obtener la posición del mouse en el mundo
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos);
    
    // Obtener la posición de la nave
    sf::Vector2f navePos = sprite.getPosition();
    
    // Calcular el ángulo hacia el mouse
    float dx = worldMousePos.x - navePos.x;
    float dy = worldMousePos.y - navePos.y;
    float angulo = std::atan2(dy, dx) * 180.f / 3.14159265f + 90.f;
    
    // Aplicar la rotación
    sprite.setRotation(angulo);
}

void Nave::ActualizarRastro() {
    // Actualizar vida de las partículas y eliminar las que han expirado
    for (auto it = particulas.begin(); it != particulas.end();) {
        it->vida -= 0.05f; // Velocidad de desvanecimiento
        it->alpha = static_cast<sf::Uint8>(it->vida * 255.f);
        
        if (it->vida <= 0.f) {
            it = particulas.erase(it);
        } else {
            ++it;
        }
    }
}

void Nave::DibujarRastro(sf::RenderWindow& window) {
    // Solo dibujar el rastro si la nave lo tiene habilitado
    if (!tieneRastro) return;
    
    // Dibujar cada partícula de rastro
    for (const auto& particula : particulas) {
        sf::CircleShape punto(2.f); // Tamaño pequeño para el efecto de rastro
        punto.setPosition(particula.posicion.x - 2.f, particula.posicion.y - 2.f);
        
        // Color azul con transparencia que se desvanece
        sf::Color colorAzul(0, 150, 255, particula.alpha);
        punto.setFillColor(colorAzul);
        
        window.draw(punto);
    }
}

void Nave::Dibujar(sf::RenderWindow& window) {
    // Primero dibujar el rastro, luego la nave
    DibujarRastro(window);
    window.draw(sprite);
}

sf::Sprite& Nave::ObtenerSprite() {
    return sprite;
}
