# 🔧 Guía de Instalación Detallada

## Requisitos
- Windows 10/11
- SFML 2.5.x
- MinGW o compilador compatible con C++17

## Pasos
1. Clona el repositorio o descarga el código fuente.
2. Instala SFML y asegúrate de que las DLL estén en el PATH o junto al ejecutable.
3. Compila usando el makefile o el siguiente comando:
   ```sh
   c++ src/Asteroids.cpp src/Ventana.cpp src/Nave.cpp src/Misil.cpp src/Margen.cpp src/Punto.cpp src/Puntaje.cpp src/Vida.cpp src/Asteroide.cpp src/Oportunidad.cpp src/VidaExtra.cpp src/EscudoItem.cpp src/TablaDePuntaje.cpp src/DobleDisparoItem.cpp src/Explosion.cpp -o Asteroids -std=c++17 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
   ```
4. Ejecuta `Asteroids.exe`.

## Notas
- Si tienes problemas con las fuentes o assets, revisa que la estructura de carpetas sea igual a la del repositorio.
- Para Linux/Mac, adapta los comandos de compilación y asegúrate de tener SFML instalado.
