#include "../include/Margen.hpp"
#include "../include/Nave.hpp"

Margen::Margen(float anchoVentana, float altoVentana, float grosor)
    : ancho(anchoVentana), alto(altoVentana), grosor(grosor)
{
    float margenIzq = 10.f;
    float margenSup = 10.f;
    float margenDer = 10.f;
    float margenInf = 10.f;
    float largoH = anchoVentana - margenIzq - margenDer;
    float largoV = altoVentana - margenSup - margenInf;

    // Líneas horizontales
    lineaSuperior.setSize(sf::Vector2f(largoH, grosor));
    lineaSuperior.setPosition(margenIzq, margenSup);
    lineaSuperior.setFillColor(sf::Color::White);

    lineaInferior.setSize(sf::Vector2f(largoH, grosor));
    lineaInferior.setPosition(margenIzq, altoVentana - margenInf - grosor);
    lineaInferior.setFillColor(sf::Color::White);

    // Líneas verticales
    lineaIzquierda.setSize(sf::Vector2f(grosor, largoV));
    lineaIzquierda.setPosition(margenIzq, margenSup);
    lineaIzquierda.setFillColor(sf::Color::White);

    lineaDerecha.setSize(sf::Vector2f(grosor, largoV));
    lineaDerecha.setPosition(anchoVentana - margenDer - grosor, margenSup);
    lineaDerecha.setFillColor(sf::Color::White);

    // Esquinas (simuladas con círculos pequeños)
    float radio = grosor * 1.5f;
    esquinaSupIzq.setRadius(radio);
    esquinaSupIzq.setPointCount(16);
    esquinaSupIzq.setFillColor(sf::Color::White);
    esquinaSupIzq.setPosition(margenIzq - radio + grosor/2, margenSup - radio + grosor/2);

    esquinaSupDer.setRadius(radio);
    esquinaSupDer.setPointCount(16);
    esquinaSupDer.setFillColor(sf::Color::White);
    esquinaSupDer.setPosition(anchoVentana - margenDer - grosor - radio + grosor/2, margenSup - radio + grosor/2);

    esquinaInfIzq.setRadius(radio);
    esquinaInfIzq.setPointCount(16);
    esquinaInfIzq.setFillColor(sf::Color::White);
    esquinaInfIzq.setPosition(margenIzq - radio + grosor/2, altoVentana - margenInf - grosor - radio + grosor/2);

    esquinaInfDer.setRadius(radio);
    esquinaInfDer.setPointCount(16);
    esquinaInfDer.setFillColor(sf::Color::White);
    esquinaInfDer.setPosition(anchoVentana - margenDer - grosor - radio + grosor/2, altoVentana - margenInf - grosor - radio + grosor/2);
}

void Margen::Dibujar(sf::RenderWindow& window) {
    window.draw(lineaSuperior);
    window.draw(lineaInferior);
    window.draw(lineaIzquierda);
    window.draw(lineaDerecha);
    window.draw(esquinaSupIzq);
    window.draw(esquinaSupDer);
    window.draw(esquinaInfIzq);
    window.draw(esquinaInfDer);
}

void Margen::Limitar(Nave& nave) {
    sf::Sprite& sprite = nave.ObtenerSprite();
    sf::FloatRect bounds = sprite.getGlobalBounds();
    float minX = grosor + bounds.width / 2;
    float maxX = ancho - grosor - bounds.width / 2;
    float minY = grosor + bounds.height / 2;
    float maxY = alto - grosor - bounds.height / 2;
    sf::Vector2f pos = sprite.getPosition();
    if (pos.x < minX) pos.x = minX;
    if (pos.x > maxX) pos.x = maxX;
    if (pos.y < minY) pos.y = minY;
    if (pos.y > maxY) pos.y = maxY;
    sprite.setPosition(pos);
}
