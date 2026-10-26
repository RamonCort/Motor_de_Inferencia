#include "../include/Ventana.hpp"
#include "../include/Oportunidad.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "../include/Nave.hpp"
#include "../include/Misil.hpp"
#include "../include/Margen.hpp"
#include "../include/Punto.hpp"
#include "../include/Puntaje.hpp"
#include "../include/Vida.hpp"
#include "../include/VidaExtra.hpp"
#include "../include/EscudoItem.hpp"
#include "../include/DobleDisparoItem.hpp"
#include <vector>
#include "../include/Asteroide.hpp"
#include <cmath> // Nueva inclusión para std::sin
#include <cstdlib> // Para rand()
#include "../include/TablaDePuntaje.hpp"
#include "../include/Explosion.hpp"

// Prototipo de la función de selección de armamento
int seleccionarArmamento(sf::RenderWindow& window, sf::Font& font, sf::Sound& sonidoGameStart, sf::Music& musicaInicio);

// Prototipo de la función de selección de nave
int seleccionarNave(sf::RenderWindow& window, sf::Font& font, sf::Sound& sonidoGameStart, sf::Music& musicaInicio);

// Prototipo de la función de instrucciones
void mostrarInstrucciones(sf::RenderWindow& window, sf::Font& font);

// Constructor
Ventana::Ventana(int width, int height) : window(sf::VideoMode(1200, 900), "Asteroids - Ventana de inicio"), fontLoaded(false) {
    if (!texture.loadFromFile("assets/images/nave.png")) {
        std::cerr << "No se pudo cargar la imagen: assets/images/nave.png" << std::endl;
    } else {
        sprite.setTexture(texture);
        sprite.setPosition(100, 100);    }    // Cargar música de fondo (pero no reproducir automáticamente)
    if (music.openFromFile("assets/music/videoplayback.ogg")) {
        music.setLoop(true);
        // No reproducir aquí, se reproducirá cuando inicie el juego
    } else {
        std::cerr << "No se pudo cargar la música de fondo: assets/music/videoplayback.ogg" << std::endl;
    }
      // Cargar música de inicio/menús
    if (musicaInicio.openFromFile("assets/music/Musica_Inicio.ogg")) {
        musicaInicio.setLoop(true);
        musicaInicio.setVolume(70.0f); // Volumen más bajo para menús
    } else {
        std::cerr << "No se pudo cargar la música de inicio: assets/music/Musica_Inicio.ogg" << std::endl;
    }
    
    // Cargar música de muerte/Game Over
    if (musicaMuerte.openFromFile("assets/music/Musica_Muerte.ogg")) {
        musicaMuerte.setLoop(true);
        musicaMuerte.setVolume(60.0f); // Volumen más bajo para ambiente de muerte
    } else {
        std::cerr << "No se pudo cargar la música de muerte: assets/music/Musica_Muerte.ogg" << std::endl;
    }
      // Cargar sonido de selección/inicio
    if (bufferGameStart.loadFromFile("assets/music/game_start.ogg")) {
        sonidoGameStart.setBuffer(bufferGameStart);
    } else {
        std::cerr << "No se pudo cargar el sonido game_start.ogg" << std::endl;
    }
      // Cargar sonido de obtener item
    if (bufferObtenerItem.loadFromFile("assets/music/obtener_Item_1.ogg")) {
        sonidoObtenerItem.setBuffer(bufferObtenerItem);
    } else {
        std::cerr << "No se pudo cargar el sonido obtener_Item_1.ogg" << std::endl;
    }
    
    // Cargar sonido de la nave
    if (bufferSonidoNave.loadFromFile("assets/music/sonido_Nave.ogg")) {
        sonidoNave.setBuffer(bufferSonidoNave);
        sonidoNave.setVolume(30.0f); // Volumen más bajo para no ser repetitivo
    } else {
        std::cerr << "No se pudo cargar el sonido sonido_Nave.ogg" << std::endl;
    }
    
    // Inicializar cámara con control de mouse
    camara = window.getDefaultView();
    ultimaPosicionMouse = sf::Vector2f(0, 0);
    arrastandoCamara = false;
    velocidadCamara = 1.0f;
    
    // (Eliminado el bloque de carga de fuente y configuración de errorText)
}

void Ventana::MostrarInicio() {
    std::cout << "Iniciando pantalla de portada..." << std::endl;
    
    // Reproducir música de inicio/menús
    if (musicaInicio.getStatus() != sf::Music::Playing) {
        musicaInicio.play();
    }
    
    // Cargar la imagen de portada
    sf::Texture portadaTexture;
    if (!portadaTexture.loadFromFile("assets/images/Portada.jpg")) {
        std::cerr << "No se pudo cargar la portada.\n";
        return;
    }
    std::cout << "Portada cargada correctamente" << std::endl;
    sf::Sprite portadaSprite(portadaTexture);
    float scaleX = window.getSize().x / portadaSprite.getLocalBounds().width;
    float scaleY = window.getSize().y / portadaSprite.getLocalBounds().height;
    portadaSprite.setScale(scaleX, scaleY);    // Cargar la imagen del título
    sf::Texture tituloTexture;
    if (!tituloTexture.loadFromFile("assets/images/Titulo.png")) {
        std::cerr << "No se pudo cargar la imagen del título: assets/images/Titulo.png\n";
        // Si falla, simplemente no se muestra el título
    } else {
        std::cout << "Título cargado correctamente" << std::endl;
    }    sf::Sprite tituloSprite(tituloTexture);
    // Escalar el título para que sea visible y centrado - AUMENTADO EL TAMAÑO
    float tituloScale = std::min(window.getSize().x / tituloSprite.getLocalBounds().width * 1.2f,
                                 window.getSize().y / tituloSprite.getLocalBounds().height * 0.4f);
    tituloSprite.setScale(tituloScale, tituloScale);
    tituloSprite.setOrigin(tituloSprite.getLocalBounds().width / 2, tituloSprite.getLocalBounds().height / 2);
    tituloSprite.setPosition(window.getSize().x / 2.f, 200);    // --- Botón Jugar centrado seleccionable por mouse ---
    sf::Font fontPlay;
    fontPlay.loadFromFile("assets/fonts/Retro Gaming.ttf");
    sf::Text playText("JUGAR", fontPlay, 64);
    playText.setFillColor(sf::Color::White);
    playText.setOutlineColor(sf::Color::Black);
    playText.setOutlineThickness(3.f);
    sf::FloatRect playBounds = playText.getLocalBounds();
    playText.setOrigin(playBounds.width / 2, playBounds.height / 2);
    playText.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + 200);

    // Fondo del botón
    sf::RectangleShape botonFondo;
    botonFondo.setSize(sf::Vector2f(playBounds.width + 60, playBounds.height + 40));
    botonFondo.setOrigin((playBounds.width + 60) / 2, (playBounds.height + 40) / 2);
    botonFondo.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + 200);
    botonFondo.setFillColor(sf::Color(0, 0, 0, 120)); // Negro semi-transparente
    botonFondo.setOutlineColor(sf::Color::White);
    botonFondo.setOutlineThickness(2.f);

    // --- Configuración de cometas pasando por el fondo ---
    sf::Texture cometaTexture;
    if (!cometaTexture.loadFromFile("assets/images/Asteroide.pixil.png")) {
        std::cerr << "No se pudo cargar la imagen del cometa: assets/images/Asteroide.pixil.png\n";
    }
    
    // Estructura para manejar cometas
    struct Cometa {
        sf::Sprite sprite;
        float velocidadX;
        float velocidadY;
        float rotacion;
        float escala;
    };
    
    // Vector de cometas
    std::vector<Cometa> cometas;
      // Crear cometas iniciales
    for (int i = 0; i < 8; i++) {
        Cometa cometa;
        cometa.sprite.setTexture(cometaTexture);
        cometa.escala = 0.5f + (rand() % 4) * 0.1f; // Escalas entre 0.5 y 0.8
        cometa.sprite.setScale(cometa.escala, cometa.escala);
        cometa.sprite.setOrigin(cometaTexture.getSize().x / 2.f, cometaTexture.getSize().y / 2.f);
        
        // Posición inicial fuera de la pantalla (lado izquierdo)
        cometa.sprite.setPosition(-100, rand() % (int)window.getSize().y);
          // Velocidades aleatorias
        cometa.velocidadX = 80 + rand() % 120; // Velocidad horizontal entre 80 y 200
        cometa.velocidadY = -40 + rand() % 80; // Velocidad vertical entre -40 y 40
        cometa.rotacion = 0.5f + (rand() % 10) * 0.1f; // Rotación entre 0.5 y 1.4
          // Color azul semi-transparente para efecto de fondo
        cometa.sprite.setColor(sf::Color(100, 150, 255, 200));
          cometas.push_back(cometa);
    }    sf::Clock animClock;
    sf::Clock cometasClock; // Reloj separado para los cometas
    sf::Clock inicioTimer; // Timer para evitar salto automático
    float animSpeed = 2.5f;
    bool puedePresionar = false; // Bandera para controlar cuándo se puede presionar
    bool botonHover = false; // Para detectar si el mouse está sobre el botón
    
    std::cout << "Entrando al bucle de portada..." << std::endl;

    while (window.isOpen()) {
        // Después de 1 segundo, permitir que se presione ENTER o se use el mouse
        if (!puedePresionar && inicioTimer.getElapsedTime().asSeconds() > 1.0f) {
            puedePresionar = true;
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            
            // Detección de mouse sobre el botón
            if (event.type == sf::Event::MouseMoved && puedePresionar) {
                sf::Vector2f mousePos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
                botonHover = botonFondo.getGlobalBounds().contains(mousePos);
            }
            
            // Click del mouse en el botón
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && puedePresionar) {
                sf::Vector2f mousePos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);                if (botonFondo.getGlobalBounds().contains(mousePos)) {
                    std::cout << "Botón JUGAR clickeado, saliendo de portada..." << std::endl;
                    sonidoGameStart.play(); // Reproducir sonido de selección
                    return; // Sale al hacer click en el botón
                }
            }
              // Mantener funcionalidad de ENTER
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter && puedePresionar) {
                std::cout << "ENTER presionado, saliendo de portada..." << std::endl;
                sonidoGameStart.play(); // Reproducir sonido de selección
                return; // Sale al presionar ENTER solo después del delay
            }
        }
        
        // Animación: solo escala del sprite del título, sin cambio de color
        float t = animClock.getElapsedTime().asSeconds();
        float scale = 1.0f + 0.08f * std::sin(t * animSpeed);
        tituloSprite.setScale(tituloScale * scale, tituloScale * scale);
        tituloSprite.setColor(sf::Color::White); // Sin animación de color
        // Mantener centrado
        tituloSprite.setOrigin(tituloSprite.getLocalBounds().width / 2, tituloSprite.getLocalBounds().height / 2);
        tituloSprite.setPosition(window.getSize().x / 2.f, 200);
        
        // Animación del botón Jugar con movimiento vertical suave
        float botonOffset = std::sin(t * 1.5f) * 15.f; // Movimiento vertical suave
        float botonY = window.getSize().y / 2.f + 200 + botonOffset;
        
        // Cambiar colores según hover y disponibilidad
        if (puedePresionar) {
            if (botonHover) {
                playText.setFillColor(sf::Color::Yellow);
                botonFondo.setFillColor(sf::Color(50, 50, 50, 180));
                botonFondo.setOutlineColor(sf::Color::Yellow);
            } else {
                playText.setFillColor(sf::Color::White);
                botonFondo.setFillColor(sf::Color(0, 0, 0, 120));
                botonFondo.setOutlineColor(sf::Color::White);
            }
        } else {
            playText.setFillColor(sf::Color(150, 150, 150));
            botonFondo.setFillColor(sf::Color(0, 0, 0, 80));
            botonFondo.setOutlineColor(sf::Color(100, 100, 100));
        }
        
        // Actualizar posiciones del botón con animación
        playText.setPosition(window.getSize().x / 2.f, botonY);
        botonFondo.setPosition(window.getSize().x / 2.f, botonY);// --- Actualizar cometas ---
        float deltaTime = cometasClock.restart().asSeconds();
        
        for (auto& cometa : cometas) {
            // Actualizar posición
            cometa.sprite.move(cometa.velocidadX * deltaTime, cometa.velocidadY * deltaTime);
            
            // Actualizar rotación
            cometa.sprite.rotate(cometa.rotacion);
              // Si el cometa sale por la derecha, reiniciar por la izquierda
            if (cometa.sprite.getPosition().x > window.getSize().x + 100) {
                cometa.sprite.setPosition(-100, rand() % (int)window.getSize().y);
                cometa.velocidadX = 80 + rand() % 120;
                cometa.velocidadY = -40 + rand() % 80;
                cometa.escala = 0.5f + (rand() % 4) * 0.1f;
                cometa.sprite.setScale(cometa.escala, cometa.escala);
            }
        }        window.clear();
        window.draw(portadaSprite);          // Dibujar cometas detrás del título
        for (const auto& cometa : cometas) {
            window.draw(cometa.sprite); // Ya tienen el color azul aplicado
        }
        
        window.draw(tituloSprite);
        window.draw(botonFondo); // Dibujar fondo del botón
        window.draw(playText); // Dibujar texto del botón
        window.display();
    }
}

// Nueva función para seleccionar nave
int seleccionarNave(sf::RenderWindow& window, sf::Font& font, sf::Sound& sonidoGameStart, sf::Music& musicaInicio) {
    // Asegurar que la música de menú esté sonando
    if (musicaInicio.getStatus() != sf::Music::Playing) {
        musicaInicio.play();
    }
    
    // Cargar imagen de fondo
    sf::Texture fondoTexture;
    fondoTexture.loadFromFile("assets/images/Fondo.png");
    sf::Sprite fondoSprite(fondoTexture);
    float scaleX = window.getSize().x / static_cast<float>(fondoTexture.getSize().x);
    float scaleY = window.getSize().y / static_cast<float>(fondoTexture.getSize().y);
    fondoSprite.setScale(scaleX, scaleY);

    sf::Texture tex1, tex2;
    tex1.loadFromFile("assets/images/AstroNave_pixil.png");
    tex2.loadFromFile("assets/images/nave.png");
    sf::Sprite nave1(tex1), nave2(tex2);
    nave1.setScale(2,2); nave2.setScale(2,2);
    nave1.setPosition(window.getSize().x/2.f-200, window.getSize().y/2.f);
    nave2.setPosition(window.getSize().x/2.f+100, window.getSize().y/2.f);
    int seleccion = 0; // 0 = AstroNave, 1 = nave normal

    bool elegido = false;
    sf::Text txt("Elige tu nave: ← → ENTER", font, 48);
    txt.setFillColor(sf::Color::White); // Cambiado a blanco
    txt.setOutlineColor(sf::Color::Black);
    txt.setOutlineThickness(3.f);
    sf::FloatRect txtBounds = txt.getLocalBounds();
    txt.setOrigin(txtBounds.width / 2, txtBounds.height / 2);
    txt.setPosition(window.getSize().x/2.f, window.getSize().y/2.f-180);

    // Cargar fuente Retro Gaming para la leyenda
    sf::Font fontRetroGaming;
    fontRetroGaming.loadFromFile("assets/fonts/Retro Gaming.ttf");
    sf::Text leyenda("Favor de seleccionar su nave", fontRetroGaming, 44);
    leyenda.setFillColor(sf::Color::White); // Cambiado a blanco
    leyenda.setOutlineColor(sf::Color::Black);
    leyenda.setOutlineThickness(4.f);
    leyenda.setStyle(sf::Text::Bold);
    sf::FloatRect leyendaBounds = leyenda.getLocalBounds();
    leyenda.setOrigin(leyendaBounds.width / 2, leyendaBounds.height / 2);
    leyenda.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 220);

    sf::Clock animClock;
    while(window.isOpen() && !elegido) {
        sf::Event event;        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) window.close();
            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Left) seleccion = 0;
                if(event.key.code == sf::Keyboard::Right) seleccion = 1;
                if(event.key.code == sf::Keyboard::Enter) {
                    sonidoGameStart.play(); // Reproducir sonido de selección
                    elegido = true;
                }
            }
        }
        float t = animClock.getElapsedTime().asSeconds();
        float offset = std::sin(t * 2.5f) * 18.f; // Rebote suave
        leyenda.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 220 + offset);
        txt.setPosition(window.getSize().x/2.f, window.getSize().y/2.f-180 + offset/2.f);
        window.clear(sf::Color::Black);
        window.draw(fondoSprite); // Dibujar el fondo primero
        window.draw(leyenda);
        window.draw(txt);
        if(seleccion==0) nave1.setColor(sf::Color::Yellow), nave2.setColor(sf::Color::White);
        else nave1.setColor(sf::Color::White), nave2.setColor(sf::Color::Yellow);
        window.draw(nave1); window.draw(nave2);
        window.display();
    }
    return seleccion;
}

// Nueva función para pedir el nombre al usuario
std::string pedirNombre(sf::RenderWindow& window, sf::Font& font) {
    std::string nombre;
    sf::Text texto("Ingresa tu nombre:", font, 48);
    texto.setFillColor(sf::Color::Yellow);
    texto.setOutlineColor(sf::Color::Black);
    texto.setOutlineThickness(4.f);
    texto.setStyle(sf::Text::Bold);
    texto.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 100);
    sf::FloatRect textoBounds = texto.getLocalBounds();
    texto.setOrigin(textoBounds.width / 2, textoBounds.height / 2);

    sf::Font inputFont;
    if (!inputFont.loadFromFile("assets/fonts/Retro Gaming.ttf")) {
        inputFont = font;
    }    // Leyenda centrada y bonita arriba del recuadro
    sf::Text leyenda("Favor de Escribir tu nombre", inputFont, 36);
    leyenda.setFillColor(sf::Color::White);
    leyenda.setOutlineColor(sf::Color::Black);
    leyenda.setOutlineThickness(3.f);
    leyenda.setStyle(sf::Text::Bold | sf::Text::Italic);
    sf::FloatRect leyendaBounds = leyenda.getLocalBounds();
    leyenda.setOrigin(leyendaBounds.width / 2, leyendaBounds.height / 2);
    leyenda.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 40);

    sf::Text textoNombre("", inputFont, 36);
    textoNombre.setFillColor(sf::Color::White);
    textoNombre.setPosition(window.getSize().x / 2.f - 190, window.getSize().y / 2.f + 5);

    sf::RectangleShape inputBox;
    inputBox.setSize(sf::Vector2f(400, 50));
    inputBox.setFillColor(sf::Color(255, 255, 255, 60)); // Blanco translúcido
    inputBox.setOutlineColor(sf::Color::White);
    inputBox.setOutlineThickness(2);
    inputBox.setPosition(window.getSize().x / 2.f - 200, window.getSize().y / 2.f);

    sf::Clock cursorClock;
    bool cursorVisible = true;

    // Cargar textura de fondo
    sf::Texture fondoTexture;
    fondoTexture.loadFromFile("assets/images/Fondo.png");
    sf::Sprite fondoSprite(fondoTexture);
    // Ajustar el fondo al tamaño de la ventana
    float scaleX = window.getSize().x / static_cast<float>(fondoTexture.getSize().x);
    float scaleY = window.getSize().y / static_cast<float>(fondoTexture.getSize().y);
    fondoSprite.setScale(scaleX, scaleY);    // Texto grande tipo pantalla de inicio
    sf::Text tituloNombre("ESCRIBE TU NOMBRE", font, 72);
    tituloNombre.setFillColor(sf::Color::White);
    tituloNombre.setOutlineColor(sf::Color::Black);
    tituloNombre.setOutlineThickness(8.f);
    tituloNombre.setStyle(sf::Text::Bold);
    sf::FloatRect tituloBounds = tituloNombre.getLocalBounds();
    tituloNombre.setOrigin(tituloBounds.width / 2, tituloBounds.height / 2);
    tituloNombre.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 180);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == '\b') { // Backspace
                    if (!nombre.empty()) nombre.pop_back();
                } else if (event.text.unicode == '\r' || event.text.unicode == '\n') { // Enter
                    if (!nombre.empty()) return nombre;
                } else if (nombre.size() < 12 && event.text.unicode < 128 && event.text.unicode >= 32) {
                    nombre += static_cast<char>(event.text.unicode);
                }
            }
        }
        textoNombre.setString(nombre + "|"); // Muestra el cursor

        // Parpadeo del cursor
        if (cursorClock.getElapsedTime().asSeconds() >= 0.5f) {
            cursorVisible = !cursorVisible;
            cursorClock.restart();
        }
        if (!cursorVisible) {
            textoNombre.setString(nombre); // Oculta el cursor
        }        window.clear(sf::Color::Black);
        window.draw(fondoSprite); // Dibuja el fondo
        window.draw(tituloNombre); // Dibuja el texto grande
        window.draw(leyenda);
        window.draw(inputBox); // Dibuja el recuadro antes del texto
        window.draw(textoNombre);
        window.display();
    }
    return nombre; // Return por defecto en caso de que la ventana se cierre
}

void Ventana::Mostrar() {
    // Mostrar la portada solo al inicio
    std::cout << "Mostrando portada..." << std::endl;
    MostrarInicio();
    
    // Limpiar buffer de eventos después de la portada
    sf::Event clearEvent;
    while (window.pollEvent(clearEvent)) {}
    
    inicio:
    // Desde aquí, solo menús y juego (SIN portada)
    // --- Variables de límites de ventana ---
    float limiteX = window.getSize().x;
    float limiteY = window.getSize().y;
    // Escudo item (invulnerabilidad)
    EscudoItem escudoItem(0, 0); // Se inicializa con valores dummy
    sf::Clock relojEscudoItem;
    bool escudoItemActivo = false;
    bool invulnerable = false;
    sf::Clock relojInvulnerable;
    float duracionInvulnerable = 5.0f;
    // Inicializar posición real de escudoItem después de definir limiteX
    escudoItem.Reiniciar(limiteX);
    // --- Fondo ---
    sf::Texture fondoTexture;
    if (!fondoTexture.loadFromFile("assets/images/Fondo2.png")) {
        std::cerr << "No se pudo cargar el fondo: assets/images/Fondo2.png" << std::endl;
    }
    sf::Sprite fondoSprite(fondoTexture);
    // Escalar el fondo para que cubra toda la ventana
    float scaleX = window.getSize().x / fondoSprite.getLocalBounds().width;
    float scaleY = window.getSize().y / fondoSprite.getLocalBounds().height;
    fondoSprite.setScale(scaleX, scaleY);

    // --- Selección de nave ---
    // Cargar la fuente una sola vez antes de cualquier bucle y mostrar error solo una vez
    static sf::Font fontGlobal;
    static bool fuenteCargada = fontGlobal.loadFromFile("assets/fonts/Retro Gaming.ttf");
    static bool fuenteErrorMostrado = false;
    if (!fuenteCargada && !fuenteErrorMostrado) {
        std::cerr << "Failed to load font \"assets/fonts/Retro Gaming.ttf\"" << std::endl;
        fuenteErrorMostrado = true;
    }
    // Pedir nombre antes de elegir nave
    std::string nombreJugador = pedirNombre(window, fontGlobal);
      // Menú de nave con título
    int naveSeleccionada = seleccionarNave(window, fontGlobal, sonidoGameStart, musicaInicio);
    
    // Menú de armamento
    int armamentoSeleccionado = seleccionarArmamento(window, fontGlobal, sonidoGameStart, musicaInicio);
    
    // Mostrar instrucciones del juego
    mostrarInstrucciones(window, fontGlobal);
    
    std::string navePath = naveSeleccionada==0 ? "assets/images/AstroNave_pixil.png" : "assets/images/nave.png";

    // --- Variables de juego declaradas fuera del bucle ---
    float cx = window.getSize().x / 2.f;
    float cy = window.getSize().y / 2.f;
    Nave nave(cx, cy, navePath);

    std::vector<Misil> misiles;
    bool disparoAnterior = false;    Margen margen(window.getSize().x, window.getSize().y);
    Punto punto;
    sf::Font fontPuntos;
    fontPuntos.loadFromFile("assets/fonts/Retro Gaming.ttf"); // Cargar la fuente aquí
    Puntaje puntaje(window.getSize().x);
    Vida vida;
    int vidas = 3;    vida.EstablecerVidas(vidas);
    Oportunidad oportunidad(3, 3);
    vida.EstablecerVidas(oportunidad.ObtenerVidas());

    std::vector<Asteroide> asteroides;
    int cantidadAsteroides = 5;
    // limiteX y limiteY ya declarados arriba

    sf::Clock relojAsteroides;
    int asteroidesCreados = 0;
    float tiempoEntreAsteroides = 2.5f;

    float velocidadAsteroide = 0.6f;
    int ultimoPuntajeVel = 0;

    // Variables para vida extra
    VidaExtra vidaExtra(static_cast<float>(rand() % static_cast<int>(limiteX - 40) + 20), 0);
    sf::Clock relojVidaExtra;
    bool vidaExtraActiva = false;

    // Doble disparo
    DobleDisparoItem dobleDisparoItem(static_cast<float>(rand() % static_cast<int>(limiteX - 40) + 20), 0);
    sf::Clock relojDobleDisparo;
    bool dobleDisparoEnPantalla = false;
    bool dobleDisparoActivo = false;
    float tiempoDobleDisparo = 0.0f;

    // Vector para animaciones de explosión
    std::vector<Explosion> explosiones;

    while (window.isOpen()) {


for (int i = 0; i < cantidadAsteroides; ++i) {
            // Crear asteroides en posiciones aleatorias del margen
            int lado = rand() % 4;
            float ax, ay;
            
            switch (lado) {
                case 0: // Arriba
                    ax = static_cast<float>(rand() % static_cast<int>(limiteX));
                    ay = -40;
                    break;
                case 1: // Derecha
                    ax = limiteX + 40;
                    ay = static_cast<float>(rand() % static_cast<int>(limiteY));
                    break;
                case 2: // Abajo
                    ax = static_cast<float>(rand() % static_cast<int>(limiteX));
                    ay = limiteY + 40;
                    break;
                case 3: // Izquierda
                    ax = -40;
                    ay = static_cast<float>(rand() % static_cast<int>(limiteY));
                    break;
            }
            
            asteroides.emplace_back(ax, ay);
        }        bool pAnterior = false;
        bool gameOver = false;
        bool pausado = false;

        // --- Variables estáticas del fondo y agujero negro para todo el método
        static sf::Texture agujeroTexture;
        static bool agujeroCargado = false;
        static sf::Sprite agujeroSprite;
        static float agujeroScale = 0.3f;
        static float agujeroRot = 0.f;
        static bool agujeroVisible = true;
        static bool fondoCambiado = false;

        // --- Bucle principal del juego ---        // --- Sonido de disparo ---
        sf::SoundBuffer bufferLaser;
        bufferLaser.loadFromFile("assets/music/Laser2.ogg");
        sf::Sound sonidoLaser;
        sonidoLaser.setBuffer(bufferLaser);
          // --- Sonido de explosión de asteroide ---
        sf::SoundBuffer bufferExplosion;
        bufferExplosion.loadFromFile("assets/music/explosion_Asteroide.ogg");
        sf::Sound sonidoExplosion;        sonidoExplosion.setBuffer(bufferExplosion);
        
        // --- Variables para el láser ---
        sf::Clock relojLaser;
        sf::Clock relojRecargaLaser;
        bool laserActivo = false;
        float duracionLaser = 1.0f;
        float recargaLaser = 2.0f;
        
        // Variables para almacenar información del láser para dibujo
        bool mostrarLaser = false;
        sf::VertexArray laserLineas(sf::Lines);
        sf::VertexArray laserIzquierdo(sf::Lines, 2);
        sf::VertexArray laserDerecho(sf::Lines, 2);
        sf::VertexArray laserCentral(sf::Lines, 2);        bool esLaserDoble = false;        // Detener música de menú e iniciar música de fondo al comenzar el juego
        musicaInicio.stop();
        music.play();

        while (window.isOpen() && !gameOver) {
            // Lógica de aparición de escudo cada 25 segundos
            if (!escudoItemActivo && relojEscudoItem.getElapsedTime().asSeconds() >= 25.0f) {
                escudoItem.Reiniciar(limiteX);
                escudoItemActivo = true;
                relojEscudoItem.restart();
            }            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    sonidoGameStart.play(); // Reproducir sonido al pausar/reanudar
                    pausado = !pausado; // Alternar pausa
                }
            }            window.clear(sf::Color::Black);
            window.draw(fondoSprite); // Dibuja el fondo antes de todo

            margen.Dibujar(window);
              // Solo actualizar la lógica del juego si no está pausado
            if (!pausado) {
                // Verificar si la nave se está moviendo hacia adelante y reproducir sonido
                static bool sonidoNaveReproduciendo = false;
                bool moviendoAdelante = sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
                
                if (moviendoAdelante) {
                    if (!sonidoNaveReproduciendo && sonidoNave.getStatus() != sf::Sound::Playing) {
                        sonidoNave.play();
                        sonidoNaveReproduciendo = true;
                    }
                } else {
                    if (sonidoNaveReproduciendo) {
                        sonidoNave.stop();
                        sonidoNaveReproduciendo = false;
                    }
                }
                
                nave.Mover(window);
                margen.Limitar(nave);// Disparo de misil o láser (Espacio o click izquierdo del mouse)
            bool disparoActual = sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Mouse::isButtonPressed(sf::Mouse::Left);
            static bool dobleDisparoHabilitado = false;
            // Control de tiempo de doble disparo
            if (tiempoDobleDisparo > 0.f) {
                tiempoDobleDisparo -= 1.f/60.f; // Aproximado a 60 FPS
                dobleDisparoHabilitado = true;
            } else {
                dobleDisparoHabilitado = false;
            }
            if (armamentoSeleccionado == 0) { // Misil
                if (disparoActual && !disparoAnterior) {
                    sf::Vector2f navePos = nave.ObtenerSprite().getPosition();
                    float angulo = nave.ObtenerSprite().getRotation();
                    sf::FloatRect bounds = nave.ObtenerSprite().getGlobalBounds();
                    float ancho = bounds.width * nave.ObtenerSprite().getScale().x;
                    float offset = ancho * 0.28f; // Separación lateral (ajustable)
                    float rad = (angulo - 90.f) * 3.14159265f / 180.f;
                    float cosA = std::cos(rad);
                    float sinA = std::sin(rad);
                    sf::Vector2f lateral(-sinA, cosA);
                    if (dobleDisparoHabilitado) {
                        sf::Vector2f posIzq = navePos + lateral * offset;
                        sf::Vector2f posDer = navePos - lateral * offset;
                        misiles.emplace_back(posIzq.x, posIzq.y, angulo);
                        misiles.emplace_back(posDer.x, posDer.y, angulo);
                    } else {
                        misiles.emplace_back(navePos.x, navePos.y - bounds.height/2, angulo);
                    }
                    sonidoLaser.play();
                }
            } else { // Láser
                if (!laserActivo && disparoActual && relojRecargaLaser.getElapsedTime().asSeconds() >= recargaLaser) {
                    laserActivo = true;
                    relojLaser.restart();
                    sonidoLaser.play();
                }
                if (laserActivo) {
                    sf::Sprite& naveSprite = nave.ObtenerSprite();
                    float angulo = naveSprite.getRotation() - 90.f;
                    float rad = angulo * 3.14159265f / 180.f;
                    float largo = naveSprite.getGlobalBounds().height * naveSprite.getScale().y / 2.f;
                    float ancho = naveSprite.getGlobalBounds().width * naveSprite.getScale().x;
                    float offsetLateral = ancho * 0.28f; // Igual que para misiles dobles
                    sf::Vector2f dirLaser(std::cos(rad), std::sin(rad));
                    float largoLaser = window.getSize().y * 1.2f;
                    // Centro
                    sf::Vector2f puntaNave = naveSprite.getPosition() + dirLaser * largo;
                    sf::Vector2f finLaser = puntaNave + dirLaser * largoLaser;
                    // Si doble disparo, dos láseres laterales
                    if (dobleDisparoHabilitado) {
                        sf::Vector2f lateral(-dirLaser.y, dirLaser.x);
                        sf::Vector2f puntaIzq = naveSprite.getPosition() + lateral * offsetLateral + dirLaser * largo;
                        sf::Vector2f puntaDer = naveSprite.getPosition() - lateral * offsetLateral + dirLaser * largo;
                        sf::Vector2f finIzq = puntaIzq + dirLaser * largoLaser;
                        sf::Vector2f finDer = puntaDer + dirLaser * largoLaser;
                        sf::VertexArray laserIzq(sf::Lines, 2);                        laserIzquierdo[0].position = puntaIzq;
                        laserIzquierdo[0].color = sf::Color::Cyan;
                        laserIzquierdo[1].position = finIzq;
                        laserIzquierdo[1].color = sf::Color::Cyan;
                        sf::VertexArray laserDer(sf::Lines, 2);
                        laserDer[0].position = puntaDer;
                        laserDer[0].color = sf::Color::Cyan;
                        laserDer[1].position = finDer;
                        laserDer[1].color = sf::Color::Cyan;
                        laserDerecho = laserDer;
                        mostrarLaser = true;
                        esLaserDoble = true;
                        // Colisión para ambos láseres laterales
                        auto colisionLaser = [&](const sf::Vector2f& start, const sf::Vector2f& end) {
                            for (auto itAst = asteroides.begin(); itAst != asteroides.end(); ) {
                                sf::FloatRect bounds = itAst->sprite.getGlobalBounds();
                                std::vector<std::pair<sf::Vector2f, sf::Vector2f>> lados = {
                                    {sf::Vector2f(bounds.left, bounds.top), sf::Vector2f(bounds.left + bounds.width, bounds.top)},
                                    {sf::Vector2f(bounds.left, bounds.top), sf::Vector2f(bounds.left, bounds.top + bounds.height)},
                                    {sf::Vector2f(bounds.left + bounds.width, bounds.top), sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height)},
                                    {sf::Vector2f(bounds.left, bounds.top + bounds.height), sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height)}
                                };
                                bool intersecta = false;
                                for (const auto& lado : lados) {
                                    float denom = (end.x - start.x) * (lado.second.y - lado.first.y) - (end.y - start.y) * (lado.second.x - lado.first.x);
                                    if (denom == 0) continue;
                                    float ua = ((lado.second.x - lado.first.x) * (start.y - lado.first.y) - (lado.second.y - lado.first.y) * (start.x - lado.first.x)) / denom;
                                    float ub = ((end.x - start.x) * (start.y - lado.first.y) - (end.y - start.y) * (start.x - lado.first.x)) / denom;
                                    if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
                                        intersecta = true;
                                        break;
                                    }
                                }                                if (intersecta) {
                                    sonidoExplosion.play(); // Reproducir sonido de explosión
                                    if (itAst->VerificarSiPuedeDestruirse()) {
                                        itAst = asteroides.erase(itAst);
                                        punto.Sumar(30);
                                    } else {
                                        std::vector<Asteroide> nuevosAsteroides = itAst->Dividir();
                                        itAst = asteroides.erase(itAst);
                                        for (const auto& nuevoAst : nuevosAsteroides) {
                                            asteroides.push_back(nuevoAst);
                                        }
                                        punto.Sumar(10);
                                    }
                                } else {
                                    ++itAst;
                                }
                            }
                        };
                        colisionLaser(puntaIzq, finIzq);
                        colisionLaser(puntaDer, finDer);} else {
                        // Láser normal (centro)
                        laserCentral[0].position = puntaNave;
                        laserCentral[0].color = sf::Color::Cyan;
                        laserCentral[1].position = finLaser;
                        laserCentral[1].color = sf::Color::Cyan;
                        mostrarLaser = true;
                        esLaserDoble = false;
                        // Colisión del láser con asteroides (destruye todo lo que toca)
                        for (auto itAst = asteroides.begin(); itAst != asteroides.end(); ) {
                            sf::FloatRect bounds = itAst->sprite.getGlobalBounds();
                            std::vector<std::pair<sf::Vector2f, sf::Vector2f>> lados = {
                                {sf::Vector2f(bounds.left, bounds.top), sf::Vector2f(bounds.left + bounds.width, bounds.top)},
                                {sf::Vector2f(bounds.left, bounds.top), sf::Vector2f(bounds.left, bounds.top + bounds.height)},
                                {sf::Vector2f(bounds.left + bounds.width, bounds.top), sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height)},
                                {sf::Vector2f(bounds.left, bounds.top + bounds.height), sf::Vector2f(bounds.left + bounds.width, bounds.top + bounds.height)}
                            };
                            bool intersecta = false;
                            for (const auto& lado : lados) {
                                float denom = (finLaser.x - puntaNave.x) * (lado.second.y - lado.first.y) - (finLaser.y - puntaNave.y) * (lado.second.x - lado.first.x);
                                if (denom == 0) continue;
                                float ua = ((lado.second.x - lado.first.x) * (puntaNave.y - lado.first.y) - (lado.second.y - lado.first.y) * (puntaNave.x - lado.first.x)) / denom;
                                float ub = ((finLaser.x - puntaNave.x) * (puntaNave.y - lado.first.y) - (finLaser.y - puntaNave.y) * (puntaNave.x - lado.first.x)) / denom;
                                if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
                                    intersecta = true;
                                    break;
                                }
                            }                            if (intersecta) {
                                sonidoExplosion.play(); // Reproducir sonido de explosión
                                if (itAst->VerificarSiPuedeDestruirse()) {
                                    itAst = asteroides.erase(itAst);
                                    punto.Sumar(30);
                                } else {
                                    std::vector<Asteroide> nuevosAsteroides = itAst->Dividir();
                                    itAst = asteroides.erase(itAst);
                                    for (const auto& nuevoAst : nuevosAsteroides) {
                                        asteroides.push_back(nuevoAst);
                                    }
                                    punto.Sumar(10);
                                }
                            } else {
                                ++itAst;
                            }
                        }
                    }                    if (relojLaser.getElapsedTime().asSeconds() >= duracionLaser || !disparoActual) {
                        laserActivo = false;
                        mostrarLaser = false;
                        relojRecargaLaser.restart();
                        sonidoLaser.stop();
                    }
                }
            }
            disparoAnterior = disparoActual;

            // --- COLISIONES ASTEROIDE-NAVE ---
            for (auto& ast : asteroides) {
                if (!invulnerable && ast.VerificarColisionConNave(nave)) {                    oportunidad.PerderVida();
                    vida.EstablecerVidas(oportunidad.ObtenerVidas());
                    ast.x = static_cast<float>(rand() % static_cast<int>(limiteX - 40) + 20);
                    ast.y = 0;
                    ast.sprite.setPosition(ast.x, ast.y);
                    if (oportunidad.VerificarSiSinOportunidades()) {
                        gameOver = true;
                        break;
                    }
                }
            }            // Dibujar y mover escudo item si está activo
            if (escudoItemActivo) {
                escudoItem.Mover(limiteY, limiteX, 1.0f);                // Colisión con nave
                if (escudoItem.VerificarColision(nave)) {
                    sonidoObtenerItem.play(); // Reproducir sonido de obtener item
                    invulnerable = true;
                    relojInvulnerable.restart();
                    escudoItemActivo = false;
                }
                if (escudoItem.ObtenerY() > limiteY) {
                    escudoItemActivo = false;
                }
            }
            // Control de duración de invulnerabilidad
            if (invulnerable && relojInvulnerable.getElapsedTime().asSeconds() >= duracionInvulnerable) {
                invulnerable = false;
            }            // --- COLISIONES MISIL-ASTEROIDE ---
            for (auto itAst = asteroides.begin(); itAst != asteroides.end(); ) {
                bool asteroideEliminado = false;
                for (auto itMisil = misiles.begin(); itMisil != misiles.end(); ) {                    if (itAst->VerificarColisionConMisil(*itMisil)) {
                        itMisil = misiles.erase(itMisil);
                        sonidoExplosion.play(); // Reproducir sonido de explosión
                        
                        // Verificar si el asteroide puede destruirse completamente
                        if (itAst->VerificarSiPuedeDestruirse()) {
                            // Asteroide pequeño, se destruye completamente
                            itAst = asteroides.erase(itAst);
                            asteroideEliminado = true;
                            punto.Sumar(30); // Más puntos por destruir completamente
                        } else {
                            // Asteroide grande o mediano, se divide
                            std::vector<Asteroide> nuevosAsteroides = itAst->Dividir();
                            itAst = asteroides.erase(itAst);
                            asteroideEliminado = true;
                            
                            // Agregar los asteroides resultantes de la división
                            for (const auto& nuevoAst : nuevosAsteroides) {
                                asteroides.push_back(nuevoAst);
                            }
                            
                            punto.Sumar(10); // Puntos por dividir
                        }
                        
                        // Crear explosión en la posición del asteroide destruido
                        explosiones.emplace_back(itAst->ObtenerX(), itAst->ObtenerY());

                        // Solo aumentar velocidad de la nave, no de los asteroides
                        nave.EstablecerVelocidad(nave.ObtenerVelocidad() + 0.05f);
                        break;
                    } else {
                        ++itMisil;
                    }
                }
                if (!asteroideEliminado) {
                    ++itAst;
                }
            }

            // Mover y dibujar misiles
            for (auto it = misiles.begin(); it != misiles.end(); ) {
                it->Mover();
                if (it->VerificarSiEstaFueraDePantalla(0)) {
                    it = misiles.erase(it);
                } else {
                    it->Dibujar(window);
                    ++it;
                }
            }            // Aumentar velocidad de los asteroides cada 90 puntos en un 20%
            int puntosActuales = punto.ObtenerPuntos();
            if (puntosActuales >= 90 && (puntosActuales / 90) > (ultimoPuntajeVel / 90)) {
                velocidadAsteroide *= 1.2f;
                ultimoPuntajeVel = puntosActuales;
            }// Crear un nuevo asteroide cada 2.5 segundos hasta llegar a la cantidad deseada
            if (asteroides.size() < cantidadAsteroides && relojAsteroides.getElapsedTime().asSeconds() >= tiempoEntreAsteroides) {
                // Crear asteroide en posición aleatoria del margen
                int lado = rand() % 4;
                float ax, ay;
                
                switch (lado) {
                    case 0: // Arriba
                        ax = static_cast<float>(rand() % static_cast<int>(limiteX));
                        ay = -40;
                        break;
                    case 1: // Derecha
                        ax = limiteX + 40;
                        ay = static_cast<float>(rand() % static_cast<int>(limiteY));
                        break;
                    case 2: // Abajo
                        ax = static_cast<float>(rand() % static_cast<int>(limiteX));
                        ay = limiteY + 40;
                        break;
                    case 3: // Izquierda
                        ax = -40;
                        ay = static_cast<float>(rand() % static_cast<int>(limiteY));
                        break;
                }
                
                asteroides.emplace_back(ax, ay);
                relojAsteroides.restart();
            }            // Mover y dibujar asteroides con movimiento teledirigido hacia la nave
            for (auto& asteroide : asteroides) {
                asteroide.MoverHaciaObjetivo(nave.ObtenerSprite().getPosition(), limiteY, limiteX, velocidadAsteroide);
            }

            // --- Agujero Negro central ---
            if (!agujeroCargado) {
                if (agujeroTexture.loadFromFile("assets/images/Agujero_Negro.png")) {
                    agujeroSprite.setTexture(agujeroTexture);
                    agujeroSprite.setOrigin(agujeroTexture.getSize().x/2, agujeroTexture.getSize().y/2);
                    agujeroCargado = true;
                }
            }            // El agujero negro aparece a los 100 puntos
            if (agujeroCargado && agujeroVisible && punto.ObtenerPuntos() >= 600) {
                agujeroScale += 0.0005f;
                agujeroRot += 1.0f;
                agujeroSprite.setScale(agujeroScale, agujeroScale);
                agujeroSprite.setPosition(window.getSize().x/2.f, window.getSize().y/2.f);
                agujeroSprite.setRotation(agujeroRot);
                window.draw(agujeroSprite);
                // Detectar colisión SOLO con el centro del agujero
                sf::Vector2f naveCentro = nave.ObtenerSprite().getPosition();
                sf::Vector2f agujeroCentro = agujeroSprite.getPosition();
                float distancia = std::sqrt(std::pow(naveCentro.x - agujeroCentro.x, 2) + std::pow(naveCentro.y - agujeroCentro.y, 2));
                float radioColision = agujeroSprite.getGlobalBounds().width * 0.15f; // Solo el centro
                if (distancia < radioColision) {
                    if (!fondoCambiado) {
                        if (fondoTexture.loadFromFile("assets/images/Portada.jpg")) {
                            fondoSprite.setTexture(fondoTexture);
                            float scaleX = window.getSize().x / fondoSprite.getLocalBounds().width;
                            float scaleY = window.getSize().y / fondoSprite.getLocalBounds().height;
                            float scale = std::max(scaleX, scaleY); // Escalado tipo 'cover'
                            fondoSprite.setScale(scale, scale);
                            fondoSprite.setOrigin(fondoSprite.getLocalBounds().width / 2, fondoSprite.getLocalBounds().height / 2);
                            fondoSprite.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
                        }
                        fondoCambiado = true;
                        agujeroVisible = false;
                    }
                }
            }
            // --- EFECTO DE ATRACCIÓN DEL AGUJERO NEGRO ---
            if (agujeroCargado && agujeroVisible && punto.ObtenerPuntos() >= 600) {
                sf::Vector2f agujeroCentro = agujeroSprite.getPosition();
                float escalaAgujero = agujeroScale; // Entre 0.3 y lo que crezca
                float factorFuerza = std::max(1.0f, escalaAgujero * 2.5f); // Aumenta con el tamaño
                // Atracción para asteroides
                for (auto it = asteroides.begin(); it != asteroides.end(); ) {
                    sf::Vector2f astPos = it->sprite.getPosition();
                    sf::Vector2f dir = agujeroCentro - astPos;
                    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
                    if (dist > 1.f) {
                        dir /= dist; // Normaliza
                        float fuerza = 0.15f * std::min(300.f, dist) / 300.f * factorFuerza;
                        it->sprite.move(dir * fuerza);
                        it->x = it->sprite.getPosition().x;
                        it->y = it->sprite.getPosition().y;
                    }                    // Eliminar si colisiona con el centro
                    float radioColision = agujeroSprite.getGlobalBounds().width * 0.15f;
                    if (dist < radioColision) {
                        sonidoExplosion.play(); // Reproducir sonido de explosión/absorción
                        it = asteroides.erase(it);
                    } else {
                        ++it;
                    }
                }
                // Atracción para la nave
                sf::Vector2f navePos = nave.ObtenerSprite().getPosition();
                sf::Vector2f dirNave = agujeroCentro - navePos;
                float distNave = std::sqrt(dirNave.x * dirNave.x + dirNave.y * dirNave.y);
                if (distNave > 1.f) {
                    dirNave /= distNave;
                    float fuerzaNave = 0.10f * std::min(300.f, distNave) / 300.f * factorFuerza;
                    nave.ObtenerSprite().move(dirNave * fuerzaNave);
                }
                // Si la nave colisiona con el centro, puedes poner gameOver o efecto
                float radioColisionNave = agujeroSprite.getGlobalBounds().width * 0.15f;
                if (distNave < radioColisionNave) {
                    // Por ahora, solo la atrae, pero puedes poner lógica de game over aquí si quieres
                }
                // Atracción para el escudo
                if (escudoItemActivo) {
                    sf::Vector2f escudoPos(escudoItem.ObtenerX(), escudoItem.ObtenerY());
                    sf::Vector2f dirEscudo = agujeroCentro - escudoPos;
                    float distEscudo = std::sqrt(dirEscudo.x * dirEscudo.x + dirEscudo.y * dirEscudo.y);
                    if (distEscudo > 1.f) {
                        dirEscudo /= distEscudo;
                        float fuerzaEscudo = 0.12f * std::min(300.f, distEscudo) / 300.f * factorFuerza;
                        escudoItem.EstablecerPos(escudoPos.x + dirEscudo.x * fuerzaEscudo, escudoPos.y + dirEscudo.y * fuerzaEscudo);
                    }
                    float radioColisionEscudo = agujeroSprite.getGlobalBounds().width * 0.15f;
                    if (distEscudo < radioColisionEscudo) {
                        escudoItemActivo = false;
                    }
                }
                // Atracción para la vida extra
                if (vidaExtraActiva) {
                    sf::Vector2f vidaPos(vidaExtra.ObtenerX(), vidaExtra.ObtenerY());
                    sf::Vector2f dirVida = agujeroCentro - vidaPos;
                    float distVida = std::sqrt(dirVida.x * dirVida.x + dirVida.y * dirVida.y);
                    if (distVida > 1.f) {
                        dirVida /= distVida;
                        float fuerzaVida = 0.12f * std::min(300.f, distVida) / 300.f * factorFuerza;
                        vidaExtra.EstablecerPosicion(vidaPos.x + dirVida.x * fuerzaVida, vidaPos.y + dirVida.y * fuerzaVida);
                    }
                    float radioColisionVida = agujeroSprite.getGlobalBounds().width * 0.15f;
                    if (distVida < radioColisionVida) {
                        vidaExtraActiva = false;
                    }
                }            }

            if (fontLoaded) {
                errorText.setString("No se pudo cargar la imagen:\nassets/nave.png");
                window.draw(errorText);
            }

            // Sumar puntos solo una vez por pulsación de 'P'
            bool pActual = sf::Keyboard::isKeyPressed(sf::Keyboard::P);
            if (pActual && !pAnterior) {
                punto.Sumar();            }
            pAnterior = pActual;

            // Lógica de aparición de vida extra cada 10 segundos (más frecuente)
            if (!vidaExtraActiva && relojVidaExtra.getElapsedTime().asSeconds() >= 10.0f) {
                vidaExtra.Reiniciar(limiteX);
                vidaExtraActiva = true;
                relojVidaExtra.restart();
            }            // Dibujar y mover vida extra si está activa
            if (vidaExtraActiva) {                vidaExtra.Mover(limiteY, limiteX, 1.0f);                // Colisión con nave
                if (vidaExtra.VerificarColision(nave)) {
                    sonidoObtenerItem.play(); // Reproducir sonido de obtener item
                    oportunidad.SumarVida(); // Suma solo una vida
                    vida.EstablecerVidas(oportunidad.ObtenerVidas());
                    vidaExtraActiva = false;
                }
                if (vidaExtra.ObtenerY() > limiteY) {
                    vidaExtraActiva = false;
                }
            }

            // Lógica de activación de doble disparo
            if (dobleDisparoActivo) {
                // nave.ActivarDobleDisparo(); // Eliminado: no existe
                if (relojDobleDisparo.getElapsedTime().asSeconds() >= tiempoDobleDisparo) {
                    dobleDisparoActivo = false;
                    // nave.DesactivarDobleDisparo(); // Eliminado: no existe
                }
            }

            // Lógica de aparición de doble disparo cada 18 segundos
            if (!dobleDisparoEnPantalla && !dobleDisparoActivo && relojDobleDisparo.getElapsedTime().asSeconds() >= 18.0f) {                dobleDisparoItem.Reiniciar(limiteX);
                dobleDisparoEnPantalla = true;
                relojDobleDisparo.restart();
            }
            // Dibujar y mover doble disparo si está en pantalla
            if (dobleDisparoEnPantalla) {
                dobleDisparoItem.Mover(limiteY, limiteX, 1.0f);
                dobleDisparoItem.Dibujar(window);                if (dobleDisparoItem.VerificarColision(nave.ObtenerSprite())) {
                    sonidoObtenerItem.play(); // Reproducir sonido de obtener item
                    dobleDisparoEnPantalla = false;
                    dobleDisparoActivo = true;
                    tiempoDobleDisparo = 5.0f; // 5 segundos de doble disparo
                    relojDobleDisparo.restart();
                }
                if (dobleDisparoItem.ObtenerY() > limiteY) {
                    dobleDisparoEnPantalla = false;
                }
            }
            // Control de tiempo de poder doble disparo
            if (dobleDisparoActivo) {
                if (relojDobleDisparo.getElapsedTime().asSeconds() >= tiempoDobleDisparo) {
                    dobleDisparoActivo = false;
                }
            }            // Actualizar y dibujar explosiones
            for (auto it = explosiones.begin(); it != explosiones.end(); ) {
                it->Actualizar();
                it->Dibujar(window);
                if (it->VerificarSiEstaTerminada()) {
                    it = explosiones.erase(it);
                } else {
                    ++it;
                }
            }
            } // Fin del bloque if (!pausado)            // Dibujar elementos que siempre se ven (pausado o no)
            nave.Dibujar(window);
            for (auto& asteroide : asteroides) {
                asteroide.Dibujar(window);
            }
            for (auto& misil : misiles) {
                misil.Dibujar(window);
            }
            
            // Dibujar láser si está activo
            if (mostrarLaser) {
                if (esLaserDoble) {
                    window.draw(laserIzquierdo);
                    window.draw(laserDerecho);
                } else {
                    window.draw(laserCentral);
                }
            }
            
            // Dibujar agujero negro si está disponible y el puntaje es suficiente
            if (agujeroCargado && agujeroVisible && punto.ObtenerPuntos() >= 600) {
                window.draw(agujeroSprite);
            }            // Dibujar elementos de interfaz (puntaje, vidas, etc.)
            vida.Dibujar(window);
            puntaje.EstablecerPuntos(punto.ObtenerPuntos());
            puntaje.Dibujar(window);
            
            // Dibujar puntaje en esquina superior derecha
            sf::Font fontPuntos;
            if (fontPuntos.loadFromFile("assets/fonts/Retro Gaming.ttf")) {
                sf::Text textoPuntos;
                textoPuntos.setFont(fontPuntos);
                textoPuntos.setString("Puntos: " + std::to_string(punto.ObtenerPuntos()));
                textoPuntos.setCharacterSize(32);
                textoPuntos.setFillColor(sf::Color::White);
                float textWidth = textoPuntos.getLocalBounds().width;
                textoPuntos.setPosition(window.getSize().x - textWidth - 40, 20);
                window.draw(textoPuntos);
            }
            
            // Dibujar oportunidades restantes
            sf::Font fontOpor;
            if (fontOpor.loadFromFile("assets/fonts/Retro Gaming.ttf")) {
                sf::Text textoOpor;
                textoOpor.setFont(fontOpor);
                textoOpor.setString("Oportunidades: " + std::to_string(oportunidad.ObtenerOportunidades()));
                textoOpor.setCharacterSize(28);
                textoOpor.setFillColor(sf::Color::White);
                textoOpor.setPosition(20, 80);
                window.draw(textoOpor);
            }
            
            // Dibujar vida extra si está activa
            if (vidaExtraActiva) {
                vidaExtra.Dibujar(window);
            }
            
            // Dibujar escudo item si está activo
            if (escudoItemActivo) {
                escudoItem.Dibujar(window);
            }
            
            // Mostrar mensaje de pausa si está pausado
            if (pausado) {
                sf::Text textoPausa;
                sf::Font fontPausa;
                if (fontPausa.loadFromFile("assets/fonts/Retro Gaming.ttf")) {
                    textoPausa.setFont(fontPausa);
                } else {
                    textoPausa.setFont(fontGlobal);
                }
                textoPausa.setString(L"JUEGO PAUSADO\nPresiona ESC para continuar");
                textoPausa.setCharacterSize(48);
                textoPausa.setFillColor(sf::Color::Yellow);
                textoPausa.setStyle(sf::Text::Bold);
                sf::FloatRect bounds = textoPausa.getLocalBounds();
                textoPausa.setOrigin(bounds.width/2, bounds.height/2);
                textoPausa.setPosition(window.getSize().x/2, window.getSize().y/2);
                
                // Fondo semi-transparente para el texto de pausa
                sf::RectangleShape fondoPausa;
                fondoPausa.setSize(sf::Vector2f(window.getSize().x, window.getSize().y));
                fondoPausa.setFillColor(sf::Color(0, 0, 0, 128));
                window.draw(fondoPausa);
                window.draw(textoPausa);
            }            window.display();
        }

        // Detener música al terminar el juego
        music.stop();

        // --- Mostrar pantalla de Game Over ---
        if (window.isOpen()) {
            // Cargar fondo de muerte
            sf::Texture fondoMuerteTexture;
            fondoMuerteTexture.loadFromFile("assets/images/Fondo_Muerte.png");
            sf::Sprite fondoMuerteSprite(fondoMuerteTexture);
            // Escalar el fondo para que cubra toda la ventana
            float fondoScaleX = window.getSize().x / fondoMuerteSprite.getLocalBounds().width;
            float fondoScaleY = window.getSize().y / fondoMuerteSprite.getLocalBounds().height;
            fondoMuerteSprite.setScale(fondoScaleX, fondoScaleY);
            
            // Guardar puntaje
            TablaDePuntaje tabla("mejores_puntajes.txt");
            std::string nombre = nombreJugador.empty() ? "JUGADOR" : nombreJugador;
            tabla.Agregar(nombre, punto.ObtenerPuntos());

            sf::Font fontRetro;
            fontRetro.loadFromFile("assets/fonts/Retro Gaming.ttf");
            // Mostrar tabla de mejores puntajes
            std::string tablaStr = "MEJORES PUNTAJES\n";
            int pos = 1;
            for (const auto& entrada : tabla.Obtener()) {
                tablaStr += std::to_string(pos++) + ". " + entrada.nombre + ": " + std::to_string(entrada.puntaje) + "\n";
            }
            // Texto de Game Over arriba
            sf::Text textoFin;
            textoFin.setFont(fontRetro);
            textoFin.setString("Juego terminado\nPuntaje: " + std::to_string(punto.ObtenerPuntos()) + "\nPresiona ESPACIO para volver a jugar");
            textoFin.setCharacterSize(40);
            textoFin.setFillColor(sf::Color::White);
            textoFin.setStyle(sf::Text::Bold);
            sf::FloatRect boundsFin = textoFin.getLocalBounds();
            textoFin.setOrigin(boundsFin.width / 2, boundsFin.height / 2);
            textoFin.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 200);

            // Tabla de puntajes centrada en el medio
            sf::Text textoTabla(tablaStr, fontRetro, 36);
            textoTabla.setFillColor(sf::Color::White);
            textoTabla.setStyle(sf::Text::Bold);
            textoTabla.setOrigin(textoTabla.getLocalBounds().width / 2, textoTabla.getLocalBounds().height / 2);
            textoTabla.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f + 40);

            // --- Animación Calaca en Game Over (2 filas x 3 columnas, frame 204x245) ---
            sf::Texture texCalaca;
            texCalaca.loadFromFile("assets/images/Calaca_Sprite.png");
            sf::Sprite calaca(texCalaca);
            int calacaFrameWidth = 204;
            int calacaFrameHeight = 245;
            int calacaTotalFrames = 6;
            int calacaFramesPorFila = 3;
            float calacaTiempoPorFrame = 0.12f;
            int calacaFrameActual = 0;
            sf::Clock calacaRelojAnimacion;
            float calacaScale = 1.5f;
            calaca.setScale(calacaScale, calacaScale);
            // Calaca abajo a la derecha
            calaca.setPosition(window.getSize().x - calacaFrameWidth * calacaScale - 30, window.getSize().y - calacaFrameHeight * calacaScale - 30);            // --- Sonidos Game Over ---
            sf::SoundBuffer bufferGO1, bufferGO2;
            bufferGO1.loadFromFile("assets/music/Game_Over1.ogg");
            bufferGO2.loadFromFile("assets/music/Game_Over2.ogg");
            sf::Sound soundGO1(bufferGO1), soundGO2(bufferGO2);
            bool go1Played = false, go2Played = false, musicaMuerteIniciada = false;

            while (window.isOpen()) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        window.close();                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                        sonidoGameStart.play(); // Reproducir sonido al reiniciar
                        music.stop(); // Detener música del juego
                        musicaMuerte.stop(); // Detener música de muerte
                        
                        // Limpiar buffer de eventos
                        sf::Event clearEvent;
                        while (window.pollEvent(clearEvent)) {}
                        
                        // Volver al menú principal (sin portada)
                        goto inicio;
                    }
                }
                // Animar calaca
                if (calacaRelojAnimacion.getElapsedTime().asSeconds() > calacaTiempoPorFrame) {
                    calacaFrameActual = (calacaFrameActual + 1) % calacaTotalFrames;
                    int fila = calacaFrameActual / calacaFramesPorFila;
                    int columna = calacaFrameActual % calacaFramesPorFila;
                    calaca.setTextureRect(sf::IntRect(columna * calacaFrameWidth, fila * calacaFrameHeight, calacaFrameWidth, calacaFrameHeight));
                    calacaRelojAnimacion.restart();
                }                // Reproducir sonidos Game Over1 y Game Over2 una vez, en orden, y luego música de muerte
                if (!go1Played) { 
                    soundGO1.play(); 
                    go1Played = true; 
                }
                else if (go1Played && !go2Played && soundGO1.getStatus() == sf::Sound::Stopped) { 
                    soundGO2.play(); 
                    go2Played = true; 
                }
                else if (go2Played && !musicaMuerteIniciada && soundGO2.getStatus() == sf::Sound::Stopped) {
                    // Detener música del juego y reproducir música de muerte
                    music.stop();
                    musicaMuerte.play();
                    musicaMuerteIniciada = true;
                }
                window.clear(sf::Color::Black);
                window.draw(fondoMuerteSprite); // Dibujar fondo de muerte
                window.draw(calaca);
                window.draw(textoFin);
                window.draw(textoTabla);
                window.display();
            }
        }
        // Si la ventana se cerró, salimos del bucle
        break;
reiniciar_juego:
        // Reiniciar variables estáticas de fondo y agujero negro
        if (fondoTexture.loadFromFile("assets/images/Fondo2.png")) {
            fondoSprite.setTexture(fondoTexture);
            float scaleX = window.getSize().x / fondoSprite.getLocalBounds().width;
            float scaleY = window.getSize().y / fondoSprite.getLocalBounds().height;
            fondoSprite.setScale(scaleX, scaleY);
        }
        fondoCambiado = false;
        agujeroVisible = true;
        agujeroScale = 0.3f;
        agujeroRot = 0.f;
        // Reiniciar estado del juego
        punto = Punto();
        puntaje.EstablecerPuntos(0);
        vidas = 3;
        vida.EstablecerVidas(vidas);
        oportunidad = Oportunidad(3, 3);
        vida.EstablecerVidas(oportunidad.ObtenerVidas());
        asteroides.clear();
        cantidadAsteroides = 5;
        relojAsteroides.restart();
        asteroidesCreados = 0;
        velocidadAsteroide = 0.6f;
        ultimoPuntajeVel = 0;
        vidaExtra = VidaExtra(static_cast<float>(rand() % static_cast<int>(limiteX - 40) + 20), 0);
        relojVidaExtra.restart();
        vidaExtraActiva = false;
        dobleDisparoItem = DobleDisparoItem(static_cast<float>(rand() % static_cast<int>(limiteX - 40) + 20), 0);
        relojDobleDisparo.restart();
        dobleDisparoEnPantalla = false;
        dobleDisparoActivo = false;
        tiempoDobleDisparo = 0.0f;        
        // Volver a la selección de nave y armamento
        music.stop(); // Detener música del juego antes de volver a los menús
        musicaMuerte.stop(); // Detener música de muerte antes de volver a los menús
        
        // Limpiar buffer de eventos
        sf::Event clearEvent;
        while (window.pollEvent(clearEvent)) {}
        
        goto inicio;    }
}

// Implementación de la función de selección de armamento
int seleccionarArmamento(sf::RenderWindow& window, sf::Font& font, sf::Sound& sonidoGameStart, sf::Music& musicaInicio) {
    // Asegurar que la música de menú esté sonando
    if (musicaInicio.getStatus() != sf::Music::Playing) {
        musicaInicio.play();
    }
    
    // Cargar imagen de fondo
    sf::Texture fondoTexture;
    fondoTexture.loadFromFile("assets/images/Fondo.png");
    sf::Sprite fondoSprite(fondoTexture);
    float scaleX = window.getSize().x / static_cast<float>(fondoTexture.getSize().x);
    float scaleY = window.getSize().y / static_cast<float>(fondoTexture.getSize().y);
    fondoSprite.setScale(scaleX, scaleY);

    // Cargar fuente Retro Gaming para todos los textos
    sf::Font fontRetro;
    fontRetro.loadFromFile("assets/fonts/Retro Gaming.ttf");
    sf::Text leyenda("Favor de seleccionar su armamento", fontRetro, 44);
    leyenda.setFillColor(sf::Color::White); // Cambiado a blanco
    leyenda.setOutlineColor(sf::Color::Black);
    leyenda.setOutlineThickness(4.f);
    leyenda.setStyle(sf::Text::Bold);
    sf::FloatRect leyendaBounds = leyenda.getLocalBounds();
    leyenda.setOrigin(leyendaBounds.width / 2, leyendaBounds.height / 2);
    leyenda.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 220);

    // Solo dos opciones: Misil y Laser
    sf::Texture texMisil, texLaser;
    texMisil.loadFromFile("assets/images/Misil_Sprite.png");
    texLaser.loadFromFile("assets/images/Laser_Sprite.png");
    sf::Sprite misil1(texMisil), misil2(texLaser);
    misil1.setScale(0.5f, 0.5f);
    misil2.setScale(0.5f, 0.5f);
    misil1.setPosition(window.getSize().x/2.f-200, window.getSize().y/2.f+40);
    misil2.setPosition(window.getSize().x/2.f+100, window.getSize().y/2.f+40);

    sf::Text txtMisil("Misil", fontRetro, 28);
    sf::Text txtLaser("Laser", fontRetro, 28);
    txtMisil.setFillColor(sf::Color::White);
    txtLaser.setFillColor(sf::Color::White);
    txtMisil.setOrigin(txtMisil.getLocalBounds().width/2, 0);
    txtLaser.setOrigin(txtLaser.getLocalBounds().width/2, 0);
    txtMisil.setPosition(misil1.getPosition().x + misil1.getGlobalBounds().width/2, misil1.getPosition().y + misil1.getGlobalBounds().height + 10);
    txtLaser.setPosition(misil2.getPosition().x + misil2.getGlobalBounds().width/2, misil2.getPosition().y + misil2.getGlobalBounds().height + 10);

    int seleccion = 0; // 0 = Misil, 1 = Laser
    bool elegido = false;
    sf::Clock animClock;
    while(window.isOpen() && !elegido) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if(event.type == sf::Event::Closed) window.close();            if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Left) seleccion = 0;
                if(event.key.code == sf::Keyboard::Right) seleccion = 1;
                if(event.key.code == sf::Keyboard::Enter) {
                    sonidoGameStart.play(); // Reproducir sonido de selección
                    elegido = true;
                }
            }
        }
        float t = animClock.getElapsedTime().asSeconds();
        float offset = std::sin(t * 2.5f) * 18.f;
        leyenda.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f - 220 + offset);
        txtMisil.setPosition(misil1.getPosition().x + misil1.getGlobalBounds().width/2, misil1.getPosition().y + misil1.getGlobalBounds().height + 10 + offset/2.f);
        txtLaser.setPosition(misil2.getPosition().x + misil2.getGlobalBounds().width/2, misil2.getPosition().y + misil2.getGlobalBounds().height + 10 + offset/2.f);
        window.clear(sf::Color::Black);
        window.draw(fondoSprite); // Dibujar el fondo primero
        window.draw(leyenda);
        window.draw(misil1); window.draw(misil2);
        window.draw(txtMisil); window.draw(txtLaser);

        // Efecto visual de selección mejorado: más pequeño, centrado y separados
        float scaleSel = 0.55f, scaleNoSel = 0.42f;
        int sizeSel = 30, sizeNoSel = 22;
        float centerY = window.getSize().y / 2.f + 40;
        float centerX = window.getSize().x / 2.f;
        float offsetX = 140.f; // Más separación
        if (seleccion == 0) {
            misil1.setColor(sf::Color::Green);
            txtMisil.setFillColor(sf::Color::Green);
            misil1.setScale(scaleSel, scaleSel);
            txtMisil.setCharacterSize(sizeSel);
            misil2.setColor(sf::Color::White);
            txtLaser.setFillColor(sf::Color::White);
            misil2.setScale(scaleNoSel, scaleNoSel);
            txtLaser.setCharacterSize(sizeNoSel);
        } else {
            misil1.setColor(sf::Color::White);
            txtMisil.setFillColor(sf::Color::White);
            misil1.setScale(scaleNoSel, scaleNoSel);
            txtMisil.setCharacterSize(sizeNoSel);
            misil2.setColor(sf::Color::Green);
            txtLaser.setFillColor(sf::Color::Green);
            misil2.setScale(scaleSel, scaleSel);
            txtLaser.setCharacterSize(sizeSel);
        }
        // Posiciones centradas y separadas
        misil1.setPosition(centerX - offsetX - misil1.getGlobalBounds().width/2, centerY);
        misil2.setPosition(centerX + offsetX - misil2.getGlobalBounds().width/2, centerY);
        txtMisil.setOrigin(txtMisil.getLocalBounds().width/2, 0);
        txtLaser.setOrigin(txtLaser.getLocalBounds().width/2, 0);
        txtMisil.setPosition(misil1.getPosition().x + misil1.getGlobalBounds().width/2, misil1.getPosition().y + misil1.getGlobalBounds().height + 8);
        txtLaser.setPosition(misil2.getPosition().x + misil2.getGlobalBounds().width/2, misil2.getPosition().y + misil2.getGlobalBounds().height + 8);

        window.display();
    }
    return seleccion;
}

// Función para mostrar las instrucciones del juego
void mostrarInstrucciones(sf::RenderWindow& window, sf::Font& font) {
    // Cargar la fuente Retro Gaming
    sf::Font retroFont;
    if (!retroFont.loadFromFile("assets/fonts/Retro Gaming.ttf")) {
        std::cerr << "No se pudo cargar la fuente Retro Gaming.ttf, usando fuente por defecto" << std::endl;
        // Si no se puede cargar, usar la fuente pasada como parámetro
    }
    sf::Font& fuenteAUsar = retroFont.getInfo().family.empty() ? font : retroFont;

    // Cargar el fondo
    sf::Texture fondoTexture;
    if (!fondoTexture.loadFromFile("assets/images/Fondo2.png")) {
        std::cerr << "No se pudo cargar el fondo para instrucciones" << std::endl;
    }
    sf::Sprite fondoSprite(fondoTexture);
    float scaleX = window.getSize().x / static_cast<float>(fondoTexture.getSize().x);
    float scaleY = window.getSize().y / static_cast<float>(fondoTexture.getSize().y);
    fondoSprite.setScale(scaleX, scaleY);    // Título principal
    sf::Text titulo;
    titulo.setFont(fuenteAUsar);
    titulo.setString(L"INSTRUCCIONES DEL JUEGO");
    titulo.setCharacterSize(48);
    titulo.setFillColor(sf::Color::Yellow);
    titulo.setStyle(sf::Text::Bold);
    sf::FloatRect tituloBounds = titulo.getLocalBounds();
    titulo.setOrigin(tituloBounds.width/2, tituloBounds.height/2);
    titulo.setPosition(window.getSize().x/2, 60);    // Sección de controles
    sf::Text tituloControles;
    tituloControles.setFont(fuenteAUsar);
    tituloControles.setString(L"CONTROLES:");
    tituloControles.setCharacterSize(32);
    tituloControles.setFillColor(sf::Color::Cyan);
    tituloControles.setStyle(sf::Text::Bold);
    tituloControles.setPosition(50, 130);

    sf::Text controles;
    controles.setFont(fuenteAUsar);
    controles.setString(L"• W / Flecha Arriba: Mover hacia adelante\n"
                       L"• Mouse: Girar la nave (apuntar)\n"
                       L"• Click Izquierdo: Disparar\n"
                       L"• ESC: Pausar el juego");
    controles.setCharacterSize(24);
    controles.setFillColor(sf::Color::White);
    controles.setPosition(70, 180);    // Título de items
    sf::Text tituloItems;
    tituloItems.setFont(fuenteAUsar);
    tituloItems.setString(L"ITEMS Y POWER-UPS:");
    tituloItems.setCharacterSize(32);
    tituloItems.setFillColor(sf::Color::Cyan);
    tituloItems.setStyle(sf::Text::Bold);
    tituloItems.setPosition(50, 320);// Cargar sprites de items
    sf::Texture spriteTexture, corazonTexture, escudoTexture, dobleDisparoTexture;
    sf::Sprite spriteItem, corazonItem, escudoItem, dobleDisparoItem;
    
    // Sistema de asteroides de fondo
    struct AsteroideBackground {
        sf::CircleShape shape;
        sf::Vector2f velocity;
        float rotation;
        float rotationSpeed;
    };
    
    std::vector<AsteroideBackground> asteroidesBackground;
    sf::Texture asteroidTexture;
    
    // Cargar textura del asteroide si está disponible
    bool asteroidTextureLoaded = asteroidTexture.loadFromFile("assets/images/Asteroide.pixil.png");
    
    // Crear asteroides iniciales
    for (int i = 0; i < 8; i++) {
        AsteroideBackground ast;
          if (asteroidTextureLoaded) {
            // Usar sprite con textura
            ast.shape.setRadius(15 + rand() % 20);
            ast.shape.setTexture(&asteroidTexture);
            ast.shape.setFillColor(sf::Color(255, 255, 255, 80)); // Transparente
        } else {
            // Usar forma básica
            ast.shape.setRadius(15 + rand() % 20);
            ast.shape.setFillColor(sf::Color(100, 100, 100, 80)); // Transparente
            ast.shape.setOutlineColor(sf::Color(255, 255, 255, 60)); // Contorno transparente
            ast.shape.setOutlineThickness(1);
        }
        
        // Posición inicial aleatoria en los bordes
        int side = rand() % 4;
        switch (side) {
            case 0: // Arriba
                ast.shape.setPosition(-50, rand() % (int)window.getSize().y);
                ast.velocity = sf::Vector2f(50 + rand() % 100, 20 + rand() % 40);
                break;
            case 1: // Derecha
                ast.shape.setPosition(window.getSize().x + 50, rand() % (int)window.getSize().y);
                ast.velocity = sf::Vector2f(-(50 + rand() % 100), 20 + rand() % 40);
                break;
            case 2: // Abajo
                ast.shape.setPosition(rand() % (int)window.getSize().x, window.getSize().y + 50);
                ast.velocity = sf::Vector2f(20 + rand() % 40, -(50 + rand() % 100));
                break;
            case 3: // Izquierda
                ast.shape.setPosition(rand() % (int)window.getSize().x, -50);
                ast.velocity = sf::Vector2f(20 + rand() % 40, 50 + rand() % 100);
                break;
        }
        
        ast.rotation = 0;
        ast.rotationSpeed = (rand() % 100 - 50) / 10.0f;  // -5 a 5 grados por frame
        ast.shape.setOrigin(ast.shape.getRadius(), ast.shape.getRadius());
        
        asteroidesBackground.push_back(ast);
    }
    
    // Sprites decorativos adicionales
    std::vector<sf::Sprite> spritesDecorativos;
    
    // Vida extra (sprite.png) - Principal
    if (spriteTexture.loadFromFile("assets/images/sprite.png")) {
        spriteItem.setTexture(spriteTexture);
        spriteItem.setScale(0.4f, 0.4f);  // Más pequeño
        spriteItem.setPosition(70, 370);
        
        // Crear sprites decorativos adicionales
        for (int i = 0; i < 3; i++) {
            sf::Sprite decorativo;
            decorativo.setTexture(spriteTexture);
            decorativo.setScale(0.2f, 0.2f);  // Muy pequeños para decoración
            spritesDecorativos.push_back(decorativo);
        }
        // Posiciones decorativas para la estrella
        spritesDecorativos[0].setPosition(900, 150);
        spritesDecorativos[1].setPosition(1000, 300);
        spritesDecorativos[2].setPosition(850, 600);
    }
    
    // Corazón (vida extra) - Principal
    if (corazonTexture.loadFromFile("assets/images/Corazon.png")) {
        corazonItem.setTexture(corazonTexture);
        corazonItem.setScale(0.3f, 0.3f);  // Más pequeño
        corazonItem.setPosition(70, 430);
        
        // Crear corazones decorativos adicionales
        for (int i = 0; i < 4; i++) {
            sf::Sprite decorativo;
            decorativo.setTexture(corazonTexture);
            decorativo.setScale(0.15f, 0.15f);  // Muy pequeños para decoración
            spritesDecorativos.push_back(decorativo);
        }
        // Posiciones decorativas para los corazones
        int baseIndex = spritesDecorativos.size() - 4;
        spritesDecorativos[baseIndex].setPosition(950, 200);
        spritesDecorativos[baseIndex + 1].setPosition(800, 450);
        spritesDecorativos[baseIndex + 2].setPosition(1050, 500);
        spritesDecorativos[baseIndex + 3].setPosition(920, 750);
    }
    
    // Escudo - Principal
    if (escudoTexture.loadFromFile("assets/images/Escudo.item.png")) {
        escudoItem.setTexture(escudoTexture);
        escudoItem.setScale(0.4f, 0.4f);  // Más pequeño
        escudoItem.setPosition(70, 490);
        
        // Crear escudos decorativos adicionales
        for (int i = 0; i < 2; i++) {
            sf::Sprite decorativo;
            decorativo.setTexture(escudoTexture);
            decorativo.setScale(0.2f, 0.2f);  // Pequeños para decoración
            spritesDecorativos.push_back(decorativo);
        }
        // Posiciones decorativas para los escudos
        int baseIndex = spritesDecorativos.size() - 2;
        spritesDecorativos[baseIndex].setPosition(880, 350);
        spritesDecorativos[baseIndex + 1].setPosition(1020, 650);
    }
    
    // Doble disparo - Principal
    if (dobleDisparoTexture.loadFromFile("assets/images/Doble_Disparo_Item.png")) {
        dobleDisparoItem.setTexture(dobleDisparoTexture);
        dobleDisparoItem.setScale(0.4f, 0.4f);  // Más pequeño
        dobleDisparoItem.setPosition(70, 550);
        
        // Crear doble disparos decorativos adicionales
        for (int i = 0; i < 2; i++) {
            sf::Sprite decorativo;
            decorativo.setTexture(dobleDisparoTexture);
            decorativo.setScale(0.2f, 0.2f);  // Pequeños para decoración
            spritesDecorativos.push_back(decorativo);
        }
        // Posiciones decorativas para doble disparo
        int baseIndex = spritesDecorativos.size() - 2;
        spritesDecorativos[baseIndex].setPosition(970, 400);
        spritesDecorativos[baseIndex + 1].setPosition(820, 700);
    }    // Descripciones de items con sprites correspondientes
    sf::Text descripcionSprite, descripcionCorazon, descripcionEscudo, descripcionDoble;      // Configurar sprites para mostrar junto a las descripciones
    if (spriteTexture.getSize().x > 0) {
        spriteItem.setScale(0.80f, 0.80f);  // Tamaño mediano y consistente
        spriteItem.setPosition(55, 375);    // Alineado a la izquierda
    }
    
    if (corazonTexture.getSize().x > 0) {
        corazonItem.setScale(0.09f, 0.09f); // Tamaño mediano y consistente
        corazonItem.setPosition(30, 430);   // Alineado a la izquierda
    }
    
    if (escudoTexture.getSize().x > 0) {
        escudoItem.setScale(0.80f, 0.80f);  // Tamaño mediano y consistente
        escudoItem.setPosition(45, 490);    // Alineado a la izquierda
    }
      if (dobleDisparoTexture.getSize().x > 0) {
        dobleDisparoItem.setScale(0.09f, 0.09f);  // Tamaño mediano y consistente
        dobleDisparoItem.setPosition(43, 547);    // Alineado a la izquierda
    }      descripcionSprite.setFont(fuenteAUsar);
    descripcionSprite.setString(L"Vida Extra - Otorga una vida adicional");
    descripcionSprite.setCharacterSize(20);
    descripcionSprite.setFillColor(sf::Color::White);
    descripcionSprite.setPosition(100, 380);
    
    descripcionCorazon.setFont(fuenteAUsar);
    descripcionCorazon.setString(L"Corazón - Restaura tu salud");
    descripcionCorazon.setCharacterSize(20);
    descripcionCorazon.setFillColor(sf::Color::White);
    descripcionCorazon.setPosition(100, 440);
    
    descripcionEscudo.setFont(fuenteAUsar);
    descripcionEscudo.setString(L"Escudo - Te hace invulnerable por 5 segundos");
    descripcionEscudo.setCharacterSize(20);
    descripcionEscudo.setFillColor(sf::Color::White);
    descripcionEscudo.setPosition(100, 500);
    
    descripcionDoble.setFont(fuenteAUsar);
    descripcionDoble.setString(L"Doble Disparo - Duplica tu poder de fuego");
    descripcionDoble.setCharacterSize(20);
    descripcionDoble.setFillColor(sf::Color::White);
    descripcionDoble.setPosition(100, 560);    // Información adicional
    sf::Text tituloInfo;
    tituloInfo.setFont(fuenteAUsar);
    tituloInfo.setString(L"INFORMACIÓN ADICIONAL:");
    tituloInfo.setCharacterSize(32);
    tituloInfo.setFillColor(sf::Color::Cyan);
    tituloInfo.setStyle(sf::Text::Bold);
    tituloInfo.setPosition(50, 620);

    sf::Text infoAdicional;
    infoAdicional.setFont(fuenteAUsar);
    infoAdicional.setString(L"• Destruye asteroides para ganar puntos\n"
                           L"• A los 600 puntos aparece el agujero negro\n"
                           L"• Evita que los asteroides te toquen\n"
                           L"• Los asteroides grandes se dividen al ser disparados");
    infoAdicional.setCharacterSize(20);
    infoAdicional.setFillColor(sf::Color::White);
    infoAdicional.setPosition(70, 670);    // Instrucción para continuar
    sf::Text continuar;
    continuar.setFont(fuenteAUsar);
    continuar.setString(L"Presiona ESPACIO para comenzar el juego");
    continuar.setCharacterSize(28);
    continuar.setFillColor(sf::Color::Yellow);
    continuar.setStyle(sf::Text::Bold);
    sf::FloatRect continuarBounds = continuar.getLocalBounds();
    continuar.setOrigin(continuarBounds.width/2, continuarBounds.height/2);
    continuar.setPosition(window.getSize().x/2, window.getSize().y - 80);// Animación del texto de continuar
    sf::Clock animClock;
    bool mostrarTexto = true;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                return; // Salir de las instrucciones
            }
        }        // Animación del texto "continuar"
        if (animClock.getElapsedTime().asSeconds() > 0.8f) {
            mostrarTexto = !mostrarTexto;
            animClock.restart();
        }

        // Actualizar asteroides de fondo
        float deltaTime = 1.0f / 60.0f; // Aproximadamente 60 FPS
        for (auto& ast : asteroidesBackground) {
            // Mover asteroide
            ast.shape.move(ast.velocity * deltaTime);
            
            // Rotar asteroide
            ast.rotation += ast.rotationSpeed;
            ast.shape.setRotation(ast.rotation);
            
            // Verificar si el asteroide salió de la pantalla y reposicionarlo
            sf::Vector2f pos = ast.shape.getPosition();
            bool outOfBounds = false;
            
            if (pos.x < -100 || pos.x > window.getSize().x + 100 ||
                pos.y < -100 || pos.y > window.getSize().y + 100) {
                outOfBounds = true;
            }
            
            if (outOfBounds) {
                // Reposicionar desde un lado aleatorio
                int side = rand() % 4;
                switch (side) {
                    case 0: // Arriba
                        ast.shape.setPosition(-50, rand() % (int)window.getSize().y);
                        ast.velocity = sf::Vector2f(50 + rand() % 100, 20 + rand() % 40);
                        break;
                    case 1: // Derecha
                        ast.shape.setPosition(window.getSize().x + 50, rand() % (int)window.getSize().y);
                        ast.velocity = sf::Vector2f(-(50 + rand() % 100), 20 + rand() % 40);
                        break;
                    case 2: // Abajo
                        ast.shape.setPosition(rand() % (int)window.getSize().x, window.getSize().y + 50);
                        ast.velocity = sf::Vector2f(20 + rand() % 40, -(50 + rand() % 100));
                        break;
                    case 3: // Izquierda
                        ast.shape.setPosition(rand() % (int)window.getSize().x, -50);
                        ast.velocity = sf::Vector2f(20 + rand() % 40, 50 + rand() % 100);
                        break;
                }
            }
        }

        // Dibujar todo
        window.clear();
        window.draw(fondoSprite);
        
        // Dibujar asteroides de fondo primero
        for (const auto& ast : asteroidesBackground) {
            window.draw(ast.shape);
        }
        
        // Solo dibujar texto, sin sprites que interfieran con la lectura
        
        window.draw(titulo);window.draw(tituloControles);
        window.draw(controles);        window.draw(tituloItems);
        
        // Dibujar sprites de items junto a sus descripciones
        if (spriteTexture.getSize().x > 0) window.draw(spriteItem);
        if (corazonTexture.getSize().x > 0) window.draw(corazonItem);
        if (escudoTexture.getSize().x > 0) window.draw(escudoItem);
        if (dobleDisparoTexture.getSize().x > 0) window.draw(dobleDisparoItem);
        
        window.draw(descripcionSprite);
        window.draw(descripcionCorazon);
        window.draw(descripcionEscudo);
        window.draw(descripcionDoble);
        window.draw(tituloInfo);
        window.draw(infoAdicional);
        
        if (mostrarTexto) {
            window.draw(continuar);
        }
        
        window.display();
    }
}