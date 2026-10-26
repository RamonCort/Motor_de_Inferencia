#include "../include/Asteroide.hpp"
#include "../include/Misil.hpp"
#include <cstdlib>
#include <cmath>

sf::Texture Asteroide::textureAsteroide;
bool Asteroide::textureCargada = false;

Asteroide::Asteroide(float posicionX, float posicionY, TamanoAsteroide tam, float targetX, float targetY) 
    : x(posicionX), y(posicionY), velocidadX(0), velocidadY(0), tamano(tam) {
    if (!textureCargada) {        if (!textureAsteroide.loadFromFile("assets/images/Asteroide.pixil.png")) {
            // Si falla, usar círculo como fallback con tamaño según el tipo
            float radio = 20.f;
            switch (tamano) {
                case TamanoAsteroide::GRANDE: radio = 80.f; break;  // 4x más grande
                case TamanoAsteroide::PEQUENO: radio = 20.f; break; // Tamaño original
            }
            shape.setRadius(radio);
            shape.setPointCount(20);
            shape.setFillColor(sf::Color(120, 120, 120));
            shape.setOutlineColor(sf::Color::Red);
            shape.setOutlineThickness(3);
            shape.setPosition(x, y);
            return;
        }
        textureCargada = true;
    }
    sprite.setTexture(textureAsteroide);
    sprite.setOrigin(textureAsteroide.getSize().x/2, textureAsteroide.getSize().y/2);
    sprite.setPosition(x, y);
      // Establecer escala según el tamaño
    float escala = 0.5f; // Escala base
    switch (tamano) {
        case TamanoAsteroide::GRANDE: escala = 2.0f; break;  // 4x más grande
        case TamanoAsteroide::PEQUENO: escala = 0.8f; break; // Un poco más grande que el original
    }
    sprite.setScale(escala, escala);
}

float Asteroide::ObtenerX() const { return x; }
float Asteroide::ObtenerY() const { return y; }

void Asteroide::Dibujar(sf::RenderWindow& window) {
    if (textureCargada) {
        sprite.setPosition(x, y);
        sprite.rotate(2.0f); // Rota el asteroide cada frame
        sprite.setColor(sf::Color::White);
        window.draw(sprite);
    } else {
        shape.setPosition(x, y);
        window.draw(shape);
    }
}

void Asteroide::Mover(float limiteY, float limiteX, float velocidadY) {
    y += velocidadY;
    if (y > limiteY) {
        x = static_cast<float>(rand() % static_cast<int>(limiteX - 40) + 20);
        y = 0;
    }
    if (textureCargada) sprite.setPosition(x, y);
    else shape.setPosition(x, y);
}

void Asteroide::Colisionar(Nave& nave) {
    if (textureCargada) {
        if (sprite.getGlobalBounds().intersects(nave.ObtenerSprite().getGlobalBounds())) {
            x = static_cast<float>(rand() %  (int)(800 - 40) + 20);
            y = 0;
        }
    } else {
        if (shape.getGlobalBounds().intersects(nave.ObtenerSprite().getGlobalBounds())) {
            x = static_cast<float>(rand() %  (int)(800 - 40) + 20);
            y = 0;
        }
    }
}

bool Asteroide::VerificarColisionConNave(Nave& nave) {
    if (textureCargada) {
        return sprite.getGlobalBounds().intersects(nave.ObtenerSprite().getGlobalBounds());
    } else {
        return shape.getGlobalBounds().intersects(nave.ObtenerSprite().getGlobalBounds());
    }
}

bool Asteroide::VerificarColisionConMisil(const Misil& misil) {
    if (textureCargada) {
        return sprite.getGlobalBounds().intersects(misil.ObtenerLimites());
    } else {
        return shape.getGlobalBounds().intersects(misil.ObtenerLimites());
    }
}

void Asteroide::MoverHaciaObjetivo(const sf::Vector2f& posicionNave, float limiteY, float limiteX, float velocidadBase) {
    // Reducir la velocidad base para que vayan más lento
    float velocidadReducida = velocidadBase * 0.4f; // Los asteroides van un 40% de la velocidad original
    
    // Calcular la dirección hacia la nave
    float dx = posicionNave.x - x;
    float dy = posicionNave.y - y;
    
    // Calcular la distancia
    float distancia = std::sqrt(dx * dx + dy * dy);
    
    if (distancia > 0) {
        // Normalizar la dirección y aplicar velocidad reducida
        velocidadX = (dx / distancia) * velocidadReducida;
        velocidadY = (dy / distancia) * velocidadReducida;
        
        // Mover el asteroide
        x += velocidadX;
        y += velocidadY;
        
        // Si el asteroide sale de los límites, reposicionarlo
        if (x < -40 || x > limiteX + 40 || y < -40 || y > limiteY + 40) {
            ReiniciarPosicion(limiteX, limiteY);
        }
        
        // Actualizar posición del sprite
        if (textureCargada) sprite.setPosition(x, y);
        else shape.setPosition(x, y);
    }
}

void Asteroide::ReiniciarPosicion(float limiteX, float limiteY) {
    // Elegir aleatoriamente uno de los 4 lados del margen
    int lado = rand() % 4;
    
    switch (lado) {
        case 0: // Arriba
            x = static_cast<float>(rand() % static_cast<int>(limiteX));
            y = -40;
            break;
        case 1: // Derecha
            x = limiteX + 40;
            y = static_cast<float>(rand() % static_cast<int>(limiteY));
            break;
        case 2: // Abajo
            x = static_cast<float>(rand() % static_cast<int>(limiteX));
            y = limiteY + 40;
            break;
        case 3: // Izquierda
            x = -40;
            y = static_cast<float>(rand() % static_cast<int>(limiteY));
            break;
    }
    
    // Actualizar posición del sprite
    if (textureCargada) sprite.setPosition(x, y);
    else shape.setPosition(x, y);
}

std::vector<Asteroide> Asteroide::Dividir() const {
    std::vector<Asteroide> nuevosAsteroides;
    
    // Solo se puede dividir si es tamaño grande
    if (tamano != TamanoAsteroide::GRANDE) {
        return nuevosAsteroides; // Vector vacío, no se divide más
    }
    
    // Crear dos asteroides pequeños
    float offset = 30.f; // Separación entre los nuevos asteroides
    
    // Primer asteroide (se mueve hacia la izquierda y arriba)
    Asteroide asteroide1(x - offset, y - offset, TamanoAsteroide::PEQUENO);
    asteroide1.velocidadX = velocidadX - 0.5f; // Velocidad más lenta
    asteroide1.velocidadY = velocidadY - 0.5f;
    
    // Segundo asteroide (se mueve hacia la derecha y abajo)
    Asteroide asteroide2(x + offset, y + offset, TamanoAsteroide::PEQUENO);
    asteroide2.velocidadX = velocidadX + 0.5f; // Velocidad más lenta
    asteroide2.velocidadY = velocidadY + 0.5f;
    
    nuevosAsteroides.push_back(asteroide1);
    nuevosAsteroides.push_back(asteroide2);
    
    return nuevosAsteroides;
}
