# 👨‍💻 Documentación para Desarrolladores

## Estructura del Proyecto
- `src/`: Código fuente principal (C++)
- `include/`: Archivos de cabecera
- `assets/`: Imágenes, fuentes y sonidos
- `docs/`: Documentación

## Compilación
Ver la [Guía de Instalación Detallada](instalacion.md).

## Principales Archivos/Ficheros
- `Asteroids.cpp`: Entrada principal
- `Ventana.cpp`: Lógica de ventanas, menús y ciclo de juego
- `Nave.cpp`, `Asteroide.cpp`, `Misil.cpp`, etc.: Lógica de entidades

## Notas de Desarrollo
- El juego usa SFML para gráficos, audio y entrada.
- Los assets deben estar en la ruta relativa correcta.
- El flujo de reinicio y menús se controla con etiquetas y bucles en `Ventana.cpp`.
- El láser y el doble disparo usan lógica especial para colisiones y efectos visuales.

## Contribuciones
- Haz fork y pull request.
- Sigue el estilo de código y comenta funciones complejas.
- Reporta bugs en Issues.
